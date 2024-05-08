# Authentication Architecture

## Security Authentication/Authorization

- `AuthenticationFilter`가 인증처리를 진행해 `Authentication` 객체를 생성해 `AuthenticationManager`에 전달
- `AuthenticationManager`는 `AuthenticationProvider`에 사용자가 존재하는지, 사용자의 아이디 & 패스워드가 맞는지에 대한 처리를 위임
- `AuthenticationProvider`는 인증이 성공하면 `Authentication` 객체를 만들어 상위 계층으로 전달한다. 
- `SecurityContextHolder`를 통해 `SecurityContext`에 `Authentication`을 저장한다.

<img src="img/03/authentication01.png">

<br/>

## 인증 Authentication

- 인증은 특정 자원에 접근하려는 유저의 신원을 확인하는 방법
- 사용자 인증의 일반적인 방법은 사용자 이름과 비밀번호를 입력하게 하는 것으로, 인증이 수행되면 신원을 확인하고 권한 부여를 할 수 있다.
- `Authentication`은 사용자 인증 정보를 저장하는 토큰 개념의 객체로 활용되며 인증 이후 `SecurityContext`에 저장되어 전역적으로 참조가 가능하다.

### Authentication API

- `Authentication`은 `Principal`을 상속한다.

```java
public interface Authentication extends Principal, Serializable {
  ...
}
```

- `Authentication`의 메소드 종류는 아래와 같다.
  - `getPrincipal()`: `Principal`은 인증 주체를 의미. 인증 요청의 경우 사용자 이름을 가져오고, 인증 후에는 UserDetails 타입의 객체를 가져온다.
  - `getCredentials()`: `Credential`이란 인증 주체가 올바른 것을 증명하는 자격 증명을 뜻함. 대개 비밀번호
  - `getAuthorities()`: 인증 주체인 `Pricipal`에 부여된 권한
  - `getDetails()`: 인증 요청에 대한 추가적인 세부사항을 저장한 객체를 받는다. IP 주소, 인증서 일련번호 등 
  - `isAuthenticated()`: 인증 상태 반환
  - `setAuthenticated(boolean)`: 인증 상태 설정

### 인증 절차

- 인증 처리 전 단계
  1. 클라이언트로부터 로그인 요청을 받았을 때, 필터체인이 실행되다가 `AuthenticationFilter` 차례가 오면 인증 처리를 시작할 준비를 한다.
  2. `AuthenticationFilter`는 사용자가 전달한 정보를 바탕으로 `Authentication` 객체를 만든다.
  3. `AuthenticationManager`에 `Authentication` 객체를 전달한다.
- 인증 처리 단계
  1. `AuthenticationManager`는 전달받은 `Authentication` 객체와 `UserDetailsService` 객체 등을 활용해 인증 처리를 수행한다.
  2. 인증 처리를 통해 신원을 확인하면 `Authentication` 객체를 만드는데, 위의 `AuthenticationFilter`가 만들었던 `Authentication` 객체와는 살짝 다르다.
  3. 이제 `Authentication` 객체를 `AuthenticationFilter`로 다시 전달하는데, 필터는 `SecurityContextHolder`를 사용해 `SecurityContext`를 생성하고 거기에 `Authentication` 객체를 넣어 저장한다.

<img src="img/03/authentication02.png">

<br/>

<br/><br/>

# 참고자료

- [스프링 시큐리티 완전 정복 [6.x 개정판]](https://www.inflearn.com/course/%EC%8A%A4%ED%94%84%EB%A7%81-%EC%8B%9C%ED%81%90%EB%A6%AC%ED%8B%B0-%EC%99%84%EC%A0%84%EC%A0%95%EB%B3%B5/dashboard)