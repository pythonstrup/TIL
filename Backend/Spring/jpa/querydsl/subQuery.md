# QueryDsl - SubQuery

- SubQuery는 MySQL에서 자동으로 Join으로 변경할 정도로 성능에 큰 영향을 준다. 따라서 최대한 지양하는 것이 좋다.
- SubQuery 대신 Join, Application단 처리 등을 고려해보는 것이 좋다.
- 그럼에도 QueryDsl에서 SubQuery를 사용해야 한다면 아래와 같이 사용할 수 있다. 

## `Select`에서의 Sub Query

- `ExpressionUtils`은 QueryDsl 내부에서 새로운 Expression을 사용할 수 있게 해준다.

```java
@Repository
@RequiredArgsConstructor
public class MemberQueryRepository {

  private final JPAQueryFactory queryFactory;
  
  public List<MemberProjection> findMember() {
    return queryFactory
        .select(
            new QMemberProjection(
                member.id,
                member.name,
                ExpressionUtils.as(
                    JPAexpressions.select(team.id)
                        .from(team)
                        .where(team.id.eq(member.team.id)),
                    "teamId")))
        .from(member)
        .fetch();
  }
}
```

## `Where`에서의 Sub Query

```java
@Repository
@RequiredArgsConstructor
public class MemberQueryRepository {

  private final JPAQueryFactory queryFactory;
  
  public List<Member> findMember(long partId) {
    return queryFactory
        .select(member)
        .from(member)
        .where(member.friends.in(
            JPAExpressions.select(member.id)
                .from(member)
                .where(member.part.id.eq(partId))))
        .fetch();
  }
}
```


# 참고자료

- [[Querydsl] 서브쿼리 사용하기](https://jojoldu.tistory.com/379)