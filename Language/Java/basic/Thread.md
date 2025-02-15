# Thread

## Thread의 기본개념

- 스레드는 프로세스를 빼놓고 얘기할 수 없다.

### 프로세스 Process

- 프로세스(Process)는 일반적으로 cpu에 의해 메모리에 올려져 실행중인 프로그램을 말하며, 자신만의 메모리 공간을 포함한 독립적인 실행 환경을 가지고 있다.
- 자바 JVM(Java Virtual Machine)은 주로 하나의 프로세스로 실행되며, 동시에 여러 작업을 수행하기 위해서 멀티 스레드를 지원하고 있다.

### 스레드 Thread

- 스레드는 프로세스 안에서 실질적으로 작업을 실행하는 가장 작은 단위이다.
- 자바에서는 JVM에 의해 관리된다. 프로세스는 적어도 한 개 이상의 스레드를 가지고 있으며 Main 스레드 하나로 시작해, 스레드를 추가 생성하면 그게 멀티 스레드 환경이다.
- 스레드는 같은 프로세스 안에서 자원을 공유하기 때문에 컨텍스트 스위칭이 빨라 효율적인 편이다.

<br/>

## Java Thread란

- Java에서는 기본적으로 Thread를 상속하고 구현한 객체를 `new`로 할당할 수 있다.
- run 메소드를 오버라이드해 구현해야한다.

<img src="img/thread1.png" />

### 기본 메소드

- `run()`
  - 스레드의 실행 코드가 작성되는 메소드
  - 사용자는 run을 오버라이드하여 구현해야한다.
- `sleep()`
  - 현재 스레드를 멈춘다. 자원을 놓아주지 않고 제어권을 넘기기 때문에 병목현상이 발생할 수 있다.
-  `interrupt()`
  - 스레드가 일시 정지 상태에 있을 때 `InterruptedException`을 발생시킨다.
  - 이를 통해 run 메소드를 정상종료할 수 있다.
- `join()`
  - 다른 스레드의 작업이 끝날 때까지 기다리게 만든다.
  - 스레드의 순서를 제어하기 위해 사용한다.

<br/>

## Thread와 Runnable

```java
@FunctionalInterface
public interface Runnable {
    
    public abstract void run();
}
```

- `Thread`를 상속해 스레드를 만들 수도 있지만 `Runnable` 인터페이스를 구현할 수도 있다.
- 실행 방법이 약간 다르다. 
  - Thread 상속 객체의 경우, start() 메소드를 직접 호출할 수 있다.
  - Runnable 구현체는 `new Thread(Runnable runnable)` 형태로 객체를 생성해야 start() 메소드를 사용할 수 있다.

```java
public class ExampleThread implements Runnable {
  @Override
  public void run() {
    System.out.println("Thread Run");
  }
}

public class Main {

  public static void main(String[] args) {
    Runnable runnable = new ExampleThread();
    Thread thread = new Thread(runnable);
    thread.start();
  }
}
```

### Thread vs Runnable

<table>
  <thead>
    <th></th>
    <th>Runnable</th>
    <th>Thread</th>
  </thead>
  <tbody>
    <tr>
      <td>람다 가능 여부</td>
      <td>가능</td>
      <td>불가능</td>
    </tr>
    <tr>
      <td>구현형태</td>
      <td>implement => 다른 객체 상속 가능</td>
      <td>extends => 다른 객체 상속 불가</td>
    </tr>
    <tr>
      <td>자원 사용량</td>
      <td>적은 편</td>
      <td>많은 편</td>
    </tr>
  </tbody>
</table>


<br/>

# 대안

- `Thread`와 `Runnable`은 쓰레드를 생성할 때 너무 저수준의 API를 사용하고 작업이 끝난 후 결과값을 반환해주지 않는다.
- 또한 스레드를 사용하기 위해 항상 새롭게 스레드를 생성하고 종료시켜야하는데, 생성 비용이 큰 편이기 때문에 효율적이지 못하다는 단점이 있다.
- 그래서 대안으로 `Executor`, `ExecutorService`, `ScheduledExecutionService`, `Callable`, `Future`가 등장하게 되었다.

## Callable & Future

### Callable

- Java5부터 추가된 기능으로 `Runnable`의 발전된 형태이다.
- 제너릭을 사용해 결과값을 받을 수 있는 객체이다.

```java
@FunctionalInterface
public interface Callable<V> {
    V call() throws Exception;
}
```

### Future

