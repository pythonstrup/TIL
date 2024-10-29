# Chapter10. 아키텍처 경계 강제하기

- 일정 규모 이상의 모든 프로젝트에서는 시간이 지나면서 아키텍처가 서서히 무너지게 된다.
  - 계층 간의 경계가 약화되고, 코드는 점점 더 테스트하기 어려워진다.
  - 또, 새로운 기능을 구현하는 데 점점 더 많은 시간이 든다.

## 경계와 의존성

- 아키텍처의 어디에 경계가 있고, '경계를 강제한다'는 것이 어떤 의미?
- 안쪽부터 바깥층까지
  - 도메인 엔티티 -> 아웃고잉 포트 -> 어댑터
  - 도메인 엔티티 <- 인커밍 포트 <- 어댑터
  - 최상층에 설정 계층은 팩토리를 포함. 의존성 주입 메커니즘을 제공
- 이렇게 의존성 규칙을 강제하는 방법에 대해 알아보자.

## 접근 제한자

- 경계를 강제하기 위해 자바에서 제공하는 가장 기본적인 도구. `접근 제한자 visibility modifier`
- `public`, `protected`, `private`, 그리고 가장 중요한 `package-private`
- `package-private`은 왜 중요할까? 자바 패키지를 통해 클래스들을 응집적인 '모듈'로 만들어 주기 때문이다.
  - 모듈 내에서는 접근 가능하지만 패키지 바깥에서는 접근할 수 없는 특성.
  - 모듈의 진입점만 public 클래스로 두면 된다. 이렇게 하면 의존성이 잘못된 방향을 가리켜서 의존성 규칙을 위반할 위험이 줄어든다.
- 예를 들어, 영속성 어댑터는 자신이 구현하는 출력 포트로 접근할 수 있기 때문에 `package-private`으로 만들어도 된다!
- 하나의 패키지에 너무 많은 클래스를 포함하는 것은 혼란스럽다.
  - 이렇게 되면 코드를 쉽게 찾을 수 있도록 하위 패키지를 만드는 방법을 고려해야 한다.
  - 하지만 자바는 하위 패키지를 다른 패키지로 취급하기 때문에 `package-private` 멤버에 접근할 수 없게 된다. (`public`을 쓸 수 밖에 없다는 얘기)

## 컴파일 후 체크

- `public` 접근 제한자를 쓰면 컴파일러는 잘못된 의존성 방향을 체크할 수 없게 된다.
- 이럴 때는 사용하는 한 가지 방법이 `컴파일 후 체크 post-compile check`를 도입하는 것이다.
  - 코드가 컴파일된 후에 런타임에 체크한다는 뜻이다.
- 이러한 체크를 도와주는 자바용 도구로 `ArchUnit`이 있다.
  - 의존성 방향이 기대한 대로 잘 설정돼 있는지 체크할 수 있는 API를 제공한다. 의존성 규칙 위반을 발견하면 예외를 던진다.
  - 또한 JUnit과 같은 단위 테스트 프레임워크 기반에서 가장 잘 도착하며, 의존성 규칙을 위반하면 테스트를 실패시킨다.

```java
public class DependencyRuleTest {

  @Test
  void domainLayerDoesNotDependOnApplicationLayer() {
    noClasses()
        .that()
        .resideInAPackage("buckpal.domain")
        .should()
        .dependOnClassesThat()
        .resideInAPackage("buckpal.application")
        .check(new ClassFileImporter().importPackages("buckpal..."));
  }
}
```

- ArchUnit API를 이용해 헥사고날 아키텍처 내에서 관련된 모든 패키지를 명시할 수 있는 일종의 도메인 특화 언어(DSL)를 만들 수 있고, 패키지 사이의 의존성 방향이 올바른지 자동으로 체크할 수 있다.

```java
public class DependencyRuleTest {

  @Test
  void validateRegistrationContextArchitecture() {
    HexagonalArchitecture.boundedContext("org.mobilohas.bell.account")
            
        .withDomainLayer("domain")
            
        .withAdaptersLayer("adapter")
          .incoming("in.web")
          .outgoing("out.persistence")
          .and()
            
        .withApplicationLayer("application")
          .services("service")
          .incomingPorts("port.in")
          .outgoingPorts("port.out")
          .and()
            
        .withConfiguration("configuration")
        .check(new ClassFileImporter().importPackages("org.mobilohas.bell.."));
  }
}
```

- 잘못된 의존성을 바로잡는 데 컴파일 후 체크가 큰 도움이 되긴 하지만, 실패에 안전(fail-safe)하지는 않다.
  - 패키지 이름에 오타를 낸다면 어떤 클래스도 찾지 못하기 때문에 의존성 규칙 위반 사례를 발견하지 못할 것이다.
  - 리팩토링에도 취약하다. 컴파일 후 체크는 언제나 코드와 함께 유지보수해야 한다.

## 빌드 아티팩트

- 코드 상에서 아키텍처 경계를 구분하는 유일한 도구는 패키지엿따.
- 모든 코드가 같은 `모놀리식 아티팩트 monolithic build artifact`의 일부였던 셈이다.
- 빌드 아티팩트는 빌드 프로세스의 결과물이다. (Maven, Gradle 빌드 툴과 스크립트를 통해)
- 빌드 도구의 주요한 기능 중 하나는 `의존성 해결 dependency resolution`이다.
  - 코드베이스가 의존하고 있는 모든 아티팩트가 사용 가능한지 확인한다. 
  - 사용 불가능한 것이 있다면 아티팩트 레포지토리로부터 가져오려고 시도.
  - 이마저도 실패한다면 코드를 컴파일도 하기 전에 에러와 함께 빌드가 실패
- 이를 활용해서 모듈과 아키텍처의 계층 간의 의존성을 강제할 수 있다.
  - 가 모듈 혹은 계층에 대해 전용 코드베이스와 빌드 아티팩트로 분리된 빌드 모듈(JAR 파일)을 만들 수 있다.
  - 각 모듈의 빌드 스크립트에서는 아키텍처에서 허용하는 의존성만 지정한다. => 의존성 잘못 지정하면 컴파일 에러

#### 빌드 모듈로 아키텍처 경계를 구분하는 것이 패키지로 구분하는 것에 비해 가지는 장점

1. 빌드 도구가 `순환 의존성 circular dependency`을 극도로 싫어한다는 점.
  - 빌드 도구는 순환 의존성을 허용하지 않는다. (반면 자바 컴파일러는 두 개 혹은 그 이상의 패키지에서 순환 의존성이 있어도 상관하지 않는다.)
2. 빌드 모듈 방식에서는 다른 모듈을 고려하지 않고 특정 모듈의 코드를 격리한채로 변경할 수 있다.
3. 모듈 간 의존성이 빌드 스크립트에 분명하게 선언돼 있기 때문에 새로 의존성을 추가하는 일은 우연이 아닌 의식적인 행동이 된다.

- 하지만 빌드 스크립트를 유지보수하는 비용을 수반하기 때문에 아키텍처를 여러 개의 빌드 모듈로 나누기 전에 아키텍처가 어느 정도는 안정된 상태여야 한다.

<br/>

# 참고자료

- 만들면서 배우는 클린 아키텍처, 톰 홈버그 지음

