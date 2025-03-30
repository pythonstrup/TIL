# DB Connection Semaphore

## 가상 스레드의 문제

- 가상 스레드의 가장 큰 장점이자 문제점. 요청이 들어오면 무제한으로 생긴다는 점.
  - Blocking I/O를 최소화할 수 있어서 좋지만.. 
  - 기존에 플랫폼 스레드의 동작 방식에 의한 Throttle이 안 되기 때문에 `Overwhelming` 현상(다량 요청에 의한 DB 커넥션 타임아웃 현상)이 발생할 수 있다.

- Throttle이 된다는 게 무슨 소리인가? 
  - 기존 톰캣의 HTTP Connector Pool은 최대 스레드 수가 제한되어 있어서 DB 커넥션 사용에 적절한 Waiting 유지가 가능했다.
  - 하지만 가상스레드를 톰캣 커넥터 풀에 적용하고 HTTP 요청을 다량으로 받으면, DB 커넥션을 풀에 대한 DB Connection 요청이 과다하게 요청되게 된다.

### Virtual Thread의 제약

- Kakao tech의 techmeet. "JDK 21의 신기능 Virtual Thread 알아보기" 발표에서는 아래와 같은 Virtual Thread의 제약 조건을 제시한다.

<img src="img/connection_and_virtual_thread01.png">

## 문제의 해결

- 자바에서 동시성을 처리해주는 `세마포어 Semaphore`를 사용하여 문제를 해결

### AOP 활용하기

```kotlin
@Pointcut("execution(* com.zaxxer.hikari..HikariDataSource.getConnection(..))")
fun hikariDataSource() = Unit

@Around("hikariDataSource()")
fun around(joinPoint: ProceedingJoinPoint): Any? {
  try {
    semaphore.acquire()
    return joinPoint.proceed()
  } finally {
  semaphore.release()
}
```

---

# Connector/J와 Virtual Thread

##  mysql jdbc의 문제

- mysql jdbc를 지원하는 `Connector/J`의 `connectionImpl` 클래스를 보면 `synchronized` 천지라서 스레드 피닝이 발생한다고 함.

### 스레드 피닝이란?

- `synchronized` 사용시 Virtual Thread에 연결된 Carrier Thread가 `Blocking` 될 수 있다고 한다.
  - 이때의 블로킹 현상을 `pinning`이라고 한다.
- 그래서 가상 스레드를 사용한다면 `synchronized`보다는 `ReentrantLock`을 사용하길 권장한다.
- 그런데 문제는 라이브러리에서 `synchronized`를 사용하면 스레드 피닝이 발생한다는 것이다.
  - 문제의 라이브러리 ex) mysql jdbc

### MySQL에서는 과연 가상 스레드를 지원하지 않을까?

- `MySQL Connector/J 9.0.0` 버전 이상으로 업그레이드를 진행하면 된다.
- `9.0.0` 버전의 릴리스 노트를 보면 아래와 같이 적혀있다.

> #### 참고
> - Functionality Added or Changed
>   - Synchronized blocks in the Connector/J code were replaced with ReentrantLocks. This allows carrier threads to unmount virtual threads when they are waiting on IO operations, making Connector/J virtual-thread friendly. Thanks to Bart De Neuter and Janick Reynders for contributing to this patch. (Bug #110512, Bug #35223851)

- `Connector/J`는 스레드 피닝을 없애기 위해 `synchronized`를 `ReentrantLock`으로 대체하는 작업을 진행했다.
- 아래와 같이 `Connector/J` 의존성을 9.0.0 버전 이상으로 설정해주자.

```kotlin
implementation("com.mysql:mysql-connector-j:9.0.0")
```

# 참고자료

- [JDK 21의 신기능 Virtual Thread 알아보기](https://www.youtube.com/watch?v=vQP6Rs-ywlQ)
- [Virtual Thread & Hikari & Semaphore](https://blo-gu.tistory.com/55)
- [MySQL 공식문서 - Connector/J 9.0.0 Release Note](https://dev.mysql.com/doc/relnotes/connector-j/en/news-9-0-0.html)
