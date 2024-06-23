# 06 다양한 연관관계 매핑

- 엔티티의 연관관계를 매핑할 때는 다음 3가지를 고려해야 한다.
1. 다중성
2. 단방향, 양방향
3. 연관관계의 주인

### 다중성

- 연관관계에는 아래와 같은 다중성이 있다.
  - 다일일(`@ManyToOne`)
  - 일대다(`@OneToMany`)
  - 일대일(`@OneToOne`)
  - 다대다(`@ManyToMany`)
- 다중성을 판단하기 어려울 때는 반대방향을 생각해보면 된다.
  - 일대다의 반대방향은 항상 다대일
  - 일대일의 반대방향은 항상 일대일

### 단방향, 양방향

- 테이블은 외래 키 하나로 조인을 사용해서 양방향으로 쿼리가 가능하므로 사실상 방향이라는 개념이 없다.
- 반면에 객체는 참조용 필드를 가지고 있는 객체만 연관된 객체를 조회할 수 있다.
  - 객체 관계에서 한 쪽만 참조하는 것을 단방향 관계
  - 양쪽이 서로 참조하는 것을 양방향 관계

### 연관관계의 주인

- 데이터베이스는 외래 키 하나로 두 테이블이 연관관계를 맺는다.
  - 따라서 테이블의 연관관계를 관리하는 포인트는 외래 키 하나다.
- 반면 엔티티를 양방향에서 매핑하면 `A->B`, `B->A` 2곳에서 서로를 참조한다.
  - 즉, 객체의 연관관계를 관리하는 포인트가 2곳이다.
- 외래 키를 가진 테이블과 매핑한 엔티티가 외래 키를 관리하는 게 효율적이므로 보통 이곳을 연관관계의 주인으로 선택한다.
  - 주인이 아닌 방향은 외래 키를 변경할 수 없고 읽기만 가능하다.
  - 연관관계의 주인은 `mappedBy` 속성을 사용하지 않는다. (반대로 연관관계의 주인이 아닌 엔티티에서는 `mappedBy`로 매핑한다.)

<br/>

## 1. 다대일 N:1

- `Member`: `Team`이 다대일 관계이다.

### 다대일 단방향

- `Membe`r는 `Team`을 참조할 수 있지만 `Team`은 `Member`를 참조할 수 없다.
- 연관관계의 주인인 `Member`에서 `@JoinColumn(name = "team_id")`을 통해 `team` 테이블의 외래 키와 매핑했다.

```java
@Entity
public class Member {

  @Id
  @GeneratedValue
  private Long id;

  private String name;

  @ManyToOne
  @JoinColumn(name = "team_id")
  private Team team;

  public void setTeam(Team team) {
    this.team = team;
  }
}
```

```java
@Entity
public class Team {

  @Id
  @GeneratedValue
  private Long id;

  private String name;
}
```

### 다대일 양방향

- 연관관계의 주인인 `Member`에서 `@JoinColumn(name = "team_id")`을 통해 `team` 테이블의 외래 키와 매핑했다.
- 연관관계의 주인이 아닌 `Team`에서는 `mappedBy`를 통해 객체를 매핑한다.
- 양방향 연관관계는 항상 서로를 참조해야 한다. 어느 한 쪽만 참조하면 양방향 연관관계가 성립하지 않는다.

```java
@Entity
public class Member {

  @Id
  @GeneratedValue
  private Long id;

  private String name;

  @ManyToOne
  @JoinColumn(name = "team_id")
  private Team team;

  public void setTeam(Team team) {
    this.team = team;
  }
}
```

```java
@Entity
public class Team {

  @Id
  @GeneratedValue
  private Long id;

  private String name;
  
  @OneToMany(mappedBy = "team")
  private List<Member> members = new ArrayList<>();
}
```

<br/>

## 2. 일대다 1:N

- `Team`: `Member`가 일대다 관계다.

### 일대다 단방향

- 일대다 단방향 관계를 매핑할 때는 `@JoinColumn`을 명시해야 한다.

```java
@Entity
public class Team {

  @Id
  @GeneratedValue
  private Long id;

  private String name;
  
  @OneToMany
  @JoinColumn(name = "team_id") // Member 테이블의 team_id 외래 키(FK)
  private List<Member> members = new ArrayList<>();
}
```

```java
@Entity
public class Member {
  
  @Id
  @GeneratedValue
  private Long id;
  
  private String name;
}
```

