# Bun Server (`Bun.serve`)

Node에서 HTTP 서버를 띄우려면 `http.createServer`를 쓰거나, 보통은 Express·Fastify·NestJS 같은 프레임워크를 얹는다. Bun은 다르다. **HTTP 서버가 런타임에 내장**돼 있다. `Bun.serve` 하나면 외부 의존성 없이 서버가 뜬다. 그리고 그 인터페이스가 Node의 `(req, res)` 콜백이 아니라 **웹 표준 `Request` → `Response`** 라는 점이 핵심이다.

전제: Bun은 엔진이 V8이 아니라 JavaScriptCore이고 자체 이벤트 루프를 쓴다. 이 배경은 [javascript-runtime.md](../javascript-runtime.md)의 런타임 비교 참고.

## 가장 작은 서버

```js
const server = Bun.serve({
  port: 3000,
  fetch(req) {
    return new Response("Hello, Bun!");
  },
});

console.log(`Listening on http://localhost:${server.port}`);
```

`fetch`는 요청 하나가 들어올 때마다 호출된다. 인자로 받는 `req`는 브라우저에서 쓰는 그 `Request` 객체이고, 반환하는 것도 그 `Response` 객체다. 즉 클라이언트에서 `fetch()`를 호출할 때 쓰던 API를 **서버 쪽에서 거울처럼 그대로** 쓴다. `req.url`, `req.method`, `await req.json()`, `new Response(body, { status, headers })` 전부 표준 그대로다.

`fetch`는 `Response`를 동기로 반환해도 되고 `Promise<Response>`로 반환해도 된다. 그래서 핸들러를 그냥 `async`로 써도 된다.

```js
Bun.serve({
  async fetch(req) {
    const data = await db.query(...);
    return Response.json(data);
  },
});
```

## 라우팅도 내장 (Bun 1.2+)

초기 Bun은 `fetch` 안에서 `req.url`을 직접 분기해야 했지만, 지금은 `routes` 옵션으로 선언적 라우팅을 지원한다.

```js
Bun.serve({
  routes: {
    "/": new Response("home"),                          // 정적 응답
    "/api/users/:id": (req) => Response.json({ id: req.params.id }),
    "/api/posts": {
      GET:  () => Response.json(posts),                  // 메서드별 분기
      POST: async (req) => Response.json(await req.json()),
    },
  },
  // routes에 안 걸리면 fetch로 폴백
  fetch(req) {
    return new Response("Not Found", { status: 404 });
  },
});
```

`:id` 같은 path 파라미터는 `req.params`로 들어온다. 라우트 매칭은 네이티브 레이어에서 처리돼 JS로 분기하는 것보다 빠르다.

## 왜 빠른가

- **HTTP 파싱이 네이티브**: 요청 라인·헤더 파싱을 JS가 아니라 Bun의 네이티브 레이어(Rust, 구 Zig)에서 한다. Node가 libuv(C) 위에서 하던 일을 Bun은 자체 구현으로 처리한다.
- **표준 객체에 최적화**: `Request`/`Response`/`ReadableStream`이 런타임에 직접 구현돼 있어 어댑터 변환 비용이 없다.
- 정적 파일은 `Bun.file()`을 `Response`에 그대로 넘기면 `sendfile`류 최적화로 메모리에 다 올리지 않고 스트리밍한다.

```js
Bun.serve({
  fetch(req) {
    return new Response(Bun.file("./public/index.html"));
  },
});
```

## Node `http` / 프레임워크와의 관계 (중요)

여기서 자주 헷갈린다. "Bun 런타임을 쓴다"와 "Bun.serve를 쓴다"는 다른 얘기다.

| 구성 | HTTP 서버 주체 | 비고 |
|---|---|---|
| `Bun.serve` 직접 | Bun 내장 | 프레임워크 없음. fetch 핸들러 |
| Hono / Elysia | Bun.serve 위에 얹힘 | 둘 다 `fetch` 핸들러를 export → Bun.serve가 그대로 소비 |
| Express (on Bun) | Node 호환 `http` 레이어 | Bun이 `node:http`를 호환 구현. Bun.serve를 **직접 쓰지 않음** |
| NestJS (on Bun) | Express/Fastify 어댑터 | 마찬가지. Bun은 런타임일 뿐, 서버는 NestJS 어댑터가 띄움 |

정리하면 **Bun 런타임 위에서 Express나 NestJS를 돌릴 수는 있지만, 그때 HTTP를 처리하는 건 Bun.serve가 아니라 그 프레임워크의 Node 호환 서버**다. Bun.serve의 이점(네이티브 파싱, 표준 fetch 모델)을 온전히 받으려면 Bun.serve 위에 직접 얹거나, Hono·Elysia처럼 fetch 기반 프레임워크를 써야 한다.

## 실전 패턴: 다른 핸들러에 위임

`fetch`는 결국 `(Request) => Response` 함수 하나라서, 풀스택 프레임워크가 export하는 핸들러를 그대로 꽂을 수 있다. 예를 들어 TanStack Start 같은 프레임워크는 `{ fetch }` 형태의 핸들러를 내보내는데, 프로덕션 서버에서 이걸 Bun.serve가 감싸 정적 에셋 서빙·캐싱만 추가로 얹는 식으로 쓴다.

```js
// 프레임워크가 만들어준 fetch 핸들러
import handler from "./app-handler";

