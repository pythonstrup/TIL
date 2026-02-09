# Functional DDD - TypeScript Agent Context

> AI가 함수형 DDD 패턴을 TypeScript로 구현할 때 참조하는 컨텍스트 문서.
> 기반: Scott Wlaschin의 "Domain Modeling Made Functional" + fp-ts TypeScript 구현체

## 핵심 원칙

1. **타입이 곧 문서다** - 도메인 지식을 타입 시스템에 인코딩한다. 별도 문서가 아닌 컴파일러가 강제하는 타입이 유일한 진실의 원천이다.
2. **불법 상태를 표현 불가능하게 만들어라** - 비즈니스 규칙을 타입 제약으로 인코딩하여, 잘못된 데이터를 컴파일 타임에 차단한다.
3. **워크플로우는 파이프라인이다** - 비즈니스 프로세스를 작은 순수 함수들의 합성으로 구현한다.
4. **부수효과를 경계로 밀어내라** - 도메인 로직은 순수하게, I/O는 함수 파라미터로 주입한다.
5. **도메인 이벤트로 소통하라** - 워크플로우의 출력은 이벤트 목록이다. 직접 부수효과를 일으키지 않는다.

---

## 기술 스택

| 라이브러리 | 용도 |
|-----------|------|
| `fp-ts` | Either, Option, TaskEither, pipe, flow |
| `ts-pattern` | 패턴 매칭 (match/with/exhaustive) |
| TypeScript `strict` mode | strictNullChecks, strictPropertyInitialization |

### fp-ts 핵심 임포트

```typescript
import { pipe, flow } from 'fp-ts/function'
import * as E from 'fp-ts/Either'
import * as TE from 'fp-ts/TaskEither'
import * as O from 'fp-ts/Option'
import * as A from 'fp-ts/Array'
import * as NA from 'fp-ts/NonEmptyArray'
import { match, P } from 'ts-pattern'
```

---

## 프로젝트 구조

```
src/
├── libs/                              # 인프라 유틸리티
│   ├── brand.ts                       # Branded type 정의
│   ├── model-type.ts                  # ValueObject, Entity 베이스 클래스
│   ├── type.ts                        # 타입 유틸리티
│   └── decorator.ts                   # @bound 데코레이터
├── {bounded-context}/                 # 바운디드 컨텍스트 단위
│   ├── common-types/                  # 공유 도메인 타입
│   │   ├── constrained-type.ts        # 검증 헬퍼 함수
│   │   ├── simple-types.ts            # 원시 래퍼 타입 (String50, Price 등)
│   │   ├── compound-types.ts          # 복합 타입 (PersonalName, Address 등)
│   │   └── index.ts                   # 재수출
│   └── {workflow-name}/               # 워크플로우 단위
│       ├── public-types.ts            # 외부 공개 타입 (워크플로우 시그니처, 이벤트, 에러)
│       ├── implementation.types.ts    # 내부 타입 (중간 상태, 스텝 함수 시그니처)
│       ├── implementation.ts          # 워크플로우 구현 (Either/TaskEither 사용)
│       ├── implementation.common.ts   # 공통 로직 (이벤트 생성 등)
│       ├── dto.ts                     # DTO 직렬화/역직렬화
│       └── api.ts                     # HTTP API 경계
```

### 파일 명명 규칙

- 디렉토리: `kebab-case` (order-taking, place-order)
- 파일: `kebab-case.ts` (simple-types.ts, compound-types.ts)
- 구현 변형: `implementation.{variant}.ts`

---

## 패턴 카탈로그

### 1. Branded Types + Smart Constructor

모든 도메인 원시값은 브랜드 타입으로 감싼다. 생(raw) 원시값을 도메인 로직에서 직접 사용하지 않는다.

#### 기본 구조

```typescript
import { Wrapper } from '@libs/brand'
import { ValueObject } from '@libs/model-type'
import * as ConstrainedType from './constrained-type'
import * as E from 'fp-ts/Either'

// 1. 고유 심볼 선언
declare const string50: unique symbol

// 2. 브랜드 타입 클래스
export class String50 extends ValueObject implements Wrapper<string, typeof string50> {
  [string50]!: never
  constructor(readonly value: string) {
    super()
  }

  // 3. Smart Constructor - 유일한 생성 경로
  static create: (s: string) => E.Either<ConstrainedType.ErrPrimitiveConstraints, String50> =
    ConstrainedType.createString(String50, 50)
}
```

