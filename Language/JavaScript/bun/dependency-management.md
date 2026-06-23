# Bun 서버에서의 의존성 관리

NestJS는 IoC 컨테이너로 의존성을 자동 와이어링해준다. 그런데 [Bun.serve](./bun-server.md)로 가면 그 컨테이너가 없다. 그럼 의존성 관리를 어떻게 하느냐 — 이 노트는 그 질문에 대한 정리다.

## 먼저: "DI"와 "DI 컨테이너"는 다르다

이 둘을 섞으면 논의가 꼬인다.

- **DI(의존성 주입)** = 의존성을 외부에서 주입받는 *패턴*. 테스트성·낮은 결합의 핵심. → **버리면 안 된다**
- **DI 컨테이너** = 그 주입을 런타임에 자동 해소해주는 *도구*(NestJS IoC, awilix 등). → **버려도 된다**

Bun.serve로 가며 버리는 건 두 번째(컨테이너)지 첫 번째(DI 패턴)가 아니다. 그리고 컨테이너를 **직접 구현하는 건 거의 항상 나쁜 선택**이다. 생명주기·순환참조·스코프 처리를 재발명하다 버그난다. 필요하면 검증된 라이브러리를 쓴다.

## NestJS DI를 분해하면

컨테이너가 실제로 해주는 일을 쪼개보면, 무엇을 직접 챙겨야 하는지 보인다.

1. **인스턴스 수명 관리** — 대부분 싱글톤
2. **와이어링** — 누가 누구를 필요로 하는지 자동 해소
3. **생명주기 훅** — `onModuleInit`(DB 연결, 워커 시작), `onApplicationShutdown`(graceful shutdown)
4. **교체성** — 테스트에서 mock으로 교체
5. **request scope** — 요청마다 새 인스턴스 (실무에선 거의 안 씀)

컨테이너 없이 가면 1·2·3·4만 직접 챙기면 되고, 5는 대개 버려도 된다.

## 방법 1: Composition Root (수동 조립) — 기본값

핵심은 **의존성을 엮는 코드를 진입점 단 한 곳(composition root)에 모으는 것**이다. 각 서비스는 필요한 걸 생성자/인자로 받기만 하고, 누가 넣어줄지는 모른다(의존성 역전). NestJS가 자동으로 하던 걸 손으로, 명시적으로 한다.

```ts
// container.ts — 의존 그래프를 한 곳에서 조립
export function createContainer(env: Env) {
  const db       = createDb(env);                         // 싱글톤
  const solapi   = new SolapiClient(env.SOLAPI_KEY);
  const repo     = createRecipientRepo(db);
  const notifier = new NotificationService(repo, solapi); // 수동 주입
  const worker   = createNotificationWorker(notifier, env.REDIS_URL);

  return {
    notifier,
    worker,
    async shutdown() {        // onApplicationShutdown 대체
      await worker.close();
      await db.destroy();
    },
  };
}
```

```ts
// index.ts — 진입점
const container = createContainer(loadEnv());
Bun.serve({ port: 3000, fetch: createApp(container).fetch });

for (const sig of ["SIGTERM", "SIGINT"]) {     // graceful shutdown
  process.on(sig, async () => { await container.shutdown(); process.exit(0); });
}
```

교체성(테스트)은 자동으로 따라온다. 컨테이너를 거치지 않고 직접 fake를 넣으면 된다.

```ts
const service = new NotificationService(fakeRepo, fakeSolapi);
```

추상화가 필요하면 NestJS injection token 대신 **그냥 TS `interface`** 를 쓴다. `RecipientRepo`를 interface로 두고 구현체를 인자로 갈아끼우는 것 — 이게 의존성 역전이다.

## "많아지면 더러워지지 않나?" — 그렇다, 그게 핵심이다

단일 composition root에 의존성 100개를 평면으로 `new` 하면 당연히 지옥이 된다. 그리고 **그게 바로 NestJS DI가 존재하는 이유**다. 다만 그 전에 두 단계 완충이 있다.

### 완충 1: 모듈별 factory로 쪼개기 (트리 구조)

평면 100줄이 아니라, **모듈 경계로 factory를 나누고 root에선 합성만** 한다. 사실 이게 NestJS `@Module`이 하는 일을 손으로 한 것이다.

```ts
// notification/module.ts — 이 도메인 의존성은 여기서만 안다
export function createNotificationModule(deps: { db: Db; env: Env }) {
  const repo   = createRecipientRepo(deps.db);
  const solapi = new SolapiClient(deps.env.SOLAPI_KEY);
  return { notifier: new NotificationService(repo, solapi) };
}

// container.ts — root는 모듈 합성 몇 줄만 남는다
export function createContainer(env: Env) {
  const db = createDb(env);
  const notification = createNotificationModule({ db, env });
  const history      = createHistoryModule({ env });
  return { ...notification, ...history, shutdown: () => db.destroy() };
}
```

