### 트랜잭션 전파 레벨이 NOT_SUPPORTED

- `MSSQL`에서는 하위 메소드에서 `@Transactional(readOnly = true)`가 달려있어도 저장이 됐음
- `MySQL`에서는 Connection이 readOnly라고 터진다.

### 내가 알고 있던 사실

- ID가 아래와 같이 `IDENTITY`로 선언되어 있을 때, `JpaRepository`의 `save`를 호출하면 `@Transactional(readOnly = true)`로 되어 있어도 저장되는 걸로 알고 있었는데..?

```java
@Id
@GeneratedValue(strategy = GenerationType.IDENTITY)
private Long id;
```

- 아래 내용의 출처는 영한님의 JPA책!

> IDENTITY 전략과 최적화
>
> - 엔티티에 식별자 값을 할당하려면 JPA는 추가로 데이터베이스를 조회해야 한다. (채번이 필요하기 때문에)
> - JDBC3에 추가된 Statement.getGeneratedKeys()를 사용하면 데이터를 저장하면서 동시에 생성된 기본 키 값도 얻어올 수 있다.

> 주의사항
>
> - 엔티티가 영속 상태가 되려면 식별자가 반드시 필요하다.
> - 그런데 IDENTITY 식별자 생성 전략은 엔티티를 데이터베이스에 저장해야 식별자를 구할 수 있으므로 em.persist()를 호출하는 즉시 INSERT SQL이 데이터베이스에 전달된다.
> - 따라서 이 전략은 쓰기 지연이 동작하지 않는다.
> - @Transaction(readOnly = true)를 걸어놔도 JpaRepository의 `save()` (혹은 EntityManger의 `em.persist()`)를 호출하면 그냥 저장이 되어버리니 주의하자.

### 가설1. 버전이 올라가면서 JPA 혹은 Spring, MySQL에 새로운 설정이 추가되었다?

- 원래 `@Transactional`은 스프링 레벨에서의 트랜잭션 제어를 하기 위해 사용된 설정이다.
- `@Transactional(readOnly = true)`도 JPA의 쓰기 지연를 활용해 저장을 안 했던 것이지 DB의 연결 자체를 readOnly로 잡는 게 아니었던 것 같은데??
- MySQL JPA 구현체에 새로운 기능이 추가된 것인가?????

### 가설2. NOT_SUPPPORTED는 프록시를 생성하지 않는다?

- 아직 확인은 하지 못함! (공식문서에도 NOT_SUPPORTED에 대한 설명이 없음.)
- `@Transactional(propagation = Propagation.***NOT_SUPPORTED***)`
- 위와 같이 선언되면 프록시 객체로 감싸지 않아 트랜잭션에 대한 설정이 각 자식들에게 위임된다.
- `ShippingInfoService.saveOf()`에 `@Transactional`이 걸려있지 않은 상태였고 따라서 제약 조건에 따라 저장이 되지 않았던 것!

