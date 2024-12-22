# 07 고급 매핑

- 상속 관계 매핑
- `@MappedSuperclass`: 공통으로 사용하는 매핑 정보 상속
- 복합 키와 식별 관계 매핑: 식별자가 하나 이상일 때 매핑하는 방법
- 조인 테이블: 연결 테이블을 매핑하는 방법
- 엔티티 하나에 여러 테이블 매핑

## 1. 상속 관계 매핑

- 관계형 데이터베이스에서는 객체지향 언에서 다루는 상속이라는 개념이 없다.
  - 대신 슈퍼타입 서브타입 관계(`Super-Type Sub-Type Relationship`)라는 모델링 기법이 그나마 가장 유사하다.
- 따라서, ORM에서 이야기 하는 상속 관계 매핑은 객체의 상속 구조와 데이터베이스의 슈퍼타입 서브타입 관계를 매핑하는 것이다.
- 슈퍼타입 서브타입 논리 모델을 실제 물리 모델인 테이블로 구현할 때는 3가지 방법을 선택할 수 있다.
  - 각각의 테이블로 변환: 조인 전략
  - 통합 테이블로 변환: 단일 테이블 전량
  - 서브타입 테이블로 변환: 구현 클래스마다 테이블 전략

### 조인 전략

- 엔티티 각각을 모두 테이블로 만들고 자식 테이블이 부모 테이블의 기본 키를 받아서 기본 키 + 외래 키로 사용하는 전략이다.
  - 조회할 때 조인을 자주 사용한다.
- 주의할 점
  - 객체는 타입으로 구분할 수 있지만 테이블은 타입의 개념이 없다. 따라서 타입을 구분하는 컬럼을 추가해야 한다.
  - DTYPE 컬럼을 구분 컬럼으로 사용한다.

```java
@Entity
@Inheritance(strategy = IngeritanceType.JOINED)
@DiscriminatorColumn(name = "DTYPE")
public abstract class Item {
  
  @Id
  @GeneratedValue
  private Long id;
  
  private String name;
  private int price;
}

@Entity
@DisciminatorValue("A")
public class Album extends Item {
  private String artist;
}

@Entity
@DiscriminatorValue("M")
public class Movie extends Item {
  private String director;
  private String actor;
}
```

1. `@Inheritance(strategy = InheritanceType.JOINED)`: 상속 매핑은 부모 클래스에 `@Inheritance` 어노테이션을 사용해야 한다.
   - `InheritanceType`은 `JOINED`, `SINGLE_TABLE`, `TABLE_PER_CLASS`이 있다.
2. `@DiscriminatorColumn(name = "DTYPE")`: 부모 클래스에 구분 컬럼을 지정한다. 기본값은 `DTYPE`이다.
3. `@DiscriminatorValue("A")`: 엔티티를 저장할 때 구분 컬럼(`DTYPE`)에 입력할 값을 지정한다.

- 기본 값으로 자식 테이블은 부모 테이블의 ID 컬럼 명을 그대로 사용하는데 만약 자식 테이블의 기본 키 컬럼명을 변경하고 싶으면 `@PrimaryKeyJoinColumn`을 사용한다.

```java
@Entity
@DiscriminatorValue("B")
@PrimaryKeyJoinColumn(name = "BOOK_ID") // ID 재정의
public class Book extends Item {
  private String author;
  private String isbn;
}
```

- 조인 전략의 장점
  - 테이블이 정규화된다.
  - 외래 키 참조 무결성 제약조건을 활용할 수 있다.
  - 저장공간을 효율적으로 사용한다.
- 조인 전략의 단점
  - 조회할 때 조인이 많이 사용되므로 성능이 저하될 수 있다.
  - 조회 쿼리가 복잡하다.
  - 데이터를 등록할 INSERT SQL을 두 번 실행한다.
- 조인 전략의 특징
  - JPA 표준 명세는 구분 컬럼을 사용하도록 하지만 하이버네이트를 포함한 몇몇 구현체는 구분 컬럼(`@DiscriminatorColumn`) 없이도 동작한다.
  - 관련 어노테이션: `@DiscriminatorColumn`, `@DiscriminatorValue`, `@PrimaryKeyJoinColumn` 