#### 문자열 패턴 타입

```typescript
declare const emailAddress: unique symbol

export class EmailAddress extends ValueObject implements Wrapper<string, typeof emailAddress> {
  [emailAddress]!: never
  constructor(readonly value: string) {
    super()
  }

  static create: (s: string) => E.Either<ConstrainedType.ErrPrimitiveConstraints, EmailAddress> =
    ConstrainedType.createLike(EmailAddress, '.+@.+')
}
```

#### 숫자 범위 타입

```typescript
declare const unitQuantity: unique symbol

export class UnitQuantity extends ValueObject implements Wrapper<number, typeof unitQuantity> {
  [unitQuantity]!: never
  constructor(readonly value: number) {
    super()
  }

  static create: (i: number) => E.Either<ConstrainedType.ErrPrimitiveConstraints, UnitQuantity> =
    ConstrainedType.createNumber(UnitQuantity, 1, 1000)
}
```

#### 비즈니스 메서드가 있는 타입

```typescript
declare const price: unique symbol

export class Price extends ValueObject implements Wrapper<number, typeof price> {
  [price]!: never
  constructor(readonly value: number) {
    super()
  }

  static create = ConstrainedType.createNumber(Price, 0, 1000)

  // 곱셈 결과도 Either로 반환 (범위 초과 가능)
  multiply(qty: number): E.Either<ConstrainedType.ErrPrimitiveConstraints, Price> {
    return Price.create(qty * this.value)
  }
}
```

#### ConstrainedType 헬퍼

```typescript
// 문자열: 최대 길이 제약
export const createString =
  <T>(ctor: { new (i: string): T }, maxLen: number) =>
  (str: string): E.Either<ErrPrimitiveConstraints, T> => {
    if (!str) return E.left(ErrEmptyString)
    if (maxLen < str.length) return E.left(new ErrStringTooLong(maxLen))
    return E.right(new ctor(str))
  }

// 숫자: 범위 제약
export const createNumber =
  <T>(ctor: { new (i: number): T }, min: number, max: number) =>
  (num: number): E.Either<ErrPrimitiveConstraints, T> => {
    if (num < min) return E.left(new ErrNumberLessThanMin(min))
    if (max < num) return E.left(new ErrNumberGreaterThanMax(max))
    return E.right(new ctor(num))
  }

// 문자열: 정규식 패턴 제약
export const createLike = <T>(ctor: { new (i: string): T }, regex: string) =>
  flow(
    isAlike(regex),
    E.map((i) => new ctor(i)),
  )
```

**규칙:**
- 모든 도메인 원시값은 반드시 Smart Constructor를 통해서만 생성한다
- `new` 직접 호출은 내부 구현에서만 허용한다
- Smart Constructor는 항상 `Either<Error, Type>`을 반환한다

---

### 2. Sum Types (합 타입 / Discriminated Union)

선택지가 있는 도메인 개념은 합 타입으로 모델링한다.

```typescript
// 제품 코드: Widget 또는 Gizmo
export type ProductCode = WidgetCode | GizmoCode

// 주문 수량: 단위 수량 또는 킬로그램 수량
export type OrderQuantity = UnitQuantity | KilogramQuantity

// 이벤트: 세 가지 중 하나
export type PlaceOrderEvent =
  | OrderPlaced
  | BillableOrderPlaced
  | OrderAcknowledgmentSent

// 에러: 세 가지 중 하나
export type PlaceOrderError =
  | ValidationError
  | PricingError
  | RemoteServiceError
```

#### 패턴 매칭 (ts-pattern)

```typescript
import { match, P } from 'ts-pattern'

// ProductCode에 따라 다른 수량 타입 생성
export const createOrderQuantity = (productCode: ProductCode) =>
  match(productCode)
    .with(P.instanceOf(WidgetCode), () => UnitQuantity.create)
    .with(P.instanceOf(GizmoCode), () => KilogramQuantity.create)
    .exhaustive()
```

**규칙:**
- `exhaustive()`를 반드시 사용하여 모든 케이스를 처리한다
- `if/else` 체인 대신 `match`를 사용한다
- 합 타입에 새 변형을 추가하면 컴파일러가 누락된 처리를 알려준다

