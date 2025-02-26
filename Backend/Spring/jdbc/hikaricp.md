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

## 최적의 HikariCP pool

- HikariCP Github의 Wiki를 확인해보면 About Pool Sizing이라는 글이 존재한다. 여기서 제공하는 공식은 아래와 같다.

```
connections = ((core_count * 2) + effective_spindle_count)
```

- 예를 들어, 하나의 하드 디스크를 가진 4-Core i7 서버에서 가장 최적의 커넥션 풀 사이즈는 아래와 같이 도출해낼 수 있다.

```
connections = (4 * 2) + 1 = 9
```

- 숫자를 반올림해서 10으로 설정하는 게 깔끔할 것이다. HikariCP 팀은 이 설정으로 3000명의 frontend 유저가 요청하는 6000TPS를 손쉽게 처리했다고 한다.
- 여기서 Spindle이란 전통적인 하드 디스크 드라이브(HDD)의 물리적 디스크 회전 장치를 가리키는 용어다. 
  - 정확히 말하자면 Spindle이란 플래터를 회전시키는 구성요소이고, HDD를 작동하게 만드는 것이다.
  - 공식의 effective_spindle_count가 의미하는 것은 병렬 처리 가능한 디스크의 I/O 경로 수를 추정한 값이다. 
  - 예를 들어, 단일 HDD라면 이 값이 1이 될 것이다. 
  - 반면 RAID 구성을 통해 병렬 처리 가능한 디스크의 수가 5개라면 이 수치가 5가 될 것이다. HDD를 동작 시킬 수 있는 Spindle 하나당 +1을 해주면 되는 것이다.

### Pool Locking

- 하지만 위에서 제시한 공식이 항상 정답인 것은 아니다. 단일 요청이 여러 커넥션을 획득해야 하는 경우 데드락의 가능성은 존재한다. 이런 경우 풀 크기를 늘려서 문제를 해결해야할 수도 있다.
- Hikari CP 팀에서 데드락을 피하기 위해 제안하는 공식은 아래와 같다.

```
pool size = Tn * (Cm - 1) + 1
```

- 여기서 Tn은 스레드의 최대 개수를 의미하고, Cm은 하나의 스레드에서 동시에 발생하는 커넥션의 수를 의미한다.
- 예를 들어, 하나의 작업을 진행할 때, 5개의 비동기 작업이 스레드에 의해 실행되고, 하나의 스레드당 4개의 커넥션이 필요하다고 가정해보자. 그렇다면 풀 사이즈는 아래와 같이 도출될 것이다.

```
pool size = 5 * (4 - 1) + 1 = 16
```

- 위의 공식은 최적의 풀 크기를 의미하는 것은 아니고, 데드락을 방지하기 위한 최소한의 크기라는 것을 알아두자. 
- 또한, 풀 크기를 확장하기 전에 애플리케이션 수준에서 해결할 수 있는 방안이 있다면 먼저 검토하는 것이 좋을 것이다.

### DBMS 최대 커넥션의 수

- 각각의 DBMS는 최대 커넥션 사이즈를 지정할 수 있으며, 관련된 환경변수는 아래와 같다.

- MySQL: `max_connections`
- PostgreSQL: `max_connections`
- Oracle: `sessions` & `processes`
- MSSQL: `user connections`

- 각각 DB를 사용하는 애플리케이션의 총 커넥션 수는 DBMS에 설정된 최대 커넥션 수를 넘지 않는 선에서 설정되어야만 한다. 
- 만약 MySQL을 사용할 경우, `max_connections` 보다 많은 양의 연결을 시도하려고 하면 아래와 같은 에러가 발생하며 문제가 생길 것이다.

```shell
ERROR 1040 (HY000): Too many connections
```

