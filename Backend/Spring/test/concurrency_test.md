# 동시성 테스트

- Redis와 같은 도구를 활용해 분산락 시스템을 만들었다고 가정해보자.
- 보통 아래와 같이 스레드와 for문에서 executorService를 실행하는 식으로 구성할 것이다.  

```java
for (int i = 1; i <= n; i++) {
    executorService.execute(() -> {
        myService.service();
    });
}
```

- 하지만 위의 코드는 완전히 동시에 요청되지 못한다. for 문이 순차적으로 실행되기 때문이다.
- 따라서 스레드를 제어할 수 있는 무언가가 필요하다.

## CountDownLatch

### 용도

#### 1. 모든 작업 스레드가 준비될 때까지 대기한다.

- 모든 작업 스레드가 준비 완료 신호를 보낼 때까지 기다렸다가, 준비가 완료되면 모든 스레드를 동시에 시작시킬 수 있다.
- 코드 예시

```java
class CountDownLatchTest {
 
  void test() {
    CountDownLatch startLatch = new CountDownLatch(1);
    for (int i = 0; i < threadCount; i++) {
      executor.execute(
          () -> {
            try {
              startLatch.await(); // 모든 스레드 대기
              ReviewLikeCommand command = new ReviewLikeCommand(memberId, reviewId);
              ReviewLikeResult result = reviewLikeService.likeReview(command);
            } catch (Exception e) {
              
            }
          });
    }
    startLatch.countDown(); // 모든 스레드 동시 시작
  }
}
```

#### 2. 여러 스레드의 작업 완료를 대기한다.

- 하나의 스레드가 여러 개의 다른 스레드에서 수행하는 작업이 모두 완료될 때까지 기다려야 할 때 사용한다.
- 예시
  - 테스트 케이스 안에서 스레드 풀을 직접 생성해서 사용하는 경우 `shutdown()` 메소드를 호출해야 하는데, 메소드를 호출하기 전에 스레드 작업이 완료되는 것이 기다리게 할 수 있다.

```java
class CountDownLatchTest {
 
  void test() {
    int n = 100;
    ExecutorService executor = Executors.newFixedThreadPool(n);
    CountDownLatch endLatch = new CountDownLatch(n);
    for (int i = 0; i < threadCount; i++) {
      executor.execute(
          () -> {
            try {
              ReviewLikeCommand command = new ReviewLikeCommand(memberId, reviewId);
              ReviewLikeResult result = reviewLikeService.likeReview(command);
            } catch (Exception e) {
              // ...
            } finally {
              endLatch.countDown(); // 끝난 스레드 카운트다운
            }
          });
    }
    endLatch.await(); // 모든 스레드가 끝날 때까지 대기
    executor.shutdown();
  }
}
```