### 단일 테이블 전략

- 단일 테이블 전략은 테이블은 단 하나만 쓴다.
- 조회할 때 조인을 사용하지 않으므로 가장 빠르다.
- 이 전략을 사용할 때 주의점은 자식 엔티티가 매핑한 컬럼은 모두 null을 허용해야 한다는 점이다.
  - 테이블 하나에 모든 것을 통합하기 때문에 구분 컬럼을 필수로 사용해야 한다.

```java
@Entity
@Inheritance(strategy = InheritanceType.SINGLE_TABLE)
@DiscriminatorColumn(name = "DTYPE")
public abstract class Item {
  @Id
  @GeneratedValue
  private Long id;

  private String name;
  private int price;
}

@Entity
@DiscriminatorValue("A")
public class Album extends Item {...}

@Entity
@DiscriminatorValue("M")
public class Movie extends Item {...}

@Entity
@DiscriminatorValue("B")
public class Book extends Item {...}
```

- 단일 테이블 전략의 장점
  - 조인이 필요 없으므로 일반적으로 조회 성능이 빠르다.
  - 조회 쿼리가 단순하다.
- 단일 테이블 전략의 단점
  - 자식 엔티티가 매핑한 컬럼은 모두 null을 허용해야 한다.
  - 단일 테이블에 모든 것을 저장하므로 테이블이 커질 수 있다. 그러므로 상황에 따라서는 조회 성능이 오히려 느려질 수 있다.
- 단일 테이블 전략의 특징
  - 구분 컬럼을 꼭 사용해야 한다. (`@DiscriminatorColumn`)
  - `@DiscriminatorValue`를 지정하지 않으면 기본으로 엔티티 이름을 사용한다. (ex. Album, Movie, Book)

### 구현 클래스마다 테이블 전략

- 자식 엔티티마다 테이블을 만든다.
  - 데이터베이스 설계자와 ORM 전문가 둘 다 추천하지 않는 전략이다.
  - 조인 전략이나 단일 테이블 전략을 고려하자.

```java
@Entity
@Inheritance(strategy = InheritanceType.TABLE_PER_CLASS)
public abstract class Item {
  @Id
  @GeneratedValue
  private Long id;

  private String name;
  private int price;
}

@Entity
@DiscriminatorValue("A")
public class Album extends Item {...}

@Entity
@DiscriminatorValue("M")
public class Movie extends Item {...}

@Entity
@DiscriminatorValue("B")
public class Book extends Item {...}
```

- 구현 클래스마다 테이블 전략 장점
  - 서브 타입을 구분해서 처리할 때 효과적이다.
  - not null 제약조건을 사용할 수 있다.
- 구현 클래스마다 테이블 전략 단점
  - 여러 자식 테이블을 함께 조회할 때 성능이 느리다. (SQL에 UNION을 사용해야 한다.)
  - 자식 테이블을 통합해서 쿼리하기 어렵다.
- 구현 클래스마다 테이블 전략 특징
  - 구분 컬럼을 사용하지 않는다.

<br/>

## 2. `@MappedSuperclass`

- 부모 클래스는 테이블과 매핑하지 않고 부모 클래스를 상속 받는 자식 클래스에게 매핑 정보만 제공하고 싶으면 `@MappedSuperclass`를 사용하면 된다.
- `@MappedSuperclass`는 추상 클래스와 비슷하다. (직접 생성할 일은 거의 없으므로 추상 클래스로 만들자!!)
  - `@Entity`는 실제 테이블과 매핑되지만, `@MappedSuperclass`는 실제 테이블과 매핑되지 않는다.
  - 단순히 매핑 정보를 상속할 목적으로만 사용된다.

```java
@MappedSuperclass
public abstract class BaseEntity {
  @Id
  @GeneratedValue
  private Long id;
}

@Entity
public class Member extends BaseEntity {
  
  private String name;
}
```

