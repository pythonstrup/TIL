# Spring Thread

## 비동기란?

- 사전적 정의로 '동시에 일어나지 않음'을 뜻한다. 
  - 완료 여부를 기다리지 않고 다른 작업을 실행하는 것.

## @Async

- `Spring`에서는 비동기 작업을 수행할 수 있는 어노테이션 `@Async`를 제공하고 있다.
  - '땡 Java'만 사용해야 한다면 `Future`, `CompletableFuture` 등을 사용할 수 있겠지만, 비동기를 훨씬 쉽게 사용할 수 있도록 제공하는 것이다.
- `@Async`를 사용하기 위해선 애플리케이션에 `@EnableAsync` 어노테이션을 달아 `@Async`를 사용할 것이라고 선언해야 한다.
- 이제 `@Async`는 클래스와 함수 단에 달 수 있다!

## @Async 사용 시 주의 사항

### 1. 같은 클래스 내에서 함수를 중첩해서 사용하면?

- 중첩된 메소드는 비동기로 작동하지 않게 된다.
- `@Async`는 `@Transactional`과 마찬가지로 Spring AOP를 통해 생성된 프록시 객체를 이용해 비동기 메소드를 처리한다.
  - 프록시 객체를 거치지 못하면 비동기로 처리되지 않는다는 뜻이다.
- 하지만, 같은 클래스 내부에서 중첩된 메소드를 호출하면 프록시 객체를 거치지 않고 호출하게 되므로 동기적으로 실행된다.
- 따라서 해당 문제를 해결하려면 클래스를 분리해야 한다.
- 접근 제한자가 private일 때도 동일한 문제가 발생하니 주의하자.

### 2. 비동기 진행 중 예외 발생

- `@Async`가 적용된 메소드를 호출하는 경우 별도의 스레드에서 동작하기 때문에 기본적으로 예외를 캐치할 수 없다.
  - 이를 방지하기 위해 추가적인 예외 핸들링이 필요하다.
- `AsyncUncaughtExceptionHandler`를 사용해 예외를 적절히 처리해야 한다.

```java
@Configuration
public class AppConfig implements AsyncConfigurer {
  
  @Override
  public AsyncUncaughtExceptionHandler getAsyncUncaughtExceptionHandler() {
    return new MyAsyncUncaughtExceptionHandler();
  }
}
```

```java
public class MyAsyncExceptionHandler implements AsyncUncaughtExceptionHandler {
  
  @Override
  public void handleUncaughtException(Throwable ex, Method method, Object... params) {
    // ...
  }
}
```

## 스레드 풀 관리하기

- `AsyncConfigurer`를 사용해 스프링의 쓰레드 풀을 관리할 수 있다.
  - `setThreadNamePrefix`: 스레드의 이름
  - `setCorePoolSzie`: 동시에 실행할 기본 스레드의 수를 설정할 수 있다.
  - `setMaxPoolSize`: 최대 스레드의 수이다. 기본 값은 `Integer.MAX_VALUE`다. (무제한으로 생겨날 수 있다는 말..) 
  - `setQueueCapacity`: corePoolSize보다 많은 요청이 들어오면 큐에 요청이 적재되는데, 해당 큐의 용량을 설정할 수 있는 메소드다.
  - `setRejectedExecutionHandler`: 큐가 가득 찼을 때, 핸들링할 객체를 전달하는 메소드

```java
@EnableAsync
@Configuration
public class AsyncConfig implements AsyncConfigurer {
  
  @Bean
  @Override
  public Executor threadPoolExecutor(
      @Value("${thread.core-pool-size}") int CORE_POOL_SIZE,
      @Value("${thread.max-pool-size}") int MAX_POOL_SIZE,
      @Value("${thread.queue-capacity}") int QUEUE_CAPACITY) {
    ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
    executor.setThreadNamePrefix("async-thread-");
    executor.setCorePoolSize(CORE_POOL_SIZE);
    executor.setMaxPoolSize(MAX_POOL_SIZE);
    executor.setQueueCapacity(QUEUE_CAPACITY);
    executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());
    executor.initialize();
    return executor;
  }
}
```

### RejectedExecutionHandler

- `setRejectedExecutionHandler`을 통해 설정하는 핸들러를 통해 예외를 처리를 사용자가 스스로 정할 수 있다.
- `ThreadPoolExecutor.AbortPolicy`: 기본 설정 값이다. reject가 발생하면 `RejectedExecutionException`을 발생시킨다.
- `ThreadPoolExecutor.CallerRunsPolicy`: shutdown 상태가 아닐 때, `ThreadPoolTaskExecutor`에 요청한 스레드에서 직접 처리한다.
- `ThreadPoolExecutor.DiscardPolicy`: reject된 작업을 무시한다. 모든 작업이 처리될 필요가 없을 경우 사용한다.
- `ThreadPoolExecutor.DiscardOldsetPolicy`: 오래된 작업을 제거하고 reject된 작업을 실행시킨다. 역시 모든 작업이 처리될 필요가 없을 경우 사용한다.

### Shutdown

- 스레드에서 작업이 이뤄질 때 어플리케이션을 종료해버리면 처리되지 못한 작업들이 유실되는 문제가 발생한다.
- `setWaitForTasksToCompleteOnShutdown` 메소드에 `true`를 전달하면 작업 유실을 방지할 수 있다.

```java
@EnableAsync
@Configuration
public class AsyncConfig implements AsyncConfigurer {
  
  @Bean
  @Override
  public Executor threadPoolExecutor(...) {
    // ...
    executor.setWaitForTasksToCompleteOnShutdown(true);
    return executor;
  }
}
```

### Timeout

- 스레드의 타임아웃 또한 설정할 수 있다. 초 단위로 설정할 수 있다.

```java
@EnableAsync
@Configuration
public class AsyncConfig implements AsyncConfigurer {
  
  @Bean
  @Override
  public Executor threadPoolExecutor(...) {
    // ...
    executor.setAwaitTerminationSeconds(30);
    return executor;
  }
}
```

### ThreadPoolTaskExecutor 동작 원리

1. 현재 사용 중인 스레드의 수가 corePoolSize만큼 있을 때, 요청이 들어오면 `LinkedBlockingQueue`를 생성하고 요청을 큐에 넣는다.
2. queue에 담긴 요청이 `queueCapacity`의 수만큼 있을 때(큐가 꽉 찼을 때) 요청이 오면 `maxPoolSize`만큼 스레드를 생성하여 처리한다.
3. 현재 사용 중인 스레드 수가 `maxPoolSize`만큼 있고, 큐에 담긴 요청이 `queueCapacity`의 수 만큼 있을 때 추가 요청이 오면 `RejectedExecution` 전략에 따라 처리된다.

### 가상 스레드 설정

```java
@EnableAsync
@Configuration
public class AsyncConfig implements AsyncConfigurer {

  private final ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();

  @Bean("virtualThreadExecutorService")
  public ExecutorService executorService() {
    return executor;
  }

  @Primary
  @Bean(TaskExecutionAutoConfiguration.APPLICATION_TASK_EXECUTOR_BEAN_NAME)
  public AsyncTaskExecutor executor() {
    return new TaskExecutorAdapter(executor);
  }
}
```

### 가상 스레드의 치명적인 단점?

- 배압 조절이 안 된다. 요청이 엄청나게 많이 발생하면 쓰레드가 무제한으로 생긴다.

<br/>


# 참고자료

- [비동기 처리 - @Async](https://han5ung.tistory.com/28)
- [[Spring] ThreadPoolTaskExecutor 설정으로 비동기 처리 효과적으로 하기](https://dev-coco.tistory.com/186)
