# Spring Data Redis

### gradle 의존성 추가

- maven에서 spring-data-redis의 버전을 확인해 의존성을 추가해주자.

```kotlin
implementation("org.springframework.boot:spring-boot-starter-data-redis")
```

- spring-date-redis 의존성을 추가하면 Redis Java Client인 Lettuce와 Jedis가 자동으로 설치된다.
- lettuce
    - Non-Blocking 방식의 Redis Java Client로 동기 및 비동기 통신을 지원한다.
    - 클러스터, 센티넬, 파이프라이닝 및 코덱 지원 등 고급 기능을 지원한다.

```xml
<dependency>
    <groupId>io.lettuce</groupId>
    <artifactId>lettuce-core</artifactId>
    <version>${lettuce}</version>
    <optional>true</optional>
</dependency>
```

- Jedis
    - Redis 내부의 클라이언트 라이브러리다.
    - 다른 클라이언트에 비해 기능이 적지만, 많은 양의 메모리를 처리할 수 있다.

```xml
<dependency>
	<groupId>redis.clients</groupId>
	<artifactId>jedis</artifactId>
	<version>${jedis}</version>
	<optional>true</optional>
</dependency>
```

# RedisDataRepository 구성해보기

## AutoConfig

[Working with NoSQL Technologies :: Spring Boot](https://docs.spring.io/spring-boot/reference/data/nosql.html#data.nosql.redis)

- properties에서 spring.data.redis에 있는 구성요소를 작성해주면 Spring-data-redis에서 자동으로 auto configuration을 진행해준다.
- Redis에는 기본적으로 0~15번(16개)까지 데이터베이스가 있다.
    - 기본값이 0이므로 생략해줘도 된다.

```yaml
spring:
	data:
		redis:
			host: "localhost"
			port: 6379
			database: 0
			username: 
			password: 
```

## Customizing Config

[Usage :: Spring Data Redis](https://docs.spring.io/spring-data/redis/reference/redis/redis-repositories/usage.html#page-title)

- 설정을 커스터마이징하고 싶다면 아래와 같이 Config 코드를 구성해볼 수 있다.
- RedisTemplate Bean을 생성할 때, 와일드카드를 사용했기 때문에 어떤 타입이 와도 사용할 수 있다.

```kotlin
@Configuration
@EnableRedisRepositories(value = "com.pythonstrup.core")
public class RedisConfig {

  @Bean
	public RedisConnectionFactory redisConnectionFactory(
      @Value("${spring.data.redis.host}") String host,
      @Value("${spring.data.redis.port}") int port,
      @Value("${spring.data.redis.database}") int database,
      @Value("${spring.data.redis.username}") String username,
      @Value("${spring.data.redis.password}") String password) {
    RedisStandaloneConfiguration config = new RedisStandaloneConfiguration();
    config.setHostName(host);
    config.setPort(port);
    config.setDatabase(database);
    config.setUsername(username);
    config.setPassword(password);
    return new LettuceConnectionFactory(config);
  }

  @Bean
  public RedisTemplate<?, ?> redisTemplate(RedisConnectionFactory redisConnectionFactory) {
    RedisTemplate<byte[], byte[]> template = new RedisTemplate<byte[], byte[]>();
    template.setConnectionFactory(redisConnectionFactory);
    return template;
  }
}
```

## Entity

- JPA에서는 @Entity
- MongoDB에서는 @Document
- 그렇다면 Redis에서는 Entity를 설정하기 위해 어떤 어노테이션을 사용할까? @RedisHash다.
- 여기서 주의할 점은 JPA와 다르게 @Id 어노테이션의 패키지가
    - JPA ID: **import** jakarta.persistence.Id;
    - Redis ID: **import** org.springframework.data.annotation.Id;

```java
@RedisHash
public class RefreshToken {

  @Id
  private Long memberId;
  private String refreshToken;
}
```

- 아래와 같이 다른 어노테이션과 겹쳐 사용하는 것은 안티패턴이라고 하니 주의하자.

```java
@Entity
@Document
class Person { ... }
```

## Repository

```java
public interface RefreshTokenRedisRepository extends CrudRepository<RefreshToken, Long> {...}
```

# Time To Live

[Time To Live :: Spring Data Redis](https://docs.spring.io/spring-data/redis/reference/redis/redis-repositories/expirations.html#page-title)

- @RedisHash 어노테이션에서 timeToLive 값을 지정해줄 수 있다.

```java
@RedisHash(value = "refreshToken", timeToLive = 3600)
@NoArgsConstructor(access = AccessLevel.PROTECTED)
@Getter
public class RefreshToken {...}
```

- 또한 @TimeToLive라는 어노테이션을 사용해 만료일시를 저장할 수 있다. 방법은 두 가지다.
- 필드값으로 받는 방법

```java
@RedisHash(value = "refreshToken")
public class RefreshToken {
  @Id private Long id;
  @TimeToLive private Long expiration;
}
```

- method를 사용하는 방법

```java
@RedisHash(value = "refreshToken")
public class RefreshToken {
  @Id private Long id;
  @TimeToLive 
  public long getTimeToLive() {
    return 3600L;
  }
}
```

- Redis Script를 아래와 같이 실행하면 TTL이 설정되었는지 알아볼 수 있다.

```bash
TTL refreshToken:{id}
```

# Redis 통합테스트 환경 구축하기

- @MockBean을 사용해 Redis Infrastructure에 대한 의존성을 끊어버리는 방법도 있겠지만, 그렇게 되면 테스트의 현실성이 떨어지게 된다.
- Testcontainers를 사용하면 Docker 환경에서 컨테이너를 띄워 Redis를 사용할 수 있는 통합테스트 환경을 구축할 수 있다.
- 의존성 설치
    - org.testcontainers:junit-jupiter 의존성이 org.testcontainers:testcontainers 라이브러리를 포함하고 있다.

```kotlin
testImplementation("org.testcontainers:junit-jupiter:1.20.1")
testImplementation("com.redis:testcontainers-redis:2.2.2")
```

- 테스트에도 properties를 추가해주자.
- redis의 기본 username과 password는 빈 값이다.

```yaml
spring:
	data:
		redis:
			host: "localhost"
			port: 6379
			database: 0
			username: 
			password: 
```

- 이제 Testcontainers를 생성한다.

```java
@SpringBootTest
@Testcontainers
public abstract class TestIntegrationSupport {

  @Container
  protected static RedisStackContainer redisContainer =
      new RedisStackContainer(
              RedisStackContainer.DEFAULT_IMAGE_NAME.withTag(RedisStackContainer.DEFAULT_TAG))
          .withExposedPorts(6379);
}
```

- 통합테스트 코드에서 위의 추상클래스를 상속받으면 Redis Testcontainers를 사용할 수 있다.

```java
public class MemberUseCaseTest extends TestIntegrationSupport {

	...
}
```