- 부모로부터 물려받은 매핑 정보를 재정의하려면 `@AttributeOverrides`나 `@AttributeOverride`를 사용한다.
- 부모로부터 물려받은 연관관계를 재정의하려면 `@AssociationOverrides`, `@AssociationOverride`를 사용한다.

```java
@Entity
@AttributeOverrides({
    @AttributeOverride(name = "id", column = @Column(name = "member_id"))
})
public class Member extends BaseEntity {...}
```

<br/>

## 3. 복합 키와 식별 관계 매핑

### 식별 관계 vs 비식별 관계

- 데이터베이스 테이블 사이에 관계는 외래 키가 기본 키에 포함되는지 여부에 따라 식별 관계와 비식별 관계로 구분한다.

### 식별 관계 Identifying Relationship

- 부모 테이블의 기본 키를 내려받아서 자식 테이블의 기본 키 + 외래 키를 사용하는 관계

### 비식별 관계 Non-Identifying Relationship

- 부모 테이블의 기본 키를 받아서 자식 테이블의 외래 키로만 사용하는 관계
- 필수적 비식별 관계(`Mandatory`): 외래 키에 NULL을 허용하지 않는다. 연관관계를 필수적으로 맺어야 한다.
- 선택적 비식별 관계(`Optional`): 외래 키에 NULL을 허용한다. 연관관계를 맺을지 말지 선택할 수 있다.

## 3-1. 복합 키: 비식별 관계 매핑

- 둘 이상의 컬럼으로 구성된 복합 기본 키는 다음처럼 `@Id` 여러 개를 사용해 매핑하면 오류가 발생한다!

```java
@Entity
public class MyClass {
  @Id private String id1;
  @Id private String id2; // 실행 시점에 매핑 예외 발생
} 
```

- 따라서 별도의 식별자 클래스를 만들어야 한다.
  - JPA는 식별자를 구분하기 위해 `equals`와 `hashCode`를 사용해 동등성 비교를 한다. => 식별자 클래스에서 구현 필수
- 또한 복합 키를 지원하기 위해 `@IdClass`와 `@EmbeddedId` 2가지 방법을 제공한다.
  - `@IdClass`: 관계형 데이터베이스에 가까운 방식
  - `@EmbeddedId`: 객체지향에 가까운 방식

### `@IdClass`

```java
@Entity
@IdClass(ParentId.class)
public class Parent {
  @Id
  @Column(name = "parent_id1")
  private String id1; // ParentId.id1과 연결
  
  @Id
  @Column(name = "parent_id2")
  private String id2; // ParentId.id2과 연결
}
```

```java
public class ParentId implements Serializable {
  private String id1;
  private String id2;
  
  public ParentId() {}

  public ParentId(String id1, String id2) {
    this.id1 = id1;
    this.id2 = id2;
  }
  
  @Override
  public boolean equals(Object o) {...}

  @Override
  public int hashCode() {...}
}
```

- `@IdClass`를 사용할 때 아래 조건을 만족해야 한다.
  - 식별자 클래스와 속성명(혹은 필드명)과 엔티티에서 사용하는 식별자의 속성명(혹은 필드명)이 같아야 한다.
  - `Serializable` 인터페이스를 구현해야 한다.
  - `equals`, `hashCode`를 구현해야 한다.
  - 기본 생성자가 있어야 한다.
  - 식별자 클래스는 `public`이어야 한다.

- 복합 키를 사용하는 엔티티 저장은 아래와 같이 이뤄진다.
  - 식별자 클래스인 `ParentId`가 보이지 않는데 `em.persist()`를 호출하면 영속성 컨텍스트에 엔티티를 등록하기 직전에 내부에서 `Parent.id1`, `Parent.id2` 값을 사용해 식별자 클래스인 `ParentId`를 생성하고 영속성 컨텍스트의 키로 사용한다.

```java
String id1 = "id1";
String id2 = "id2";
Parent parent = new Parent(id1, id2);
em.persist(parent);
```

- 복합 키를 통한 조회는 아래와 같다.
  - 식별자 클래스인 ParentId를 사용해 엔티티를 조회한다.

