# 스프링 병렬 테스트

- Spring의 TestContext 프레임워크는 하나의 JVM에서 병렬 테스트 실행에 대한 기본적인 지원을 제공한다. 

## 유의사항

- 아래의 경우에는 병렬 테스트를 하지 않는 것이 좋다.
1. Spring Framework의 `@DirtiesContext` 지원을 사용하는 경우.
2. Spring Framework의 `@MockitoBean` 또는 `@MockitoSpyBean` 지원을 사용하는 경우.
3. Spring Boot의 `@MockBean` 또는 `@SpyBean` 지원을 사용하는 경우.
4. JUnit 4의 `@FixMethodOrder` 지원 또는 테스트 메서드가 특정 순서로 실행되도록 설계된 테스트 프레임워크 기능을 사용하는 경우. 
   1. 그러나, 전체 테스트 클래스가 병렬로 실행되는 경우는 해당되지 않는다.
5. 데이터베이스, 메시지 브로커, 파일 시스템 등과 같이 공유 서비스 또는 시스템의 상태를 변경하는 경우. 이 내용은 임베디드 시스템과 외부 시스템 모두에 적용된다.

> #### `@DirtiesContext`
> - 테스트 실행 후 애플리케이션 컨텍스트(Application Context)가 더러워졌음을 표시하는 데 사용된다.
> - 이는 해당 컨텍스트가 변경되었기 때문에 다음 테스트에서는 재사용하지 않고 새로 생성해야 한다는 것을 Spring에게 알려준다.

## `@MockBean`을 사용할 수 없다????

- 실제로 `@MockBean`이 붙어 있는 테스트를 돌려보니 테스트가 실패한다.
- `@MockBean`은 Spring의 애플리케이션 컨텍스트에 Mock 객체를 등록한다. 
  - 이는 테스트 실행 중 컨텍스트 상태를 변경하는 것이므로, Spring 테스트 프레임워크가 이를 감지하여 컨텍스트를 더럽다고(`@DirtiesContext`) 판단할 수 있다.
  - 결과적으로, 이후 테스트에서 컨텍스트를 재사용하지 못하고 매번 새로 생성해야 하므로 테스트 실행 시간이 크게 늘어날 수 있다.
  - 병렬 테스트 실행 시 다른 테스트에 영향을 줄 가능성이 커진다.
- 따라서 통합 테스트에서 Mock 객체를 사용해야 한다면, 테스트 전용 `Configuration`을 만들어 `Mock`을 `Bean`으로 등록해줘야 한다.

## 코드 예시

```java
@TestConfiguration
public class MockBeanTestConfig {

  @Bean
  public MessageSender messageSender() {
    return Mockito.mock(MessageSender.class);
  }
}

@WebMvcTest
class ControllerTest {
  
  @Autowired
  MessageSender messageSender;
}
```

- 여기서 주의해야할 사항이 있다. 만약 messageSender라는 빈이 다른 곳에서도 등록되면 에러가 발생할 것이다.
- 따라서 아래와 같이 `@Profile`을 통해 제외해줘야 한다.

```java
@Component
@Profile("!test")
public class MyMessageSender implements MessageSender {
  // ...
}
```

## 여기서 또 문제!

- H2 Memory DB를 사용하니 문제가 발생한다.
- 하나의 H2 Memory DB를 모든 테스트에서 동시에 사용하니, 데이터 셋의 오염으로 인한 테스트 실패, 충돌 등이 발생한다.
- 각 병렬 테스트가 자신만의 독립된 데이터베이스 인스턴스를 사용하도록 설정할 수 있다. 
  - 이는 데이터 충돌을 방지하고 각 테스트가 다른 테스트와 독립적으로 실행될 수 있게 한다.
- @AutoConfigureTestDatabase라는 어노테이션?
- @TestContainers를 통해 각각의 환경을 구성해 병렬 테스트를 실행해볼 수 있을 것이다.

## TestContainers 문제

