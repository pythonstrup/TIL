# Chapter09. 애플리케이션 조립하기

## 왜 조립까지 신경 써야 할까?

- 유스케이스가 영속성 어댑터를 호출해야 하고 스스로 인스턴스화한다면 코드 의존성이 잘못된 방향으로 만들어진 것이다.
  - 이것이 바로 아웃고잉 포트 인터페이스를 생성한 이유다.
  - 유스케이스는 인터페이스만 알아야 하고, 런타임에 이 인터페이스의 구현을 제공받아야 한다.
- 그럼 우리의 객체 인스턴스를 생성할 책임은 누구에게 있을까?
  - 어떻게 의존성 규칙을 어기지 않으면서 그렇게 할 수 있을까?
- 해답은 아키텍처에 대해 중립적이고 인스턴스 생성을 위해 모든 클래스에 대한 의존성을 가지는 `설정 컴포넌트 configuration component`가 있어야 한다는 것이다.
- 설정 컴포넌트는 애플리케이션 조립을 책임진다. 역할을 아래와 같다.
  - 웹 어댑터 인스턴스 생성
  - HTTP 요청이 실제로 웹 어댑터로 전달되도록 보장
  - 유스케이스 인스턴스 생성
  - 웹 어댐터에 유스케이스 인스턴스 제공
  - 영속성 어댑터 인스턴스 생성
  - 유스케이스에 영속성 어댑터 인스턴스 제공
  - 영속성 어댑터가 실제로 데이터베이스에 접근할 수 있도록 보장
- 책임이 굉장히 많다.
  - 단일 책임 원칙? 위반하는 것 맞다.
  - 그러나 애플리케이션의 나머지 부분을 깔끔하게 유지하고 싶다면 이처럼 구성요소들을 연결하는 바깥쪽 컴포넌트가 필요하다.

## 평번한 코드로 조립하기

- 가장 기본적인 방법이다.

```java
class Application {

  public static void main(String[] args) {
    AccountRepository accountRepository = new AccountRepository();
    ActivityRepository activityRepository = new ActivityRepository();
    
    AccountPersistenceAdapter accountPersistenceAdapter = new AccountPersistenceAdapter(accountRepository, activityRepository);
    
    SendMoneyUseCase sendMoneyUseCase = new SendMoneyUseService(
        accountPersistenceAdapter,  // LoadAccountPort
        accountPersistenceAdapter); // UpdateAccountPort
    
    SendMoneyController sendMoneyController = new SendMoneyController(sendMoneyUseCase);
    
    startProcessingWebRequests(sendMoneyController);
  }
}
```

- 하지만 몇 가지 단점이 있다.

1. 완전한 앤터프라이즈 애플리케이션에서는 적합하지 않다. (너무 많은 객체들이 산재)
2. 각 클래스가 속한 패키지 외부에서 인스턴스를 생성하기 때문에 클래스들이 전부 public이어야 한다.
    - `package-private` 접근 제한자를 이용해 원치 않는 의존성을 피할 수 있었다면 더 좋았을 것이다.

## 스프링 클래스패스 스캐닝으로 조립하기

- 스프링 프레임워크를 이용해서 애플리케이션을 `조립한 결과물`을 `애플리케이션 컨테그스트 application context`라고 한다.
- 애플리케이션 컨텍스트는 애플리케이션을 구성하는 모든 객체를 포함한다. (여기서 `구성 객체`는 자바 용어로 `Bean`)
- 스프링은 클래스패스 스캐닝으로 클래스패스에서 접근 가능한 모든 클래스 중 `@Component` 어노테이션이 붙은 것을 찾는다.
  - 그리고 어노테이션이 붙은 각 클래스의 객체를 생성한다.

```java
@Component
class AccountPersistenceAdpter implements LoadAccountPort, UpdateAccountStatePort {
  // ...
}
```

- 스프링이 인식할 수 있는 어노테이션을 직접 만들 수도 있다.

```java
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Component
public @interface PersistenceAdapter {

  @AliasFor(annotation = Component.class)
  String value() default "";
}
```

- 메타 어노테이션으로 `@Component`를 포함하고 있어서 스프링이 클래스패스 스캐닝을 할 때 인스턴스를 생성해준다.
- 하지만 스프링의 클래스패스 스캐닝에도 몇 가지 단점이 있다.

1. 클래스에 프레임워크에 특화된 어노테이션을 붙여야 한다. => 침투적이다.
    - 강경한 클린 아키텍처파는 반대할 것
2. '마법 같은' 일이 일어날 수 있다.
    - 스프링 전문가가 아니라면 문제의 원인을 찾는 데 수일이 걸릴 수 있는 숨겨진 부수효과를 야기할 수도 있다.

- 조금 더 제어하기 쉬운 대안은 `Java Config`다.

## 스프링의 자바 컨피그로 조립

- 클래스패스 스캐닝 = 곤봉
- `자바 컨피그 Java Config` = 수술용 메스
- 이 방식은 애플리케이션 컨텍스트에 추가할 빈을 생성하는 설정 클래스를 만든다.

```java
@Configuration
@EnableJpaRepositories
class PersistenceAdapterConfiguration {
  
  @Bean
  AccoutPersistenceAdapter accountPersistenceAdapter(
        AccountRepository accountRepository,
        ActivityRepository activityRepository,
        AccountMapper accountMaper) {
    return new AccoutPersistenceAdapter(
        accountRepository,
        activityRepository,
        accountMaper);
  }
  
  @Bean
  AccountMapper accountMapper() {
    return new AccountMapper();
  }
}
```

- 클래스패스 스캐닝과 달리 `@Component` 어노테이션을 붙여야 할 필요가 없기 때문에 스프링 프레임워크에 대한 의존성 없이 깔끔하게 유지할 수 있다.
- 하지만 설정 클래스가 생성하는 빈이 설정 클래스와 같은 패키지에 존재하지 않으면 `public`으로 만들어야 한다.

<br/>

# 참고자료

- 만들면서 배우는 클린 아키텍처, 톰 홈버그 지음
