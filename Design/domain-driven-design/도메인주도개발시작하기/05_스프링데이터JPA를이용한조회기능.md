# 05 스프링 데이터 JPA를 이용한 조회 기능

## 1. 시작에 앞서

- CQRS: 명령 모델과 조회 모델을 분리하는 패턴.

----

## 2. 검색을 위한 스펙

- 검색을 다양하게 조합해야 할 때 사용할 수 있는 것이 `스펙 Specification`이다.
  - 애그리거트가 특정 조건을 충족하는지를 검사할 때 사용하는 인터페이스
- 스펙 인터페이스는 아래와 같이 정의한다.

```java
public interface Specification<T> {
  public boolean isSatisfiedBy(T agg);
}
```

- `isSatisfiedBy` 메소드의 agg 파라미터는 검사 대사잉 되는 객체다.
  - 스펙을 Repository에서 사용하면 agg는 애그리거트 루트가 되고,
  - 스펙을 DAO에서 사용하면 agg는 검색 결과로 리턴할 데이터 객체가 된다.

```java
public class OrdererSpec implements Specification<Order> {
  private String ordererId;
  
  public OrdererSpec(String ordererId) {
    this.ordererId = ordererId;
  }
  
  public boolean isSatisfiedBy(Order agg) {
    return agg.getOrdererId().getMemberId().getId().equals(ordererId);
  }
}
```

- 하지만 실제 스펙은 이렇게 구현하지 않는다. 모든 애그리거트 객체를 메모리에 보관하기도 어렵고 설사 메모리에 다 보관할 수 있다 하더라도 조회 성능에 심각한 문제가 발생하기 때문이다.
- 실제 스펙은 사용하는 기술에 맞춰 구현하게 된다.

----

## 3. 스프링 데이터 JPA를 이용한 스펙 구현

- 아래는 스프링 데이터 JPA가 제공하는 `Specification` 인터페이스다.

```java
public interface Specification<T> extends Serializable {

  long serialVersionUID = 1L;

  @Nullable
  Predicate toPredicate(Root<T> root, CriteriaQuery<?> query, CriteriaBuilder criteriaBuilder);

  // ...
}
```

- 스펙 인터페이스에서 제네릭 타입 파라미터 T는 JPA Entity 타입을 의미한다.
- `toPredicate()` 메소드는 JPA `크리테리아 Creteria` API에서 조건을 표현하는 `Predicate`을 생성한다.
-예를 들어 아래와 같이 구현해 볼 수 있을 것이다.
  - 요구사항: Entity 타입은 `OrderSummary`, `ordererId` 프로퍼티 값이 지정한 값과 동일.

```java
public class OrdererIdSpec implements Specification<OrderSummary> {
  private String ordererId;
  
  public OrdererIdSpec(String ordererId) {
    this.ordererId = ordererId;
  }
  
  @Override
  public Predicate toPredicate(
      Root<OrderSummary> root,
      CriteriaQuery<?> query,
      CriteriaBuilder cb) {
    return cb.equal(root.get(OrderSummary_.ordererId), ordererId);
  }
}
```

> #### 참고
> - 여기서 `OrderSummary_` 클래스는 JPA 정적 메타 모델을 정의한 코드다.
> ```java
> @StaticMetamodel(OrderSummary.class)
> public class OrderSummary_ {
>   public static volatile SingularAttribute<OrderSummary, String> number;
>   public static volatile SingularAttribute<OrderSummary, Long> version;
>   public static volatile SingularAttribute<OrderSummary, String> ordererId;
>   public static volatile SingularAttribute<OrderSummary, String> ordererName;
>   // ...
> }
> ```
> 
> - 정적 메타 모델은 `@Staticmetamodel` 어노테이션을 활용해서 관련 모델을 지정한다.
> - 대상 모델의 각 프로퍼티와 동일한 이름을 갖는 정적 필드를 정의한다. 이 정적 필드는 프로퍼티에 대한 메타 모델로서 프로퍼티 타입에 따라 `SingularAttribute`, `ListAttribute` 등의 타입을 사용해서 메타 모델을 정의한다.
> - 정적 메타 모델을 사용하는 대신 문자열로 프로퍼티를 지정할 수도 있다. 예를 들어 아래와 같이.
> ```java
> cb.equal(root.<String>get("ordererId"), ordererId)
> ```
> - 하지만 문자열은 오타 가능성이 있고 실행하기 전까지는 오타가 있다는 것을 놓치기 쉽다.

----

## 4. 리포지터리/DAO에서 스펙 사용하기

- 스펙을 충족하는 Entity를 검색하고 싶다면 `findAll` 메소드를 사용하면 된다.

```java
public interface OrderSummaryDao extends Repository<OrderSummary, String> {
  List<OrderSummary> findAll(Specification<OrderSummary> spec);
}
```

----

## 5. 스펙 조합

- 스프링 데이터 JPA가 제공하는 스펙 인터페이스는 스펙을 조합할 수 있는 두 메소드를 제공하고 있다.
  - `and`와 `or`

----

## 6. 정렬 지정하기

- 스프링 데이터 JPA에서는 두 가지 방법을 사용해서 정렬을 지정할 수 있다.
1. 메소드 이름에 `OrderBy`를 사용해서 정렬 기준 지정
2. `Sort`를 인자로 전달

----

## 7. 페이징 처리하기

- 목록을 보여줄 때 전체 데이터 중 일부만 보여주는 페이징 처리는 기본.
- 스프링 데이터 JPA는 페이징 처리를 위해 `Pageable` 타입 사용

----

## 8. 스펙 조합을 위한 스펙 빌더 클래스

- 스펙을 생성하다 보면 아래와 같이 스펙을 조합해야 할 때가 있다.
  - 실수하기 쉬워진다.
  - 이 때는 스펙 빌더를 사용하면 좋다.

----

## 9. 동적 인스턴스 생성

- 조회 전용 모델 만들기
- 조회 전용 모델을 만드는 이유는 표현 영역을 통해 사용자에게 데이터를 보여주기 위함이다.
  - 많은 웹 프레임워크는 새로 추가한 Value 타입을 알맞은 형식으로 출력하지 못하므로 위 코드처럼 값을 기본 타입으로 변환하면 편리하다.

----

## 10. 하이버네이트 `@Subselect` 사용

- 하이버네이트는 JPA 확장 기능으로 `@Subselect`를 제공
  - 쿼리 결과를 `@Entity`로 매핑할 수 있는 유용한 기능.

```java
@Entity
@Immutable
@Subselect("{쿼리 내용}")
@Synchronize({"purchase_order", "order_line", "product"})
public class OrderSummary {
  // ...
}
```

- `@Immutable`, `@Subselect`, `@Synchronize`는 하이버네이트 전용 어노테이션인데 이 태그를 사용하면 테이블이 아닌 쿼리 결과를 `@Entity`로 매핑할 수 있다.
- `@Subselect`는 DBMS의 View를 사용한다고 보면 된다.
  - View도 수정할 수 없듯이 `@Subselect`로 조회한 Entity 역시 수정할 수 없다. => 매칭되는 테이블이 없어 에러가 발생할 것이다.
  - `@Immutable`을 통해 무시하는 게 가장 편리하다.

----

# 참고자료

- 도메인 주도 개발 시작하기, 최범균 지음
