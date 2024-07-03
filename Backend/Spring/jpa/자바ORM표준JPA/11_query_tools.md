# 다양한 쿼리 도구들

## 01 Criteria

- Criteria 쿼리는 JPQL을 자바 코드로 작성하도록 도와주는 빌더 클래스 API다.
- Criteria를 사용하면 문자가 아닌 코드로 JPQL을 작성하므로 문법 오류를 컴파일 단계에서 잡을 수 있고 문자 기반의 JPQL보다 동적 쿼리를 안전하게 생성할 수 있다는 장점이 있다.
- 하지만 코드가 복잡하고 장황해서 직관적으로 이해하기 어렵다는 단점이 있다.

### 기초

- `jakarta.persistence.criteria` 패키지에 있다. (구 `javax.persistence.criteria` 패키지)

```java
public class MyClass {

  public void query() {
    // 1. 쿼리 빌더
    CriteriaBuilder cb = em.getCriteriaBuilder();
    
    // 2. 생성, 반환 타입 지정
    CriteriaQuery<Member> cq = cb.createQuery(Member.class);
    
    // 3. FROM 절
    Root<Member> m = cq.from(Member.class);
    // 4. Select 절
    cq.select(m);
    
    TypedQuery<Member> query = em.createQuery(cq);
    List<Member> members = query.getResultList();
  }
}
```

1. Criteria 쿼리를 생성하려면 먼저 CriteriaBuilder를 얻어야 한다. CriteriaBuilder는 EntityManager나 EntityManagerFactory에서 얻을 수 있다.
2. Criteria 쿼리 빌더에서 CriteriaQuery를 생성한다. 이 때 반환 타입을 지정할 수 있다.
3. FROM 절을 생성한다. 반환된 값 m은 Criteria에서 사용하는 특별한 별칭이다. m을 조회의 시작점이라는 의미로 쿼리 루트라고 한다.
4. SELECT 절을 생성한다.

<br/>

### 검색 조건과 정렬 조건

1. `m.get("username")`은 JPQL의 `m.username`과 동일한 표현이다.
2. `cd.desc(m.get("age))`는 JPQL의 `m.age desc`와 같은 표현이다.
3. 만들어둔 조건을 `where`, `orderBy`에 넣어서 원하는 쿼리를 생성하면 된다.

```java
public class MyClass {

  public void query() {
    CriteriaBuilder cb = em.getCriteriaBuilder();
    CriteriaQuery<Member> cq = cb.createQuery(Member.class);
    Root<Member> m = cq.from(Member.class);
    
    // 1. 검색 조건 정의
    Predicate usernameEqual = cb.equal(m.get("username"), "memberA");
    
    // 2. 정렬 조건 정의
    jakarta.persistence.criteria.Order ageDesc = cb.desc(m.get("age"));
    
    // 3. 쿼리 생성
    cq.select(m)
        .where(usernameEqual)
        .orderBy(ageDesc);
    
    TypedQuery<Member> query = em.createQuery(cq);
    List<Member> members = query.getResultList();
  }
}
```

<br/>

## 02 QueryDSL

- Criteria는 너무 복잡하고 어렵다. 작성된 코드의 복잡성으로 인해 어떤 JPQL이 생성될 지 파악하기 쉽지 않다.
- QueryDSL은 쿼리를 코드로 작성할 수 있으면서도 쉽고 간결하며 쿼리와도 굉장히 유사하다.

### 설정

