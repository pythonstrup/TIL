# 스프링 기본 설정

## Lettuce 설정

### `destoryMethod`

- [깃허브 Redis/Lettuce](https://github.com/redis/lettuce/wiki/Spring-Support)
- Lettuce는 내부적으로 Netty의 네이티브 리소스를 직접 관리하여 논블로킹 처리를 진행한다.
  - Spring의 일반적인 빈 생명주기만으로는 정리되지 않는다는 말이다.
  - 내부적으로 네트워크 커넥션 풀과 이벤트 루프를 사용하므로 꼭 정리해줘야 한다.

```java
private static final String DESTROY_SHUTDOWN = "shutdown";

@Bean(destroyMethod = DESTROY_SHUTDOWN)
public ClientResources redisClientResources() {
  return DefaultClientResources.builder()
    .commandLatencyPublisherOptions(
        DefaultEventPublisherOptions.disabled()) // metric 수집 비활성화 (메모리 누수 방지)
    .ioThreadPoolSize(redisProperties.ioPoolSize())
    .computationThreadPoolSize(redisProperties.computationPoolSize())
    .build();
}
```

- 이런 옵션들은 정리 X

```java
@Bean
public ClientOptions redisClientOptions() {
    final SocketOptions socketOptions =
        SocketOptions.builder().connectTimeout(CONNECTION_TIMEOUT).build();
    return ClientOptions.builder().socketOptions(socketOptions).build();
}
```

## 커넥션 설정

- 참고자료: [우아한기술블로그](https://techblog.woowahan.com/23121/)
- 참고자료2: [Hikari와 Redis Lettuce 비교](https://cheese10yun.github.io/redis-lettuce-connection/#google_vignette)

- 먼저 의존성을 추가해야 한다.

```kotlin
implementation("org.apache.commons:commons-pool2")
```

- Redis의 yaml 자동 설정도 가능하고,
- `LettucePoolingClientConfiguration`를 레디스 커넥션 풀 설정을 할 수 있다.

```java
GenericObjectPoolConfig<?> poolConfig = new GenericObjectPoolConfig<>();
poolConfig.setMaxTotal(20);      // 최대 커넥션 수
poolConfig.setMaxIdle(10);       // 유휴 상태 최대
poolConfig.setMinIdle(5);        // 유휴 상태 최소
poolConfig.setMaxWait(Duration.ofMillis(3000));

final LettucePoolingClientConfiguration clientConfig =
    LettucePoolingClientConfiguration.builder()
        ...
        .poolConfig(poolConfig)
        .build();
```

## RedisCacheWriter 설정

- 참고자료: [우아한기술블로그](https://techblog.woowahan.com/23138/)
- 기본적으로 `keys` 명령어로 설정되어 있는데, 이는 redis에 장애를 불러올 수 있을만큼 치명적이다.
- 따라서 `scan` 전략을 사용하도록 재설정해줘야 한다.