Bun.serve({
  port: Number(process.env.PORT) || 3000,
  fetch(req) {
    // 정적 에셋이면 직접 서빙, 아니면 앱 핸들러로 위임
    return serveStaticOrFallback(req, handler.fetch);
  },
});
```

이 패턴이 "런타임은 Bun, 서버는 Bun.serve, 앱은 프레임워크" 조합이다.

## WebSocket

WebSocket도 내장이다. `fetch`에서 `server.upgrade(req)`로 업그레이드하고, 핸들러는 `websocket` 옵션에 따로 둔다.

```js
Bun.serve({
  fetch(req, server) {
    if (server.upgrade(req)) return;          // 업그레이드 성공 시 Response 불필요
    return new Response("HTTP only");
  },
  websocket: {
    open(ws)        { ws.subscribe("room"); },
    message(ws, m)  { ws.publish("room", m); },  // pub/sub 내장
    close(ws)       { ws.unsubscribe("room"); },
  },
});
```

Bun의 WebSocket은 pub/sub(`subscribe`/`publish`)을 네이티브로 지원해서, 채팅·브로드캐스트류를 별도 라이브러리 없이 구현할 수 있다.

## 주의점

- **Express 미들웨어 생태계와 직접 호환 안 됨**: Bun.serve는 `(req, res, next)` 모델이 아니라 fetch 모델이다. `app.use(...)` 류 미들웨어를 그대로 못 쓴다. 미들웨어가 필요하면 Hono·Elysia처럼 fetch 기반 프레임워크의 미들웨어를 쓰거나 직접 함수로 합성한다.
- **포팅 시 사고방식 전환**: `res.send()`/`res.json()`(가변 응답 객체에 쓰기) → `return Response.json()`(불변 응답 객체 반환)으로 바뀐다. 응답을 "만들어서 반환"하는 함수형에 가깝다.
- **`Bun.serve`는 Bun 전용**: 코드를 Node에서도 돌려야 한다면 이식성이 떨어진다. 이식성이 중요하면 Hono(런타임 무관) 같은 추상화를 한 겹 두는 편이 안전하다.

## 더 볼 것

- Hono / Elysia: fetch 기반 프레임워크가 Bun.serve를 어떻게 감싸는지, 미들웨어·라우터 설계
- `Bun.file` + `Response`: 정적 서빙 시 sendfile/스트리밍 동작과 Range 요청 처리
- `routes`의 HTML import: Bun이 라우트에서 번들링까지 묶는 풀스택 모드
- Node 호환 레이어: `node:http`를 Bun이 어디까지 구현하는지, Express/NestJS가 그 위에서 도는 경계
- 클러스터링: `reusePort: true`로 여러 Bun 프로세스가 같은 포트를 나눠 받는 SO_REUSEPORT 방식 (Node cluster와 비교)
