# 함수형 DDD (Domain Modeling Made Functional)

> Scott Wlaschin의 "Domain Modeling Made Functional"을 기반으로 정리.
> TypeScript + fp-ts 구현체 참조: [on-the-ground/domain-modeling-made-functional-typescript](https://github.com/on-the-ground/domain-modeling-made-functional-typescript)

---

## 왜 함수형 DDD인가?

전통적인 OOP DDD는 도메인을 **클래스 계층구조**로 모델링한다. Aggregate Root, Repository, Service 같은 패턴을 사용하며, 객체의 **가변 상태**를 메서드로 변경한다. 이 방식에는 근본적인 문제가 있다:

| 문제 | OOP DDD | 함수형 DDD |
|------|---------|-----------|
| 상태 관리 | 객체 내부 상태를 직접 변경 (mutation) | 불변 데이터 + 새 객체 생성 |
| 에러 처리 | 예외를 throw하고 catch | Either로 성공/실패를 명시적 반환 |
| 부수효과 | 메서드 안에서 DB 호출, 이메일 발송 | 순수 함수 + 의존성 주입 |
| 상속 복잡도 | 깊은 상속 계층, 다형성 | 합 타입 + 패턴 매칭 |
| 테스트 용이성 | Mock 객체, 복잡한 셋업 | 순수 함수, 입력→출력 검증 |
| 동시성 | 가변 상태로 인한 race condition | 불변 데이터라 공유 안전 |

함수형 DDD의 핵심 통찰:

> **도메인 모델링은 "객체와 행위"가 아니라 "타입과 함수"의 문제다.**

---

## 핵심 철학

### 1. 타입이 곧 문서다

코드와 별도로 작성하는 문서는 시간이 지나면 반드시 코드와 어긋난다. 함수형 DDD에서는 **타입 시스템 자체가 도메인 문서** 역할을 한다.

```typescript
// 이 타입 정의만 봐도 비즈니스 규칙이 보인다:
// - 주문에는 ID, 고객 정보, 배송지, 청구지, 주문 항목들이 있다
// - 주문 ID는 그냥 문자열이 아니라 OrderId라는 특별한 타입이다
// - 주문 항목은 최소 하나 이상이다 (readonly 배열)
class ValidatedOrder {
  constructor(
    readonly orderId: OrderId,
    readonly customerInfo: CustomerInfo,
    readonly shippingAddress: Address,
    readonly billingAddress: Address,
    readonly lines: readonly ValidatedOrderLine[],
  ) {}
}
```

도메인 전문가와 대화할 때 사용하는 **유비쿼터스 언어**(Ubiquitous Language)가 그대로 타입 이름이 된다. `OrderId`, `CustomerInfo`, `Address` 같은 이름은 코드를 모르는 사람도 읽을 수 있다.

---

### 2. 불법 상태를 표현 불가능하게 (Make Illegal States Unrepresentable)

이것이 함수형 DDD의 가장 강력한 원칙이다.

#### 문제: 원시 타입의 함정

```typescript
// 나쁜 예: 아무 문자열이나 들어갈 수 있다
function createOrder(orderId: string, email: string) {
  // orderId에 빈 문자열이? 1000자짜리 문자열이?
  // email에 "hello"가 들어오면?
}
```

`string`이라는 타입은 너무 넓다. 빈 문자열, 100만 자 문자열, SQL injection 코드까지 모두 `string`이다. 런타임에 검증 코드를 넣어도 까먹기 쉽고, 검증을 통과한 값인지 아닌지 타입만으로 구분할 수 없다.

#### 해결: Branded Type + Smart Constructor

```typescript
// String50: 1~50자 사이의 비어있지 않은 문자열
class String50 {
  private constructor(readonly value: string) {}  // 외부에서 new 불가

  static create(s: string): Either<Error, String50> {
    if (!s) return left(new Error('비어있음'))
    if (s.length > 50) return left(new Error('50자 초과'))
    return right(new String50(s))
  }
}

// EmailAddress: @ 기호가 포함된 문자열
class EmailAddress {
  private constructor(readonly value: string) {}

  static create(s: string): Either<Error, EmailAddress> {
    if (!s.match('.+@.+')) return left(new Error('이메일 형식이 아님'))
    return right(new EmailAddress(s))
  }
}
```

이제 `String50`을 받는 함수는 "이미 검증된 1~50자 문자열"이라는 것이 **타입 레벨에서 보장**된다. 런타임 검증은 `create()` 한 곳에서만 일어나고, 이후로는 타입 시스템이 유효성을 보장한다.

#### 합 타입으로 선택지 모델링

"제품 코드는 위젯 코드이거나 기즈모 코드이다" 같은 비즈니스 규칙:

```typescript
type ProductCode = WidgetCode | GizmoCode  // 이 둘 중 하나만 가능

// 패턴 매칭으로 모든 케이스를 빠짐없이 처리
const getPrice = (code: ProductCode) =>
  match(code)
    .with(P.instanceOf(WidgetCode), (w) => lookupWidgetPrice(w))
    .with(P.instanceOf(GizmoCode), (g) => lookupGizmoPrice(g))
    .exhaustive()  // 새로운 코드 타입이 추가되면 컴파일 에러!
```

`exhaustive()`가 핵심이다. 나중에 `SuperCode`라는 새 제품 코드를 추가하면, `exhaustive()` 덕분에 처리하지 않은 곳에서 **컴파일 에러**가 발생한다. 누락을 런타임이 아닌 컴파일 타임에 잡는다.

---

### 3. Railway Oriented Programming (철도 지향 프로그래밍)

에러 처리를 설명하는 비유다. 함수 실행을 **두 개의 철도 선로**로 생각한다:

```
성공 트랙 ─────→ 함수1 ────→ 함수2 ────→ 함수3 ────→ [결과]
                  ↓            ↓            ↓
실패 트랙 ←───── Error ←───── Error ←───── Error ←── [에러]
```

- 모든 함수는 **성공하면 성공 트랙**으로, **실패하면 실패 트랙**으로 보낸다
- 한번 실패 트랙에 빠지면 이후 함수들은 **건너뛴다** (자동 전파)
- 최종적으로 성공 또는 실패 **둘 중 하나**의 결과를 얻는다

이것을 구현하는 것이 `Either` 타입이다:

```typescript
type Either<Error, Success> = Left<Error> | Right<Success>
```

#### 실전 예: 고객 정보 검증

```typescript
const toCustomerInfo = (raw: UnvalidatedCustomerInfo) =>
  pipe(
    E.Do,                                                        // 파이프라인 시작
    E.bind('firstName', () => String50.create(raw.firstName)),   // 성공 시 계속, 실패 시 중단
    E.bind('lastName', () => String50.create(raw.lastName)),     // 성공 시 계속, 실패 시 중단
    E.bind('email', () => EmailAddress.create(raw.emailAddress)),// 성공 시 계속, 실패 시 중단
    E.map(({ firstName, lastName, email }) =>                    // 전부 성공하면 조립
      new CustomerInfo(new PersonalName(firstName, lastName), email),
    ),
  )
```

`firstName` 검증이 실패하면? → `lastName`과 `email` 검증은 **실행되지 않는다**. 자동으로 실패 트랙을 타고 최종 에러가 반환된다. `try-catch`가 필요 없다.

#### throw vs Either

```typescript
// OOP 스타일: 에러가 어디서 날지 함수 시그니처만 봐서는 알 수 없다
function createOrder(data: OrderData): Order {
  // 내부에서 throw할 수도 있고 안 할 수도 있고...
}

// 함수형 스타일: 실패 가능성이 타입에 명시된다
function createOrder(data: OrderData): Either<ValidationError, Order> {
  // 반환 타입만 봐도 "이 함수는 실패할 수 있다"는 걸 안다
}
```

`Either`를 사용하면 **함수 시그니처가 정직해진다**. 호출자는 반드시 성공과 실패 양쪽을 처리해야 하므로, 에러 처리를 까먹을 수 없다.

---

### 4. 워크플로우 = 파이프라인

비즈니스 프로세스를 **데이터 변환 파이프라인**으로 모델링한다.

#### PlaceOrder 워크플로우 예시

도메인 전문가와의 대화:
> "고객이 주문을 넣으면, 먼저 주문 정보를 **검증**하고, 그 다음 **가격을 산정**하고, 그 다음 **확인 이메일**을 보내고, 관련 **이벤트**들을 발행해요."

이것이 바로 파이프라인이다:

```
UnvalidatedOrder
    │
    ▼ [검증]
ValidatedOrder
    │
    ▼ [가격 산정]
PricedOrder
    │
    ▼ [확인 + 이벤트 생성]
PlaceOrderEvent[]
```

코드로:

```typescript
const placeOrder = (deps) => flow(
  validateOrder(deps.checkCode, deps.checkAddress),  // Step 1: 검증
  TE.flatMap(TE.fromEitherK(priceOrder(deps.getPrice))),  // Step 2: 가격
  TE.map(placeOrderEvents(deps.createAck, deps.sendAck)), // Step 3: 이벤트
)
```

각 단계의 핵심:

**Step 1 - 검증 (Validate)**
- 입력: `UnvalidatedOrder` (raw 문자열/숫자)
- 출력: `ValidatedOrder` (검증된 도메인 타입)
- raw 데이터를 도메인 타입으로 변환하면서 비즈니스 규칙을 검증

**Step 2 - 가격 산정 (Price)**
- 입력: `ValidatedOrder`
- 출력: `PricedOrder` (가격이 붙은 주문)
- 각 주문 항목에 가격을 매기고 총액을 계산

**Step 3 - 이벤트 생성**
- 입력: `PricedOrder`
- 출력: `PlaceOrderEvent[]` (발생한 이벤트 목록)
- 주문 완료, 청구, 이메일 확인 등의 이벤트를 생성

#### 핵심: 각 단계가 다른 타입을 갖는다

같은 `Order` 타입에 `validated: boolean`, `priced: boolean` 플래그를 넣는 게 아니라, **완전히 다른 타입**을 사용한다. 이렇게 하면:

- `ValidatedOrder`를 받는 함수에 `UnvalidatedOrder`를 넘기면 **컴파일 에러**
- 검증 단계를 건너뛸 수 없다 (타입이 다르므로)
- 각 단계에서 필요한 필드만 정확히 가진다

---

### 5. 부수효과의 격리

함수형 DDD에서 도메인 로직은 **순수 함수**다. DB 조회, API 호출, 이메일 발송 같은 부수효과는 어떻게 처리할까?

#### 의존성 주입 = 함수 파라미터

```typescript
// 의존성 타입 정의 (인터페이스 역할)
type CheckAddressExists = (addr: UnvalidatedAddress) => TaskEither<Error, CheckedAddress>
type GetProductPrice = (code: ProductCode) => Price
type SendOrderAcknowledgment = (ack: OrderAcknowledgement) => SendResult

// 워크플로우: 의존성을 파라미터로 받는다
const placeOrder = (
  checkAddress: CheckAddressExists,   // 주소 검증 서비스
  getPrice: GetProductPrice,          // 가격 조회 서비스
  sendAck: SendOrderAcknowledgment,   // 이메일 발송 서비스
): PlaceOrder => flow(/* 구현 */)
```

테스트할 때는 가짜 구현을 넣으면 된다:

```typescript
// 테스트용 가짜 의존성
const fakeCheckAddress = (addr) => TE.right(new CheckedAddress(addr))
const fakeGetPrice = (code) => new Price(10.0)
const fakeSendAck = (ack) => Sent

const workflow = placeOrder(fakeCheckAddress, fakeGetPrice, fakeSendAck)
const result = await workflow(testOrder)()
```

OOP에서 Mock 프레임워크가 필요한 것을, 함수형에서는 **그냥 함수를 넘기면** 된다.

#### 동기(sync) vs 비동기(async)

- 순수 계산: `Either<Error, Result>` → 동기, 즉시 결과
- I/O가 필요한 작업: `TaskEither<Error, Result>` → 비동기, Promise 기반

```typescript
// 동기: 가격 산정 (계산만 하므로)
type PriceOrder = (dep: GetProductPrice) =>
  (order: ValidatedOrder) => Either<PricingError, PricedOrder>

// 비동기: 주소 검증 (외부 API 호출)
type CheckAddressExists = (addr: UnvalidatedAddress) =>
  TaskEither<AddressValidationError, CheckedAddress>
```

파이프라인에서 동기→비동기 전환이 필요할 때 `TE.fromEither`를 사용한다.

---

### 6. 바운디드 컨텍스트와 이벤트

대규모 도메인은 하나의 모델로 표현할 수 없다. **바운디드 컨텍스트**로 분리한다.

```
┌──────────────┐     이벤트     ┌──────────────┐
│  주문 접수    │ ──────────→   │  배송         │
│  (Order-     │  OrderPlaced  │  (Shipping    │
│   Taking)    │               │   Context)    │
└──────────────┘               └──────────────┘
       │
       │ BillableOrderPlaced
       ▼
┌──────────────┐
│  청구         │
│  (Billing    │
│   Context)   │
└──────────────┘
```

각 컨텍스트는:
- **자신만의 유비쿼터스 언어**를 가진다 (같은 "주문"이라도 배송 컨텍스트에서는 다른 의미)
- **자신만의 타입**을 가진다 (배송의 Order ≠ 주문 접수의 Order)
- **이벤트**로만 소통한다 (직접 함수 호출 금지)

#### Command vs Event

```
Command (명령, 현재형)        Event (이벤트, 과거형)
───────────────────         ─────────────────────
"PlaceOrder"                "OrderPlaced"
"ShipProduct"               "ProductShipped"
"SendInvoice"               "InvoiceSent"

→ 하나의 핸들러만 처리        → 여러 구독자가 반응 가능
→ 실패할 수 있다              → 이미 일어난 사실
→ 워크플로우를 시작한다        → 워크플로우의 출력이다
```

#### 흐름

```
외부 이벤트/사용자 행동
    → Command 생성
    → Workflow 실행 (순수 함수 파이프라인)
    → Event[] 출력
    → 다른 컨텍스트로 전파
```

---

### 7. DTO: 외부 세계와의 경계

도메인 타입은 내부에서만 사용한다. 외부(HTTP, DB, 메시지 큐)와 통신할 때는 **DTO(Data Transfer Object)**를 사용한다.

```
외부 세계                경계               도메인
──────────             ──────             ──────
JSON string   →   DTO (raw 타입)   →   도메인 타입
              역직렬화              검증+변환

도메인 타입    →   DTO (raw 타입)   →   JSON string
              값 추출               직렬화
```

DTO는 도메인 타입의 "바보 버전"이다. `String50` 대신 `string`, `Price` 대신 `number`를 사용한다. 변환 시:

- **외부 → 도메인**: `Either`를 반환 (검증 실패 가능)
- **도메인 → 외부**: 항상 성공 (`.value`로 값 추출)

이 분리가 중요한 이유:
1. 도메인 타입을 바꿔도 외부 API에 영향 없음 (DTO가 완충)
2. 외부 데이터 형식이 바뀌어도 도메인은 안전 (DTO만 수정)
3. 직렬화 관심사가 도메인을 오염시키지 않음

---

## 전체 구현 흐름 정리 (PlaceOrder 워크플로우)

```
[1] 타입 정의
    ├── Simple Types: OrderId, String50, EmailAddress, Price, ...
    ├── Compound Types: PersonalName, CustomerInfo, Address, ...
    ├── State Types: UnvalidatedOrder → ValidatedOrder → PricedOrder
    ├── Event Types: OrderPlaced, BillableOrderPlaced, ...
    └── Error Types: ValidationError, PricingError, ...

[2] 워크플로우 시그니처
    PlaceOrder = UnvalidatedOrder → TaskEither<PlaceOrderError, PlaceOrderEvent[]>

[3] 스텝 구현
    ├── validateOrder: Unvalidated → TaskEither<Error, Validated>
    ├── priceOrder: Validated → Either<Error, Priced>
    └── createEvents: Priced → Event[]

[4] 파이프라인 합성
    flow(validateOrder, TE.flatMap(priceOrder), TE.map(createEvents))

[5] DTO + API
    ├── OrderFormDto.toUnvalidatedOrder()
    ├── placeOrder(deps)(unvalidatedOrder)
    └── PlaceOrderEventDto.fromDomain(events)
```

---

## 요약: 함수형 DDD의 사고방식

1. **먼저 타입을 설계하라** - 코드를 쓰기 전에, 도메인 전문가와 대화하며 타입을 정의한다
2. **타입으로 규칙을 인코딩하라** - "50자 이내 문자열", "1~1000 사이 정수" 같은 규칙을 타입으로 만든다
3. **상태 전이를 타입으로 표현하라** - 검증 전/후, 가격 산정 전/후를 다른 타입으로 구분한다
4. **순수 함수로 비즈니스 로직을 구현하라** - 부수효과 없이, 입력→출력 변환에 집중한다
5. **Either로 에러를 다루어라** - 예외를 던지지 말고, 성공/실패를 타입으로 표현한다
6. **작은 함수를 합성하라** - 파이프라인으로 복잡한 워크플로우를 구축한다
7. **경계에서만 I/O를 수행하라** - 도메인은 순수하게, 외부 통신은 가장자리에서

> "좋은 도메인 모델은 코드를 읽는 것만으로 비즈니스 규칙이 보이는 모델이다."
