# 이벤트 루프 블로킹과 가짜 stalled (2편)

> [1편](./01_message-delivery-reliability.md)에서 "stalled job이 왜 위험한가"를 분산 관점으로 봤다면,
> 이번 편은 한 층 내려가 **"그 stalled가 사실은 분산 문제가 아니라 단일 프로세스 안의 시간 문제였다"**를 파헤친다.
> 무대: BullMQ 워커 하나가 CPU 무거운 핸들러를 도는 중.

## 핵심 결론 (TL;DR)

1. BullMQ 워커는 **자기 자신이** 주기적으로 Redis lock의 TTL을 갱신한다(하트비트). 별도 감시자가 아니다.
2. Node(JS)는 **선점형이 아니라 협조형**이다. 동기 블록이 이벤트 루프를 안 놓으면, 하트비트 타이머는 *죽지 않았는데도* 실행되지 못한다.
3. Redis는 "신호 없음"을 "워커 죽음"으로 **번역**하고 job을 stalled 처리한다 → 재처리 → **중복**. 워커는 멀쩡했으므로 이건 **가짜(false positive)**.
4. 이건 1편 §6 Two Generals의 **"침묵 ≠ 죽음, 구분 불가"**가 *단일 프로세스 안에서* 재현된 것이다. 불확실성의 원천이 네트워크가 아니라 **이벤트 루프의 시간**일 뿐.
5. 가짜 stalled를 0으로 막는 건 불가능하다(GC 멈춤·failover 등). 최후의 방어선은 막기가 아니라 **핸들러 멱등성(흡수)** — 1편과 같은 결론.

---

## 출발 장면

```ts
async function handler(job) {
  const users = await db.query('SELECT * FROM users'); // 50만 행
  const result = users
    .map(transformHeavy)      // 행마다 무거운 동기 변환
    .filter(complexPredicate)
    .reduce(aggregate, {});   // 전부 동기, await 없음
  await sendReport(result);
}
```

데이터가 커지자 이 job이 "stalled"로 찍히고 다른 워커가 같은 job을 다시 집어 **리포트가 두 번 발송**됐다.
CPU도 안 죽었고, 프로세스도 안 죽었고, Redis도 멀쩡. 워커는 하나뿐이라 노드 간 통신 실패도 아니다.

---

## 1. lock 하트비트 — 살아있다는 신호

BullMQ는 job을 집을 때 Redis에 lock 키(`bull:queue:jobId:lock`, 값=워커 토큰)를 만들고 **TTL**을 건다(기본 `lockDuration = 30s`).
워커 프로세스 안의 타이머가 `lockRenewTime`(기본 = `lockDuration / 2` = 15s)마다 그 키의 TTL을 `PEXPIRE`로 다시 밀어준다. 이게 "나 아직 처리 중" 신호다.

**핵심: 갱신을 보내는 주체는 그 job을 처리 중인 바로 그 워커 자신**이다. 별도 감시 프로세스가 아니다. → §2의 함정으로 직결.

---

## 2. 협조형 스케줄링 — 오늘의 핵심

`map().filter().reduce()`는 중간에 `await`가 하나도 없는 **하나의 거대한 동기 블록**이다. 그게 도는 내내 **call stack이 안 비운다.**

Node에서 타이머 콜백(=lock 갱신)은 **call stack이 비어야** 실행된다. 그래서 15초가 지나 "갱신할 시간"이라며 타이머 콜백이 큐에 올라와도 — **차례가 오지 않는다.**

| | 선점형 (OS 스레드) | 협조형 (Node/JS) |
|---|---|---|
| 누가 끊나 | OS가 강제로 시분할 | **아무도 안 끊음** |
| 양보 | 자동 | **내가 자발적으로 손을 떼야만** |
| 동기 무한루프 | 다른 일도 돎 | **모든 게 멈춤** |

> ⚠️ **라운드로빈 오해**: 동기 블록이 도는 동안 타이머가 끼어드는 일은 **절대 없다.** Node는 끊어주지 않는다.
> ⚠️ **CPU 개수 오해**: 코어가 1개든 16개든 상관없다. JS 실행 스레드는 코어 수와 무관하게 **하나**다. 동기 블록은 그 하나를 잡고 안 놓는다.

핵심 아이러니: 워커는 죽어서 신호를 못 보내는 게 아니라, **너무 바빠서** 못 보낸다. 살아 있는데 말할 입이 막힌 상태.

---

## 3. 가짜 stalled — Two Generals의 재현

30초 뒤 lock 키가 만료돼 사라진다. stalled 체커가 "lock은 없는데 active인 job"을 발견하면 판정한다:

> "lock 없음 → 처리하던 워커가 죽음 → stalled → 회수해서 재실행"

(`maxStalledCount`, 기본 1을 넘으면 stalled가 아니라 failed 처리.)

**왜 가짜인가**: 워커는 안 죽었다. Redis가 본 건 "신호 없음"이고 그걸 "죽음"으로 번역한 건데, 그 번역이 틀렸다. **"신호 없음 ≠ 죽음"인데 둘을 구분할 방법이 없다.**

