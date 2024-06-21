# 연관관계 매핑

## 0. 연관관계 매핑을 이해하기 위한 핵심 키워드

### 방향 Direction

- 단방향, 양방향이 있다.
- 방향은 객체 관계에만 존재하고 테이블 관계는 항상 양방향이다.

### 다중성 Multiplicity

- N:1, 1:N, 1:1, N:N 다중성이 있다.

### 연관관계의 주인 Owner

- 객체를 양방향 연관관계로 만들면 연관관계의 주인을 정해야 한다.

<br/>

## 1. 단방향 연관관계

### 객체 연관관계와 테이블 연관관계의 가장 큰 차이

- 참조를 통한 연관관계는 언제나 단방향이다. 객체 간에 연관관계를 양방향으로 만들고 싶으면 반대쪽에도 필드를 추가해서 참조를 보관해야 한다.
    - 결국 연관관계를 하나 더 만들어야 한다. 이렇게 양쪽에서 서로 참조하는 것을 양방향 연관관계라 한다.
    - 하지만 정확히 얘기하면 이것은 양방향 관계가 아니라 서로 다른 단방향 관계 2개다.
- 반면 테이블은 외래 키 하나로 양방향으로 조인할 수 있다.

### 객체 연관관계 vs 테이블 연관관계 정리

- 객체는 참조(주소)로 연관관계를 맺는다.
- 테이블은 외래 키로 연관관계를 맺는다.

### 객체 관계 매핑

```java

@Entity
public class Member {

  @Id
  private String id;

  private String name;

  @ManyToOne
  @JoinColumn(name = "team_id")
  private Team team;

  // 연관관계 설정
  public void setTeam(Team team) {
    this.team = team;
  }
}
```

```java

@Entity
public class Team {

  @Id
  private String id;

  private String name;
}
```

### @JoinColumn

|속성|기능|기본값|
|:--:|:--:|:--:|
|name|매핑할 외래 키 이름|테이블의 기본 키 컬럼명|
|referencedColumnName|외래 키가 참조하는 대상 테이블의 컬럼명| 참조하는 테이블의 기본 키 컬럼명|
|foreignKey(DDL)|외래 키 제약조건을 직접 지정. 테이블을 생성할 때만 사용||
|unique, nullable, insertable, updatable, columnDefinition, table|@Column과 동일||

### @ManyToOne

|속성|기능|기본값|
|:--:|:--:|:--:|
|optional|false로 설정하면 연관된 엔티티가 항상 있어야 한다.||
|fetch|클로벌 페치 전략을 설정한다.|- @ManyToOne=FetchType.EAGER - @OneToMany=FetchType.LAZY|
|cascade|영속성 전이 기능을 사용한다.||
|targetEntity|연관된 엔티티의 타입 정보를 설정한다. 이 기능은 거의 사용 X||

<br/>

## 2. 연관관계 사용

### 저장

> 주의
> - JPA에서 엔티티를 저장할 때 연관된 모든 엔티티는 영속 상태여야 한다.

- 회원 테이블에 회원을 저장할 때 team의 식별자를 외래 키로 저장한다.

```java
public class TeamSaving {
  public void save() {
    Team team = new Team(...);
    em.persist(team);
    
    Member member = new Member(...);
    member.setTeam(team);
    em.persist(member);
  }
}
```

### 조회

- 연관관계가 있는 엔티ㅣ를 조회하는 방법은 크게 2가지다
  1. 객체 그래프 탐색(객체 연관관계를 사용한 조회)
  2. 객체지향 쿼리 사용(JPQL)

- 객체 그래프 탐색

```java
member.getTeam();
```

- 객체지향 쿼리

```java
String jpql = "select m from Member m join m.team t " 
        + "where t.name=:teamName";

List<Member> resultList = em.createQuery(jpql, Member.class)
        .setParameter("teamName", "team1")
        .getResultList();
```

### 수정

- 더티 체킹을 통해 팀을 수정해보자. 

```java
public class TeamUpdating {
  public void update() {
    Team newTeam = new Team(...);
    em.persist(newTeam);
    
    Member member = em.find(Member.class, "member1");
    member.setTeam(newTeam);
  }
}
```

### 연관관계 제거

```java
public class TeamDeletion {
  public void delete() {
    Member member = em.find(Member.class, "member1");
    member.setTeam(null);
  }
}
```

### 연관관계 삭제

- 연관된 엔티티를 삭제하려면 기존에 있는 연관관계를 먼저 제거하고 삭제해야 한다.
- 그렇지 않으면 외래 키 제약 조건으로 인해 데이터베이스에서 오류가 발생한다.

<br/>

## 3. 양방향 연관관계 

