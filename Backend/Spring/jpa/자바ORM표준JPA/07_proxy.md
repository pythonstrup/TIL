# 08 프록시와 연관관계 관리

### 프록시와 즉시 로딩, 지연로딩

- 객체는 객체 그래프로 연관된 객체들을 탐색한다. 그런데 객체가 데이터베이스에 저장되어 있으므로 연관된 객체를 마음껏 탐색하기는 어렵다.
- JPA 구현체는 이 문제를 해결하려고 프록시라는 기술을 사용한다. 프록시를 사용하면 연관된 객체를 처음부터 데이터베이스에서 조회하는 것이 아니라 실제 사용하는 시점에 데이터베이스에서 조회할 수 있다.
- 하지만 자주 사용하는 객체들은 조인을 사용해서 함께 조회하는 것이 효과적이다.

### 영속성 전이와 고아 객체

- JPA는 연관된 객체를 함께 저장(영속성 전이)하거나 함께 삭제(고아 객체 제거)할 수 있다.

<br/>

## 1. 프록시

- 엔티티를 조회할 때 연관된 엔티티가 항상 사용되는 것은 아니다.
- JPA는 엔티티가 실제 사용될 때까지 데이터베이스 조회를 지연하는 방법을 제공하는데 이것을 지연 로딩(Lazy Loading)이라고 한다.

> 참고
> - JPA 표준 명세는 지연 로딩의 구현 방법을 JPA 구현체에 위임했다.
> - 하이버네이트는 지연 로딩을 지원하기 위해 `프록시`를 사용하는 방법과 `바이트코드`를 수정하는 두 가지 방법을 제공한다.
> - `바이트코드`를 수정하는 방법은 설정이 복잡하고 `프록시`는 별다른 설정 없이 사용할 수 있다.

## 1-1 프록시 기초

```java
Member member = em.find(Member.class, "member1");
```

- JPA는 엔티티를 직접 조회하면 조회한 엔티티를 실제 사용하든 사용하지 않든 데이터베이스를 조회하게 된다.
- 엔티티를 실제 사용하는 시점까지 데이터베이스 조회를 미루고 싶으면 `EntityManger.getReference()` 메소드를 사용할 수 있다.

```java
Member member = em.getReference(Member.class, "member1");
```

- `getReference()` 메소드를 호출할 때 JPA는 데이터베이스를 조회하지 않고 실제 엔티티 객체도 생성하지 않는다.
- 대신 데이터베이스 접근을 위임한 프록시 객체를 반환한다.

### 프록시의 특징

- 프록시 클래스는 실제 클래스를 상속 받아서 만들어진다.
  - 사용자 입장에서는 이것이 진짜 객체인지 프록시 객체인지 구분하지 않고 사용할 수 있다.
- 프록시 객체는 실제 객체에 대한 참조(target)를 보관한다. 그리고 프록시 객체의 메소드를 호출하면 프록시 객체는 실제 객체의 메소드를 호출한다.

### 프록시 객체의 초기화

- 프록시 객체는 `member.getName()` 처럼 실제 값을 사용할 때 데이터베이스를 조회해 실제 엔티티 객체를 생성하는데 이것을 **프록시 객체의 초기화**라고 한다.

```java
// 프록시 클래스 예상 코드
class MemberProxy extends Member {
  Member target = null; // 실제 엔티티를 참조하는 필드
  
  // 1. 아래 메소드가 호출되면 
  public String getName() {
    if (target == null) {
      // 2. 초기화 요청
      // 3. DB 조회
      // 4. 실제 엔티티 생성 및 참조 보관
      this.target = ...;
    }
    return target.getName(); // 5. target.getName();
  }
}
```

1. 프록시 객체에 `member.getName()`을 호출해 실제 데이터를 조회한다.
2. 프록시 객체는 실제 엔티티가 생성되어 있지 않으면 영속성 컨텍스트에 실제 엔티티 생성을 요청하는데 이것을 초기화라고 한다.
3. 영속성 컨텍스트는 데이터베이스를 조회해서 실제 엔티티 객체를 생성한다.
4. 프록시 객체는 생성된 실제 엔티티 객체의 참조를 `Member target` 필드에 보관한다.
5. 프록시 객체느느 실제 엔티티 객체의 `getName()`을 호출해서 결과를 반환한다.