→ 1편 §6의 **"(가) 진짜 실패 / (나) 응답만 유실 — 구분 불가"**와 정확히 같은 모양. 분산에서 본 "침묵을 죽음으로 오해" 문제가, **단일 프로세스 안에서 이벤트 루프 때문에** 재현된 것.

---

## 4. 처방 — 4갈래, 사실은 2종류

| # | 처방 | 무엇을 함 | 한계 |
|---|---|---|---|
| 1 | `lockDuration` ↑ | 판정 시계를 늦춤 | 진짜 장애 탐지 지연 (아래 trade-off) — 증상 가림 |
| 2 | 배치 + `setImmediate` 양보 | 동기 블록 쪼개 틈 만듦 | 쪼갤 수 있을 때만 (§5) |
| 3 | `worker_threads` 오프로드 | CPU를 메인 밖으로 | 직렬화·복잡도 비용 (§6) |
| 4 | **핸들러 멱등 (1편 §5)** | 막지 않고 **흡수** | — |

1·2·3 = "중복이 **일어나지 않게** 막기"(확률 게임). 4 = "중복이 **일어나도 결과가 안 변하게**"(무해화).

### `lockDuration`을 늘리는 대가 (trade-off)

> 길게 잡으면 → **가짜** 죽음 오판은 줄지만, **진짜** 죽음(크래시) 탐지가 그만큼 느려진다.

워커가 `kill -9`/OOM으로 진짜 죽으면, `lockDuration`이 5분일 때 그 job은 **5분간 lock을 쥔 채 active에 박제**된다. 죽은 워커의 lock이 만료될 때까지 아무도 회수 못 함. 즉 **민감도 다이얼**: 짧게=빨리 회수하나 가짜 양산 / 길게=가짜 줄지만 진짜 장애에 굼뜸. "정답 값"은 없고 비용 판단(1편 §7).

---

## 5. 양보(yield) — 단, 아무 await나 되는 게 아니다

```ts
// 전: 50만 행을 한 호흡에 — 30초간 손 안 뗌
const result = users.map(transformHeavy).filter(complexPredicate).reduce(aggregate, {});

// 후: 배치로 쪼개고, 배치마다 이벤트 루프에 양보
let acc = {};
for (let i = 0; i < users.length; i += 1000) {
  const batch = users.slice(i, i + 1000);
  acc = batch.map(transformHeavy).filter(complexPredicate).reduce(aggregate, acc);
  await setImmediate();   // ★ 손을 뗀다 → call stack 비움 → 대기 중이던 하트비트 타이머 실행
}
// import { setImmediate } from 'node:timers/promises'; (Node 16+, 가독성 ↑)
```

### ⚠️ 함정: 마이크로태스크는 양보가 아니다

Node 큐 우선순위: **마이크로태스크(Promise 콜백) 전부 비움 → 그제서야 매크로태스크(타이머·setImmediate·I/O)**.

```ts
await Promise.resolve();   // ❌ 마이크로태스크 — call stack 잠깐 비워도 다시 마이크로태스크로 복귀
                           //    타이머(lock 갱신)엔 차례가 안 감 → 하트비트 여전히 굶음
await setImmediate();      // ✅ 매크로태스크 — "마이크로태스크 다 비우고 → 타이머 차례"로 넘어감
```

→ **그래서 굳이 `setImmediate`다.** 아무거나 await하면 되는 게 아니다.

> 양보 패턴은 *어쩔 수 없이 메인에서 돌리는 중간 규모 작업*의 band-aid다. 정말 무거우면 §6이 정답. 매 배치마다 루프 한 바퀴 = 오버헤드라 배치 크기 튜닝 필요.

---

## 6. worker_threads — 메인 밖으로 내보내기

쪼갤 수 없는 단일 무거운 연산(거대 이미지 변환·암호·압축)은 §5로 안 풀린다. 양보할 틈 자체가 안 생기므로 **메인 스레드 밖**으로 보낸다.

> **worker_threads** = 메인과 별개의 진짜 OS 스레드를, 각자 독립된 V8 + 독립된 이벤트 루프를 갖고 띄우는 것.

| | 격리 단위 | 메모리 | 용도 |
|---|---|---|---|
| libuv 스레드풀 | (내부 C++ 스레드) | — | `fs`·`crypto`·`zlib` 비동기 I/O를 Node가 알아서 |
| **worker_threads** | 스레드 | **기본 비공유** (메시지 전달) | **CPU 바운드 JS** 병렬 |
| child_process / cluster | 프로세스 | 완전 분리 | 더 강한 격리, 멀티코어 서버 |

성질: ① 메모리 비공유 기본 — `postMessage`로 주고받고 **구조화 복제(structured clone)로 복사**(큰 데이터 잦으면 비용; 진짜 공유는 `SharedArrayBuffer`). ② 각 워커가 자기 이벤트 루프 → 워커가 CPU 100% 써도 **메인 루프는 멀쩡** → 하트비트 정상. ③ 시작 비용·메모리 있어 **풀로 재사용**.