---

### 3. Compound Types (복합 타입)

검증된 Simple Type들을 합성하여 도메인 객체를 구성한다.

```typescript
export class PersonalName {
  constructor(
    readonly firstName: String50,
    readonly lastName: String50,
  ) {}
}

export class CustomerInfo {
  constructor(
    readonly name: PersonalName,
    readonly emailAddress: EmailAddress,
  ) {}
}

export class Address {
  constructor(
    readonly addressLine1: String50,
    readonly addressLine2: O.Option<String50>,  // 선택적 필드는 Option
    readonly addressLine3: O.Option<String50>,
    readonly addressLine4: O.Option<String50>,
    readonly city: String50,
    readonly zipCode: ZipCode,
  ) {}
}
```

**규칙:**
- 모든 필드는 `readonly`
- 선택적 필드는 `null | undefined` 대신 `Option<T>`을 사용한다
- 복합 타입의 필드는 반드시 검증된 Simple Type이다 (raw string/number 금지)

---

### 4. 상태 전이 타입

워크플로우의 각 단계를 별도 타입으로 표현한다. 같은 타입을 재사용하지 않는다.

```
UnvalidatedOrder → ValidatedOrder → PricedOrder → PlaceOrderEvent[]
```

#### Unvalidated (외부 입력, raw 타입)

```typescript
export class UnvalidatedCustomerInfo extends ValueObject {
  constructor(
    readonly firstName: string,      // raw string
    readonly lastName: string,       // raw string
    readonly emailAddress: string,   // raw string
  ) { super() }
}

export class UnvalidatedOrder extends ValueObject {
  constructor(
    readonly orderId: string,
    readonly customerInfo: UnvalidatedCustomerInfo,
    readonly shippingAddress: UnvalidatedAddress,
    readonly billingAddress: UnvalidatedAddress,
    readonly lines: UnvalidatedOrderLine[],
  ) { super() }
}
```

#### Validated (검증 완료, 도메인 타입)

```typescript
export class ValidatedOrderLine extends Entity<OrderLineId> {
  constructor(
    readonly orderLineId: OrderLineId,    // 검증된 타입
    readonly productCode: ProductCode,     // 검증된 타입
    readonly quantity: OrderQuantity,      // 검증된 타입
  ) { super() }
}

export class ValidatedOrder extends Entity<OrderId> {
  constructor(
    readonly orderId: OrderId,
    readonly customerInfo: CustomerInfo,
    readonly shippingAddress: Address,
    readonly billingAddress: Address,
    readonly lines: readonly ValidatedOrderLine[],
  ) { super() }
}
```

#### Priced (가격 산정 완료)

```typescript
export class PricedOrderLine extends Entity<OrderLineId> {
  constructor(
    readonly orderLineId: OrderLineId,
    readonly productCode: ProductCode,
    readonly quantity: OrderQuantity,
    readonly linePrice: Price,           // 가격 추가
  ) { super() }
}

export class PricedOrder extends Entity<OrderId> {
  constructor(
    readonly orderId: OrderId,
    readonly customerInfo: CustomerInfo,
    readonly shippingAddress: Address,
    readonly billingAddress: Address,
    readonly amountToBill: BillingAmount, // 총액 추가
    readonly lines: readonly PricedOrderLine[],
  ) { super() }
}
```

**규칙:**
- Unvalidated 타입: raw 원시값 (string, number) 사용
- Validated 이후 타입: 반드시 도메인 타입 (String50, OrderId 등) 사용
- 각 상태 전이마다 새 타입을 정의한다. 기존 타입에 optional 필드를 추가하지 않는다

---

### 5. Railway Oriented Programming (ROP)

모든 실패 가능한 연산은 `Either<Error, Success>`를 반환한다. 파이프라인에서 에러는 자동으로 전파된다.

#### Do-Notation 패턴 (순차 검증)

