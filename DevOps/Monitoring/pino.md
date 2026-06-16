# pino

Node.js용 초경량(low-overhead) JSON 로거. "로그는 데이터다(logs are data)"라는 철학으로, 사람이 읽기 좋은 문자열이 아니라 **구조화된 JSON**을 1순위 출력으로 삼는다. v10 기준으로 정리.

- 만든 사람: Matteo Collina (Fastify 제작자), nearForm 진영
- npm: `pino`, 공식 문서: https://getpino.io
- Fastify의 기본 내장 로거

> **v10 변경점**: v10.0.0의 유일한 breaking change는 **Node 18 지원 중단**(Node 20+ 필요). API 자체는 v9와 사실상 호환. 그 외에는 `LogFnFields` 타입 추가, 테스트 러너를 `node:test`로 전환 등 내부 정비 위주. 즉 "v10이라 새로 배울 것"은 거의 없고, 마이그레이션 부담이 낮다. (출처: pino releases / issue #2317)

---

## 핵심 철학

- **로그는 데이터다**: 로그를 사람이 grep 하는 텍스트가 아니라, 수집기(Elasticsearch, Loki, Datadog 등)가 파싱·인덱싱하는 구조화 데이터로 본다. 그래서 출력 기본형이 한 줄 JSON(NDJSON).
- **JSON-first**: 처음부터 JSON으로 직렬화. 사람이 보기 좋은 포맷은 부가 기능(pino-pretty)으로 분리.
- **low-overhead**: 애플리케이션 핫패스에서 로깅이 차지하는 비용을 극단적으로 줄인다. "로깅 때문에 앱이 느려지면 안 된다"가 설계 목표.
- **transport는 별도 스레드**: 로그를 가공/전송하는 무거운 일은 메인 스레드가 아니라 worker thread에서 한다.

---

## 왜 빠른가 (아키텍처)

핵심은 **메인 스레드의 핫패스에서 하는 일을 최소화**하는 것.

- **직렬화 최소화**: 로그 객체를 JSON 문자열로 만드는 작업이 매우 최적화되어 있다. 불필요한 포매팅·색상·정렬을 하지 않는다.
- **레벨을 숫자로 처리**: `info`가 아니라 `30` 같은 정수로 비교/기록 → 문자열 비교 비용 제거.
- **transport를 worker thread로 오프로드**: 파일 쓰기, 외부 전송, pretty 포매팅 등 I/O·CPU 비용이 큰 작업을 별도 worker thread로 넘긴다. 메인 스레드는 "로그 라인을 워커로 넘기는 일"까지만 한다 (v7+).
- **비동기 버퍼링 옵션**: `sync: false`로 로그를 메모리에 모았다가 큰 청크 단위로 flush → write 시스템콜 횟수 감소.

벤치마크상 기본 JSON 로깅에서 Winston 대비 높은 처리량을 보인다. **다만 배수는 출처마다 크게 다르므로 단일 숫자로 단정하면 안 된다.** 확인된 범위:
- pino 공식/일부 벤치: pino ~142k msg/s vs winston ~19.8k → 약 **7배** (johal.in, pino 8 vs winston 3.11)
- 일반 가이드류: "초당 5만+ vs 1만" → 약 **5배**, "JSON 직렬화 4~5배", "전반 5~8배" (BetterStack/DEV 등)
- 최신 Node(22/24)에서의 보수적 측정: 약 **1.9배** 수준이라는 보고도 있음
- 결론(교차검증): "조건·Node 버전·포맷터 설정에 따라 대략 **2배~7배**", 핵심은 "pino가 일관되게 더 빠르되 정확한 배수는 환경 의존적".

```js
import pino from 'pino'

const logger = pino()
logger.info('hello world')
// {"level":30,"time":1531171074631,"pid":657,"hostname":"x","msg":"hello world"}
```

---

## 로그 레벨

문자열 이름과 숫자값이 1:1로 매핑된다. 숫자가 클수록 심각.

| 레벨 | 숫자값 |
|:---:|:---:|
| `trace` | 10 |
| `debug` | 20 |
| `info`  | 30 |
| `warn`  | 40 |
| `error` | 50 |
| `fatal` | 60 |

- 기본 레벨은 `info` (30). `logger.level`보다 낮은 레벨의 로그는 출력되지 않는다.
- 출력 JSON에는 숫자(`"level":30`)로 찍힌다. 수집 단계에서 사람이 읽을 이름으로 바꾸려면 `formatters.level`을 쓰거나 pino-pretty가 변환.

```js
const logger = pino()
logger.debug('nope')          // 기본 레벨 info라 출력 안 됨
logger.level = 'debug'        // 런타임에 레벨 변경 가능
logger.debug('이제 보임')
```

---

## 구조화 로깅 API — 인자 순서가 핵심

pino의 로그 메서드는 **`(mergingObject, message, ...interpolationArgs)`** 순서다. 객체가 **먼저**, 메시지가 나중이다. (console.log나 winston과 헷갈리기 쉬운 부분)

```js
// 올바른 순서: 객체 먼저, 메시지 나중
logger.info({ userId: 456, tunnelId: 'abc' }, 'tunnel established')
// {"level":30,...,"userId":456,"tunnelId":"abc","msg":"tunnel established"}

// 메시지 보간도 가능 (% 포맷)
logger.info('listening on port %d', 8080)

// 에러는 err 키로 넘기면 스택까지 직렬화됨 (std serializer)
logger.error({ err }, 'failed to forward connection')
```

- 객체를 먼저 넘기면 그 키들이 로그 라인 최상위에 병합된다.
- `msg` 키가 메시지 자리.

---

## child 로거 (bindings)

부모 로거의 설정·컨텍스트를 상속하면서, **고정 key-value(bindings)** 를 모든 로그에 자동으로 붙이는 자식 로거. 요청별/모듈별 컨텍스트를 흘려보낼 때 핵심 도구.

```js
const logger = pino()

// 요청 단위 컨텍스트
const reqLog = logger.child({ requestId: 'req-123', userId: 456 })
reqLog.info('processing request')
// {... ,"requestId":"req-123","userId":456,"msg":"processing request"}

// 자식의 자식은 부모 bindings를 모두 상속
const dbLog = reqLog.child({ module: 'database' })
dbLog.info('query executed')
// {... ,"requestId":"req-123","userId":456,"module":"database",...}

// 현재 bindings 조회 / 동적 추가
dbLog.bindings()                  // { requestId, userId, module }
dbLog.setBindings({ took: '45ms' }) // 덮어쓰지 않고 추가 (중복 키 주의)
```

- child 생성 시 두 번째 인자로 `level`, `msgPrefix`, `redact`, `serializers` 등을 개별 오버라이드 가능.
- burrow처럼 연결(connection)/터널(tunnel) 단위로 컨텍스트가 명확한 네트워크 도구에서는, 연결마다 `logger.child({ connId })`를 만들어 쓰면 로그 추적이 쉬워진다.

```js
const child = logger.child({ foo: 'bar' }, { level: 'debug', msgPrefix: '[proxy] ' })
```

---

## serializers (직렬화기)

특정 키의 값을 로그에 찍기 전에 가공하는 함수. 객체 전체를 그대로 찍지 않고 필요한 필드만 추출하거나 형태를 정리할 때 쓴다.

```js
const logger = pino({
  serializers: {
    req: (req) => ({ method: req.method, url: req.url }),
    // pino가 제공하는 표준 직렬화기도 있음: pino.stdSerializers.err / .req / .res
    err: pino.stdSerializers.err,
  }
})

logger.info({ req }, 'incoming request') // req는 method/url만 남음
```

- 자식 로거는 부모 serializer를 상속하며, child의 `serializers`로 덮어쓰거나 추가할 수 있다.
- `pino.stdSerializers.err`는 Error 객체를 `type/message/stack`으로 안전하게 직렬화.

---

## redaction (민감정보 마스킹)

지정한 경로의 값을 `[Redacted]`로 가리거나 아예 제거. password, token, authorization 같은 값이 로그로 새는 걸 막는 1차 방어선.

```js
const logger = pino({
  redact: ['password', 'headers.authorization', '*.token'],
  // 고급 형태: 커스텀 마스킹 / 키 제거
  // redact: { paths: ['password'], censor: '***', remove: false }
})

logger.info({ password: 'secret', headers: { authorization: 'Bearer x' } })
// {... ,"password":"[Redacted]","headers":{"authorization":"[Redacted]"}}
```

- 경로는 정확히 적어야 한다. `headers.authorization`을 잡았다고 `Authorization`(대문자)나 `headers.Cookie`가 자동으로 잡히지 않는다 → 흔한 누락 지점.
- 와일드카드(`*.token`, `*.password`) 지원.
- child 로거에서 `redact`를 추가 지정 가능. **주의**: child에 건 redact는 그 child에만 적용되고 부모 logger에는 영향이 없다(공식 api.md 예제로 확인). 즉 redact는 "로거 인스턴스 단위"다.

```js
const logger = pino({ redact: ['hello'] })
const child = logger.child({ foo: 'bar' }, { redact: ['foo'] })
child.info({ hello: 'world' })  // hello, foo 둘 다 [Redacted]
logger.info({ hello: 'world' }) // hello만 [Redacted] (foo는 부모에 무관)
```

---

## transport 시스템 (v7+, worker thread 기반)

v7부터 transport는 **별도 worker thread**에서 동작한다. `worker_thread` + `WeakRef`/`FinalizationRegistry`로 구현. 메인 스레드는 로그를 워커로 넘기고, 워커가 포매팅/파일쓰기/외부전송을 담당 → 메인 이벤트 루프가 막히지 않는다.

```js
// 단일 transport
const transport = pino.transport({
  target: 'pino/file',
  options: { destination: '/var/log/app.log', mkdir: true }
})
const logger = pino(transport)

// 여러 target (레벨별 분기 가능)
const transport = pino.transport({
  targets: [
    { target: 'pino/file', options: { destination: '/var/log/app.log' } },
    { target: 'pino/file', options: { destination: 1 } },          // 1 = stdout
    { target: 'pino-pretty', level: 'debug' },                      // dev에서만
  ]
})
const logger = pino(transport)
```

- `pino/file`의 `destination`에 파일 디스크립터 번호를 줄 수 있다: **1 = STDOUT, 2 = STDERR**.
- 여러 target일 때 레벨 필터는 `logger.level`이 먼저, 각 `targets[i].level`이 그 다음 적용.
- transport target은 절대경로 모듈이거나 설치된 패키지명.

**dedupe 모드** (`dedupe: true`): 여러 target이 레벨로 겹칠 때, 기본은 조건을 만족하는 **모든** target에 다 쓴다. dedupe를 켜면 해당 로그를 받을 자격이 있는 target 중 **가장 높은(specific한) 레벨 하나에만** 보낸다 → 같은 줄이 여러 파일에 중복 기록되는 걸 방지. (출처: pino transports 공식 문서)

```js
const logger = pino(pino.transport({
  targets: [
    { target: 'pino/file', level: 'error', options: { destination: './error.log' } },
    { target: 'pino/file', level: 'info',  options: { destination: './all.log' } },
  ],
  dedupe: true,
}))
logger.error('critical') // dedupe로 error.log 에만 (all.log 중복 안 됨)
```

**destination을 코드에서 직접** 줄 수도 있다(transport 없이, 동기/단순 경로):

```js
// stderr로 직접
const logger = pino({ name: 'burrow' }, pino.destination(2))
// 파일 경로 자동 래핑
const fileLogger = pino('/var/log/burrow.log')
// 비동기 버퍼링
const asyncLogger = pino(pino.destination({ dest: '/var/log/burrow.log', sync: false }))
```

---

## pino-pretty (사람이 읽는 출력)

JSON은 기계용이라 개발 중 콘솔에서 보기 불편하다. pino-pretty가 색·정렬된 사람 친화 출력으로 변환.

**dev: 파이프 방식 (권장)** — 앱은 순수 JSON을 stdout으로 내보내고, 셸에서 파이프로만 예쁘게 본다. 앱 코드/성능에 영향 0.

```bash
node app.js | npx pino-pretty
```

**dev: transport 방식** — 코드에서 직접 끼우기. 편하지만 prod로 새어나가기 쉬움.

```js
const logger = pino({
  transport: { target: 'pino-pretty', options: { colorize: true } }
})
```

---

## 베스트 프랙티스 / 흔한 함정

- **prod에서 pino-pretty를 transport로 끼우지 말 것.** pretty 변환은 비용이 크고 JSON 수집을 방해한다. prod는 순수 JSON을 stdout으로 내보내고, pretty는 dev에서 파이프(`| pino-pretty`)로만 사용.
- **stdout vs stderr**: 12-factor 원칙상 로그는 stdout으로 흘려보내고 수집은 인프라(컨테이너 런타임, 수집기)에 맡기는 게 기본. 단, 진짜 에러/치명 로그를 stderr로 분리하고 싶으면 transport target의 `destination: 2`로 라우팅 가능. 한 스트림으로 통일하는 편이 단순하고 흔하다.
- **redaction을 처음부터 켜라.** 사고는 보통 "그 필드가 로그에 찍히는 줄 몰랐다"에서 난다. 대소문자/중첩 경로 누락 주의.
- **비동기 로깅과 프로세스 종료 시 flush**: `sync:false`(또는 transport 사용)면 버퍼에 남은 로그가 종료 시 유실될 수 있다. `process.on('exit')`에서는 비동기 작업이 불가하므로, 종료 전 명시적으로 flush 하거나 `pino.destination({ sync: false })` + flush 콜백을 챙긴다. (`logger.flush()`는 pino-pretty와 함께 쓰면 동작하지 않는 점 주의.)
- **child 로거로 컨텍스트를 흘려라.** 요청/연결마다 `requestId` 등을 매번 손으로 붙이지 말고 child binding에 맡긴다.
- **레벨은 환경변수로.** prod=info, dev=debug 식으로 `pino({ level: process.env.LOG_LEVEL ?? 'info' })`.

```js
// 종료 시 flush 패턴 (개념)
const logger = pino(pino.destination({ sync: false }))
process.on('SIGINT', () => {
  logger.flush()
  process.exit(0)
})
```

---

## 생태계

- **Fastify 기본 로거**: Fastify는 pino를 내장. `fastify({ logger: true })`면 끝. 요청마다 `request.log` child가 자동 제공.
- **pino-http**: Express 등에서 HTTP 요청/응답을 자동 로깅하는 미들웨어. `req`/`res` 직렬화, 요청별 child 로거 자동 생성.
- **OpenTelemetry 연동 (두 갈래)**:
  - `@opentelemetry/instrumentation-pino`: 활성 trace context(trace_id/span_id)를 모든 로그에 **주입**해 로그↔트레이스 상관관계 형성.
  - `pino-opentelemetry-transport`: 로그를 OTel Log Data Model로 바꿔 **OTLP 수집기로 전송** (worker thread transport 메커니즘 활용).
- **로그 수집기(Fluent Bit 등)**: pino는 stdout JSON만 내보내고, 수집·전송은 인프라(Fluent Bit / OTel Collector / Vector)가 맡는 구성이 일반적. Fluent Bit 직결 공식 transport보다는 OTLP→Collector→Fluent Bit 경로가 흔함.

---

## winston 대비

| 관점 | pino | winston |
|---|---|---|
| 철학 | "로그는 데이터" JSON-first, 성능 우선 | 유연성·설정 우선 |
| 성능 | 매우 빠름 (핫패스 최소화, worker transport) | 상대적으로 느림 (벤치마크상 수 배 차이) |
| 무게 | 코어가 가볍고 의존성 적음 | 코어가 무겁고 기능 풍부 |
| transport | worker thread 기반, 메인 스레드 보호 | 메인 프로세스 내 transport(80+ 커뮤니티 transport) |
| 출력 | 기본 JSON, pretty는 분리 | 포맷 조합 자유도 큼 |
| 적합 | 고처리량/관측가능성 중심, 컨테이너·12factor 환경 | 출력 대상이 다양하고 세밀한 포맷 제어가 필요한 경우 |

- 초당 수천 req 미만의 평범한 앱이라면 성능 차이는 사실상 무의미. 선택은 "구조화 로깅+관측성(pino)" vs "다양한 출력·세밀한 포맷(winston)" 취향 문제에 가깝다.
- burrow처럼 경량 CLI/네트워크 도구라면 의존성이 가볍고 JSON 기본인 pino가 결이 맞는다.

---

## burrow에서의 실제 적용 (`src/shared/logger.ts`)

현재 burrow 설정은 베스트 프랙티스에 부합한다:

```ts
const level = process.env.LOG_LEVEL ?? "info";
// TTY(대화형 터미널)일 때만 pino-pretty를 transport로 끼우고,
// 파이프/리다이렉트(CI·Docker·systemd)면 순수 NDJSON 출력
const usePretty = Boolean(process.stdout.isTTY) && level !== "silent";
const root = pino({
  level,
  transport: usePretty
    ? { target: "pino-pretty", options: { colorize: true } }
    : undefined,
});
// 컴포넌트 태깅 child: logger("relay") → 모든 줄에 {"component":"relay"}
export function logger(component: string): Logger {
  return root.child({ component });
}
```

- 잘한 점: ① 레벨을 env로, ② **pretty를 `isTTY`로 게이팅**해 prod(파이프/Docker)에서는 자동으로 raw JSON → "prod pretty 금지" 원칙을 코드로 강제, ③ child로 컴포넌트 컨텍스트 전파.
- 향후 검토할 만한 점(추정, 현재 코드엔 없음): redaction 미설정 — 터널 도구라 인증 토큰/헤더가 로그에 들어갈 여지가 있으면 `redact` 추가 고려. transport(worker thread)나 `sync:false`를 쓰게 되면 종료 시 flush 처리 필요(현재 pretty-or-undefined 구성이라 일반적 stdout은 동기에 가까워 당장은 덜 시급).

---

## 참고 링크

- pino 공식 문서: https://getpino.io
- pino GitHub: https://github.com/pinojs/pino
- transport 문서: https://github.com/pinojs/pino/blob/main/docs/transports.md
- pino v7 worker thread transport 소개(nearForm): https://nearform.com/insights/pino7-0-0-pino-transport-worker-thread-transport/
- pino-pretty: https://github.com/pinojs/pino-pretty
- pino-http: https://github.com/pinojs/pino-http
- pino-opentelemetry-transport: https://github.com/pinojs/pino-opentelemetry-transport
- @opentelemetry/instrumentation-pino: https://www.npmjs.com/package/@opentelemetry/instrumentation-pino
- pino 공식 벤치마크: https://github.com/pinojs/pino/blob/main/docs/benchmarks.md
- pino API 문서(v10.1.0): https://github.com/pinojs/pino/blob/v10.1.0/docs/api.md
- pino transports 문서: https://github.com/pinojs/pino/blob/v10.1.0/docs/transports.md
- pino LTS/Node 지원 정책: https://github.com/pinojs/pino/blob/main/docs/lts.md
- v10 breaking change 논의(issue #2317): https://github.com/pinojs/pino/issues/2317
- 벤치(교차검증): https://johal.in/benchmark-winston-311-vs-pino-80-nodejs-24/ , https://betterstack.com/community/guides/scaling-nodejs/pino-vs-winston/