- Java5부터 사용.
- 스레드의 작업은 기본적으로 비동기로 실행되기 때문에 실행 결과를 바로 얻지 못하고 예상치 못한 시점에 결과값을 받게 된다. (가용가능한 스레드가 없으면 더 늦춰지기도 한다.)
- 미래에 완료된 `Callable`의 반환값을 받기 위해 사용되는 것이 `Future`다. (javascript의 `await` 역할을 해줄 수 있다.)
- `ExecutorService`에서 `submit()` 메소드를 활용하면 `Future` 객체로 결과값을 받아올 수 있다.

```java
public interface Future<V> {

    boolean cancel(boolean mayInterruptIfRunning);

    boolean isCancelled();

    boolean isDone();

    V get() throws InterruptedException, ExecutionException;

    V get(long timeout, TimeUnit unit)
        throws InterruptedException, ExecutionException, TimeoutException;
}
```

- 하지만 여러 `Future`의 결괏값을 조합하거나, 예외를 효과적으로 핸들링할 수가 없다.
- 그리고 Future는 오직 get 호출로만 작업 완료가 가능한데, get은 작업이 완료될 때까지 대기하는 블로킹 호출이므로 비동기 작업 응답에 추가 작업을 하기 적합하지 않다.

### CompletableFuture

- Java8에서는 이런 문제들을 모두 해결한 `CompletableFuture`가 소개되었다.

```java
public class CompletableFuture<T> implements Future<T>, CompletionStage<T> {...}
```

- `CompletableFuture` 클래스는 `Future` 인터페이스를 구현함과 동시에 `CompletionStage` 인터페이스를 구현한다.
  - `CompletionStage`의 특징을 살펴보면 `CompletableFuture`의 장점을 알 수 있다.
- `CompletionStage`는 결국은 계산이 완료될 것이라는 의미의 약속이다.
  - 계산의 완료는 단일 단계의 완료뿐만 아니라 다른 여러 단게 혹은 다른 여러 단계 중의 하나로 이어질 수 있음도 포함한다.
  - 또한, 각 단계에서 발생한 에러를 관리하고 전달할 수 있다.

#### 기본 사용법

- `runAsync`는 반환 값이 없는 경우의 비동기 작업을 실행한다.

```java
CompletableFuture<Void> cf = CompletableFuture.runAsync(() -> System.out.println("Hello"));
cf.join(); // 실행된다.
```

- `supplyAsync`는 반환 값이 있을 때 비동기 작업을 실행한다.

```java
CompletableFuture<File> cf = CompletableFuture.runAsync(() -> saveFile(myFile));
File file = cf.join();
```

#### get과 join

- `get`
  - `Future` 인터페이스에 정의된 메소드다. 
  - Checked Exception인 `InterruptedException`과 `ExecutionException`을 던지므로 예외 처리 로직이 필요하다.
- `join`
  - `CompletableFuture` 인터페이스에 정의되어 있다.
  - Unchecked Exception인 `CompletionException`이 발생한다.

<br/>

## Executor & Executors & ExecutorService

- 객체 간의 관계

<img src="img/thread2.jpeg">

### Executor

- 애플리케이션에서 병렬 처리를 할 때마다 쓰레드를 생성하는 것은 굉장히 비효율적이다.
- 쓰레드를 미리 생성해 쓰레드 풀에서 가지고 있다가 대여해주는 식으로 구현한다면 쓰레드 생성 비용을 아낄 수 있다.
- `Executor`는 쓰레드 풀의 구현을 위한 인터페이스다.
- 인터페이스 분리 원칙에 맞게 등록된 작업을 실행하는 책임만 지니고 있다.

```java
public interface Executor {
  
  void execute(Runnable command);
}
```

- 만약 `Executor`를 상속받아 실행한다고 했을 때, 새로운 스레드를 생성하지 않고 아래와 같이 동작을 할당하면 메인 스레드에서 처리해버린다.

```java
class MyExecutor implements Executor {
  @Override
  public void execute(Runnable command) {
    command.run();
  }
}

public class Main {

  public static void main(String[] args) {
    Runnable runnable = () -> System.out.println("thread run");
    
    Executor executor = new MyExecutor();
    executor.excute(runnable);
  }
} 
```

- 따라서 메인 쓰레드가 아닌 다른 쓰레드를 이용해 동작을 실행시키려면 새로운 쓰레드를 만들어 동작하도록 변경해줘야 한다.

```java
class MyExecutor implements Executor {
  @Override
  public void execute(Runnable command) {
    new Thread(command).run();
  }
}
```

- <u>그렇다면 여기서 궁금증! 만약 Spring에서 `ThreadPoolTaskExecutor`을 사용해 스레드 풀을 만들고 `Executor`를 주입받는다고 했을 때, 그냥 Executor.excute()를 실행하면 메인스레드에서 실행될까? 쓰레드 풀에서 쓰레드를 받아올까?</u>

