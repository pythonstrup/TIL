# Spring Event

- 스프링은 이벤트를 `Publish/Subcribe`하는 기능을 제공한다.
- 이벤트는 어떻게 주고 받을 수 있을까?
  - 이벤트를 발행은 `ApplicationEventPublisher`가 담당한다. 이벤트를 발행하고 싶은 서비스에서 주입받아 `publishEvent()` 메소드로 이벤트를 발행하면 된다. 
  - 이벤트 구독은 `@EventListener`, `@TransactionalEventListener` 등의 어노테이션을 활용하거나 `ApplicationListener` 인터페이스를 구현하면 된다.

<br/>

## 언제 사용해야 할까?

- 예를 들어 주문 도메인이 있다고 해보자. 주문은 회원, 포인트, 쿠폰, 결제, 알림, 서포터즈, 제품 등등 너무 많은 의존성이 엮어 있어 관리가 힘든 상태다.
- 이 때 의존성을 떼어내고 주문이라는 관심사에 집중하기 위해 스프링 이벤트를 사용해볼 수 있다.

```java
@Server
@RequiredArgsConstructor
public class OrderService {
  private final MemberService memberService;
  private final ProductService productService;
  private final PaymentService paymentService;
  private final PointService pointService;
  private final CouponService couponService;
  private final NotifiactionService notifiactionService;
}
```

- 주문은 회원, 제품, 결제 정보 없이는 로직 진행이 안되기 때문에 의존성을 남겨두기로 하고 나머지 포인트 및 쿠폰 사용 및 적립, 알림 등을 이벤트로 떼어낸다고 해보자.
  - `OrderEventPublisher`를 `OrderEvent`라는 주문 이벤트를 발행하기 시작한다. 
  - 쿠폰, 포인트, 알림 서비스는 `OrderEvent`를 수신해 로직을 실행하도록 변경했다.
- 이제 주문의 서비스 의존성은 아래와 같이 줄어든다.
- 굳이 쿠폰, 포인트, 알림 서비스의 메소드를 호출할 필요 없이 이벤트만 발행하면 로직이 자동으로 실행되는 마법같은 일이 벌어진다.

```java
@Server
@RequiredArgsConstructor
public class OrderService {
  private final MemberService memberService;
  private final ProductService productService;
  private final PaymentService paymentService;
  private final OrderEventPublisher orderEventPublisher;
}
```

### 이벤트 발행과 구독의 장단점

|장점|단점|
|:--:|:--:|
|의존성을 분리하여 느슨하게 결합할 수 있다.|코드를 파악하기 어렵다. 이벤트를 파악하기 위해 이벤트를 구독하는 모든 메소드를 찾아다녀야할 수도 있다.|
|서비스를 분리하기 용이하다.|메시지 구독 순서를 고려해야할 경우 로직 처리가 어려워질 수 있다.|
|단위 테스트가 쉬워진다.|전체적인 이벤트 발행/구독 과정을 파악하기 어렵기 때문에 통합 테스트가 어려워진다.|

<br/>

## `ApplicationEventPublisher`

- `ApplicationEventPublisher`는 Spring에서 `ApplicationContext`로 구현된다.
- `ApplicationContext`는 빈 탐색과 등록, 리소스 처리 등의 역할을 수행하는 구현체다.
  - 인터페이스 분리 원칙(Interface Segregation Principle)에 따라 이벤트 발행 책임만 처리하는 것이 `ApplicationEventPublisher` 인터페이스다.

```java
public interface ApplicationContext extends 
    EnvironmentCapable, 
    ListableBeanFactory, 
    HierarchicalBeanFactory,
    MessageSource, 
    ApplicationEventPublisher, 
    ResourcePatternResolver {...}
```

<br/>

## `@EventListener`

- 이벤트를 수신할 때 사용하는 어노테이션이다.
- `@EventListener`가 달려 있는 메소드는 이벤트 객체 파라미터를 필수로 넣어야 한다.
  - 이벤트 객체 파라미터를 넣지 않으면 실행 과정에서 에러가 발생한다.
