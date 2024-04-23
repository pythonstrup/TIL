# Spring Security Initialization

## 자동 설정에 의한 기본 보안 작동

- 서버가 가동되면 스프링 시큐리티의 초기화 작업 및 보안설정이 이뤄진다. (별도의 작성 없이)

1. 기본적으로 모든 요청에 대하여 인증여부를 검증하고 인증이 승인되어야 자원에 접근 가능
2. 인증 방식은 폼 로그인 방식과 httpBasic 로그인 방식 제공
3. 인증을 시도할 수 있는 로그인 페이지가 자동적으로 생성되어 렌더링
4. 인증 승인이 이루어질 수 있도록 한 개의 계정이 기본적으로 제공 (SecurityProperties 설정 클래스에서 계정을 생성)
   - username: user
   - password: 랜덤 문자열 => 로그에 찍힘

- 아래의 `defaultSecurityFilterChain` 메소드를 유의깊게 보자.

```java
@Configuration(proxyBeanMethods = false)
@ConditionalOnWebApplication(type = Type.SERVLET)
class SpringBootWebSecurityConfiguration {

	@Configuration(proxyBeanMethods = false)
	@ConditionalOnDefaultWebSecurity
	static class SecurityFilterChainConfiguration {

		@Bean
		@Order(SecurityProperties.BASIC_AUTH_ORDER)
		SecurityFilterChain defaultSecurityFilterChain(HttpSecurity http) throws Exception {
			http.authorizeHttpRequests((requests) -> requests.anyRequest().authenticated());
			http.formLogin(withDefaults());
			http.httpBasic(withDefaults());
			return http.build();
		}

	}
	
	@Configuration(proxyBeanMethods = false)
	@ConditionalOnMissingBean(name = BeanIds.SPRING_SECURITY_FILTER_CHAIN)
	@ConditionalOnClass(EnableWebSecurity.class)
	@EnableWebSecurity
	static class WebSecurityEnablerConfiguration {

	}
}
```

- `SecurityProperties`에서 User를 생성하고 Security 기본 설정에서 사용하게 한다.

```java
@ConfigurationProperties(
  prefix = "spring.security"
)
public class SecurityProperties {
    ...
    
    private final User user = new User();
    
    ...
}
```

- 자동 설정은 무조건 실행되는 것이 아니라 `@ConditionalOnDefaultWebSecurity`의 조건에 의해 실행된다.

```java
@Target({ ElementType.TYPE, ElementType.METHOD })
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Conditional(DefaultWebSecurityCondition.class)
public @interface ConditionalOnDefaultWebSecurity {}
```

- 조건은 `@Conditional`의 `DefaultWebSecurityCondition`에 존재한다.
- `@ConditionalOnClass`와 `@ConditionalOnMissingBean` 조건이 모두 참이어햐만 기본 보안 작동이 이뤄진다.
  - Security 의존성이 존재하면 `SecurityFilterChain` 클래스와 `HttpSecurity` 클래스가 존재하기 때문에 참이다.
  - `SecurityFilterChain`의 빈을 따로 생성해준 적이 없다면 참이 된다.

```java
class DefaultWebSecurityCondition extends AllNestedConditions {

	DefaultWebSecurityCondition() {
		super(ConfigurationPhase.REGISTER_BEAN);
	}

	@ConditionalOnClass({ SecurityFilterChain.class, HttpSecurity.class })
	static class Classes { }
	
	@ConditionalOnMissingBean({ SecurityFilterChain.class })
	static class Beans { }
}
```