root가 `new` 50개가 아니라 factory 호출 5~6개로 유지된다. 더러워지는 원인은 "수동이라서"가 아니라 "안 쪼개서"인 경우가 많다.

### 완충 2: 그래도 크면 awilix (자동 와이어링)

모듈 합성으로도 안 되면 awilix가 **NestJS급 자동 와이어링**을 데코레이터 없이 해준다. 생성자 파라미터 이름으로 의존성을 자동 해소하므로, 늘어도 "등록"만 추가하면 된다.

```ts
container.register({
  db:       asFunction(createDb).singleton(),
  repo:     asFunction(createRecipientRepo).singleton(),
  solapi:   asClass(SolapiClient).singleton(),
  notifier: asClass(NotificationService).singleton(),
  // NotificationService(repo, solapi) 를 이름 매칭으로 자동 주입
});
```

| 라이브러리 | 방식 | 데코레이터 | Bun 적합도 |
|---|---|---|---|
| **awilix** | 함수/클래스 등록, 이름 기반 해소 | 불필요 | ✅ 권장 |
| tsyringe | `@injectable`/`@inject` | 필요 | ⚠️ Bun 데코레이터 메타데이터 이슈 |
| InversifyJS | 데코레이터 + 바인딩 | 필요 | ⚠️ 동일 |

tsyringe·InversifyJS는 NestJS와 똑같이 `emitDecoratorMetadata`에 의존한다. 이건 Bun의 데코레이터 메타데이터 이슈(`tsconfig extends` 시 `design:type` 유실 등)를 그대로 떠안는다는 뜻이다. 데코레이터 컨테이너를 쓸 거면 애초에 NestJS를 유지하는 게 낫다. **awilix는 데코레이터 없이 동작**해서 Bun으로 넘어가는 이점을 살린다.

## 함수형 백엔드라면: 컨테이너를 버려라

함수형으로 간다면 컨테이너는 버리는 게 **맞고, 일관성 측면에서도 옳다.** 함수형의 DI는 **부분 적용(클로저)** 으로 표현된다. 컨테이너가 풀어주던 "누가 누구를 필요로 하는가"를 함수 합성 순서가 그대로 해결한다.

```ts
// 의존성을 받아 핸들러를 반환하는 팩토리 (부분 적용)
const makeSendNotification =
  (deps: { repo: RecipientRepo; solapi: Solapi }) =>
  async (job: Job) => {
    const r = await deps.repo.findById(job.userId);
    return deps.solapi.send(r, job.template);
  };

// composition root = 그냥 함수를 호출해 엮을 뿐. 컨테이너 없음
const env = loadEnv();
const db  = createDb(env);
const sendNotification = makeSendNotification({
  repo:   makeRecipientRepo(db),
  solapi: makeSolapi(env.SOLAPI_KEY),
});
```

테스트도 컨테이너를 거치지 않는다: `makeSendNotification({ repo: fakeRepo, solapi: fakeSolapi })`.

### 왜 함수형엔 컨테이너가 안 맞나

컨테이너는 본래 **OOP 클래스 그래프**의 문제(인스턴스 수명, 순환참조, 자동 와이어링)를 풀려고 나온 도구다. 함수형은 그 문제가 약하다 — 함수는 인스턴스화 시점이 명확하고, 부분 적용으로 묶으면 그래프가 값으로 고정된다. 게다가 철학이 충돌한다. 함수형은 **명시적 데이터 흐름**을 지향하는데, 컨테이너는 **암묵적 자동 해소**를 한다. 함수형으로 가놓고 자동 컨테이너를 얹으면 둘 다 놓친다.

### 함수형에서도 버리면 안 되는 것

1. **Composition Root는 유지** — 컨테이너(자동 해소기)는 없어도, 의존성을 한 곳에서 조립하는 지점은 패러다임 무관하게 필요하다.
2. **전역 import로 도망가지 말 것** — 의존성을 모듈 레벨 싱글톤으로 `import`해 쓰면 그건 DI를 버린 것(숨은 결합)이다. 여전히 인자로 주입해야 테스트·교체가 된다.

> 참고: TS 순수 FP엔 Reader 모나드로 의존성을 흘리는 방식도 있지만, 러닝커브 대비 실익이 낮아 실무 채택은 낮다. **부분 적용 + composition root**가 현실적인 답이다.

## 모듈 캡슐화/추상화로 그래프 단순화

"모듈별 캡슐화·추상화를 잘하면 그래프가 단순해지지 않나?" — 맞다. 이게 컨테이너 없이 큰 시스템을 감당하는 핵심 기법이다. 단 "단순화"의 의미를 정확히 봐야 한다.