### 다시, 프록시의 특징

- 프록시 객체는 처음 사용할 때 한 번만 초기화된다.
- 프록시 객체를 초기화한다고 프록시 객체가 실제 엔티티로 바뀌는 것은 아니다. 프록시 객체가 초기화되면 프록시 객체를 통해서 실제 엔티티에 접근할 수 있다.
- 프록시 객체는 원본 엔티티를 상속받은 객체이므로 타입 체크 시 주의해서 사용해야 한다.
- 영속성 컨텍스트에 찾는 엔티티가 이미 있으면 데이터베이스를 조회할 필요가 없으므로 `em.getReference()`를 호출해도 프록시가 아닌 실제 엔티티를 반환한다.
- 초기화는 영속성 컨텍스트의 도움을 받아야 가능하다. 따라서 영속성 컨텍스트의 도움을 받을 수없는 준영속 상태의 프록시를 초기화하면 문제가 발생한다.
  - 하이버네이트의 `org.hibernate.LazyInitializationException` 예외가 발생한다.ㄴ

> 참고
> - JPA 표준 명세는 지연 로딩에 대한 내용을 JPA 구현체에 맡겼다.
> - 따라서 준영속 상태의 엔티티를 초기화할 때 어떤 일이 발생할지 표준 명세에는 정의되어 있지 않다.

## 1-2. 프록시와 식별자

- 엔티티를 프록시로 조회할 때 식별자 값을 파라미터로 전달하는데 프록시 객체는 이 식별자 값을 보관한다.
- 프록시 객체는 식별자 값을 가지고 있으므로 식별자 값을 조회하는 (예를 들어, `getId()`) 메소드를 호출해도 프록시를 초기화하지 않는다.
- 엔티티 접근 방식을 프로퍼티(`@Access(AcessType.PROPERTY)`)로 설정한 경우에는 초기화하지 않는다.
- 엔티티 접근 방식을 필드(`@Access(AccessType.FIELD)`)로 설정하면 JPA는 getId() 메소드가 id만 조회하는 메소드인지 다른 필드까지 활용해서 어떤 일을 하는 메소드인지 알지 못하기 때문에 프록시 객체를 초기화한다.
  - 하지만 연관관계를 설정할 때는 엔티티 접근 방식을 필드로 해도 프록시를 초기화하지 않는다.

## 1-3. 프록시 확인

- JPA가 제공하는 `PersistenceUnitUtil.isLoaded(Object entity)` 메소드를 사용하면 프록시 인스턴스의 초기화 여부를 확인할 수 있다.
- 아직 초기화되지 않은 프록시 인스턴스는 `false`를 반환한다. 이미 초기화되었거나 프록시 인스턴스가 아니면 `true`를 반환한다.

```java
boolean is Loaded = em.getEntityManagerFactory()
    .getPersistenceUnitUtil().isLoaded(entity);
// 또는 emf.getPersistenceUnitUtil().isLoaded(entity);
```

- 조회한 엔티티가 진짜 엔티티인지 프록시로 조회한 것인지 확인하려면 클래스명을 직접 출력해보면 된다.

```java
member.getClass().getName();
```

<br/>

## 2. 즉시 로딩과 지연 로딩

- JPA는 개발자가 연관된 엔티티의 조회 시점을 선택할 수 있도록 2가지 방식을 제공한다.
- 즉시 로딩 Eager Loading
  - `FetchType.EAGER`
- 지연 로딩 Lazy Loading
  - - `FetchType.Lazy`

## 2-1. 즉시 로딩

- 아래와 같이 `FetchType.EAGER`를 주면 된다.
- `@...ToOne` 관계는 `fetch` 속성의 default 값이 `FetchType.EAGER`이다.

```java
@Entity
public class Member {
  
  @ManyToOne(fetch = FetchType.EAGER)
  @JoinColumn(name = "team_id")
  private Team team;
}
```

