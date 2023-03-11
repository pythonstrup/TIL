# Dependency Injection

## 의존성 주입이란?

- 의존성 주입(Dependency Injection)은 객체의 생성과 사용 관심사를 분리하는 프로그래밍 설계 방식이다. 추상화 도구(인터페이스, 추상 클래스)를 이용하면 손쉽게 의존성 주입하는 코드를 만들 수 있다.
- 여기서 주의! 의존성 주입과 의존성 역전은 완전히 다른 개념이다!

> 의존성 역전
> Dependency Inversion Principle <br/>
> SOLID 원칙 중 하나로, 상위 모듈은 하위 모듈에 의존해서는 안되고 둘 다 추상화에 의존해야하며 추상화는 세부사항에 의존해서는 안 되고 세부사항(구체적인 구현)은 추상에 의존해야한다는 원칙

### 의존성이란?

> 의존대상 B가 변하면 그것이 A에 영향을 미친다. <br/>
> 이일민, 토비의 스프링

- 의존성은 "다른 것에 의지하여 생활하거나 존재하는 성질"을 의미한다. 코드에서의 의존성이란 `다른 객체와 연결되어 있는 성질`정도로 해석할 수 있을 것 같다.
- 클래스 A가 클래스 B를 사용할 때 A는 B에 의존한다고 표현한다. 객체 A의 코드에서 객체 B를 생성하거나 메소드를 호출할 때, 또는 파라미터로 객체를 전달받아 사용하면 의존성이 발생한다고 볼 수 있다.

### 의존성의 위험성

- A가 B에 의존 중이라면 B의 변경은 A에 영향을 미친다는 말이다. 즉, B의 변경으로 인해 A를 변경해야할 수도 있다는 말이다.
- 의존성은 Unit Test를 어려운 코드를 만든다. Unit Test는 특정 모듈이 의도된대로 작동하는지 테스트하는 과정인데, 이 특정 모듈이 다른 모듈을 필요로 한다면 독립적으로 테스트하기가 굉장히 어려워진다.
- 또한, 과도한 의존성은 모듈을 재사용하기 어렵게 만든다.

### 그래서 의존성 주입의 필요하다.

- 의존성 주입은 의존성의 위험성을 해소하기 위해 사용되는 패턴이다. 필요한 객체를 직접 생성하거나 찾지 않고, 외부에서 넣어준다. 즉, 객체의 의존관계를 내부에서 결정하지 않고 외부에서, 런타임 시점에 결정하는 것이다. 이를 통해 아래와 같은 효과를 얻을 수 있다.

  - Unit Test가 용이해진다.
  - 코드의 재활용성을 높여준다.
  - 객체 간의 의존성을 줄이거나 없애준다.
  - 객체 간의 결합도를 낮추고 유연한 코드를 작성할 수 있다.
  - 가독성이 좋아진다.

- 아래 코드는 의존 관계를 가지고 있는 코드지만 의존성 주입 방식을 사용하지 않은 코드다. 코드를 수정할 때 굉장히 비효율적일 것이다.

```java
class Program {
    private Programmer programmer;

    public Program() {
        this.programmer = new BackendDeveloper();

        // 위와 아래 둘 중에 하나를 직접 선택해 적어줘야함..
        this.programmer = new FrontendDeveloper();
    }

    public startProgram() {
        this.programmer.start();
    }
}
```

- 위 코드에 의존성 주입 패턴을 적용하면 아래와 같이 바꿀 수 있다.
- 아래와 같이만 바꿔줘도 나중에 코드를 수정할 일이 굉장히 많이 줄어들 것이다.

```java
class Program {
    private Programmer programmer;

    public Program(Programmer programmer) {
        this.programmer = programmer;
    }

    public startProgram() {
        this.programmer.start();
    }
}
```

<br/>

## 스프링 - 의존 관계 주입의 유형

- 컴포넌트 스캔으로 Bean을 등록하고 나면 의존 관계를 만들어야한다. 설정 파일에서 수동으로 의존 관계를 주입할 때는 직접 주입할 코드를 적어줘야했지만, 자동으로 Bean을 등록할 때는 `@Autowired`라는 어노테이션을 사용해야한다. 이 방법에도 4가지가 존재한다.
- 생성자 주입, 수정자 주입, 필드 주입, 일반 메소드 주입을 차례로 알아가보자.
- @Autowired는 스프링 Bean이 아니라면 작동하지 않는다는 점을 유의하자.

### 1. 생성자 주입

- 생성자를 통해 의존 관계를 주입받는 방법이다.
- 스프링이 버전업되면서 생성자가 1개만 존재한다면 @Autowired를 생략해도 의존관계가 자동으로 주입된다.
- 생성자 주입은 한 번만 호출하는 것이 보장되기 때문에 불변 혹은 필수 의존 관계에 사용된다. (스프링을 사용할 때는 다른 주입 방식보다 생성자 주입을 권장)
- Lombok을 이용하면 `@RequiredArgsConstructor`를 통해 필드에 Bean을 선언만 해도 생성자 주입할 수 있게 만들어놨다.

```java
@Component
public class MyController {

    private final MyService myService;

    @Autowired // 생략 가능
    public MyController(MyService myService) {
        this.myService = myService;
    }

    public void method() {
        ...
    }

}
```

### 2. 수정자 주입

- setter를 통해 의존 관계를 주입하는 방법니다.
- 변경 가능성이 있는 의존 관계에서 사용된다.
- 상속과 캐스팅을 사용하면 선택 관계를 만들 수 있다.

```java
@Component
public class MyController {

    private final MyService myService;

    public void method() {
        ...
    }

    @Autowired
    public void setMyService(MyService myService) {
        this.myService = myService;
    }
}
```

### 3. 필드 주입

- 필드에 바로 주입하는 방식으로 setter 없이 주입할 수 있지만 문제가 있다.
  1. 외부에서 변경이 불가능하기 때문에 Test가 어렵다.
  2. 코드의 유연성이 떨어진다. (예를 들어 스프링 프레임워크를 걷어냈을 때, 동작하게 만들려면 `@Autowired` 부분을 다 변경해야함)
- 생성자 주입이나 수정자 주입의 경우 @Autowired가 없다고 해서 코드에 심각한 영향을 주지는 않는다고 한다.
- 필드 주입은 테스트 코드에서 많이 사용하는 방식으로 Production에서는 되도록 사용하지 않는 것을 권장한다.

```java
@Component
public class MyController {

    @Autowired
    private final MyService myService;

    public void method() {
        ...
    }
}
```

### 4. 일반 메소드 주입

- setter가 아닌 메소드를 통해 의존 관계를 주입하는 방식이다. setter와 다른 점은 한 번에 여러 필드를 주입할 수 있다는 것이다.
- 일반적으로 사용하지 않는 주입 방식이다.

```java
@Component
public class MyController {

    @Autowired
    private final MyService myService;

    public void init(MyService myService) {
        this.myService = myService;
    }
}
```

## 참고자료

- [의존성 주입이란](https://velog.io/@wlsdud2194/what-is-di)

- [의존성 주입](https://7942yongdae.tistory.com/177)

- [의존성과 의존성 주입](https://hudi.blog/dependency-injection/)

- [IoC & DI](https://steady-coding.tistory.com/458)