- 하지만 서비스 트래픽을 감당하기 위해 DBMS의 기본 설정 값보다 많은 커넥션이 필요하다면 설정값을 변경할 수도 있다. 
- 하지만 너무 높은 `max_connections` 값은 운영 중 불필요한 연결을 방치할 가능성을 높이게 된다. 
- 또한 connection은 생각보다 자원을 많이 사용하기 때문에 connection 수의 증가에 따라 CPU나 RAM을 얼마나 사용하는지 확인하고, 성능에는 문제가 없는지 따로 확인해야 한다. 따라서 적절한 값을 설정할 수 있도록 해야 한다.

## Hikari Metrics

- Spring Boot는 지표 수집, 추적, 감사 등의 모니터링을 쉽게 할 수 있는 다양한 편의 기능을 제공하는 Actuator라는 툴을 기본으로 제공한다. 
- 이는 Micrometer라는 벤더 독립적인 인터페이스를 내장하여 사용한 것으로 HikariCP에 대한 수치도 Actuator를 통해 수집되게 된다.

- `hikaricp.connections`: 현재 Connection Pool 사이즈
- `hikaricp.connections.active`: 현재 사용 중인 활성 Conneciton 수
- `hikaricp.connections.idle`: 현재 유휴 상태의 Connection 수
- `hikaricp.connections.pending`: 현재 대기 중인 Connection 수
- `hikaricp.connections.creation`: 생성된 Connection 수
- `hikaricp.connections.timeout`: Connection 요청 시간 초과 횟수
- `hikaricp.connections.acquire`: Connection 획득하는 데 걸린 시간
- `hikaricp.connections.usage`: Connection 사용 시간
- `hikaricp.connections.max`: 설정된 최대 Connection Pool 사이즈
- `hikaricp.connections.min`: 설정된 최소 유휴 Connection Pool 사이즈

## HikariCP는 왜 빠른가?

- 성능과 Connection Pool을 생각할 때, 많은 사람들은 풀 자체가 성능 공식에서 가장 중요한 부분이라고 착각할 수도 있다.
- 하지만 반드시 그렇다고 말할 수는 없는게, `getConnection()` 호출의 횟수는 다른 JDBC 작업에 비해 적은 편이다.
  - 성능 향상의 상당 부분은 `Conneciton`, `Statement` 등을 감싸는 `위임 객체 delegates`의 최적화에서 이뤄진다.

### 바이트코드

- HikariCP는 지금처럼 빨라지기 위해 바이트코드 수준의 엔지니어링을 넘어섰다고 한다.
- `Just-In-Time Compiler`가 개발자를 도울 수 있도록 모든 트릭을 동원했다.
- 컴파일러의 바이트코드 출력과 JIT의 어셈블리 출력까지 연구해 주요 루틴을 JIT의 인라인 임계값 이하로 제한한다.
- 상속 계층 구조를 단순화하고, 멤버 볌수를 섀도잉하며, 형 변환을 제거했다.

### micro 최적화

- 거의 측정되지 않을 정도의 미세한 최적화가 많이 포함되어 있지만, 이러한 최적화가 결합되면 전체적인 성능이 크게 향상된다.

#### ArrayList

- 가장 중요한 최적화 중 하나는 `ProxyConnection`에 의해 열린 `Statement` 인스턴스를 추적하기 위해 사용되던 `ArrayList<Statement>`를 제거한 것이다.
  - 기존에는 이 컬렉션에서 해야할 작업이 많았다.
  - `Statement`가 닫힐 때 해당 `Statement` 제거해야 한다.
  - `Connection`이 닫히면 반복을 통해 열려 있는 모든 `Statement`를 닫야하고, 컬렉션을 비워야 했다.
- `ArrayList`의 `get(int index)`는 호출될 때마다 범위 검사를 하는데 이는 불필요한 오버헤드다.
- `remove(Object)`는 컬렉션의 head부터 tail까지 스캔을 수행해야 하는데, 일반적인 JDBC 프로그래밍의 일반적인 패턴은 사용 후 `Statement`를 즉시 닫거나, 열린 순서의 역순으로 닫는 경우가 많다.
  - 이런 경우 뒤에서부터 하는 스캔이 유리하다.
