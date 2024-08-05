# 16장 트랜잭션과 락, 2차 캐시

## 1. 트랜잭션과 락

### 1-1. 트랜잭션과 격리 수준

#### 트랜잭션 ACID 원칙

- `원자성 Atomicity`: 트랜잭션 내에서 실행한 작업들은 마치 하나의 작업인 것처럼 모두 성공하거나 실패해야 한다.
- `일관성 Consistency`: 모든 트랜잭션은 일관성 있는 데이터베이스 상태를 유지해야 한다. 예를 들어 데이터베이스에서 정한 무결성 제약 조건을 항상 만족해야 한다.
- `격리성 Isolation`: 동시에 실행되는 트랜잭션들이 서로에게 영향을 미치지 않도록 격리한다. 예를 들어 동시에 같은 데이터를 수정하지 못하도록 해야 한다. 격리성은 동시성과 관련된 성능 이슈로 인해 격리 수준을 선택할 수 있다.
- `지속성 Durability`: 트랜잭션을 성공적으로 끝내면 그 결과가 항상 기록되어야 한다. 중간에 시스템에 문제가 발생해도 데이터베이스 로그 등을 사용해서 성공한 트랜잭션 내용을 복구해야 한다.ㄴ

#### 격리 수준

- 트랜잭션 격리 수준이 아래와 같이 4가지를 지원한다.
1. READ UNCOMMITTED
2. READ COMMITTED
3. REPEATABLE READ
4. SERIALIZABLE

> - 트랜잭션 격리 수준에 따른 동작 방식은 데이터베이스마다 조금씩 다르다.
> - 데이터베이스들이 더 많은 동시성을 처리하기 위해 락보다는 MVCC를 사용하므로 락을 사용하는 데이터베이스와 양간 다른 특성을 지닌다.

### 1-2. 낙관적 락과 비관적 락

#### 낙관적 락

- `낙관적 락`은 이름 그대로 트랜잭션 대부분은 충돌이 발생하지 않는다고 낙관적으로 가정하는 방법이다.
- JPA가 제공하는 버전 관리 기능을 사용해 처리할 수 있다. (애플리케이션이 제공하는 락)
- 낙관적 락은 트랜잭션을 커밋하기 전까지는 트랜잭션의 충돌을 알 수 없다는 특징이 있다.

#### 비관적 락

- `비관적 락`은 이름 그대로 트랜잭션의 충돌이 발생한다고 가정하고 우선 락을 걸고 보는 방법이다.
- 데이터베이스가 제공하는 락 기능을 사용한다.
- 대표적으로 `select for update` 구문이 있다.

#### 두 번의 갱신 분실 문제 second lost updates problem

- 두 번의 갱신 분실 문제는 데이터베이스 트랜잭션의 범위를 넘어선다. 트랜잭션만으로는 문제를 해결할 수 없다는 얘기다.
- 이때는 아래 3가지 방법 중 하나를 선택할 수 있다.

1. **마지막 커밋만 인정하기**
2. **최초 커밋만 인정하기**
3. **충돌되는 갱신 내용 병합하기**

- 기본적으로 **마지막 커밋만 인정하기** 방식이 사용된다.
- JPA가 제공하는 버전 관리 기능을 사용해 손쉽게 **최초 커밋만 인정하기** 기능을 구현할 수 있다.

### 1-3. @Version

- `@Version`의 적용 가능 타입은 아래와 같다.
  - `Lon`
  - `Integer`
  - `Short`
  - `Timestamp`
- 아래는 코드 예시이다.

```java
@Entity
public class Board {
  @Id
  private String id;
  private String title;
  
  @Version
  private Integer version;
}
```

- 위와 같이 `@Version`을 붙여 놓으면 엔티티를 수정할 때마다 버전이 하나씩 자동으로 증가한다.
- 그리고 엔티티를 수정할 때 조회 시점의 버전과 수정 시점의 버전이 다르면 예외가 발생한다.

<img src="img/optimistic_lock01.jpg">

- 트랜잭션2가 커밋되면 `version`이 2로 올라간 상태로 데이터베이스에 저장된다.
- 트랜잭션1에서 엔티티를 조회할 때와 버전(`1`)과 데이터베이스의 버전(`2`)이 다르므로 예외가 발생한다.
- 따라서 버전 정보를 사용하면 **최초 커밋만 인정하기**가 적용된다.

#### 버전 정보 비교 방법

- 아래와 같은 쿼리로 비교를 진행하게 된다.

```sql
UPDATE BOARD
SET
    TITLE=?,
    VERSION=? # (버전 +1 증가)
WHERE
    ID=?
    AND VERSION # (버전 비교)
```