- `@EventListener`는 호출 시점에 바로 실행된다.

```java
@Service
@RequiredArgsConstructor
public class SupporterService {
  
  @EventListener
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  } 
}
```

<br/>

## `@TransactionalEventListner`

- 트랜잭션과 연관된 이벤트 리스너다.
- 트랜잭션을 사용하지 않으면 이벤트를 수신하지 않는 특징이 있다.
  - `@Transactional(propagation = Propagation.NOT_SUPPORTED)`에 감싸인 메소드에서 이벤트를 쏘면 `@TransactionalEventListner`는 이벤트를 수신하지 않는다.
  - `@TransactionalEventListner`는 트랜잭션 커밋, 롤백, 종료 시점 등이 트리거가 되는데, 트랜잭션이 실행되지 않은 상태면 트리거가 작동하지 않기 때문에 이벤트가 당연히 수신되지 않는다.
- `phase`라는 속성을 가지고 있는데, 아래 표와 같은 역할을 한다.

|TransactionPhase|설명|
|:--:|:--:|
|BEFORE_COMMIT|트랜잭션 커밋 직전에 실행된다.|
|AFTER_COMMIT|트랜잭션 커밋 이후 실행된다. default 값이다.|
|AFTER_ROLLBACK|트랜잭션 롤백 이후에 실행된다.|
|AFTER_COMPLETION|트랜잭션이 끝난 이후에 실행된다.|

<br/>

## 스프링 이벤트의 특징

- 스프링 이벤트는 멀티캐스팅이다.
  - 이벤트 하나가 발생하면 다수의 사용자가 수신할 수 있다.
- 기본적으로 동기 방식으로 동작한다. 
- 트랜잭션을 결합할 수 있다.
  - 트랜잭션을 하나의 범위로 묶어서 사용할 수 있다. 이벤트를 구독하는 곳과 동일한 트랜잭션을 공유할 수 있다는 얘기다.
  - 위에서 설명한 TransactionPhase 설정을 통해 트랜잭션에 관여할 수 있다.

<br/>

## 코드 예시

- 아래 코드는 주문을 진행하는 코드이다.

```java
@Service
@RequiredArgsConstructor
public class OrderService {
  
  ...
  private final OrderEventPublisher orderEventPublisher;
  
  @Transactional
  public Order order(final OrderRequest request) {
    ...
    orderEventPublisher.publishOrderEvent(order);
    return order;
  }
  
  ...
}
```

- 이벤트 생산자를 살펴보자.
- 먼저 `ApplicationEventPulisher`를 주입받는다.
- 이벤트 객체를 만들고 `ApplicationEventPulisher`의 `publishEvent()` 메소드를 사용해 이벤트를 발행하는 예시이다.

```java
@Component
@RequiredArgsConstructor
public class OrderEventPublisher {
  
  private final ApplicationEventPulisher eventPublisher;
  
  public void publishOrderEvent(final Order order) {
    final OrderEvent orderEvent = OrderEvent.from(order);
    eventPublisher.publishEvent(orderEvent);
  }
}
```

- 아래와 같이 `@EventListener`를 달아주고 이벤트 객체인 `OrderEvent`를 파라미터에 포함하면 이벤트가 수신된다.

```java
@Service
@RequiredArgsConstructor
public class SupporterService {
  
  @EventListener
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  } 
}
```

### 트랜잭션 시점을 조정하고 싶다면?

- 만약 트랜잭션의 시점을 조절하여 이벤트를 수신하고 싶다면 위에서 설명했던 `@TransactionalEventListener`의 `phase` 속성을 사용하면 된다.

```java
@Service
@RequiredArgsConstructor
public class SupporterService {
  
  @TransactionalEventListener(phase = TransactionPhase.BEFORE_COMMIT)
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  } 
}
```

### 비동기 이벤트 수신

```java
@Service
@RequiredArgsConstructor
public class SupporterService {
  
  @Async
  @TransactionalEventListener
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  } 
}
```