- 따라서 범위 검사를 제거하고, 스캔을 뒤에서부터 하는 `FastList`라는 커스텀 클래스를 만들어 사용하고 있다고 한다.

#### ConcurrentBag

- `잠금 없는 컬렉션 lock-free collection`이다.
  - C# .NET의 `ConcurrentBag` 클래스에서 착안했지만, 내부 구현은 상당히 다르다고 한다.
- `ConcurrentBag`은 아래와 같은 기능을 제공한다.
  - 잠금 없는 설계
  - ThreadLocal 캐싱
  - Queue-stealing
  - Direct hand-off 최적화
- 이 기능을 통해 높은 수준의 동시성과 극도로 낮은 대기 시간, `false-sharing` 발생을 최소화한다.

> #### 참고: False sharing 
> - 분산된 일관성 캐시를 사용하는 시스템에서 가장 작은 리소스 블록 크기 단위로 캐시 메커니즘이 데이터를 관리할 때 발생할 수 있는 성능 저하 패턴
> - 시스템의 한 참여자가 다른 참여자에 의해 변경되지 않은 데이터를 주기적으로 접근하려고 할 때, 해당 데이터가 변경되고 있는 데이터와 동일한 캐시 블록을 공유하고 있다면, 캐싱 프로토콜은 논리적으로는 불필요함에도 불구하고 첫 번째 참여자가 전체 캐시 블록을 다시 로드하도록 강제할 수 있다.
> - 캐싱 시스템은 이 블록 내에서 이루어지는 활동에 대해 알지 못하며, 실제로 리소스를 공유 접근할 때 발생하는 오버헤드와 동일한 캐싱 시스템의 오버헤드를 첫 번째 참여자가 감당하도록 만든다.

#### invokevirtual vs ivokestatic

- HikariCP는 `Connection`, `Statement`, `ResultSet` 인스턴스를 위한 프록시를 생성하기 위해 처음에는 `싱글턴 팩토리 singleton factory`를 사용했다.
  - `ProxyConnection`의 경우, 정적 필드에 저장되어 있었다.(`PROXY_FACTORY`)

```java
public final PreparedStatement prepareStatement(String sql, String[] columnNames) throws SQLException {
    return PROXY_FACTORY.getProxyPreparedStatement(this, delegate.prepareStatement(sql, columnNames));
}
```

- 기존의 싱글톤 팩토리를 사용하면 아래와 같은 바이트코드가 만들어진다.

```shell
public final java.sql.PreparedStatement prepareStatement(java.lang.String, java.lang.String[]) throws java.sql.SQLException;
  flags: ACC_PRIVATE, ACC_FINAL
  Code:
    stack=5, locals=3, args_size=3
       0: getstatic     #59                 // Field PROXY_FACTORY:Lcom/zaxxer/hikari/proxy/ProxyFactory;
       3: aload_0
       4: aload_0
       5: getfield      #3                  // Field delegate:Ljava/sql/Connection;
       8: aload_1
       9: aload_2
      10: invokeinterface #74,  3           // InterfaceMethod java/sql/Connection.prepareStatement:(Ljava/lang/String;[Ljava/lang/String;)Ljava/sql/PreparedStatement;
      15: invokevirtual #69                 // Method com/zaxxer/hikari/proxy/ProxyFactory.getProxyPreparedStatement:(Lcom/zaxxer/hikari/proxy/ConnectionProxy;Ljava/sql/PreparedStatement;)Ljava/sql/PreparedStatement;
      18: return
```