- connection refuse가 발생

### 생명주기 관리 따로 해주는 방법!

```java
public class MySQLTestContainers {

  static String MYSQL_IMAGE = "mysql:8.0.33";
  static MySQLContainer MYSQL_CONTAINER;

  static {
    MYSQL_CONTAINER = new MySQLContainer(MYSQL_IMAGE);
    MYSQL_CONTAINER.start();
    System.setProperty("spring.datasource.url", MYSQL_CONTAINER.getJdbcUrl());
    System.setProperty("spring.datasource.username", MYSQL_CONTAINER.getUsername());
    System.setProperty("spring.datasource.password", MYSQL_CONTAINER.getPassword());
  }

  @PreDestroy
  void stop() {
    MYSQL_CONTAINER.stop();
  }

  public String getJdbcUrl() {
    return MYSQL_CONTAINER.getJdbcUrl();
  }

  public String getUsername() {
    return MYSQL_CONTAINER.getUsername();
  }

  public String getPassword() {
    return MYSQL_CONTAINER.getPassword();
  }
}
```

```java
public class RedisTestContainers {

  private static String REDIS_VERSION = "7.2.5";

  protected static RedisContainer REDIS_CONTAINER;

  static {
    REDIS_CONTAINER =
        new RedisContainer(RedisContainer.DEFAULT_IMAGE_NAME.withTag(REDIS_VERSION))
            .withExposedPorts(6379);
    REDIS_CONTAINER.start();
    System.setProperty("spring.data.redis.host", REDIS_CONTAINER.getRedisHost());
    System.setProperty("spring.data.redis.port", Integer.toString(REDIS_CONTAINER.getRedisPort()));
  }

  @PreDestroy
  void stop() {
    REDIS_CONTAINER.stop();
  }

  public String getHost() {
    return REDIS_CONTAINER.getRedisHost();
  }

  public int getPort() {
    return REDIS_CONTAINER.getRedisPort();
  }
}
```

```java
@Configuration
public class TestDataSourceConfig {

  @Bean(name = "mysqlTestContainers")
  public MySQLTestContainers mysqlTestContainers() {
    return new MySQLTestContainers();
  }

  @Bean
  @DependsOn("mySQLTestContainers")
  public DataSource dataSource(MySQLTestContainers containers) {
    HikariDataSource dataSource = new HikariDataSource();
    dataSource.setDriverClassName("com.mysql.jdbc.Driver");
    dataSource.setJdbcUrl(containers.getJdbcUrl());
    dataSource.setUsername(containers.getUsername());
    dataSource.setPassword(containers.getPassword());
    return dataSource;
  }

  @Bean(name = "redisTestContainers")
  public RedisTestContainers redisTestContainers() {
    return new RedisTestContainers();
  }

  @Bean(name = "redisConnectionFactory")
  @DependsOn("redisTestContainers")
  public RedisConnectionFactory redisConnectionFactory(RedisTestContainers redisTestContainers) {
    return new LettuceConnectionFactory(
        redisTestContainers.getHost(), redisTestContainers.getPort());
  }

  @Bean
  @DependsOn("redisConnectionFactory")
  public RedisTemplate<?, ?> redisTemplate(final RedisConnectionFactory redisConnectionFactory) {
    RedisTemplate<byte[], byte[]> template = new RedisTemplate<>();
    template.setConnectionFactory(redisConnectionFactory);
    return template;
  }
}
```

## Gradle에서 병렬 처리?

- 아래 파일을 프로젝트 root에 추가해준다.
- `gradle.properties`

```properties
org.gradle.parallel=true
```

### 각 테스트 유형별로 병렬 실행하기

- Unit 테스트는 전체 병렬로 실행하되, 상태가 변경될 수 있는 테스트에 대해서는 아래와 같이 직렬로 실행하기 위해 `@Execution(value = ExecutionMode.SAME_THREAD)`을 붙인다.
- 직렬로 실행해야 하는 Test 환경에 대한 코드