```typescript
const toCustomerInfo = (
  unvalidated: UnvalidatedCustomerInfo,
): E.Either<ValidationError, CustomerInfo> =>
  pipe(
    E.Do,
    E.bind('firstName', () =>
      pipe(unvalidated.firstName, String50.create, E.mapLeft(ValidationError.from)),
    ),
    E.bind('lastName', () =>
      pipe(unvalidated.lastName, String50.create, E.mapLeft(ValidationError.from)),
    ),
    E.bind('emailAddress', () =>
      pipe(unvalidated.emailAddress, EmailAddress.create, E.mapLeft(ValidationError.from)),
    ),
    E.let('name', ({ firstName, lastName }) => new PersonalName(firstName, lastName)),
    E.map((scope) => new CustomerInfo(scope.name, scope.emailAddress)),
  )
```

#### 배열 검증 (sequenceArray)

```typescript
// Array<Either<E, A>> → Either<E, Array<A>>
E.bind('validLines', () =>
  pipe(
    lines,
    A.map(toValidatedOrderLine(checkProductCodeExists)),
    E.sequenceArray,  // 하나라도 실패하면 전체 실패
  ),
)
```

#### 선택적 필드 검증

```typescript
// Option<Either<E, T>> → Either<E, Option<T>>
const optEthToEthOpt = <Err, T>(
  i: O.Option<E.Either<Err, T>>,
): E.Either<Err, O.Option<T>> =>
  O.match(
    () => E.right(O.none),
    E.map(O.some),
  )(i)

E.bind('addressLine2', () =>
  pipe(
    checkedAddress.addressLine2,
    O.map(flow(String50.create, E.mapLeft(ValidationError.from))),
    optEthToEthOpt,
  ),
)
```

#### Sync → Async 전환

```typescript
// Either → TaskEither 변환
TE.fromEither(someEitherValue)

// Either를 반환하는 함수 → TaskEither를 반환하는 함수
TE.fromEitherK(priceOrder(getPrice))

// 혼합 파이프라인
pipe(
  E.Do,
  E.bind('validId', () => toOrderId(orderId)),         // sync
  E.bind('validInfo', () => toCustomerInfo(info)),      // sync
  TE.fromEither,                                         // sync → async 전환점
  TE.bind('checkedAddr', () => checkAddress(address)),   // async
  TE.map((scope) => new ValidatedOrder(...)),
)
```

#### 에러 타입 변환 (mapLeft)

```typescript
// 저수준 에러 → 도메인 에러 변환
E.mapLeft(ValidationError.from)
E.mapLeft(PricingError.from)

// 패턴 매칭으로 에러 변환
TE.mapLeft((addrError) =>
  match(addrError)
    .with(P.instanceOf(AddressNotFound), () => new ValidationError('Address not found'))
    .with(P.instanceOf(InvalidFormat), () => new ValidationError('Address has bad format'))
    .exhaustive(),
)
```

**규칙:**
- 예외(throw)를 사용하지 않는다. 모든 실패는 `Either.left`로 표현한다
- `E.bind`는 이전 bind가 성공해야 실행된다 (fail-fast)
- 에러 타입은 반드시 도메인 수준으로 변환한다 (mapLeft)
- sync와 async 경계에서 `TE.fromEither`로 명시적 전환한다

---

### 6. Workflow 구현

#### 워크플로우 타입 시그니처

```typescript
// 공개 시그니처 (public-types.ts)
export type PlaceOrder = (
  i: UnvalidatedOrder,
) => TE.TaskEither<PlaceOrderError, PlaceOrderEvent[]>
```

#### 스텝 함수 타입 정의 (implementation.types.ts)

```typescript
// 의존성 타입
export type CheckProductCodeExists = (i: ProductCode) => boolean
export type CheckAddressExists = (i: UnvalidatedAddress) => TE.TaskEither<AddressValidationError, CheckedAddress>
export type GetProductPrice = (i: ProductCode) => Price

// 스텝 함수 타입
type ValidateOrder = (
  dep1: CheckProductCodeExists,
  dep2: CheckAddressExists,
) => (i: UnvalidatedOrder) => TE.TaskEither<ValidationError, ValidatedOrder>

export type PriceOrder = (
  dep: GetProductPrice,
) => (i: ValidatedOrder) => E.Either<PricingError, PricedOrder>

export type AcknowledgeOrder = (
  dep1: CreateOrderAcknowledgmentLetter,
  dep2: SendOrderAcknowledgment,
) => (i: PricedOrder) => O.Option<OrderAcknowledgmentSent>
```

#### 워크플로우 합성 (flow)

