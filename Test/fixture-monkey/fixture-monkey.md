
- 필드를 제어하려고 하는 순간 코드가 굉장히 난해해진다.
- 필드명을 일일이 다 문자열로 적어줘야 한다.

```java
private Supporter createSupporter(
  final SupporterGeneration generation,
  final long memberId,
  final ItemType itemType,
  final int couponIssueCount,
  final int couponUseCount) {
    return fixtureMonkey
        .giveMeBuilder(Supporter.class)
        .set("supporterGeneration", generation)
        .set("memberId", memberId)
        .set("itemType", itemType)
        .set("couponIssueCount", couponIssueCount)
        .set("couponUseCount", couponUseCount)
        .setNull("deletedAt")
        .sample();
}
```