```java
String id1 = "id1";
String id2 = "id2";
ParentId parentId = new ParentId(id1, id2);
Parent parent = em.find(Parent.class, parentId);
```

- 자식 클래스에 추가하는 방법은 아래와 같다.
  - 부모 테이블의 기본 키 컬럼이 복합 키이므로 자식 테이블의 외래 키도 복합 키다.
  - 따라서 외래 키 매핑 시 여러 컬럼을 매핑해야 하는데, `@JoinColumns`과 `@JoinColumn`을 사용해 매핑한다.
  - 예제에는 `referencedColumnName`을 적었지만 `name` 속성과 이름이 같다면 생략할 수 있다.

```java
@Entity
public class Child {
  @Id
  private String id;
  
  @ManyToOne
  @JoinColumns({
          @JoinColumn(name = "parent_id1", referencedColumnName = "parent_id1"),
          @JoinColumn(name = "parent_id2", referencedColumnName = "parent_id2")
  })
  private Parent parent;
}
```

### `@EmbeddedId`

- `@IdClass`와는 다르게 `@EmbeddedId`를 적용한 식별자 클래스는 식별자 클래스에 기본 키를 직접 매핑한다.
- `@EmbeddedId`를 적용한 식별자 클래스는 아래 조건을 만족해야 한다.
  - `@Embeddable`이 식별자 클래스에 붙어있어야 한다.
  - `Serializable` 인터페이스를 구현해야 한다.
  - `equals`, `hashCode`를 구현해야 한다.
  - 기본 생성자가 있어야 한다.
  - 식별자 클래스는 `public`이어야 한다.

```java
@Entity
public class Parent {
  @EmbeddedId
  private ParentId id; 
}
```

```java
@Embeddable
public class ParentId implements Serializable {
  @Column(name = "parent_id1")
  private String id1;
  @Column(name = "parent_id2")
  private String id2;
  
  public ParentId() {}

  public ParentId(String id1, String id2) {
    this.id1 = id1;
    this.id2 = id2;
  }
  
  @Override
  public boolean equals(Object o) {...}

  @Override
  public int hashCode() {...}
}
```

- `@EmbeddedId`가 붙은 엔티티를 저장하는 예시는 아래와 같다.
  - 식별자 클래스를 직접 생성해 엔티티에 넣어준다.

```java
ParentId parentId = new ParentId("id1", "id2");
Parent parent = new Parent(parentId);
em.persist(parent);
```

- 조회 코드도 식별자 클래스를 직접 사용한다.

```java
ParentId parentId = new ParentId("id1", "id2");
Parent parent = em.find(Parent.class, parentId);
```

### 복합 키와 `equals()`, `hashCode()`

- 영속성 컨텍스트는 엔티티의 식별자를 키로 사용해 엔티티를 관리한다. (Map 형태의 저장소!)
- 그리고 식별자를 비교하기 위해 `equals()`와 `hashCode()`를 사용한다.
- 동등성이 지켜지지 않으면 예상과 다른 엔티티가 조회되거나 엔티티를 찾을 수 없게되는 등 심각한 문제가 발생할 수 있다.
- 식별자 클래스에서 `equals()`와 `hashCode()`를 구현할 땐 모든 필드를 사용하자!

### `@IdClass` vs `@EmbeddedId`

- 본인 취향에 맞는 걸 일관성 있게 사용하면 된다.
- `@EmbeddedId`가 `@IdClass`와 비교해서 더 객체 지향적이고 중복도 없다.
  - 하지만 특정 상황에서 JPQL이 조금 더 길어질 수 있다. (`@EmbeddedId`가 식별자 객체를 참조하는 방식이기 때문!)

## 3-2. 복합 키: 식별 관계 매핑

### `@IdClass`와 식별 관계