```typescript
export const placeOrder = (
  checkCode: CheckProductCodeExists,
  checkAddress: CheckAddressExists,
  getPrice: GetProductPrice,
  createAck: CreateOrderAcknowledgmentLetter,
  sendAck: SendOrderAcknowledgment,
): PlaceOrder =>
  flow(
    validateOrder(checkCode, checkAddress),                  // Step 1
    TE.flatMap(TE.fromEitherK(priceOrder(getPrice))),       // Step 2
    TE.map(placeOrderEvents(createAck, sendAck)),           // Step 3
  )
```

#### 의존성 주입 패턴

```typescript
// 고차 함수: 의존성 → 도메인 함수
const validateOrder: ValidateOrder =
  (checkProductCodeExists, checkAddressExists) =>    // 의존성 (1단계)
  (unvalidatedOrder) =>                               // 입력 (2단계)
    pipe(/* 구현 */)

// API 경계에서 의존성 제공
placeOrder(
  checkProductExists,              // 실제 구현 주입
  checkAddressExists,
  getProductPrice,
  createOrderAcknowledgmentLetter,
  sendOrderAcknowledgment,
)
```

**규칙:**
- 워크플로우 시그니처는 `public-types.ts`에 정의한다
- 내부 스텝 시그니처는 `implementation.types.ts`에 정의한다
- 각 스텝은 `(dependencies) => (input) => output` 형태의 고차 함수다
- `flow()`로 전체 파이프라인을 합성한다
- 모든 외부 의존성(DB, API, 이메일 등)은 함수 파라미터로 주입한다

---

### 7. 이벤트 생성

워크플로우의 최종 출력은 도메인 이벤트 배열이다.

#### 이벤트 타입

```typescript
// 항상 발생
export class OrderPlaced extends ValueObject {
  constructor(
    readonly orderId: OrderId,
    readonly customerInfo: CustomerInfo,
    readonly shippingAddress: Address,
    readonly billingAddress: Address,
    readonly amountToBill: BillingAmount,
    readonly lines: readonly PricedOrderLine[],
  ) { super() }
}

// 금액이 0보다 클 때만 발생
export class BillableOrderPlaced extends ValueObject {
  constructor(
    readonly orderId: OrderId,
    readonly billingAddress: Address,
    readonly amountToBill: BillingAmount,
  ) { super() }
}

// 이메일 발송 성공 시에만 발생
export class OrderAcknowledgmentSent extends ValueObject {
  constructor(
    readonly orderId: OrderId,
    readonly emailAddress: EmailAddress,
  ) { super() }
}
```

#### 조건부 이벤트 (Option)

```typescript
// 금액 > 0일 때만 청구 이벤트 생성
export const createBillingEvent = (
  { orderId, billingAddress, amountToBill }: PricedOrder,
): O.Option<BillableOrderPlaced> =>
  amountToBill.value > 0
    ? O.some(new BillableOrderPlaced(orderId, billingAddress, amountToBill))
    : O.none

// 이메일 발송 결과에 따른 이벤트
export const acknowledgeOrder: AcknowledgeOrder =
  (createAcknowledgmentLetter, sendAcknowledgment) => (pricedOrder) => {
    const letter = createAcknowledgmentLetter(pricedOrder)
    const acknowledgment = new OrderAcknowledgement(
      pricedOrder.customerInfo.emailAddress,
      letter,
    )

    return match(sendAcknowledgment(acknowledgment))
      .with(Sent, () =>
        O.some(new OrderAcknowledgmentSent(
          pricedOrder.orderId,
          pricedOrder.customerInfo.emailAddress,
        )),
      )
      .with(NotSent, () => O.none)
      .exhaustive()
  }
```

#### 이벤트 수집

```typescript
const optionToList = <T>(opt: O.Option<T>): T[] =>
  pipe(opt, O.match(() => [], (v) => [v]))

export const createEvents: CreateEvents = (pricedOrder, acknowledgmentEventOpt) => [
  createOrderPlacedEvent(pricedOrder),           // 항상 포함
  ...pipe(acknowledgmentEventOpt, optionToList), // 조건부
  ...pipe(pricedOrder, createBillingEvent, optionToList), // 조건부
]
```