- 일대다 단방향 매핑의 단점
  - 일대다 단방향 매핑의 단점은 매핑한 객체가 관리하는 외래 키가 다른 테이블에 있다는 점이다.
  - 본인 테이블에 외래 키가 있으면 엔티티의 저장과 연관관계 처리를 INSERT SQL 한 번으로 끝낼 수 있지만, 다른 테이블에 외래 키가 있으면 연관관계를 처리하기 위한 UPDATE SQL을 별도로 실행해야 한다.
- 일대다 단방향 매핑보다는 다대일 양방향 매핑을 사용하자.
  - 일대다 단방향 매핑은 다른 테이블의 외래 키를 관리해야 한다. 성능 문제도 있지만 관리도 부담스럽다.
  - 그냥 다대일 양방향 매핑을 사용하자.

### 일대다 양방향

- 일대다 양방향은 존재하지 않는다. 대신 다대일 양방향 매핑을 사용해야 한다.
- 더 정확히 말하자면 양방향 매핑에서 `@OneToMany`는 연관관계의 주인이 될 수 없다.
  - 관계형 데이터베이스 특성상 `1:N`, `N:1` 관계는 항상 `N` 쪽에 외래 키가 있다.  
  - 그런 이유로 `@ManyToOne`에는 `mappedBy` 속성이 없다.
- `N` 쪽의 엔티티의 `@JoinColumn`에 `insertable=false`, `updatable=false` 옵션을 줘서 읽기 전용으로 만드는 방법이 있지만.. 되도록 사용하지 말자. 

<br/>

## 일대일 1:1

- 일대일 관계는 주 테이블이나 대상 테이블 둘 중 어느 곳이나 외래 키를 가질 수 있다.

### 주 테이블에 외래 키

- 외래 키를 객체 참조와 비슷하게 사용할 수 있어서 객체지향 개발자들이 선호한다.
- 이 방법의 장점은 주 테이블이 외래 키를 가지고 있으므로 주 테이블만 확인해도 대상 테이블과 연관관계가 있는지 알 수 있다.

- 일대일 단방향 (주 테이블이 연관관계 주인)

```java
@Entity
public class Member {
  ...
  
  @OneToOne
  @JoinColumn(name = "locker_id")
  private Locker locker;
}

@Entity
public class Locker {
  ...
}
```

- 일대일 양방향 (주 테이블이 연관관계 주인)

```java
@Entity
public class Member {
  ...
  
  @OneToOne
  @JoinColumn(name = "locker_id")
  private Locker locker;
}

@Entity
public class Locker {
  ...
  
  @OneToOne(mappedBy = "locker")
  private Member member;
}
```

### 대상 테이블에 외래 키

- 전통적인 데이터베이스 개발자들은 보통 대상 테이블에 외래 키를 두는 것을 선호한다.
- 이 방법의 장점은 테이블 관계를 일대일에서 일대다로 변경할 때 테이블 구조를 그대로 유지할 수 있다는 점이다.

- 일대일 단방향 (대상 테이블이 연관관계 주인) 
  - 이런 경우는 JPA에서 지원하지 않는다.
  - 매핑할 수 있는 방법도 없다.
  - 이 때는 단방향 관계를 `Locker`에서 `Member` 방향으로 수정하거나, 양방향 관계로 만들고 `Locker`를 연관관계 주인으로 설정해야 한다.

- 일대일 양방향 (대상 테이블이 연관관계 주인)

```java
@Entity
public class Member {
  ...

  @OneToOne(mappedBy = "member")
  private Locker locker;
}

@Entity
public class Locker {
  ...

  @OneToOne
  @JoinColumn(name = "member_id")
  private Member member;
}
```

<br/>

## 4. 다대다 N:N

- 관계형 데이터베이스는 정규화된 테이블 2개로 다대다 관계를 표현할 수 없다.
  - 보통 다대다 관계를 일대다, 다대일 관계로 풀어내는 연결 테이블을 사용한다.
  - 다대다 관계를 만들기 위해 3개의 테이블이 필요하다는 의미다. 
- 그런데 객체는 테이블과 다르게 객체 2개로 다대다 관계를 만들 수 있다.
  - 서로 컬렉션을 통해 참조를 하면 되기 때문이다.

### 다대다 단방향

- `@JoinTable.name`: 연경 테이블을 지정한다.
- `@JoinTable.joinColumns`: 현재 방향인 주 테이블과 매핑할 조인 컴럼 정보를 지정한다.
- `@JoinTable.inverseJoinColumns`: 반대 방향인 대상 테이블과 매핑할 조인 컬럼 정보를 지정한다.

