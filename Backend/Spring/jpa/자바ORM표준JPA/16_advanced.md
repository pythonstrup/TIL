# 15장 고급 주제와 성능 최적화

## 1. 예외처리

### 1-1. JPA 표준 예외

- JPA 표준 예외들은 `jakarta.persistence.PersistenceException`의 자식 클래스다.
    - 또 이 예외 클래스는 `RuntimeException`의 자식이다.
    - 따라서 JPA 예외는 모두 언체크 예외다.
- JPA 표준 예외는 크게 2가지로 나눌 수 있다.
    1. 트랜잭션 롤백을 표시하는 예외
    2. 트랜잭션 롤백을 표시하지 않는 예외
- 트랜잭션 롤백을 표시하는 예외는 심각한 예외이므로 복구해선 안 된다.
    - 이 예외가 발생하면 트랜잭션을 강제로 커밋해도 트랜잭션이 커밋되지 않고 대신에 `jakarta.persistence.RollbackException` 예외가 발생한다.
- 반면 트랜잭션 롤백을 표시하지 않는 예외는 심각한 예외가 아니므로 개발자가 트랜잭션을 커밋할지 롤백할지를 판단하면 된다.

#### 트랜잭션 예외를 표시하는 예외

|Exception|Description|
|:--:|:--:|
|`jakarta.persistence.EntityExistsException`|`EntityManaber.persist()` 호출 시 이미 같은 엔티티가 있으면 발생한다.|
|`jakarta.persistence.EntityNotFoundException`|`EntityManager.getReference()`를 호출했는데 실제 사용 시 엔티티가
존재하지 않으면 발생한다. `refresh()`, `lock()`에서도 발생한다.|
|`jakarta.persistence.OptimisticLockException`|낙관적 락 충돌 시 발생한다.|
|`jakarta.persistence.PessimisticLockException`|비관적 락 충돌 시 발생한다.|
|`jakarta.persistence.RollbackException`|`EntityTransaction.commit()` 실패 시 발생한다. 롤백이 표시되어 있는 트랜잭션 커밋
시에도 발생한다.|
|`jakarta.persistence.TransactionRequiredException`|트랜잭션이 필요할 때 트랜잭션이 없으면 발생한다. 트랜잭션 없이 엔티티를 변경할 때
주로 발생한다.|

#### 트랜잭션 롤백을 표시하지 않는 예외

|Exception|Description|
|:--:|:--:|
|`jakarta.persistence.NoResultException`|`Query.getSingleResult()`를 호출했는데 결과가 하나도 없으면 발생한다.|
|`jakarta.persistence.NonUniqueResultException`|`Query.getSingleResult()`를 호출했는데 결과가 둘 이상이면 발생한다.|
|`jakarta.persistence.LockTimeoutException`|비관적 락에서 시간 초과 시 발생한다.|
|`jakarta.persistence.QueryTimeoutException`|쿼리 실행 시간 초과 시 발생한다.|

### 1-2. 스프링 프레임워크의 JPA 예외 변환

- 서비스 계층에서 데이터 접근 계층의 구현 기술에 직접 의존하는 것은 좋은 설계라고 할 수 없다.
    - 이건 예외도 마찬가지다.
- 예를 들면 `jakarta.persistence.PersistenceException`는 `org.springframework.ormjpa.JpaSystemException`로
  변환한다.

### 1-3. 스프링 프레임워크에 JPA 예외 변환기 적용

- JPA 예외를 스프링 프레임워크가 제공하는 추상화된 예외로 변경하려면 `PersistenceExceptionTranslationPostProcessor`를 스프링 빈으로
  등록하면 된다.

```java

@Configuration
public class JpaConfig {

  @Bean
  public PersistenceExceptionTranslationPostProcessor exceptionTranslation() {
    return new PersistenceExceptionTranslationPostProcessor();
  }
}
```

### 1-4 트랜잭션 롤백 시 주의사항

- 트랜잭션을 롤백하는 것은 데이터베이스의 반영사항만 롤백하는 것이지 수정한 자바 객체까지 원상태로 복구해주지는 않는다.
- 따라서 트랜잭션이 롤백된 영속성 컨텍스트를 그대로 사용하는 것은 위험하다.
- 새로운 영속성 컨텍스트를 생성해 사용하거나 `EntityManager.clear()`를 호출해서 영속성 컨텍스트를 초기화한 다음에 사용해야 한다.
    - 특히 OSIV를 사용할 때 문제가 된다.

<br/>

## 2. 엔티티 비교

- 영속성 컨텍스트 내부에는 엔티티 인스턴스를 보관하기 위한 1차 캐시가 있다. 이 1차 캐시는 영속성 컨텍스트와 생명주기를 같이 한다.
- 영속성 컨텍스를 통해 데이터를 저장하거나 조회하면 1차 캐시에 엔티티가 저장된다.
  - 이 1차 캐시 덕분에 변경 감지 기능도 동작하고 이름 그대로 1차 캐시로 사용되어 데이터베이스를 통하지 않고 데이터를 바로 조회할 수 있다.
- 영속성 컨텍스트를 더 정확히 이해하기 위해서는 1차 캐시의 가장 큰 장점인 **애플리케이션 수준의 반복 가능한 읽기**를 이해해야 한다.
  - 같은 영속성 컨텍스트에서 엔티티를 조회하면 항상 같은 엔티티 인스턴스를 반환한다.
  - 단순히 동등성 비교 수준이 아니라 정말 주소값이 같은 인스턴스를 반환한다.

### 2-1. 영속성 컨텍스트가 같을 때 엔티티 비교

- 영속성 컨텍스트가 같으면 엔티티를 비교할 때 다음 3가지 조건이 모두 만족한다.
1. 동일성 identical: `==` 비교가 같다.
2. 동등성 equivalent: `equals()` 비교가 같다.
3. 데이터베이스 동등성: @Id인 데이터베이스 식별자가 같다.

### 2-2. 영속성 컨텍스트가 다를 때 엔티티 비교

- 영속성 컨텍스트가 다를 때 엔티티 비교는 아래와 같다.
1. 동일성 비교에 실패한다.
2. 동등성 비교를 성공하기 위해 `equals()`를 구현해야 한다. (비즈니스 키)
3. 데이터베이스 동등성은 만족한다.

<br/>

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
