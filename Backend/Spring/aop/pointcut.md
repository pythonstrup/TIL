# Pointcut

- 관심 조인 포인트를 결정, 어드바이스가 실행되는 시기를 제어

## Pointcut 지시자 종류

- execution, within, args, this, target, @target, @within, @annotation, @args, bean 등이 있다.

### execution 

- 메소드 실행 조인 포인트를 매칭한다. 
- 스프링 AOP에서 일반적으로 사용하는 방식
- 아래는 예시이다.
  - 처음의 `*`은 반환 타입
  - `com.hello.domain`은 패키지 경로
  - 그리고 클래스와 메소드
  - 괄호 안은 파라미터의 개수와 타입 등을 지정할 수 있다.

```
"execution(* com.hello.domain.MyClass.myMethod(String, ..))"
```

### within

- 특정 타입 내의 조인 포인트를 매칭

### args

- 인자가 주어진 타입의 인스턴스인 조인 포인트

### this

- 스프링 빈 객체(AOP 프록시)를 대상으로 하는 조인 포인트

### target

- 스프링 빈 객체(AOP 프록시가 참조하는 실제 대상)를 대상으로 하는 조인 포인트

### @target

- 실행 객체의 클래스에 주어진 타입의 어노테이션으로 있는 조인 포인트

### @within

- 주어진 어노테이션이 있는 타입 내 조인 포인트

### @annotation

- 메소드가 주어진 어노테이션을 가지고 있는 조인 포인트

### @args

- 전달된 실제 인수의 런타임 타입이 주어니진 타입의 어노테이션을 갖는 조인 포인트

### bean

- 스프링 전용 포인트컷 지시자
- 빈의 이름으로 포인트컷을 지정

<br/>

## 표현식의 결합

- &&(AND), ||(OR), !(not) 연산자를 사용할 수 있다.