# JPA Projection

## DATA JPA Projection

### 인터페이스 기반의 Closed Projections

- 조회하기 원하는 칼럼의 이름으로 인터페이스를 만든다.

```java
@Entity
public class Account {
  private long id;
  private String name;
  private int deposit;
  
  @ManyToOne
  @JoinColumn(name = "user_id")
  private User user;
} 
```

```java
public interface AccountName {
    String getName();
} 
```

- 위와 같이 인터페이스를 선언하면 된다. 해당 타입을 Data JPA 레포지토리에 사용하면 자동으로 프록시 객체를 만들어준다!
- hibernate로 실행되는 쿼리를 확인해보면 name만 select되는 것을 확인할 수 있다. (쿼리 최적화가 가능)

```java
public interface AccountRepository extends JpaRepository<Account, Long> {
    List<AccountName> findAll();
}
```

- 아래와 같이 중첩된 구조의 프로젝션도 가능하다. 하지만 root에 위치한 필드와는 다르게, 연관된 엔티티의 모든 칼럼을 SELECT한다.
- 예시의 경우 Account 테이블에 대해서는 name 칼럼만 SELECT 하지만, User 테이블은 LEFT OUTER JOIN한 다음 모든 칼럼을 SELECT하고 필요한 칼럼을 따로 계산해서 프록시 객체에 넣어준다. 

```java
public interface AccountInfo {
  String getName();
  
  interface UserInfo {
    String getNickname();
  }
}
```

### 인터페이스 기반의 Open Projection

- @Value 어노테이션을 이용해 새로운 반환 속성 값을 만들 수 있다.
- 하지만 모든 칼럼을 SELECT 한 다음 계산을 진행하기 때문에 쿼리 최적화가 불가능하다. 

```java
public interface AccountInfo {
  @Value("#{target.name + ' ' + target.deposit + ' ' + target.user.nickname}")
  String getInfo();
}
```

### Class 기반의 Projections

- 아래와 같이 Dto를 만들어 Data JPA의 타입으로 조회하면 파라미터 이름과 그대로 매칭시킬 수 있다.

```java
public class AccountInfo {
  private String name;
  private int deposit;
  public AccountInfo(String name, int deposit) {
    this.name = name;
    this.deposit = deposit;
  }
}
```

```java
public interface AccountRepository extends JpaRepository<Account, Long> {
  List<AccountInfo> findAll();
}
```

## QueryDSL Projection

### 스칼라 타입

- 프로젝션의 대상이 필드값 하나라면 아래와 같이 타입을 맞춰 바로 받을 수 있다.
- 필드값을 2개 이상 받으면 리턴값이 Tuple이 되는데, DTO를 통해 받는 것이 좋다.

```java
List<String> nameList = queryFactory
    .select(account.name)
    .from(account)
    .fetch();
```
    
### 생성자 조회

- Projections의 static method인 `constructor`를 사용하는 방식이다.

```java
public class AccountInfo {
  private String name;
  private int deposit;
  public AccountInfo(String name, int deposit) {
    this.name = name;
    this.deposit = deposit;
  }
}
```

```java
List<AccountInfo> accountList = queryFactory
    .select(Projections.constructor(AccountInfo.class, account.name, account.deposit))
    .from(account)
    .fetch();
```

### Setter 조회

- setter를 사용해 매칭할 수 있다. 대신 아래의 유의사항이 있다.
  - setter 함수를 선언해야 한다.
  - DTO의 멤버변수와 entity의 필드명이 일치해야 한다.

```java
List<AccountInfo> accountList = queryFactory
    .select(Projections.bean(AccountInfo.class, account.name, account.deposit))
    .from(account)
    .fetch();
```

### 필드 조회

- filed를 직접 조회해서 사용할 수 있다.
- setter와 마찬가지로 DTO의 멤버 변수와 entity의 필드명이 일치해야 한다.

```java
List<AccountInfo> accountList = queryFactory
    .select(Projections.fields(AccountInfo.class, account.name, account.deposit))
    .from(account)
    .fetch();
```

- 아래와 같이 as()를 이용해 별칭 적용을 해 DTO의 멤버 변수 이름에 맞추는 방법도 있다.

```java
List<AccountInfo> accountList = queryFactory
    .select(Projections.fields(
        AccountInfo.class, 
        account.name,
        ExpressionUtils.as(
            JPAExpressions
                .select(user.nickname)
                .from(user), "username"
    )))
    .from(account)
    .fetch();
```

### @QueryProjection 어노테이션 사용

- `@QueryProjection` 어노테이션을 사용하면 DTO의 생성자를 본따서 Q클래스가 생성된다.

```java
public class AccountInfo {
  private String name;
  private int deposit;
  
  @QueryProjection
  public AccountInfo(String name, int deposit) {
    this.name = name;
    this.deposit = deposit;
  }
}
```

- 아래와 같이 Q클래스를 이용해 조회할 수 있다.

```java
List<AccountInfo> accountList = queryFactory
    .select(new QAccountInfo(AccountInfo.class, account.name, account.deposit))
    .from(account)
    .fetch();
```

- 컴파일러로 타입을 체크할 수 있기 때문에 가장 안전한 방법이다. 
- 하지만 DTO에 QueryDSL 어노테이션을 유지한다는 점, QueryDSL로 생성한 QClass가 필요하다는 점 때문에 지나치게 QueryDSL에 의존적이라는 단점이 있다. 