**규칙:**
- 조건부 이벤트는 `Option<Event>`으로 표현한다
- `null`이나 빈 배열 필터링 대신 `Option`을 사용한다
- 이벤트는 과거형으로 명명한다 (OrderPlaced, not PlaceOrder)
- 이벤트 수집 시 `optionToList`로 `Option`을 배열로 변환한다

---

### 8. DTO 패턴 (직렬화 경계)

도메인 타입과 외부 데이터(JSON, DB) 사이의 변환 계층.

#### DTO 클래스 구조

```typescript
export class CustomerInfoDto extends ValueObject {
  constructor(
    readonly firstName: string,    // raw 타입
    readonly lastName: string,
    readonly emailAddress: string,
  ) { super() }

  // 도메인 → DTO (항상 성공, 직렬화용)
  static fromDomain(domainObj: CustomerInfo): CustomerInfoDto {
    return new CustomerInfoDto(
      domainObj.name.firstName.value,
      domainObj.name.lastName.value,
      domainObj.emailAddress.value,
    )
  }

  // DTO → 도메인 (실패 가능, 역직렬화용)
  toDomain(): E.Either<ErrPrimitiveConstraints, CustomerInfo> {
    return pipe(
      E.Do,
      E.bind('first', () => String50.create(this.firstName)),
      E.bind('last', () => String50.create(this.lastName)),
      E.bind('email', () => EmailAddress.create(this.emailAddress)),
      E.let('name', ({ first, last }) => new PersonalName(first, last)),
      E.map((scope) => new CustomerInfo(scope.name, scope.email)),
    )
  }

  // DTO → Unvalidated 도메인 (항상 성공, 워크플로우 입력용)
  toUnvalidatedCustomerInfo(): UnvalidatedCustomerInfo {
    return new UnvalidatedCustomerInfo(
      this.firstName,
      this.lastName,
      this.emailAddress,
    )
  }
}
```

#### 이벤트 DTO (합 타입 직렬화)

```typescript
export type PlaceOrderEventDto =
  | { OrderPlaced: OrderPlacedDto }
  | { BillableOrderPlaced: BillableOrderPlacedDto }
  | { OrderAcknowledgmentSent: OrderAcknowledgmentSentDto }

export const placeOrderEventDtoFromDomain = (
  domainObj: PlaceOrderEvent,
): PlaceOrderEventDto =>
  Object.fromEntries([
    match(domainObj)
      .with(P.instanceOf(OrderPlaced), (i) =>
        ['OrderPlaced', OrderPlacedDto.fromDomain(i)] as const,
      )
      .with(P.instanceOf(BillableOrderPlaced), (i) =>
        ['BillableOrderPlaced', BillableOrderPlacedDto.fromDomain(i)] as const,
      )
      .with(P.instanceOf(OrderAcknowledgmentSent), (i) =>
        ['OrderAcknowledgmentSent', OrderAcknowledgmentSentDto.fromDomain(i)] as const,
      )
      .exhaustive(),
  ]) as PlaceOrderEventDto
```

**규칙:**
- DTO 필드는 raw 원시 타입만 사용한다 (string, number, boolean)
- `fromDomain()`: static 메서드, 도메인 → DTO 변환 (항상 성공)
- `toDomain()`: 인스턴스 메서드, DTO → 도메인 변환 (Either 반환)
- `toUnvalidated()`: 인스턴스 메서드, DTO → Unvalidated 도메인 (항상 성공)
- 합 타입은 `{ TypeName: DataDto }` 형태의 discriminated object로 직렬화한다

---

### 9. API 경계 (HTTP 계층)

```typescript
export const placeOrderApi: PlaceOrderApi = (request: HttpRequest) =>
  pipe(
    request.body,                                          // JSON string
    Json.deserialize(OrderFormDto),                        // → DTO
    (orderForm) => orderForm.toUnvalidatedOrder(),         // → Unvalidated Domain
    TE.of,                                                  // → TaskEither
    TE.flatMap(
      placeOrder(                                          // → 워크플로우 실행
        checkProductExists,
        checkAddressExists,
        getProductPrice,
        createOrderAcknowledgmentLetter,
        sendOrderAcknowledgment,
      ),
    ),
  )().then(
    E.match(
      flow(                                                // 에러 → HTTP 응답
        PlaceOrderErrorDto.fromDomain,
        Json.serialize,
        (json) => new HttpResponse(401, json),
      ),
      flow(                                                // 성공 → HTTP 응답
        A.map(placeOrderEventDtoFromDomain),
        Json.serialize,
        (json) => new HttpResponse(200, json),
      ),
    ),
  )
```