```java
@Entity
public class Parent {
  @Id
  @Column(name = "parent_id")
  private String id;
}

@Entity
@IdClass(ChildId.class)
public class Child {
  @Id
  @ManyToOne
  @JoinColumn(name = "parent_id")
  public Parent parent;
  
  @Id
  @Column(name = "child_id")
  private String childId;
}

public class ChildId implements Serializable {
  private String parent; // Child.parent mapping
  private String childId; // Child.childId mapping 

  // equals, hashCode
}

// 손자
@Entity
@IdClass(GrandChildId.class)
public class GrandChild {
  @Id
  @ManyToOne
  @JoinColumns({
          @JoinColumn(name = "parent_id"),
          @JoinColumn(name = "child_id")
  })
  private Child child;
  
  @Id
  @Column(name = "grand_child_id")
  private String id;
}

public class GrandChildId implements Serializable {
  private ChildId childId; // GrandChild.child mapping
  private String id; // GrandChild.childId mapping 

  // equals, hashCode
}
```

### `@EmbeddedId`와 식별 관계

```java
@Entity
public class Parent {
  @Id
  @Column(name = "parent_id")
  private String id;
}

@Entity
public class Child {
  @EmbeddedId
  public ChildId id;
  
  @MapsId("parentId") // ChildId.parentId mapping
  @ManyToOne
  @JoinColumn(name = "parent_id")
  public Parent parent;
}

@Embeddable
public class ChildId implements Serializable {
  private String parentId; // @MapsId("parentId") mapping
  
  @Column(name = "child_id")
  private String childId; // Child.childId mapping
  
  // equals, hashCode
}

// 손자
@Entity
public class GrandChild {

  @EmbeddedId
  private GrandChildId id;

  @MapsId("childId") // GrandChildId.childId mapping
  @ManyToOne
  @JoinColumns({
          @JoinColumn(name = "parent_id"),
          @JoinColumn(name = "child_id")
  })
  private Child child;
 
  ...
}

public class GrandChildId implements Serializable {
  private ChildId childId; // @MapsId("childId") mapping
  
  @Column(name = "grand_child_id")
  private String id; // GrandChild.childId mapping 

  // equals, hashCode
}
```

## 3-4. 비식별 관계로 구현

- 식별 관계의 복합 키를 사용한 코드와 비교하면 매핑도 쉽고 코드도 단순하다.
- 복합 키가 없으므로 복합 키 클래스를 만들지 않아도 된다.

```java
@Entity
public class Parent {
  @Id
  @Column(name = "parent_id")
  private String id;
}

@Entity
public class Child {
  
  @Id
  @GeneratedValue
  @Column(name = "child_id")
  public ChildId id;
  
  @ManyToOne
  @JoinColumn(name = "parent_id")
  public Parent parent;
}

// 손자
@Entity
public class GrandChild {

  @Id
  @GeneratedValue
  @Column(name = "grand_child_id")
  private GrandChildId id;

  @ManyToOne
  @JoinColumn(name = "child_id")
  private Child child;
 
  ...
}
```

## 3-5. 일대일 식별 관계

- 조금 특별한 관계다.
- 자식 테이블의 기본 키 값으로 부모 테이블의 기본 키 값만 사용한다. 
- 그래서 부모 테이블의 기본 키가 복합 키가 아니면 자식 테이블의 기본 키는 복합 키로 구성하지 않아도 된다.

```java
@Entity
public class Board {
  
  @Id
  @GeneratedValue
  @Column(name = "board_id")
  private Long id;
  
  @OneToOne(mappedBy = "board")
  private BoardDetail boardDetail;
}

@Entity
public class BoardDetail {
  
  @Id
  private Long boardId;
  
  @MapsId // BoardDetail.boardId 매핑
  @OneToOne
  @JoinColumn(name = "board_id")
  private Board board;
}
```

- `BoardDetail`처럼 식별자가 단순히 컬럼 하나면 `@MapsId`를 사용하고 속성 값은 비워두면 된다.

## 3-6. 식별, 비식별 관계의 장단점

### 데이터베이스 설계 관점에서 보면 다음과 같은 이유로 식별 관계보다는 비식별 관계를 선호한다.

