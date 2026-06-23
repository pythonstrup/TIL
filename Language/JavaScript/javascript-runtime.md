# JavaScript Runtime

JS는 싱글 스레드인데 어떻게 동시에 많은 요청을 처리할까. 핵심은 JS가 직접 기다리지 않는다는 데 있다. 오래 걸리는 일은 런타임(libuv 등)에 넘겨두고 자기는 다음 줄로 넘어간다. 넘긴 일이 끝나면 콜백이 큐에 쌓이고, 콜 스택이 비었을 때 그 큐에서 하나씩 꺼내 실행한다. 이 "꺼내서 실행하는 반복"이 이벤트 루프다.

## 기본 구조

세 가지만 잡으면 된다.

- JS 엔진은 콜 스택 하나로 한 번에 한 줄씩 실행한다. 동기 코드는 위에서 아래로 순서대로 돈다.
- 타이머, 파일 I/O, 네트워크 같은 비동기 작업은 JS가 아니라 런타임과 OS가 처리한다. JS는 콜백만 등록하고 바로 넘어간다.
- 작업이 끝나면 콜백이 큐로 들어가고, 콜 스택이 비면 이벤트 루프가 큐에서 꺼내 실행한다.

```js
console.log('1');
setTimeout(() => console.log('2'), 0);
console.log('3');
// 1 → 3 → 2
```

setTimeout 콜백이 마지막인 이유는 동기 코드가 다 끝나고 콜 스택이 빈 다음에야 큐에서 꺼내지기 때문이다.

## 큐는 한 종류가 아니다

큐가 둘로 나뉘고 우선순위가 다르다.

- 매크로태스크: `setTimeout`, `setImmediate`, I/O 콜백
- 마이크로태스크: `Promise.then`, `await` 뒤 코드, `queueMicrotask`

규칙은 하나다. 동기 코드 한 덩어리가 끝날 때마다, 매크로태스크를 하나 꺼내기 전에 마이크로태스크 큐를 전부 비운다. 그래서 마이크로태스크가 항상 먼저다. 비동기 실행 순서는 대부분 이 규칙으로 설명된다.

```js
console.log('1');
setTimeout(() => console.log('2'), 0);
Promise.resolve().then(() => console.log('3'));
console.log('4');
// 1 → 4 → 3 → 2
```

## Node의 추가 규칙

Node에는 마이크로태스크보다도 먼저 처리되는 `process.nextTick` 큐가 따로 있다.

우선순위는 `process.nextTick` > Promise(마이크로) > setTimeout/IO(매크로) 순이다.

```js
setTimeout(() => console.log('timeout'), 0);
Promise.resolve().then(() => console.log('promise'));
process.nextTick(() => console.log('nextTick'));
console.log('sync');
// sync → nextTick → promise → timeout
```

이벤트 루프는 내부적으로 여러 페이즈를 순서대로 돈다.

```
timers → pending callbacks → poll → check → close
```

- timers: `setTimeout`, `setInterval` 콜백
- poll: 대부분의 I/O 콜백이 여기서 실행된다. 처리할 이벤트가 없으면 여기서 잠깐 대기(블로킹)하며 새 이벤트를 기다린다.
- check: `setImmediate` 콜백

그래서 `setTimeout(fn, 0)`과 `setImmediate(fn)`의 순서는 상황에 따라 갈린다. I/O 콜백 안에서 둘을 호출하면 setImmediate가 먼저인 게 보장되지만, 메인 모듈에서 그냥 호출하면 순서가 들쭉날쭉하다.

## libuv: 스레드 풀과 OS 비동기

비동기 작업이라고 다 같은 방식으로 처리되는 건 아니다.

- 네트워크 소켓은 OS의 비동기 기능(epoll, kqueue, IOCP)으로 처리한다. 별도 스레드를 쓰지 않는다.
- 파일 I/O, `dns.lookup`, crypto, zlib 같은 일부 작업은 OS가 마땅한 비동기 API를 안 주기 때문에 libuv가 스레드 풀에 던져 처리한다.

스레드 풀은 기본 4개이고 `UV_THREADPOOL_SIZE`로 조절한다. 무거운 crypto나 파일 작업이 몰리면 이 풀이 병목이 될 수 있다.

여기서 "Node는 싱글 스레드"라는 말의 정확한 뜻이 드러난다. 우리가 짜는 JS 실행은 싱글 스레드가 맞지만, 그 아래 libuv는 스레드 풀을 돌린다. 싱글 스레드인 건 JS 레이어일 뿐이다.

## async/await의 정체

await는 Promise와 마이크로태스크로 분해된다. await를 만나면 함수 실행을 멈추고 제어를 호출자에게 돌려준다. 기다리던 Promise가 resolve되면 그 다음 코드가 마이크로태스크 큐에 올라간다.

```js
async function f() {
  console.log('1');
  await Promise.resolve();
  console.log('2');
}
console.log('3');
f();
console.log('4');
// 3 → 1 → 4 → 2
```

f()를 부르면 '1'까지 동기로 실행되고 await에서 멈춰 제어가 빠져나온다. '4'가 찍힌 뒤 동기 코드가 끝나면 큐에 있던 '2'가 실행된다. 결국 await 뒤 코드는 `.then` 콜백과 같다고 보면 된다.

## 블로킹이 위험한 이유

이벤트 루프가 한 콜백을 실행하는 동안에는 다른 콜백을 꺼내지 못한다. 그래서 동기로 오래 걸리는 코드 하나가 서버 전체를 멈춘다.