### Promise로 감싼 실전형

```ts
// run-worker.ts
import { Worker } from 'node:worker_threads';

export function runWorker<TInput, TOutput>(workerFile: URL, input: TInput): Promise<TOutput> {
  return new Promise((resolve, reject) => {
    const worker = new Worker(workerFile, { workerData: input });
    worker.once('message', (value: TOutput) => resolve(value)); // once: 1회만 확정, 누수 방지
    worker.once('error', reject);                                // 워커 안에서 throw
    worker.once('exit', (code) => {                              // 0 아니면 비정상
      if (code !== 0) reject(new Error(`Worker stopped with exit code ${code}`));
    });
  });
}
```

### BullMQ 핸들러에 꽂기 — 가짜 stalled 소멸

```ts
worker.process('report', async (job) => {
  const users = await db.query('SELECT * FROM users');        // I/O — 메인 OK
  const report = await runWorker<User[], Report>(             // ★ CPU를 별도 스레드로
    new URL('./report-worker.ts', import.meta.url), users);   //   이 동안 메인 루프 자유 → 하트비트 정상
  await sendReport(report, { idempotencyKey: `report:${job.id}` }); // 1편 §5
});
```

`await runWorker(...)` 동안 메인은 **논블로킹 대기 상태** → lock 갱신·다른 job I/O 다 정상. §5 양보 패턴이 필요 없어진다(애초에 메인에서 안 돌리니까).

### 실무 현실 — 풀(piscina)

`new Worker`는 새 V8 isolate를 띄우는 일(시작 비용·메모리). job마다 만들면 손해 → 미리 N개 만들어 재사용.

```ts
import Piscina from 'piscina';
const pool = new Piscina({ filename: new URL('./report-worker.ts', import.meta.url).href }); // 앱 시작 1회

worker.process('report', async (job) => {
  const users = await db.query('SELECT * FROM users');
  const report = await pool.run(users);                      // 풀에서 재사용, await로 결과
  await sendReport(report, { idempotencyKey: `report:${job.id}` });
});
```
```ts
// report-worker.ts (piscina: export default 함수)
export default function buildReport(users: User[]): Report {
  return users.map(transformHeavy).filter(complexPredicate).reduce(aggregate, {});
}
```

여기서 비로소 **CPU 코어 여러 개**가 의미를 가진다 — 풀의 워커들이 서로 다른 코어에서 진짜 병렬로.

---

## 7. 가짜 stalled를 0으로? — 불가능. 그래서 멱등이 최후의 방어선

코드를 완벽히 쪼개도(§5) 메인을 못 비우는 경우가 또 있다 — 내 코드 밖의 일들:

- **GC 멈춤**: V8 major GC는 **stop-the-world**. 힙이 크면 그 멈춤이 길어져 하트비트를 놓침. 런타임이 루프를 막는 것.
- **Redis failover / 네트워크 순단**: lock 갱신 명령이 잠깐 안 닿으면 lock 만료. 워커는 멀쩡한데도.
- **OS 스케줄링 지연**(컨테이너 CPU throttling 등).

→ **가짜 stalled를 0으로 만드는 건 원리적으로 불가능.** 1·2·3은 *확률을 낮출* 뿐. (1편에서 exactly-once delivery가 불가능했던 것과 같은 구조 — 이번엔 네트워크가 아니라 **단일 프로세스의 시간**이 불확실성의 원천.)

**그래서 마지막 안전망은 §4의 핸들러 멱등성.**

> 1·2·3은 "중복이 **일어나지 않게**" 막는다 — 확률 게임이라 언젠가 샌다.
> 4는 "중복이 **일어나도 결과가 안 변하게**" 만든다 — 앞선 모두가 실패해도 리포트는 한 번만.
> 막는 계층은 뚫릴 수 있지만, **흡수하는 계층은 뚫려도 무해**하다. 그래서 방어선의 *마지막*에 둔다.

### 최종 — 1편과 닫히는 한 고리

> **"중복이 안 생긴다"는 못 만든다. 하지만 "중복이 생겨도 사용자에겐 한 번"은 만들 수 있다.**
> *exactly-once 처리(prevention)*는 불가능, **at-least-once + 멱등 처리 = exactly-once 효과(effect)**만 달성 가능.

이게 1편 §2의 *"막는 게 아니라 흡수한다"*와 같은 문장이다. **분산(네트워크)에서든 런타임(이벤트 루프)에서든, 도달 가능한 목표는 언제나 "효과상 한 번"이지 "진짜 한 번"이 아니다.**

---

_출처: 분산/신뢰성 자가학습 세션 2편 (2026-06). [1편: 메시지 전달 신뢰성](./01_message-delivery-reliability.md) · [3편: 백프레셔와 워크로드 격리](./03_backpressure-and-workload-isolation.md). 다음 편(후보): 관측성(observability)._