- 설정하는 방법은 아래 블로그에 정리해뒀다.
- [Spring Boot 환경 QueryDSL 설정 (build.gradle & .kts)
  출처: https://myvelop.tistory.com/213 [MYVELOP 마이벨롭:티스토리]](https://myvelop.tistory.com/213)

### 동적 쿼리

- QueryDSL의 가장 큰 장점은 동적 쿼리를 만드는 게 정말 쉽다는 점이다.

- `com.querydsl.core.BooleanBuilder`를 사용해 동적 쿼리를 만들 수 있다.

```java
QItem item = QItem.item;
QueryCondition qc = new QueryCondition("txt", 100);

CriteriaBuilder criteriaBuilder = em.getCriteriaBuilder();
BooleanBuilder builder = new BooleanBuilder();
if (StringUtils.hasText(qc.getName())) {
  builder.and(item.name.contains(qc.getName()));
}
if (qc.getPrice() != null) {
  builder.and(item.price.gt(qc.getPrice()));
}
```

- `com.querydsl.core.types.dsl.BooleanExpression`을 반환하는 함수나 변수 여러 개를 where절에 담아 동적쿼리를 만들 수 있다.

```java
queryFactory.selectFrom(member)
    .where(searchByName(qc.getName()), searchByAge(qc.getAge()))
    .fetch();
```

- `BooleanExpression`을 반환하는 함수의 예시는 아래와 같다.

```java
public BooleanExpression searchByName(final String name) {
  if (name == null) {
    return null;
  }
  return member.name.eq(name);
}

public BooleanExpression searchByAge(final Integer age) {
    if (age == null) {
    return null;
    }
    return member.age.eq(age);
}
```

### 메소드 위임

- `@QueryDelegate`를 사용해 메소드 위임(Delegate Methods) 기능을 사용할 수 있다.
  - 메소드를 위임을 사용하려면 정적 메소드(static)로 만들어야 한다.
  - 첫 번째 파라미터는 대상 엔티티의 쿼리 타입(QType)을 지정하고 나머지는 필요한 파라미터를 지정한다.

```java
public class ItemExpression {
  
  @QueryDelegate(Item.class)
  public static BooleanExpression isExpensive(
      final QItem item, final Integer price) {
    return item.price.gt(price);
  }
}
```

- 이제 QItem을 보면 `isExpensive(Integer price)` 라는 함수가 추가된 것을 확인할 수 있다.
  - 아래와 같이 활용할 수 있다.

```java
queryFactory.selectFrom(item)
    .where(item.isExpensive(30_000))
    .fetch();
```

### 주의할 점

- QueryDSL에서 수정이나 삭제 쿼리를 실행하면 JPQL 배치 쿼리와 같이 동작한다. 
  - 영속성 컨텍스트를 무시하고 데이터베이스를 직접 쿼리한다는 걸 명심하자.

<br/>

## 03 네이티브 SQL

- JPQL은 표준 SQL이 지원하는 대부분의 문법과 SQL 함수들을 지원하지만 특정 데이터베이스 종속적인 기능은 지원하지 않는다. 예를 들어 아래와 같은 것들이다.
  - 특정 데이터베이스만 지원하는 함수, 문법, SQL 쿼리 힌트
  - 인라인 뷰(From 절에서 사용하는 서브쿼리), UNION, INTERSECT
  - 스토어드 프로시저

### 특정 데이터베이스에 종속적인 기능을 사용하는 방법은 아래와 같다.

- 특정 데이터베이스만 사용하는 함수
  - JPQL에서 네이티브 SQL 함수를 호출할 수 있다.
  - 하이버네이트는 데이터베이스 방언에 각 데이터베이스에 종속적인 함수들을 정의해줌. 이걸 호출해서 사용할 수 있음
- 특정 데이터베이스만 지원하는 SQL 쿼리 힌트
  - 하이버네이트를 포함한 몇몇 JPA 구현체들이 지원
- 인라인 뷰, UNION, INTERSECT
  - 하이버네이트는 지원하지 않지만 일부 JPA 구현체들이 지원
- 스토어드 프로시저
  - JPQL에서 스토어드 프로시저 호출 가능
- 특정 데이터베이스에서만 지원하는 문법
  - 오라클의 `CONNECT BY`처럼 특정 데이터베이스에 너무 종속적인 SQL 문법은 지원하지 않는다.
  - **이 때 네이티브 쿼리를 사용해야 한다.**

### 네이티브 SQL 사용

- 네이티브 쿼리 API는 아래와 같이 3가지가 있다. (`EntityManager`에 정의되어 있음)

```java
public interface EntityManager extends AutoCloseable {
  
  ...

  // 결과 타입을 정의할 수 없을 때 
  public Query createNativeQuery(String sqlString);

  // 결과 타입 정의
  public Query createNativeQuery(String sqlString, Class resultClass);

  // 결과 매핑 사용
  public Query createNativeQuery(String sqlString, String resultSetMapping);
}
```

### 엔티티 조회

- JPQL과 다른 점
  - 실제 데이터베이스 SQL과 동일하게 작성한다.
  - 위치기반 파라미터만 지원한다.
- JPQL과 같은 점
  - 위의 차이점 빼고는 다 같다.
  - **조회한 엔티티가 동일하게 영속성 컨텍스트에서 관리된다!!**

```java
String sql = "select id, age, name, team_id from member where age > ?";
Query nativeQuery = em.createNativeQuery(sql, Member.class);
List<Member> members = nativeQuery.getResultList();
```

### NamedNativeQuery

- JPQL로 NamedQuery를 작성한 것 처럼 네이티브 쿼리로도 NamedNativeQuery를 작성할 수 있다.

```java
@Entity
@NamedNativeQuery(
    name = "Member.memberSQL",
    query = "select id, age, name, team_id from member where age > ?",
    resultClass = Member.class)
public class Member {...}
```

- 위에서 정의한 네임드 네이티브 쿼리는 아래와 같이 사용할 수 있다.

```java
TypedQuery<Member> nativeQuery =
      em.createNamedQuery("Member.memberSQL", Member.class)
        .setParameter(1, 20);
```

### 스토어드 프로시저 사용

- `createStoredProcedureQuery` 메소드에 프로시저 이름을 문자열로 넣어준다.

```java
StoredProcedureQuery spq = em.createStoredProcedureQuery("proc_multiply");
spq.regitserStoredProcedureParameter(1, Integer.class, ParamterMode.IN);
spq.regitserStoredProcedureParameter(2, Integer.class, ParamterMode.OUT);
spq.setParaeter(1, 100);
spq.excute();
Integer out = (Integer) spq.getOutputParameterValue(2);
```

<br/>

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