```js
app.get('/bad', (req, res) => {
  let sum = 0;
  for (let i = 0; i < 1e10; i++) sum += i;
  res.json({ sum });
});
```

이 반복문이 도는 동안 다른 요청은 전부 대기한다. Node가 CPU 바운드 작업에 약하다는 말이 이 뜻이다. 무거운 연산은 worker_threads로 빼거나 외부로 넘긴다. 거대한 객체를 `JSON.parse`로 파싱하는 것, 동기 버전 crypto, 정규식 백트래킹(ReDoS)도 같은 함정이다.

## V8은 이벤트 루프가 없다

자주 헷갈리는 부분. V8은 JS 엔진일 뿐 이벤트 루프를 갖고 있지 않다.

- V8이 하는 일: 파싱, 컴파일, 실행(콜 스택), GC, 그리고 Promise의 마이크로태스크 큐
- V8이 안 하는 일: setTimeout, 파일/네트워크 I/O, 이벤트 루프

setTimeout은 JS 언어 기능이 아니라 런타임(호스트)이 끼워주는 함수다. 그래서 같은 V8을 써도 Node와 브라우저의 타이머·I/O API가 다르다. 비동기의 절반(마이크로태스크)은 엔진이, 나머지 절반(타이머·I/O·루프)은 런타임이 맡는 셈이다.

## V8 내부 (엔진 레벨)

엔진이 코드를 어떻게 빠르게 굴리는지도 알아두면 성능 감각이 생긴다.

- JIT 컴파일: 먼저 Ignition 인터프리터로 바이트코드를 실행하다가, 자주 호출되는 뜨거운 코드를 TurboFan으로 최적화 컴파일한다. 타입이 일관되면 최적화가 잘 되고, 도중에 타입이 바뀌면 최적화가 해제(deopt)된다.
- 히든 클래스: 객체 속성 접근을 빠르게 하려고 V8이 내부적으로 만드는 구조. 객체에 속성을 늘 같은 순서로 추가하면 같은 히든 클래스를 공유해 빠르다.
- 메모리: 힙은 young generation(짧게 사는 객체)과 old generation으로 나뉜다. GC도 둘로 갈려 young은 자주 가볍게(scavenge), old는 가끔 무겁게(mark-sweep-compact) 수거한다. GC가 도는 동안엔 잠깐 멈춤(stop-the-world)이 생긴다.

## 런타임 비교

개념 모델은 셋 다 같다. 싱글 스레드 JS, 이벤트 루프, 마이크로/매크로 큐. 다른 건 엔진과 I/O 구현이다.

| | JS 엔진 | 이벤트 루프 / 비동기 I/O | 구현 언어 |
|---|---|---|---|
| Node.js | V8 | libuv (C) | C/C++ |
| Deno | V8 | Tokio (Rust) | Rust |
| Bun | JavaScriptCore | 자체 이벤트 루프 | Rust (구 Zig, 2026.5 전환) |

- Node: V8 + libuv. epoll/kqueue/IOCP와 스레드 풀을 함께 쓴다.
- Deno: 엔진은 V8 그대로지만 I/O는 Rust의 Tokio. 웹 표준 API를 지향한다.
- Bun: 엔진이 V8이 아니라 JavaScriptCore다. 자체 이벤트 루프를 쓴다. 구현 언어는 Zig였다가 2026년 5월 Rust로 리라이트했다(Anthropic 인수 후 Zig의 AI 기여 제한이 계기). 엔진은 그대로고 바뀐 건 런타임 구현 언어다.

이벤트 루프 모델 자체가 셋 다 같은 이유는, JS 동시성 모델이 언어 스펙으로 정해져 있기 때문이다. 마이크로태스크 우선 규칙은 어디서나 동일하고, setTimeout과 setImmediate 같은 매크로태스크 세부 순서는 런타임마다 다를 수 있다.

## 연습

출력 순서를 먼저 적어보고 실행해서 확인한다.

```js
// Q1
console.log('A');
setTimeout(() => console.log('B'), 0);
Promise.resolve().then(() => console.log('C'));
console.log('D');
// A → D → C → B
```

```js
// Q2
console.log('A');
Promise.resolve().then(() => console.log('B'));
process.nextTick(() => console.log('C'));
setTimeout(() => console.log('D'), 0);
console.log('E');
// A → E → C → B → D   (nextTick > promise > timeout)
```

```js
// Q3
async function f() {
  console.log('1');
  await Promise.resolve();
  console.log('2');
}
console.log('3');
f();
console.log('4');
// 3 → 1 → 4 → 2
```

## 더 볼 것

- libuv 페이즈를 코드로 확인하기: I/O 콜백 안에서 setImmediate vs setTimeout 순서
- worker_threads와 cluster: 멀티코어 활용, 둘의 차이와 쓰임새
- 스트림과 백프레셔: 큰 데이터를 메모리에 다 안 올리고 처리하는 방법
- AsyncLocalStorage: 요청 컨텍스트를 비동기 호출 사이로 전달
- 메모리 누수 추적: heap snapshot, `--max-old-space-size`, GC 튜닝
- 이벤트 루프 lag 측정: 운영 환경에서 블로킹을 감지하는 관점
- 타이머 정밀도: `setTimeout(fn, 0)`이 실제로 0ms가 아닌 이유(최소 지연, 타이머 정리 시점)