### 캡슐화는 "줄이는" 게 아니라 "숨긴다"

캡슐화는 전체 노드 수를 줄이지 않는다. **각 레벨에서 보이는 노드를 줄인다(은닉).** 모듈이 외부로 노출하는 표면(public surface)을 좁히면 — 모듈 *간* 상위 그래프는 진짜 단순해지고, 내부 그래프는 그 안에 봉인된다.

```
[캡슐화 전] root가 전부 안다
  db, redis, repo1, repo2, solapi, validator, mapper, worker, ...  (N개 평면)

[캡슐화 후] root는 모듈 경계만 안다
  root → notificationModule → (내부 7개 캡슐화)
       → historyModule      → (내부 5개 캡슐화)
```

root의 복잡도가 **전체 의존성 수가 아니라 모듈 수에 비례**하게 된다. 이게 위 "완충 1: 모듈 factory"가 작동하는 원리다.

### 추상화는 실제로 엣지를 줄인다

캡슐화가 *은닉*이라면, 추상화(interface)는 의존성 역전으로 **실제 결합 개수를 줄인다.** 구체 구현끼리 직접 엮으면 N×M으로 폭발하지만, 인터페이스를 경유시키면 N+M으로 떨어지고 양쪽이 서로를 모르게 된다.

```
[직접 결합]  서비스 3 × 구현 3 → 최악 9 엣지
[인터페이스] 서비스 3 → interface ← 구현 3 → 6 엣지, 상호 무지
```

즉 캡슐화는 *보이는 범위*를, 추상화는 *결합의 개수*를 줄인다. 역할이 다르고, 같이 쓸 때 효과가 크다.

### "잘"이 전부다 — 조건과 함정

이 효과는 조건부다. 경계를 잘못 그으면 오히려 더 더러워진다.

| 지켜야 할 것 | 어기면 |
|---|---|
| 높은 응집·낮은 결합 (경계가 도메인을 따름) | 기능 하나가 여러 모듈에 흩어져 cross-module 엣지 폭증 |
| 단방향 의존 (순환 금지) | A↔B 순환 → composition root 조립 순서가 꼬임 |
| 안정 의존성 (변하기 쉬운 게 안정적인 것에 의존) | 안정 모듈이 자주 바뀌는 모듈에 묶여 연쇄 변경 |
| 적정 추상화 (필요한 곳만 interface) | 모든 것에 interface → 간접성 지옥, 추적 불가 |

특히 과한 추상화가 함정이다. 모든 것에 interface를 두면 "이 인터페이스의 실제 구현이 뭐지?"를 매번 추적해야 해서 복잡도가 늘어난다. 모듈 경계가 내부 구현을 흘리면(leaky abstraction) 캡슐화 효과 자체가 무효가 된다.

요점:
1. 전체 복잡도가 **사라지는 게 아니라 격리되는 것** — 내부 복잡도는 모듈 안에 여전히 있되 관리 가능한 단위가 된다.
2. "잘"의 조건(응집·단방향·안정의존·적정 추상화)을 어기면 **역효과** — 경계 설계가 8할이다.

## 정리: 규모가 도구를 결정한다

| 상황 | 권장 |
|---|---|
| 의존성 작음~중간, 큰 증가 없음 | **Composition Root (수동)** + 모듈 factory |
| 계속 커지고 와이어링이 부담 | **awilix** (데코레이터 없는 자동 컨테이너) |
| 함수형 백엔드 | 컨테이너 버리고 **부분 적용 + composition root** |
| 도메인 복잡 + 의존성 폭증 + 팀이 NestJS 익숙 | **그냥 NestJS 유지가 정답** |

결국 **"내가 모듈 경계를 얼마나 잘 그을 수 있나"가 도구 선택의 기준**이다. 경계 설계에 자신 있는 규모면 컨테이너 없이(혹은 함수형으로) 충분히 스케일된다. 반대로 경계가 계속 흔들리고 cross-cutting이 많은 도메인이라면, 모듈 시스템과 DI를 프레임워크가 강제해주는 NestJS가 그 규율을 대신 잡아주는 값을 한다.

## 더 볼 것

- Composition Root 패턴 (Mark Seemann, *Dependency Injection* 원전)
- awilix: `asFunction`/`asClass`, PROXY vs CLASSIC injection mode, scoped 컨테이너
- 함수형 DI: 부분 적용 vs Reader 모나드 (fp-ts), 어디까지가 실용적인가
- 안정 의존성 원칙(SDP)·안정 추상화 원칙(SAP): 모듈 경계를 정량적으로 보는 관점
- Hono `c.set`/`c.get`, Elysia `.decorate`/`.derive`: fetch 프레임워크 내장 DI