- 아래와 같이 회원을 조회하면 팀도 함께 조회한다.
  - 이 때 회원과 팀 두 테이블을 조회해야 하므로 쿼리를 2번 실행할 것 같지만, 대부분의 JPA 구현체는 즉시 로딩을 최적화하기 위해 가능하면 조인 쿼리를 사용한다.
  - `em.find`(Data JPA의 `findById`도 `em.find`로 돌아감) 조인 최적화가 일어난다.
  - 하지만 JPQL을 작성해 넘기면 SQL문 그대로 받아들이므로 조인 최적화가 되지 않아 JPA N+1 문제가 발생하니 주의하자.

```java
Member member = em.find(Member.class, "member1");
Team team = member.getTeam();
```

```sql
select
    m.member_id as member_id,
    m.team_id as team_id
from
    member m left outer join team t on m.team_id=t.team_id
where
    m.member_id='member1';
```

> 참고
> - 외부 조인보다는 내부 조인이 성능 최적화에 더 유리하다.
> - 외래 키에 `NOT NULL` 제약 조건이 걸려 있는 경우 내부 조인을 보장할 수 있다.
> - 내부 조인을 사용하기 위해서는 `@JoinColumn(nullable = false)` 설정을 해서 이 외래 키는 `NULL` 값을 허용하지 않는다고 알려주면 JPA는 외부 조인 대신에 내부 조인을 사용한다.

## 2-2 지연 로딩

- 아래와 같이 `FetchType.EAGER`를 주면 된다.
- `@...ToMany` 관계는 `fetch` 속성의 default 값이 `FetchType.EAGER`이다.

```java
@Entity
public class Member {
  
  @ManyToOne(fetch = FetchType.LAZY)
  @JoinColumn(name = "team_id")
  private Team team;
}
```

- 아래와 같이 조회해도 `team`은 조회하지 않는다. 대신 `member`의 `team` 멤버변수에 프록시 객체를 넣어둔다.
- 이제 `team`이 실제로 사용될 때까지 조회가 유예된다.

```java
Member member = em.find(Member.class, "member1");
Team team = member.getTeam();
```

> 참고
> - 조회 대상이 영속성 컨텍스트에 이미 있으면 프록시 객체를 사용할 이유가 없다.
> - 따라서 프록시 객체가 아닌 실제 객체를 사용한다.

### 프록시와 컬렉션 레퍼

- 하이버네이트는 엔티티를 영속 상태로 만들 때 엔티티에 컬렉션이 있으면 컬렉션을 추적하고 관리할 목적으로 원본 컬렉션을 하이버네이트가 제공하는 내장 컬렉션으로 변경하는데 이것을 `컬렉션 래퍼`라고 한다.
  - `org.hibernate.collection.internal.PersistentBag`
- 엔티티를 지연 로딩하면 프록시 객체를 사용해서 지연 로딩을 수행하지만 컬렉션은 컬렉션 래퍼가 지연 로딩을 처리해준다.
- 컬렉션 래퍼는 아래와 같이 실제 데이터를 조회할 대 데이터베이스를 조회해서 초기화한다.

```java
member.getOrders().get(0);
```

### JPA 기본 Fetch 전략

- `@ManyToOne`, `@OneToOne`: `FetchType.EAGER`
- `@OneToMany`, `@ManyToMany`: `FetchType.LAZY`
- 연관된 엔티티가 하나면 즉시 로딩을, 컬렉션이면 지연 로딩을 사용한다.
- 추천하는 방법은 모든 연관관계에 지연 로딩을 사용하는 것이다.

### 컬렉션에 FetchType.EAGER 사용 시 주의점

- 컬렉션을 하나 이상 즉시 로딩하는 것은 권장하지 않는다.
  - 일대다 조인은 `N 곱하기 M`의 결과를 반환하고 결과적으로 애플리케이션 성능 저하에 직격타가 될 수 있다.
  - JPA는 이렇게 조회된 결과를 메모리에 필터링해서 반환한다. (메모리가 터질 수도 있다.)