- 먼저 정적 필드 `PROXY_FACTORY`의 값을 가져오기 위해 `getstatic` 호출이 있으며, 마지막으로 `ProxyFactory` 인스턴스에서 `getProxyPreparedStatement()`를 호출하기 위한 `invokevirtual` 호출이 이뤄지는 것을 볼 수 있다.
- HikariCP는 Javassist가 생성한 싱글턴 팩토리를 제거하고, Javassist가 메소드 본문을 생성하는 정적 메소드를 가진 final 클래스로 대체했다.

> #### Javassist
> - Java 애플리케이션에서 바이트코드 조작(Bytecode Manipulation)을 쉽게 수행할 수 있도록 도와주는 라이브러리

```java
public final PreparedStatement prepareStatement(String sql, String[] columnNames) throws SQLException {
    return ProxyFactory.getProxyPreparedStatement(this, delegate.prepareStatement(sql, columnNames));
}
```

- 이제 바이트코드는 아래와 같이 만들어진다.

```shell
private final java.sql.PreparedStatement prepareStatement(java.lang.String, java.lang.String[]) throws java.sql.SQLException;
  flags: ACC_PRIVATE, ACC_FINAL
  Code:
    stack=4, locals=3, args_size=3
       0: aload_0
       1: aload_0
       2: getfield      #3                  // Field delegate:Ljava/sql/Connection;
       5: aload_1
       6: aload_2
       7: invokeinterface #72,  3           // InterfaceMethod java/sql/Connection.prepareStatement:(Ljava/lang/String;[Ljava/lang/String;)Ljava/sql/PreparedStatement;
      12: invokestatic  #67                 // Method com/zaxxer/hikari/proxy/ProxyFactory.getProxyPreparedStatement:(Lcom/zaxxer/hikari/proxy/ConnectionProxy;Ljava/sql/PreparedStatement;)Ljava/sql/PreparedStatement;
      15: areturn
```

1. `getstatic` 호출이 사라졌고,
2. `invokevirtual` 호출이 `invokestatic` 호출로 대체되어 JVM에서 더 쉽게 최적화할 수 있게 되었으며,
3. 스택 크기가 5개에서 4개로 줄었다. 이는 `invokevirtual` 호출의 경우, 스택에 `ProxyFactory` 인스턴스(`this`)가 암묵적으로 전달되기 때문이다. 또한 `getProxyPreparedStatement()`가 호출될 때 해당 값이 스택에서 추가적으로 `pop`되는 동작이 발생한다. 

- 결론적으로 정적 필드 접근, 스택의 push와 pop 동작을 제거했으며, `호출 지점 callsite`이 변경되지 않음을 보장하여 JIT 컴파일러가 호출을 더 쉽게 최적화할 수 있게 만들었다.

#### 기타

- 그 외에도 코드의 모든 중요한 경로에서 명령어 수를 줄여, 각 메소드가 OS 스케줄러의 실행 퀀텀 내에 맞도록 최적화하여 캐시 라인 무효화로 인한 성능 저하를 피할 수 있었다고 한다.

## MySQL의 wait_timeout & HikariCP의 maxLifetime

- [HikariCP의 커넥션 maxLifetime과 MySQL wait timeout간의 관계 알아보기!](https://seongonion.tistory.com/157)
- [HikariCP와 커넥션 누수(Connection Leak) 관련 트러블슈팅](https://do-study.tistory.com/97)

# 참고자료

- [HikariCP Dead lock에서 벗어나기 (실전편)](https://techblog.woowahan.com/2663/)
- [[Spring] DB커넥션풀과 Hikari CP 알아보기](https://velog.io/@miot2j/Spring-DB%EC%BB%A4%EB%84%A5%EC%85%98%ED%92%80%EA%B3%BC-Hikari-CP-%EC%95%8C%EC%95%84%EB%B3%B4%EA%B8%B0)
- [JDBC Connection 에 대한 이해, HikariCP 설정 팁](https://jiwondev.tistory.com/291)
- [Down the Rabbit Hole](https://github.com/brettwooldridge/HikariCP/wiki/Down-the-Rabbit-Hole)