#### 데이터 흐름 정리

```
[외부]                  [경계]                [도메인]
HttpRequest.body    →   OrderFormDto      →   UnvalidatedOrder
  (JSON string)         (raw 타입)             (raw 타입)
                                           ↓ validate
                                          ValidatedOrder
                                           (도메인 타입)
                                           ↓ price
                                          PricedOrder
                                           ↓ acknowledge + events
                                          PlaceOrderEvent[]
[외부]                  [경계]                [도메인]
HttpResponse.body   ←   EventDto[]        ←   PlaceOrderEvent[]
  (JSON string)         (raw 타입)             (도메인 타입)
```

---

### 10. ValueObject vs Entity

```typescript
// ValueObject: 구조적 동등성 (모든 필드가 같으면 같다)
export abstract class ValueObject implements Equatable {
  equals(obj: unknown): boolean {
    // deepStrictEqual로 비교
  }
}

// Entity: 식별자 동등성 (ID가 같으면 같다)
export abstract class Entity<ID extends RawId | ValueObject> implements Equatable {
  abstract readonly id: ID

  equals(obj: unknown): boolean {
    // this.id === other.id 비교
  }
}
```

**사용 기준:**
- **ValueObject**: 주소, 금액, 이름 등 값 자체가 의미인 것 → `extends ValueObject`
- **Entity**: 주문, 주문 항목 등 고유 ID가 있는 것 → `extends Entity<OrderId>`

---

## DO / DON'T 체크리스트

### DO (반드시 해야 할 것)

- [ ] 모든 도메인 원시값에 Branded Type + Smart Constructor 적용
- [ ] 실패 가능한 연산은 반드시 `Either<Error, T>` 반환
- [ ] 선택적 값은 `Option<T>` 사용
- [ ] 모든 필드에 `readonly` 적용
- [ ] 워크플로우의 각 상태를 별도 타입으로 정의
- [ ] 외부 의존성은 함수 파라미터로 주입
- [ ] 합 타입에 `exhaustive()` 패턴 매칭 사용
- [ ] 에러 타입을 도메인 수준으로 변환 (`mapLeft`)
- [ ] DTO와 도메인 타입을 분리
- [ ] 이벤트는 과거형으로 명명

### DON'T (절대 하지 말 것)

- [ ] ~~throw/try-catch를 도메인 로직에 사용~~ → Either 사용
- [ ] ~~null/undefined를 도메인에 사용~~ → Option 사용
- [ ] ~~raw 원시값을 도메인 로직에 직접 사용~~ → Branded Type 사용
- [ ] ~~가변 상태(let, mutation)~~ → readonly + 새 객체 생성
- [ ] ~~하나의 타입에 optional 필드로 상태 구분~~ → 상태별 별도 타입
- [ ] ~~도메인 로직에서 직접 I/O 수행~~ → 함수 파라미터로 주입
- [ ] ~~if/else 체인으로 합 타입 분기~~ → match().exhaustive()
- [ ] ~~DTO와 도메인 타입 혼용~~ → 명확한 변환 계층
- [ ] ~~빈 배열이나 null로 "없음" 표현~~ → Option.none
- [ ] ~~new로 직접 도메인 원시값 생성~~ → Smart Constructor만 사용

---

## 새 워크플로우 구현 체크리스트

1. **도메인 모델링**: 유비쿼터스 언어로 타입 정의
2. **Simple Types 정의**: Branded + Smart Constructor
3. **Compound Types 정의**: Simple Type 합성
4. **상태 전이 타입 정의**: Unvalidated → Validated → ... → Events
5. **워크플로우 시그니처 정의**: `public-types.ts`
6. **스텝 함수 시그니처 정의**: `implementation.types.ts`
7. **각 스텝 구현**: Either/TaskEither 파이프라인
8. **워크플로우 합성**: `flow()`로 스텝 연결
9. **DTO 정의**: 직렬화/역직렬화 변환
10. **API 경계 구현**: HTTP 요청/응답 처리
