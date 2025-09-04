# Domain Events

- 참고자료: https://namjug-kim.github.io/2020/03/24/spring-ddd-domain-event.html

## ApplicationEventPublisher

- 아래와 같이 간단하게 Service 빈에서 `ApplicationEventPublisher`를 주입받아 이벤트를 명시적으로 발행할 수 있다.

```java
@Service
@RequiredArgsConstructor
public class ReviewService {

  private final ReviewRepository reviewRepository;
  private final ApplicationEventPublisher eventPublisher;

  public ReviewResult save(final ReviewCommand command) {
    Review review = reviewRepository.save(command.toDomain());
    eventPublisher.publishEvent(ReviewCreatedEvent.from(review)); // 이벤트 발행
    return ReviewResult.from(review);
  }
}
```

- 만약 DDD를 지향한다면 `Aggregate Root`에서 이벤트를 발행하는 것이 더 바람직할 수 있다.
- `Aggregate Root`는 비즈니스 로직의 단일 진입점이다. 
  - DDD에서는 `Aggregate Root`를 중심으로 도메인 모델의 캡슐화와 응집도를 높여 더 풍부하고 표현력 있는 도메인을 만든다.
  - 만약 `이벤트`를 애플리케이션 레이어에 작성하면, 도메인 객체에 있는 비즈니스 로직와 분리되면서 코드를 유지보수하는 것에 어려움을 겪게 될 수도 있다. 

## 도메인 객체에서 이벤트 발생시키기

- Domain 객체에서 이벤트를 발생시키기 위해 사용할 `Events` 객체를 정의

```java
public class Events {

  private static ApplicationEventPublisher publisher;

  static void setPublisher(final ApplicationEventPublisher publisher) {
    Events.publisher = publisher;
  }

  public static void raise(Object event) {
    if (publisher != null) {
      publisher.publishEvent(event);
    }
  }
}
```

```java
@Configuration
@RequiredArgsConstructor
public class EventConfig {

  private final ApplicationContext applicationContext;

  @Bean
  public InitializingBean eventInitializer() {
    return () -> Events.setPublisher(applicationContext);
  }
}
```

- 아래와 같이 도메인 객체에서 명시적으로 이벤트를 발행할 수 있다.

```java
public class Review extends AbstractAggregateRoot<Review> {

  public void update(final int rating, final String content) {
    this.ratring = rating;
    this.content = content;
    
    Events.raise(ReviewUpdatedEvent.of(review, userId)); // 이벤트 발행
  }
}
```


## 트랜잭션 롤백의 문제

- 위의 예시는 객체가 저장되거나 업데이트되지도 않았는데 이벤트를 발행한다는 문제가 있다. 
- 이때 트랜잭션 롤백이 발생하면 주요 작업은 처리되지 않았는데, 이벤트에 의해 부가 작업들이 처리되어 버리는 문제가 발생한다.
- 이때 사용할 수 있는 것이 Spring의 `DomainEvents`라는 개념이다.

### AbstractAggregateRoot 클래스 (@DomainEvents 활용)

- 아래와 같이 `AbstractAggregateRoot<T>`를 상속하고 `registerEvent` 메소드를 사용해 이벤트를 등록할 수 있다.

```java
public class Review extends AbstractAggregateRoot<Review> {

  public static Review create(final int rating, final String content) {
    final Review review = new Review(rating, content);
    review.registerEvent(ReviewCreatedEvent.of(review, userId)); // 이벤트 저장
    return review;
  }
}
```

- 이 방식은 JpaRepository의 `save`가 명시적으로 호출되면 이벤트를 발행하면서 이벤트 목록을 `clear`한다.
- 하지만 더티 체킹을 통한 이벤트 발행이 되지 않는다는 단점이 있다. 


### 명시적 발행 + @DomainEvents

- 새롭게 생성하는 객체에 대해선 `DomainEvents`를 활용하고, 존재하는 객체의 상태 변경할 때는 `Events.raise()`를 사용해 명시적으로 이벤트를 발행한다.