- 컬렉션 즉시 로딩은 항상 외부 조인을 사용해야 한다.
  - `1:N` 관계에서 `1`측은 외래 키를 가지고 있지 않기 때문에 `N` 측에 `not null` 제약 조건이 있어봤자 내부 조인으로 조회된다는 보장이 없다.
  - 예를 들어 팀:회원 => `1:N` 관계일 때, 내부 조인을 하면 회원이 하나도 없는 팀은 조회되지 않을 수 있다.

<br/>

## 4. 영속성 전이 Transitive Persistence

- 특정 엔티티를 영속 상태로 만들 때 연관된 엔티티도 함께 영속 상태로 만들고 싶으면 영속성 전이 기능을 사용하면 된다.
- JPA는 `CASCADE` 옵션으로 영속성 전이를 제공한다.
  - 부모 엔티티를 저장하면 자식 엔티티도 함께 저장할 수 있다.
- JPA에서 엔티티를 저장할 때 연관된 모든 엔티티는 영속 상태여야 하기 때문에 부모 엔티티를 영속 상태로 만들고 자식 엔티티도 각각 영속 상태로 만든다.
  - 매번 `em.persist()`를 사용해 영속 상태로 만들어야 한다면 불편할 것이다.
  - 하지만 영속성 전이를 사용하면 한 번에 영속 상태로 만들 수 있다.

### 4-1. 영속성 전이: 저장

- `cascade = CascadeType.PERSIST` 옵션을 주면 연관된 자식들도 함께 영속화된다.

```java
@Entity
public class Parent {
  @OneToMany(mappedBy = "parent", cascade = CascadeType.PERSIST)
  private List<Child> children = new ArrayList<>();
}
```

- 부모에 자식의 연관관계를 추가하고 `persist()`를 하면 자식도 영속화되는 것을 확인할 수 있다.

### 4-2. 영속성 전이: 삭제

- `cascade = CascadeType.REMOVE`를 설정하면 된다. 
- 부모를 삭제하면, 자식을 먼저 삭제하고 그 다음 부모를 삭제한다.

### 4-3. CASCADE 종류

```java
public enum CascadeType {
  ALL,
  PERSIST,
  MERGE,
  REMOVE,
  REFRESH,
  DETACH
}
```

- 따라서 아래와 같이 여러 속성을 같이 사용할 수도 있다.

```java
cascade ={CascadeType.PERSIST, CascadeType.REMOVE}
```

<br/>

## 5. 고아 객체

- JPA는 부모 엔티티와 연관관계가 끊어진 자식 엔티티를 자동으로 삭제하는 기능을 제공한다. 이것을 고아 객체(ORPHAN) 제거라고 한다.
  - 참조가 제거되면 고아 객체로 보고 삭제하는 기능이다. (약간 가비지 컬렉션이랑 비슷해보인다.)
- 부모 엔티티의 컬렉션에서 자식 엔티티의 참조만 제거하면 자식 엔티티가 자동으로 삭제된다.
  - 부모에서만 사용되어야 하기 때문에 `@OneToMany`나 `@OneToOne`에서만 사용할 수 있다.

```java
@Entity
public class Parent {
  
  @OneToMany(mappedBy = "parent", orphanRemoval = true)
  private List<Child> children = new ArrayList<>();
}
```

```java
parent.getChildren().remove(0);
```

```sql
delete from child where id=?
```

- 모든 자식 엔티티를 삭제하려면 아래와 같이 컬렉션을 비우면 된다.

```java
parent.getChildren().clear();
```

<br/>

## 6. 영속성 전이 + 고아 객체, 생명 주기

- `CascadeType.ALL` + `orphanRemoval = true`를 동시에 사용하면 어떻게 될까?
- 두 옵션을 모두 활성화하면 부모 엔티티를 통해서 자식의 생명주기를 관리할 수 있게 된다.
  - 자식을 저장하려면 부모에 등록만 하면 된다. (CASCADE)
  - 자식을 삭제하려면 부모에서 제거하면 된다. (orphanRemoval)

> 참고
> - 영속성 전이는 DDD의 Aggregate Root 개념을 구현할 때 사용하면 편리하다.

<br/>

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