### ExecutorService

- `ExecutorService`는 `Executor`를 상속한 인터페이스로, `execute()` 메소드 외에도 비동기 작업을 지원, 스레드를 생성하고 생명주기를 관리하기 위한 메소드가 추가로 정의되어 있다.
- 예를 들어, 기존 `Executor.execute()` 메소드는 결과값에 전혀 관심이 없었지만, `ExecutorService`는 Future 형태로 작업을 묶어 작업이 완료된 후의 결과값을 획득하는 메소드를 지원해준다.
- 아래 그림과 같이 `ExecutorService`는 Thread Pool과 Blocking Queue로 구성되어 있다. Task들이 제출(Submit)되면 Queue에 입력되고 순차적으로 쓰레드에 할당된다. 만약 쓰레드 풀의 쓰레드가 전부 대여 중이라면 Task는 Queue 안에서 머물게 된다. 

<img src="img/thread3.jpeg">

- Task가 Queue에서 대기하고 있다가 Thread1과 Thread2에서 순차적으로 처리되는 것을 확인할 수 있다.

<img src="img/thread4.gif">

- ExecutorService에서 

```java
package java.util.concurrent;

import java.util.Collection;
import java.util.List;

public interface ExecutorService extends Executor {

  /**
   * 생명주기 관리
   */
  void shutdown();
    
  List<Runnable> shutdownNow();
  
  boolean isShutdown();
  
  boolean isTerminated();
  
  boolean awaitTermination(long timeout, TimeUnit unit)
      throws InterruptedException;
  
  /**
   * 비동기 처리
   */
  <T> Future<T> submit(Callable<T> task);
    
  <T> Future<T> submit(Runnable task, T result);
  
  Future<?> submit(Runnable task);
  
  <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks)
      throws InterruptedException;
  
  <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit)
      throws InterruptedException;
  
  <T> T invokeAny(Collection<? extends Callable<T>> tasks)
      throws InterruptedException, ExecutionException;
  
  <T> T invokeAny(Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit)
      throws InterruptedException, ExecutionException, TimeoutException;
}

```

- 비동기 작업을 처리하기 위해 `ExecutorService`는 아래와 같은 메소드를 사용할 수 있다.
  - `submit()`
  - `invokeAll()`
  - `invokeAny()`
- 생명주기를 관리하기 위한 메소드는 다음과 같다.
  - `shutdown()`
  - `shutdownNow()`
  - `isShutdown()`
  - `isTerminated()`
  - `awaitTermination()`

- `CompletableFuture`를 통해 비동기를 처리하는 로직이다.
  - `supplyAsync`에 생성한 ThreadPool을 전달하여 비동기로 처리할 수 있다.
  - `allOf`는 List<Future<?>>가 한꺼번에 실행되게 해준다.

```java
public class FileService {
  public List<File> uploadFile(List<MultipartFile> multipartFiles) {
    ExecutorService executorService = Executor.newFixedThreadPool(20);

    try {
      List<Future<File>> futures = multipartFiles.stream()
              .map(file -> CompletableFuture.supplyAsync(() -> save(file), executorService))
              .toList();

      CompletableFuture<List<File>> allOf =
              CompletableFuture
                      .allOf(futures.toArray(new CompletableFuture[futures.size()]))
                      .thenApply(v -> futures.stream()
                              .map(CompletableFuture::join)
                              .collect(Collectors.toList()));
      return allOf.join();
    } finally {
      // 해당 executorService를 다른 곳에서 사용할 것이 아니라면 shutdown 메소드로 종료해줘야 한다.
      // 그렇지 않으면 해당 프로세스가 끝나지 않고 계속해서 다음 작업을 기다리게 된다.
      executorService.shutdown();
    }
  }
  
  private File save(MultipartFile file) {
    ...
  }
}
```

### Executors



<br/>


<br/>

## 


# 참고자료

- [[Java] Thread와 Runnable에 대한 이해 및 사용법](https://mangkyu.tistory.com/258)
- [Interrupt Method](https://cornswrold.tistory.com/190)
- [자바: Thread 클래스와 Runnable 인터페이스](https://www.daleseo.com/java-thread-runnable/)
- [[Java] Callable, Future 및 Executors, Executor, ExecutorService, ScheduledExecutorService에 대한 이해 및 사용법](https://mangkyu.tistory.com/259) 
- [Java Concurrency: Executor와 Callable/Future](https://javacan.tistory.com/entry/134)
- [[JAVA8 병렬프로그래밍] Executors 클래스, ExecutorService 인터페이스](https://devfunny.tistory.com/807)
- [[Java] CompletableFuture 사용법](https://dev-coco.tistory.com/185)