- 식별 관계는 부모 테이블의 기본 키를 자식 테이블로 전파하면서 자식 테이블의 기본 키 컬럼이 점점 늘어난다. 결국 조인할 때 SQL이 복잡해지고 기본 키 인덱스가 불필요하게 커질 수 있다.
- 식별 관계는 2개 이상의 컬럼을 합해서 복합 기본 키를 만들어야 하는 경우가 많다.
- 식별 관계를 사용할 때 기본 키로 비즈니스 의미가 있는 자연 키 컬럼을 조합하는 경우가 많다. 반면에 비식별 관계의 기본 키는 비즈니스와 전혀 관계없는 대리 키를 주로 사용한다.
  - 비즈니스 요구사항은 시간이 지남에 따라 언제가는 변한다. 식별 관계의 자연 키 컬럼들이 자식에 손자까지 전파되면 변경하기 힘들다.
- 식별 관계는 부모 테이블의 기본 키를 자식 테이블의 기본 키로 사용하므로 비식별 관계보다 테이블 구조가 유연하지 못하다.

### 객체 관계 매핑의 관점에서 비식별 관계를 선호하는 이유

- 일대일 관계를 제외하고 식별 관계는 2개 이상의 컬럼을 묶은 복합 기본 키를 사용한다. JPA에서 복합 키는 별도의 복합 키 클래스로 만들어서 사용해야 한다. 따라서 컬럼이 하나인 기본 키를 매핑하는 것보다 많은 노력이 필요하다.
- 비식별 관계의 기본 키는 주로 대리 키를 사용하는데 JPA는 `@GeneratedValue`처럼 대리 키를 생성하기 위한 편리한 방법을 제공한다.

### 하지만 식별 관계가 가지는 장점도 있다.

- 기본 키 인덱스를 활용하기 좋다.
- 상위 테이블들의 기본 키 컬럼을 자식, 손자 테이블들이 가지고 있으므로 특정 상황에서 조인 없이 하위 테이블만으로 검색을 완료할 수 있다.

<br/>

## 4. 조인 테이블

- 데이터베이스 테이블의 연관관계를 설계하는 방법은 2가지다.
1. 조인 컬럼 사용 (외래 키)
2. 조인 테이블 사용 (테이블 사용)

### 조인 테이블 사용

- 조인 테이블이라는 별도의 테이블을 사용해서 연관관계를 관리하는 방법이다.
- 조인 컬럼은 단순히 외래 키 컬럼만 추가해서 연관관계를 맺지만 조인 테이블을 사용하는 방식은 연관관계를 관리하는 조인 테이블을 추가하고 여기서 두 테이블의 외래 키를 가지고 연관관계를 관리한다.
- 가장 큰 단점은 테이블을 하나 추가해야 한다는 점이다.
  - 따라서 관리해야 하는 테이블이 늘어나고 회원과 사물함 두 테이블을 조인하려면 테이블까지 추가로 조인해야 한다.
- 객체와 테이블을 매핑할 때 
  - 조인 컬럼은 `@JoinColumn`으로 매핑
  - 테이블은 `@JoinTable`로 매핑
- 조인 테이블은 주로 N:N 관계를 1:N, N:1 관계로 풀어내기 위해 사용한다.
  - 하지만 1:1, N:1, 1:N 관계에서도 사용한다.

## 4-1. 일대일 조인 테이블

- 일대일 관계를 만들려면 조인 테이블의 외래 키 컬럼에 각각에 총 2개의 유니크 제약조건을 걸어야 한다.

```java
@Entity
public class Parent {
  
  @Id
  @GeneratedValue
  @Column(name = "parent_id")
  private Long id;
  
  @OneToOne
  @JoinTable(
      name = "parent_child",
      joinColumns = @JoinColumn(name = "parent_id"),
      inverseJoinColumns = @JoinColumn(name = "child_id"))
  private Child child;
}

@Entity
public class Child  {
  
  @Id
  @GeneratedValue
  @Column(name = "child_id")
  private Long id;
  
  // 양방향 매핑하려면 추가
  @OneToOne(mappedBy = "child")
  private Parent parent;
}
```

- 부모 엔티티를 확인해보자.
  - `@JoinTable` 어노테이션을 사용한 것을 볼 수 있다.
