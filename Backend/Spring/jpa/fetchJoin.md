# FetchJoin

- N+1 문제를 해결하기 위한 방법 중 하나
- 처음 조회할 때부터 데이터를 매핑해 가지고 올 수 있다.

## FetchJoin을 2번 하면?

```java
@Repository
@RequiredArgsConstructor
public class MyQueryRepository {
  private final JPAQueryFactory queryFactory;
  
  public List<Member> find() {
    return queryFactory
        .select(member)
        .from(member)
        .join(member.team, team).fetchJoin()
        .join(member.school, school).fetchJoin()
        .fetch();
  }
}
```

- 아래 에러가 발생한다.

```
org.hibernate.query.sqm.AliasCollisionException: Alias [member] used for multiple from-clause elements 
```

- fetchJoin은 2개 이상의 관계에 적용할 수 없다.

### 간단한 해결방법

- N+1 문제가 발생하도록 내버려 둔다.
- fetchJoin을 둘 중 하나에만 적용한다.

### BatchSize를 사용한 해결방법

- 아래와 같이 `default_batch_fetch_size`를 설정해주면 조회 수를 줄일 수 있다. 

```yaml
spring:
  jpa:
    properties:
      hibernate.default_batch_fetch_size: 1000
```