```java
@Tag("controllerTest")
@Execution(value = ExecutionMode.SAME_THREAD)
public abstract class ControllerTest {}
```

```java
@Tag("e2eTest")
@Execution(value = ExecutionMode.SAME_THREAD)
public abstract class E2eTestSupport {}
```

```java
@Tag("integrationTest")
@Execution(value = ExecutionMode.SAME_THREAD)
public abstract class TestIntegrationSupport {}
```

- 아래 코드를 통해 각 테스트가 병렬로 처리될 수 있도록 했다.
- `build.gradle.kts`

```kotlin
tasks.register<Test>("integrationTest") {
    description = "Run all integration tests"
    group = "verification"
    useJUnitPlatform {
        includeTags("integrationTest") // 'integration' 태그 포함
    }
    maxParallelForks = 1
}

tasks.register<Test>("e2eTest") {
    description = "Run all e2e tests"
    group = "verification"
    useJUnitPlatform {
        includeTags("e2eTest") // 'e2e' 태그 포함
    }
    maxParallelForks = 1
}

tasks.register<Test>("controllerTest") {
    description = "Run all controller tests"
    group = "verification"
    useJUnitPlatform {
        includeTags("controllerTest") // 'controller' 태그 포함
    }
    maxParallelForks = 1
}

tasks.register<Test>("unitTest") {
    description = "Run all unit tests"
    group = "verification"
    useJUnitPlatform {
        excludeTags("integration", "e2e", "controller") // 태그가 있는 테스트 제외
    }
    maxParallelForks = 3 // 병렬 실행
}
```

# 참고자료

- [Parallel Test Execution](https://docs.spring.io/spring-framework/reference/testing/testcontext-framework/parallel-test-execution.html)
- [[Test] 기묘한 병렬 테스트](https://ryumodrn.tistory.com/41)
- [테스트 코드에서 @MockBean 사용의 문제점과 해결법](https://velog.io/@glencode/%ED%85%8C%EC%8A%A4%ED%8A%B8-%EC%BD%94%EB%93%9C%EC%97%90%EC%84%9C-MockBean-%EC%82%AC%EC%9A%A9%EC%9D%98-%EB%AC%B8%EC%A0%9C%EC%A0%90%EA%B3%BC-%ED%95%B4%EA%B2%B0%EB%B2%95)
- [병렬 테스트 시 DB처리](https://www.inflearn.com/community/questions/1321624/%EB%B3%91%EB%A0%AC-%ED%85%8C%EC%8A%A4%ED%8A%B8-%EC%8B%9C-db%EC%B2%98%EB%A6%AC?srsltid=AfmBOorYSWEX6Dou0cfa0uPItupW1juKlNF-Hkw-Lg9e5Cvt4abuGqur)
- [TestContainer 로 멱등성있는 integration test 환경 구축하기](https://medium.com/riiid-teamblog-kr/testcontainer-%EB%A1%9C-%EB%A9%B1%EB%93%B1%EC%84%B1%EC%9E%88%EB%8A%94-integration-test-%ED%99%98%EA%B2%BD-%EA%B5%AC%EC%B6%95%ED%95%98%EA%B8%B0-4a6287551a31)
- [Testcontainers에 의한 docker container 생성 폭발을 막아라](https://flex.team/blog/2024/07/29/tech-testcontainers/)
- [멀티모듈 환경에서 Testcontainers로 일관성있는 테스트 환경 만들기](https://binaryflavor.com/%EB%A9%80%ED%8B%B0%EB%AA%A8%EB%93%88-%ED%99%98%EA%B2%BD%EC%97%90%EC%84%9C-testcontainers%EB%A1%9C-%EC%9D%BC%EA%B4%80%EC%84%B1%EC%9E%88%EB%8A%94-%ED%85%8C%EC%8A%A4%ED%8A%B8-%ED%99%98%EA%B2%BD-%EB%A7%8C%EB%93%A4%EA%B8%B0/)
