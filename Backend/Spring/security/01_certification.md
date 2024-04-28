# Certification

## Form Login

- HTTP 기반의 폼 로그인 인증 메커니즘을 활성화하는 API
- 사용자 인증을 위한 사용자 정의 로그인 페이지를 쉽게 구현할 수 있다.
- 기본적으로 스프링 시큐리티가 제공하는 기본 로그인 페이지를 사용하며 사용자 이름과 비밀번호 필드가 포함된 간단한 로그인 양식을 제공
- 사용자는 웹 폼을 통해 자격 증명을 제공, `Spring Security`는 `HttpServletRequest`에서 이 값을 읽어온다.

<img src="img/02/form_login01.png">

### Form Login API

- `FormLoginConfigurer` 설정 클래스를 통해 API 설정 가능
- 내부적으로 `UsernamePasswordAuthenticationFilter`가 생성되어 폼 방식의 인증 처리 담당

```java
HttpSecurity.formLogin ( httpSecurityFormLoginConfigurer -> httpSecurityFormLoginConfigurer
    .loginPage("/loginPage")             // 사용자 정의 로그인 페이지로 전환, 기본 로그인페이지 무시
    .loginProcessingUrl("/loginProc")    // 사용자 이름과 비밀번호를 검증할 URL 지정 (Form action)
    .defaultSuccessUrl("/", [alwaysUse]) // 로그인 성공 이후 이동 페이지, alwaysUse 가 true 이면 무조건 지정된 위치로 이동(기본은 false) 
                                         // 인증 전에 보안이 필요한 페이지를 방문하다가 인증에 성공한 경우이면 이전 위치로 리다이렉트 됨
    .failureUrl("/failed")               // 인증에 실패할 경우 사용자에게 보내질 URL 을 지정, 기본값은 "/login?error" 이다
    .usernameParameter("username")       // 인증을 수행할 때 사용자 이름(아이디)을 찾기 위해 확인하는 HTTP 매개변수 설정, 기본값은 username
    .passwordParameter("password")       // 인증을 수행할 때 비밀번호를 찾기 위해 확인하는 HTTP 매개변수 설정, 기본값은 password
    .failureHandler(AuthenticationSuccessHandler) // 인증 실패 시 사용할 AuthenticationFailureHandler를 지정
                                                  // 기본값은 SimpleUrlAuthenticationFailureHandler 를 사용하여 "/login?error"로 리다이렉션 함
    .successHandler(AuthenticationFailureHandler) // 인증 성공 시 사용할 AuthenticationSuccessHandler를 지정
                                                  // 기본값은 SavedRequestAwareAuthenticationSuccessHandler 이다
    .permitAll());                                // failureUrl(), loginPage(), loginProcessingUrl() 에 대한 URL 에 모든 사용자의 접근을 허용 함
```

<br/>

## UsernamePasswordAuthenticationFilter

- 스프링 시큐리티는 `AbstractAuthenticationProcessingFilter` 클래스를 사용자의 자격 증명을 인증하는 기본 필터로 사용
- `UsernamePasswordAuthenticationFilter`는 `AbstractAuthenticationProcessingFilter`를 확장한 클래스로 `HttpServletRequest`에서 제출된 사용자 이름과 비밀번호로부터 인증을 수행
- 인증 프로세스가 초기화 될 때 로그인 페이지와 로그아웃 페이지 생성을 위한 `DefaultLoginPageGeneratingFilter` 및 `DefaultLogoutPageGeneratingFilter`가 초기화

<img src="img/02/username_password_authentication_filter01.png">

### 처리과정

- 사용자가 Get 방식으로 로그인 요청을 하면 `UsernamePasswordAuthenticationFilter`가 요청을 가로챈다.
- 이 때 인증 처리를 해야하는 요청인지 아닌지를 검증하고 과정을 진행한다. (`RequestMatcher`에서 true를 반환해야 다음 과정으로 넘어감)
  - `RequestMatcher`에서 false를 반환하면 `chain.doFilter()`가 호출되어 다음 필터로 넘어가 버린다. 
- `UsernamePasswordAuthenticationToken`은 인증 처리를 하기 위한 인증 객체다.
- `AuthenticationManager`는 아이디와 패스워드가 담긴 토큰(`UsernamePasswordAuthenticationToken`)을 받아 인증 처리를 진행한다.

<img src="img/02/username_password_authentication_filter02.png">