- `@JoinTable`의 속성
  - `name`: 매핑할 조인 테이블 이름 
  - `joinColumns`: 현재 엔티티를 참조하는 외래 키
  - `inverseJoinColumns`: 반대 방향 엔티티를 참조하는 외래 키

## 4-2. 일대다 조인 테이블

- 조인 테이블 중 N에 해당하는 테이블과 관련된 `child_id`에 유니크 제약 조건을 걸어야 한다.
  - 사실 `child_id`는 기본 키므로 이미 유니크 제약 조건이 걸려 있을 것이다.

```java
@Entity
public class Parent {
  
  @Id
  @GeneratedValue
  @Column(name = "parent_id")
  private Long id;
  
  @OneToMany
  @JoinTable(
      name = "parent_child",
      joinColumns = @JoinColumn(name = "parent_id"),
      inverseJoinColumns = @JoinColumn(name = "child_id"))
  private List<Child> child = new ArrayList<>();
}

@Entity
public class Child  {
  
  @Id
  @GeneratedValue
  @Column(name = "child_id")
  private Long id;
  
  // 양방향 매핑하려면 추가
  @OneToOne(mappedBy = "child")
  private Parent parent;
}
```

## 4-3 다대일 조인 테이블

```java
@Entity
public class Parent {
  
  @Id
  @GeneratedValue
  @Column(name = "parent_id")
  private Long id;
  
  @OneToMany(mappedBy = "parent")
  private List<Child> child = new ArrayList<>();
}

@Entity
public class Child  {
  
  @Id
  @GeneratedValue
  @Column(name = "child_id")
  private Long id;
  
  @ManyToOne(optional = false)
  @JoinTable(
          name = "parent_child",
          joinColumns = @JoinColumn(name = "parent_id"),
          inverseJoinColumns = @JoinColumn(name = "child_id"))
  private Parent parent;
}
```

## 4-4 다대다 조인 테이블

- 다대다 관계를 만들려면 조인 테이블의 두 컬럼을 합해서 하나의 복합 유니크 제약 조건을 걸어야 한다.

```java
@Entity
public class Parent {
  
  @Id
  @GeneratedValue
  @Column(name = "parent_id")
  private Long id;
  
  @ManyToMany
  @JoinTable(
          name = "parent_child",
          joinColumns = @JoinColumn(name = "parent_id"),
          inverseJoinColumns = @JoinColumn(name = "child_id"))
  private List<Child> child = new ArrayList<>();
}

@Entity
public class Child  {
  
  @Id
  @GeneratedValue
  @Column(name = "child_id")
  private Long id;
}
```

> 참고
> - 조인 테이블에 컬럼을 추가하면 @JoinTable 전략을 사용할 수 없다.
> - 대신 새로운 엔티티를 만들어서 조인 테이블과 매핑하도록 하자.

<br/>

## 5. 엔티티 하나에 여러 테이블 매핑

- 잘 사용하지는 않는 기능이다.
- `@SecondaryTable`을 사용하면 한 엔티티에 여러 테이블을 매핑할 수 있다.

```java
@Entity
@Table(name = "board")
@SecondaryTable(
    name = "board_detail",
    pkJoinColums = @PrimaryKeyJoinColumn(name = "board_detail_id"))
public class Board {
  
  @Id
  @GeneratedValue
  @Column(name = "board_id")
  private Long id;
  
  @Column(table = "board_detail")
  private String content;
}
```

- `Board` 엔티티는 `@Table`을 사용해 `board` 테이블과 매핑했다.
- 그리고 `@SecondaryTable`을 사용해 `board_detail` 테이블을 추가로 매핑했다.

### `@SecondaryTable`의 속성

- `@SecondaryTable.name`: 매핑할 다른 테이블의 이름. 예제에서는 테이블명을 `board_detail`로 지정했다.
- `@SecondaryTable.pkJoinColumn`: 매핑할 다른 테이블의 기본 키 컬럼 속성. 예제에서는 기본 키 컬럼명을 `board_detail_id`로 지정했다.
- 이렇게 매핑한 `board_detail`을 `@Column(table = "board_detail")`와 같이 사용해서 `board_detail` 테이블의 컬럼에 매핑했다.

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