- 데이터베이스 버전과 엔티티 버전이 같으면 데이터를 수정하면서 동시에 버전도 하나 증가시킨다.
- 만약 데이터베이스에 버전이 이미 증가해서 수정 중인 엔티티의 버전과 다르면 UPDATE 쿼리의 WHERE 문에서 VERSION 값이 다르므로 수정할 대상이 없다.
  - 이때는 버전이 이미 증가한 것으로 판단해서 JPA가 예외를 발생시킨다.
- `@Version`으로 추가한 버전 관리 필드는 JPA가 직접 관리하므로 개발자가 임의로 수정하면 안 된다.
- 만약 버전 값을 강제로 증가하려면 특별한 락 옵션을 선택하면 된다.

### 1-4. JPA 락 사용

- 락은 다음 위치에 적용할 수 있다.
  - `EntityManager.lock()`
  - `EntityManager.find()`
  - `EntityManager.refresh()`
  - `Query.setLockMode()` => `TypeQeury`도 포함
  - `@NamedQuery`

- 아래는 즉시 락을 거는 방법이다.

```java
em.find(Board.class, id, LockModeType.OPTIMISTIC);
```

- 반면 아래와 같이 필요할 때 락을 걸 수도 있다.

```java
Board board = em.find(Board.class, id);
...
em.lock(board, LockModeType.OPTIMISTIC);
```

### 1-5. JPA 낙관적 락

- JPA가 제공하는 낙관적 락은 `@Version`을 사용한다.
- 낙관적 락에서 발생하는 예외는 아래와 같다.
  - `jakarta.persistence.OptimisticLockException` (JPA 예외)
  - `org.hibernate.StaleObjectStateException` (하이버네이트 예외)
  - `org.springframework.orm.ObjectOptimisticLockingFailureException` (스프링 예외 추상화)

#### LockModeType.NONE

- 락 옵션을 적용하지 않아도 엔티티에 `@Version`이 적용된 필드만 있으면 낙관적 락이 적용된다.

#### LockModeType.OPTIMISTIC

- `@Version`만 적용했을 때는 엔티티를 수정해야 버전을 체크하지만 이 옵션을 추가하면 엔티티를 조회만 해도 버전을 체크한다.
- 쉽게 이야기해서 한 번 조회한 엔티티는 트랜잭션을 종료할 때까지 다른 트랜잭션에서 변경하지 않음을 보장한다.

#### LockModeType.OPTIMISTIC_FORCE_INCREMENT

- 낙관적 락을 사용하면서 버전 정보를 강제로 증가시킨다.
- 엔티티를 수정하지 않아도 버전 정보를 강제로 증가시킨다. 이때 데이터베이스의 버전이 엔티티의 버전과 다르면 예외가 발생한다.

### 1-6. JPA 비관적 락

- JPA가 제공하는 비관적 락은 데이터베이스 트랜잭션 락 매커니즘에 의존하는 방법이다.
- 주로 SQL 쿼리에 `select for update` 구문을 사용하면서 시작하고 버전 정보는 사용하지 않는다.
- 비관적 락 모드는 주로 `PESSIMISTIC_WRITE` 모드를 사용한다.

#### 비관적 락의 특징

- 엔티티가 아닌 스칼라 타입을 조회할 때도 사용할 수 있다.
- 데이터를 수정하는 즉시 트랜잭션 충돌을 감지할 수 있다.

#### PESSIMISTIC_WRITE

- 비관적 락이라 하면 일반적으로 이 옵션을 뜻한다. 데이터베이스에 쓰기 락을 걸 때 사용한다.

#### PESSIMISTIC_READ

- 데이터를 반복 읽기만 하고 수정하지 않는 용도로 락을 걸 때 사용한다. 일반적으로 잘 사용하지 않는다.

#### PESSIMISTIC_FORCE_INCREMENT

- 비관적 락 중 유일하게 버전 정보를 사용한다.
- 비관적 락이지만 버전 정보를 강제로 증가시킨다.

#### 비관적 락과 타임아웃

- 비관적 락을 사용하면 락을 획득할 때까지 트랜잭션이 대기한다.
- 무한정 대기할 수는 없으므로 타임아웃 시간을 줄 수 있다.

```java
Map<String, Object> properties = new HashMap<String, Object>();
properties.put("jakarta.persistence.lock.timeout", 10000); // 타임아웃 10초로 설정
Board board = em.find(Board.class, id, LockModeType.PESSIMISTIC_WRITE, properties);
```

<br/>

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음

