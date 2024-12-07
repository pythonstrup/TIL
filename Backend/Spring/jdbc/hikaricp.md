# Hikari CP 원리

## 먼저 DB 커넥션 풀이란

- 웹 어플리케이션이 실행됨과 동시에 연동할 데이터베이스와의 연결을 미리 설정해두는 풀이다.
- 이렇게 미리 데이터베이스와 연결시킨 상태를 유지하는 기술을 커넥션 풀(Connection Pool, CP)이라고 한다.
- Spring Boot 2.0 이상부터는 Hikari CP가 기본 커넥션 풀로 지정

## Hikari CP 설정

- Hikari CP의 상세 설정은 `application.properties`나 `application.yaml`를 통해 할 수 있다.

```yaml
spring:
 datasource:
   url: jdbc:mysql://my-db-host/db?serverTimeZone=UTC&CharacterEncoding=UTF-8
   username: your_usernaem
   password: your_password
   hikari:
     maximum-pool-size: 10
     connection-timeout: 5000
     connection-init-sql: SET TIME ZONE 'UTC' # 커넥션 생성할 때 마다 쿼리
     validation-timeout: 2000
     minimum-idle: 10
     idle-timeout: 600000
     max-lifetime: 1800000
```

- `maximum-pool-size`: 최대 pool size (defailt 10)
- `connection-timeout`:  최초 연결을 맺는 시간(임계치, threshold)을 설정. 초과되면 timeout을 발생시킨다. DB 구현체마다 다르겠지만 보통 커넥션이 끊어진 경우, 실행중인 쿼리가 취소되고 트랜잭션이 롤백.
- `connection-init-sql`: 커넥션을 생성할 때마다 실행할 쿼리를 지정
- `validation-timeout`: 새로운 커넥션을 주기 전 DB 헬스체크 timeout 시간, 참고로 connection-init-sql에도 같이 적용됨
- `minimum-idle`: 연결 풀에서 HikariCP가 유지 관리하는 최소 유휴 연결 수
- `idle-timeout`: 커넥션이 유휴 상태로 있을 수 있는 최대 시간(밀리초), DB 자체 커넥션 수가 부족하다면 시간을 줄여야 한다.
- `max-lifetime`: 풀에 커넥션이 생성된 후 있을 수 있는 최대시간 (밀리초), DB 자체 커넥션 수가 부족하다면 시간을 줄여야 한다.
- `auto-commit`: auto commit 여부 (default true)

### AutoCommit이란?

- 대부분의 DBMS 는 `AutoCommit` 트랜잭션 모드와 `명시적인 BEGIN-COMMIT-ROLLBACK` 트랜잭션 모드를 둘 다 지원한다.  
- 트랜잭션을 걸지 않았더라도 `AutoCommit`에 의해 작업 완료 후 `Commit`이 찍힌다.

### 만약 reader와 writer를 따로 사용한다면?

- `@ConfigurationProperties`를 활용해 따로 설정해줄 수 있다.

```yaml
spring:
  my-db:
    writer:
      # ...
    reader:
      # ...
```

```java
@Configuration
public class JdbcConfig {
  
  @Bean
  @ConfigurationProperties(prefix = "spring.my-db.writer")
  public DataSource writerDataSource() {
    return DataSourceBuilder.create().type(HikariDataSource.class).build();
  }

  @Bean
  @ConfigurationProperties(prefix = "spring.my-db.reader")
  public DataSource readerDataSource() {
    return DataSourceBuilder.create().type(HikariDataSource.class).build();
  }
}
```

# 참고자료

- [HikariCP Dead lock에서 벗어나기 (실전편)](https://techblog.woowahan.com/2663/)
- [[Spring] DB커넥션풀과 Hikari CP 알아보기](https://velog.io/@miot2j/Spring-DB%EC%BB%A4%EB%84%A5%EC%85%98%ED%92%80%EA%B3%BC-Hikari-CP-%EC%95%8C%EC%95%84%EB%B3%B4%EA%B8%B0)
- [JDBC Connection 에 대한 이해, HikariCP 설정 팁](https://jiwondev.tistory.com/291)
