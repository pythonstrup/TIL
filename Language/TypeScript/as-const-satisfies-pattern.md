# as const satisfies — 리터럴 타입 보존과 타입 제약을 동시에

## 문제

`Record` 타입으로 매핑 테이블을 선언하면, value의 리터럴 타입이 유실되어 제네릭 함수에서 타입 추론이 깨진다.

```typescript
type EventType = 'USER_SIGNUP' | 'ORDER_PLACED' | 'PAYMENT_FAILED';
type EmailTemplate = 'welcome_email' | 'order_confirmation' | 'payment_retry';

// Record로 선언하면 value 타입이 EmailTemplate (union 전체)로 추론됨
const TEMPLATE_MAP: Record<EventType, EmailTemplate> = {
  USER_SIGNUP: 'welcome_email',
  ORDER_PLACED: 'order_confirmation',
  PAYMENT_FAILED: 'payment_retry',
};

TEMPLATE_MAP['USER_SIGNUP']; // 타입: EmailTemplate (union 전체)
```

이 상태에서 제네릭 함수에 전달하면 `T`가 union으로 추론되어 variables의 타입 체크가 동작하지 않는다.

```typescript
interface TemplateVariableMap {
  welcome_email: { username: string; signupDate: string };
  order_confirmation: { orderId: string; totalAmount: number };
  payment_retry: { orderId: string; retryUrl: string };
}

function sendEmail<T extends EmailTemplate>(
  template: T,
  variables: TemplateVariableMap[T],  // T가 union이면 여기가 intersection 또는 never
): void { ... }

// switch case 안에서도 narrowing 안 됨
sendEmail(TEMPLATE_MAP[eventType], { wrongKey: '값' }); // 에러 안 남!
```

## 해결: as const satisfies

```typescript
const TEMPLATE_MAP = {
  USER_SIGNUP: 'welcome_email',
  ORDER_PLACED: 'order_confirmation',
  PAYMENT_FAILED: 'payment_retry',
} as const satisfies Record<EventType, EmailTemplate>;

TEMPLATE_MAP['USER_SIGNUP']; // 타입: 'welcome_email' (리터럴)
```

- `as const` — 모든 value를 리터럴 타입으로 보존
- `satisfies` — `Record<EventType, EmailTemplate>` 제약을 컴파일 타임에 검증 (key 누락, value 오타 방지)

## 효과: switch case에서 자동 narrowing

```typescript
type Event =
  | { type: 'USER_SIGNUP'; username: string; signupDate: string }
  | { type: 'ORDER_PLACED'; orderId: string; totalAmount: number }
  | { type: 'PAYMENT_FAILED'; orderId: string; retryUrl: string };

function handleEvent(event: Event): void {
  switch (event.type) {
    case 'USER_SIGNUP':
      // TEMPLATE_MAP[event.type]의 타입이 'welcome_email' 리터럴로 추론됨
      // → sendEmail의 T가 'welcome_email'로 narrow
      // → variables 객체의 키가 정확히 타입 체크됨
      return sendEmail(TEMPLATE_MAP[event.type], {
        username: event.username,
        signupDate: event.signupDate,  // OK
        wrongKey: 'value',             // 컴파일 에러!
      });
  }
}
```

## 비교 정리

| 선언 방식 | 리터럴 보존 | 타입 제약 | 제네릭 추론 |
|-----------|:---------:|:--------:|:---------:|
| `Record<K, V>` | X | O | X |
| `as const` | O | X | O |
| `as const satisfies Record<K, V>` | O | O | O |

## 활용 사례

discriminated union의 타입 → 다른 discriminated union 타입으로 매핑하는 테이블에 적합하다.

- Event type → Email template 매핑
- API endpoint → Response schema 매핑
- Locale code → Date format 매핑
- Error code → User-facing message 매핑

핵심은 **매핑 테이블의 key-value 관계를 타입 시스템이 추적할 수 있게** 하는 것이다.
