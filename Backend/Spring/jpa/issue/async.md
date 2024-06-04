# @Async 에서는 영속성 컨텍스트가 열려 있지 않을 수 있다.

## @Async 설정?

- 자바에서 비동기 처리를 위해 사용
- AsyncConfig에 @EnableAsync 어노테이션을 붙이면 자바 비동기에 대한 설정이 이뤄진다.
- 이제 @Async를 사용할 수 있다.

```java
@EnableAsync
@Configuration
public class AsyncConfig implements AsyncConfigurer {

  private final ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();

  @Bean(TaskExecutionAutoConfiguration.APPLICATION_TASK_EXECUTOR_BEAN_NAME)
  public AsyncTaskExecutor executor() {
    return new TaskExecutorAdapter(executor);
  }
}
```

- @Async는 Type과 Method 타겟이다.

```java
@Target({ElementType.TYPE, ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Reflective
public @interface Async {

	String value() default "";

}
```

- 아래와 같이 비동기로 처리하고자 하는 class 최상단에 붙여 사용하거나, method에 붙여 사용할 수 있다.

```java
@Async
@Service
@Transactional(readOnly = true)
@RequiredArgsConstructor
public class MessageSendServiceImpl implements MessageSendService {

	...
}
```

```java
@Service
@Transactional(readOnly = true)
@RequiredArgsConstructor
public class MessageSendServiceImpl implements MessageSendService {

  private final MessageSender messageSender;

	@Async
  @Override
  @Transactional
  public void sendMessage(final MessageDto message) {
		messageSender.send(message);
  }
}
```

## @Async와 영속성 컨텍스트

### 로컬에서는 Lazy Loading이 되는 경우가 있었다?

- 만약 Entity의 Lazy Loading이 잘 된다면 완전 Lucky 케이스다.
- Dev 서버보다 속도가 빨랐기 때문에 영속성 컨텍스트를 비우기 전에 LazyLoading이 먼저 일어나고 그 다음에 Transaction이 닫혔기 때문일 것이다.
- 하지만 Lazy Loading을 운에 맡겨야 한다니.. 너무 위험하지 않은가?

### @Async에 Entity를 넘기는 것은 anti-pattern이다.

- Entity를 넘겨야한다면 주의해야한다.
- 아래 예시를 보면 order와 연관된 객체인 Member를 불러오는 것을 볼 수 있다.
- Member가 Fetch Join이 되었다면 해당 코드에 문제가 없겠지만, Lazy Loading을 해야하는 상황이라면 충분히 문제가 발생할 수 있는 코드다.

```java
@Service
@Transactional(readOnly = true)
@RequiredArgsConstructor
public class MessageSendServiceImpl implements SmsSendFacadeService {

  private final MessageSendService messageSendService;

  @Async
  @Override
  @Transactional
  public void sendMessage(final Order order) {
		messageSender.send(order.getOrderNo(), order.getMember().getTel(), order.getMember().getName);
  }
}
```

### 그렇다면 어떻게 전달해야할까?

- 가장 좋은 방법은 DTO를 하나 만들어 필요한 정보를 전달하는 것이다.

```java
@Service
@Transactional(readOnly = true)
@RequiredArgsConstructor
public class OrderService {

	private final MessageSendService messageSendService;

	public OrderResponse order(final OrderRequest request) {
		...

      messageSendService.send(MessageDto.of(order));
		
		...
	}
}
```

```java
@Service
@Transactional(readOnly = true)
@RequiredArgsConstructor
public class MessageSendServiceImpl implements MessageSendService {

  private final MessageSender messageSender;

	@Async
  @Override
  @Transactional
  public void sendMessage(final MessageDto message) {
		messageSender.send(message);
  }
}
```

<br/>

# 참고자료

- [entity를 async 메소드 파라미터로 넘길 경우에 대한 질문입니다. - 인프런](https://www.inflearn.com/questions/619336/entity를-async-메소드-파라미터로-넘길-경우에-대한-질문입니다)

- [Spring 프레임워크에서 Transaction과 Async를 사용할 때 고려사항](https://www.slipp.net/questions/10)

- [Getting Started | Creating Asynchronous Methods](https://spring.io/guides/gs/async-method)

- [How To Do @Async in Spring | Baeldung](https://www.baeldung.com/spring-async)