- 하지만 위의 방식은 위험하다. 트랜잭션이 분리되어 있지 않기 때문에 외부에서 Exception이 발생할 시, 이벤트 수신 메소드의 작업이 롤백되어 버릴 수 있다.
- 아래와 같이 트랜잭션 전파 레벨을 `REQUIRES_NEW`로 하면 트랜잭션이 분리되어 안전하게 처리할 수 있다.

```java
@Service
@RequiredArgsConstructor
public class SupporterService {

  @Async
  @Transactional(propagation = Propagation.REQUIRES_NEW)
  @TransactionalEventListener
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  }
}
```

<br/>

## 잘못된 사용 예시

### 트랜잭션을 사용하지 않는데 `@TransactionalEventListener`로 이벤트를 수신하려는 경우

- 모종의 이유로 주문 로직에서 트랜잭션을 사용하지 않는다고 가정해보자.

```java
@Service
@RequiredArgsConstructor
public class OrderService {
  
  ...
  private final OrderEventPublisher orderEventPublisher;

  @Transactional(propagation = Propagation.NOT_SUPPORTED)
  public Order order(final OrderRequest request) {
    ...
    orderEventPublisher.publishOrderEvent(order);
    return order;
  }
  
  ...
}
```

- 트랜잭션을 사용하지 않는 메소드로부터 `@TransactionalEventListener`을 사용해 이벤트를 수신하려고 하면 아무런 일도 발생하지 않는다.
- `@TransactionalEventListener`는 트랜잭션이 롤백되거나 커밋 혹은 종료되는 시점을 기준으로 이벤트가 실행되는데 트랜잭션 자체가 없으면 이벤트 트리거가 발동되지 않기 때문이다.

```java
@Service
@RequiredArgsConstructor
public class SupporterService {

  @Async
  @Transactional(propagation = Propagation.REQUIRES_NEW)
  @TransactionalEventListener
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  }
}
```

- 따라서 트랜잭션을 사용하지 않는다면 아래와 같이 `@EventListener`를 사용하자.

```java
@Service
@RequiredArgsConstructor
public class SupporterService {

  @Async
  @Transactional(propagation = Propagation.REQUIRES_NEW)
  @EventListener
  public void onOrderEvent(final OrderEvent orderEvent) {
    ...
  }
}
```

### 이벤트의 방향

- 만약 주문, 회원이 서포터즈 이벤트 발행자에게 의존하고 있다고 해보자.
- 뭔가 부자연스럽지 않은가? 이벤트 소비자가 주체가 되어 각각의 서비스에게 이벤트를 뜯어내고 있는 형국이다.

```java
@Service
@RequiredArgsConstructor
public class OrderService {
  private final SupporterEventPublisher supporterEventPublisher;
  
  public void order() {
    ...
    supporterEventPublisher.publishOrderEvent(order);
  }
}

@Service
@RequiredArgsConstructor
public class MemberService {
  private final SupporterEventPublisher supporterEventPublisher;
  
  public void signUp() {
    ...
    supporterEventPublisher.publishSignUpEvent(member);
  }
}
```

```java
@Service
@RequiredArgsConstructor
public class SupporterService {

  @EventListener
  public void onSignUp(final SignUpEvent signUpEvent) {...}
  
  @EventListener
  public void onOrderEvent(final OrderEvent orderEvent) {...}
}
```

- 이벤트 발행은 해당 도메인이 주체가 되어 발행하는 것이 자연스럽다.
- 이벤트 소비자는 이벤트를 발행시켜 가져오는 역할을 하기보다는 그저 이벤트가 발행되기만을 기다리는 것이 맞다. 
  - 이벤트를 소비자는 불특정 다수라는 사실을 인지하자.

<br/>

# 참고 자료

- [공식 문서: Working with Application Event](https://docs.spring.io/spring-modulith/reference/events.html)
- [[Spring] 스프링에서 이벤트의 발행과 구독 방법과 주의사항, 이벤트 사용의 장/단점과 사용 예시
  출처: https://mangkyu.tistory.com/292 [MangKyu's Diary:티스토리]](https://mangkyu.tistory.com/292)