```java

@Entity
public class Member {

  @Id
  private String id;

  private String name;

  @ManyToOne
  @JoinColumn(name = "team_id")
  private Team team;

  // 연관관계 설정
  public void setTeam(Team team) {
    this.team = team;
  }
}
```

- Team에 Member에 대한 참조(연관관계)가 추가되었다.

```java

@Entity
public class Team {

  @Id
  private String id;

  private String name;
  
  @OneToMany(mappedBy = "team")
  private List<Member> members = new ArrayList<>();
}
```

<br/>

## 4. 연관관계의 주인

- `mappedBy`는 왜 필요할까?
- 실제 객체에는 양방향 연관관계라는 것이 없다. 서로 다른 단방향 연관관계 2개를 애플리케이션 로직으로 잘 묶어서 양방향인 것처럼 보이게 할 뿐이다.
- JPA에서는 두 객체의 연관관계 중 하나를 정해서 테이블의 외래 키를 관리해야 하는데 이것을 `연관관계의 주인`이라 한다.

### 양방향 매핑의 규칙: 연관관계의 주인

- 연관관계의 주인만이 데이터베이스 연관관계와 매핑되고 외래 키를 관리(등록, 수정, 삭제)할 수 있다. 반면에 주인이 아닌 쪽은 읽기만 할 수 있다.
- 어떤 연관관계를 주인으로 정할지는 `mappedBy` 속성을 사용하면 된다.
- 주인은 `mappedBy` 속성을 사용하지 않는다.
- 주인이 아니면 `mappedBy` 속성을 사용해서 속성의 값으로 연관관계의 주인을 지정해야 한다.

### 연관관계의 주인은 외래 키가 있는 곳

- 연관관계의 주인을 정한다는 것은 사실 외래 키 관리자를 선택하는 것이다.
- 연관관계 주인은 테이블에 외리 키가 있는 곳으로 지정하면 된다.

<br/>

## 5. 양방향 연관관계 저장

## 6. 양방향 연관관계의 주의점

- 가장 흔히 하는 실수는 연관관계의 주인에는 값을 입력하지 않고 주인이 아닌 곳에만 값을 입력하는 것이다.
- 데이터베이스에 외래 키 값이 정상적으로 입력되지 않으면 이것부터 의심해보자.

### 순수한 객체까지 고려한 양방향 연관관계

- 주인에만 값을 저장하면 어떨까? 객체 관점에서 양쪽 방향에 모두 값을 입력해주는 것이 가장 안전하다.
- 양쪽 방향 모두 값을 입력하지 않으면 JPA를 사용하지 않는 순수한 객체 상태에서 심각한 문제가 발생할 수 있다.

### 연관관계 편의 메소드

- 각각의 연관관계를 신경쓰다 보면 실수로 둘 중 하나만 호출해 양방향이 깨지는 경우가 발생할 수 있다.

```java
member.setTeam(team);
team.getMembers().add(member);
```

- 연관관계 설정을 객체 하나에 캡슐화하는 것이 좋다.

```java
public class Member {
  
  private Team team;
  
  public void setTeam(Team team) {
    this.team = team;
    team.getMembers().add(this);
  }
}
```

### 편의 메소드 작성 시 주의사항

- 만약 연관관계를 수정한다면 어떻게 될까?
  - `Member`는 기존의 `Team`을 참조하지 않지만 `Team`은 여전히 `Member`를 참조할 것이다.
- 기존의 관계를 제거하는 코드를 추가해야 한다.

```java
public void setTeam(Team team) {
  if (this.team != null) { // 기존 팀과의 관계 제거
    this.team.getMembers().remove(this);
  } 
  this.team = team;
  team.getMembers().add(this);
}
```

> 참고
> - 사실 위 로직이 추가되지 않아도 외래 키 변경하는 데는 전혀 문제가 없다.
> - 연관관계의 주인이 `Member`이기 때문이다.
> - 하지만 (연관관계를 바꾸는 트랜잭션 안에서) 영속성 컨텍스트가 살아있는 상태에서 `Team`이 getMembers()를 했을 때 기존의 Member를 반환하기 때문에 어떤 사이드 이펙트가 발생할지 모른다.
> - 결론: 관계를 제거하는 것이 안전하다.

## 6. 정리

- 비즈니스 로직상 더 중요하다고 연관관계의 주인으로 선택하면 안 된다.
- 비즈니스 중요도를 배제하고 단순히 외래 키 관리자 정도의 의미만 부여해야 한다.
- 양방향 매핑 시에는 무한 루프에 빠지지 않도록 주의하자.
  - 이런 문제는 엔티티를 JSON으로 변환할 때나 Lombok 라이브러리를 사용할 때 자주 발생한다.
 