```java
@Entity
public class Member {
  ...
  
  @ManyToMany
  @JoinTable(
      name = "member_product",
      joinColumns = @JoinColumn(name = "member_id"),
      inverseJoinColumns = @JoinColumn(name = "product_id"))
  private List<Product> products = new ArrayList<>();
}
```

```java
@Entity
public class Product {
  ...
}
```

### 다대다 양방향

- 위의 관계에서 `Product` 쪽에 `mappedBy`만 추가된 형태다.

```java
@Entity
public class Member {
  ...
  
  @ManyToMany
  @JoinTable(
      name = "member_product",
      joinColumns = @JoinColumn(name = "member_id"),
      inverseJoinColumns = @JoinColumn(name = "product_id"))
  private List<Product> products = new ArrayList<>();
}
```

```java
@Entity
public class Product {
  ...
  
  @ManyToMany(mappedBy = "products")
  private List<Member> members;
}
```

### 다대다: 매핑의 한계와 극복, 연결 엔티티 사용

- `@ManyToMany`를 사용하면 연결 테이블을 자동으로 처리해주므로 도메인 모델이 단순해지고 편리하다.
- 하지만 실무에서 이 매핑을 사용하기에는 한계가 있다.
  - `@ManyToMany`를 사용하면 중간 테이블이 자동 생성되기 때문에 연결 테이블에 비즈니스 로직 상 필요한 필드를 넣을 수 없다.
- 차라리 연결 엔티티를 만드는 것이 좋다!

```java
@Entity
public class Member {
  ...
  
  @OneToMany(mappedBy = "member")
  private List<MemberProduct> memberProducts; 
}
```

```java
@Entity
public class Product {
  ...
  
  @OneToMany(mappedBy = "product")
  private List<MemberProduct> memberProducts; 
}
```

```java
@Entity
@IdClass(MemberProductId.class)
public class MemberProduct {
  
  @Id
  @ManyToOne
  @JoinColumn(name = "member_id")
  private Member member;

  @Id
  @ManyToOne
  @JoinColumn(name = "product_id")
  private Product product;
  
  // 비즈니스 로직을 위한 필드 추가 가능
  private int orderAmount;
}
```

- 연결 테이블을 위한 식별자 클래스를 구성할 수 있다.
  - 외래 키 2개로 만드는 식별자 키

```java
public class MemberProductId implements Serializable {
  
  private String member;  // MemberProduct.member 와 연결
  private String product; // MemberProduct.product 와 연결
  
  // equals and hashCode
}
```

- 복합 기본 키
  - JPA에서 복합 키를 사용하려면 별도의 식별자 클래스를 만들어야 한다.
  - 그리고 엔티티에 `@IdClass`를 사용해서 식별자 클래스를 지정하면 된다.
- 복합 기본 키 주의사항
  - 복합 키는 별도의 식별자 클래스로 만들어야 한다.
  - `Serializable`을 구현해야 한다.
  - `equals`, `hashCode`를 구현해야 한다.
  - 기본 생성자가 있어야 한다.
  - 식별자 클래스가 `public class`여야 한다.
  - `@IdClass`를 지정하는 방법 외에 `@EmbeddedId`를 사용하는 방법이 있다.
- 식별 관계
  - 관계를 위한 외래 키를 자신의 기본 키로 사용하는 것을 데이터베이스 용어로 식별 관계(identifying relationship)이라고 한다.

### 다대다: 새로운 기본 키 사용 (복합 기본 키를 사용하지 않는 방법)

- 연결 테이블에서 기본 키를 만들어 사용하는 방법이다. (비식별 관계)
- 객체 입장에서 보면 비식별 관계를 사용하는 것이 복합 키를 위한 식별자 클래스를 만들지 않아도 되므로 단순하고 편리하게 ORM 매핑을 할 수 있다.
  - 이런 이유로 식별 관계보다는 비식별 관계를 추천한다.
- 아래 예시는 `Member` <-> `Order` <-> `Product`의 관계를 코드로 표현한 것이다.
  - 여기서 `Order`가 연결 테이블 역할을 한다.

```java
@Entity
public class Order {
  @Id
  @GeneratedValue
  private Long id;
  
  @ManyToOne
  @JoinColumn(name = "member_id")
  private Member member;
  
  @ManyToOne
  @JoinColumn(name = "product_id")
  private Product product;
  
  private int orderAmount;
}
```

```java
@Entity
public class Member {
  ...
  
  @OneToMany(mappedBy = "member")
  private List<Order> orders = new ArrayList<>(); 
}
```

```java
@Entity
public class Product {
  ...
  
  @OneToMany(mappedBy = "product")
  private List<Order> orders = new ArrayList<>();
}
```

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
