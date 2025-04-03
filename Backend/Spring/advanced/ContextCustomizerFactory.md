# ContextCustomizerFactory

## ContextCustomizer란?

- `ContextCustomizer`는 Spring 테스트 컨텍스트(`ConfigurableApplicationContext`)를 사용자 정의 방식으로 조정하는 역할을 한다.
- 이 과정은 빈 정의가 로드된 후 발생하지만, 컨텍스트가 새로고침되기 전(before refresh)에 수행된다.

## ContextCustomizerFactory란?

- `ContextCustomizerFactory`는 테스트 클래스에 맞는 `ContextCustomizer`를 생성하는 역할을 한다.
- 이때, 특정 조건을 기반으로 `ContextCustomizer`가 필요한지 판단한 후, 해당되는 경우 적절한 `ContextCustomizer`를 생성하여 반환한다.
  - 주로 특정 어노테이션의 존재 여부 등을 기준으로 판단

> #### 정리: ContextCustomizerFactory 실행 시점
> 1. Spring의 `ContextLoader`가 컨텍스트 구성 정보를 처리한 후
> 2. 테스트 클래스의 `MergedContextConfiguration`이 생성되기 전

### 예시: `MockServerContainerContextCustomizerFactory`

- WebSocket 관련 테스트를 위해 기본 등록
- 다음 조건을 만족하면 `MockServerContainerContextCustomizer`를 생성
  - `classpath`에 WebSocket이 존재하고
  - 테스트 클래스나 상위 클래스에 `@WebAppConfiguration`이 붙어 있다.
- `MockServerContainerContextCustomizer`는 `MockServerContainer`를 생성하고, `ServletContext`에 `jakarta.websocket.server.ServerContainer`라는 이름으로 저장한다.

## ContextCustomizerFactory를 등록하는 방법

### 1. `@ContextCustomizerFactories`를 사용하여 개별 테스트 클래스에 등록

- 특정 테스트 클래스, 해당 하위 클래스 또는 중첩 클래스에서 특정 `ContextCustomizerFactory`만 사용하고 싶을 경우, `@ContextCustomizerFactories` 애너테이션을 활용하면 된다.

```java

@ContextCustomizerFactories(MyCustomContextCustomizerFactory.class)
@SpringBootTest
public class MyTest {
// 테스트 코드
}
```

### 2. `spring.factories` 파일을 활용한 자동 등록 (`SpringFactoriesLoader` 사용)

- 테스트 전반에서 `ContextCustomizerFactory`를 자동으로 적용하고 싶다면 Spring의 `SpringFactoriesLoader`를 사용하여 등록할 수 있다.
-  `spring.factories` 파일을 통해 등록하는 방법 `src/test/resources/META-INF/spring.factories` 파일을 생성하고 아래 내용을 추가한다.

```properties
org.springframework.test.context.ContextCustomizerFactory=\
com.example.MyCustomContextCustomizerFactory
```

- 이렇게 하면 Spring이 자동으로 `MyCustomContextCustomizerFactory`를 감지하여 테스트에 적용한다.
- 즉, 개별 테스트마다 `@ContextCustomizerFactories`를 추가할 필요 없이, 전역적으로 적용할 수 있다.

## 여러 ContextCustomizerFactory가 등록될 경우 어떻게 병합되는가?

- 만약 `@ContextCustomizerFactories`로 등록된 사용자 정의 팩토리와 `spring.factories`를 통해 자동 등록된 기본 팩토리가 동시에 존재하면 Spring은 자동으로 두 개의 목록을 병합하여 적용한다.

### 💡 병합 과정

- 동일한 팩토리가 중복되지 않도록 제거한다.
- `spring.factories`를 통해 자동 등록된 기본 팩토리가 먼저 적용된다.
- `@ContextCustomizerFactories`를 통해 등록된 사용자 정의 팩토리가 기본 팩토리 뒤에 추가된다.


✅ 즉, `spring.factories`에 등록된 팩토리가 기본적으로 적용되지만, 특정 테스트에서 `@ContextCustomizerFactories`를 사용하여 추가적인 팩토리를 적용할 수도 있다.

----

## 📌 요약

- `ContextCustomizer` → Spring 테스트 컨텍스트(`ApplicationContext`)를 커스터마이징하는 역할
- `ContextCustomizerFactory` → 특정 조건에 따라 `ContextCustomizer`를 생성하는 팩토리
- `@ContextCustomizerFactories` 사용 → 특정 테스트 클래스에서 개별적으로 등록 가능
- `spring.factories` 사용 → 전체 테스트 환경에서 자동 적용 가능 (`SpringFactoriesLoader` 이용)
- 자동 병합 기능 → `spring.factories`에 등록된 기본 팩토리 + `@ContextCustomizerFactories`에 등록된 팩토리가 자동으로 병합됨

---

## Why

### 언제 사용해야 할까?

#### 1. 테스트 클래스에 따라 동적으로 설정을 바꿔야 할때

- ex1) 특정 어노테이션이 붙은 테스트에서만 Redis 컨테이너 실행
- ex2) 특정 테스트에서는 MongoDB 대신 InMemory DB 사용

### 안 써도 되는 경우

- `@TestConfiguration`, `@MockBean`, `@DynamicPropertySource` 등으로 충분할 때!
- 커스텀 설정이 고정되어 있고 모든 테스트에 동일하게 적용될 때! (`application-test.yaml`)

---

# 참고자료

- [스프링 공식문서](https://docs.spring.io/spring-framework/reference/testing/testcontext-framework/ctx-management/context-customizers.html)
- [Spring은 어떻게 테스트 환경에 맞는 WebTestClient를 자동 주입해주는걸까?](https://velog.io/@viiviii/Spring-WebTestClient%EB%8A%94-%EC%96%B4%EB%96%BB%EA%B2%8C-%EC%9E%90%EB%8F%99-%EC%A3%BC%EC%9E%85%EB%90%98%EB%8A%94%EA%B1%B8%EA%B9%8C)
