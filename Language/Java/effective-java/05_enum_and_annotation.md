# 6장 열거 타입과 애너테이션

## item34. int 상수 대신 열거 타입을 사용하라

- 변수를 나열한 패턴(`정수 열거 패턴 int enum pattern`, `문자열 나열 패턴 string enum pattern` 등)은 깨지기 쉽다.
  - 평범한 상수를 나열한 것뿐이라 컴파일하면 그 값이 클라이언트 파일에 그대로 새겨진다.
  - 따라서 상수의 값이 바뀌면 클라이언트도 반드시 다시 컴파일해야 한다.

```java
public static final int APPLE_FUJI = 0;
public static final int APPLE_PIPPIN = 1;
public static final int APPLE_GRANNY_SMITH = 2;
```

- 자바는 이런 열거 패턴의 단점을 말끔히 씻어주는 동시에 여러 장점을 안겨주는 대안을 제시했다. 바로 `열거 타입 enum type`이다.

```java
public enum Apple {
  FUJI, PIPPIN, GRANNY_SMITH
}
```

- 다른 언어의 열거 타입과 달리의 자바의 열거 타입은 완전한 형태의 클래스라서 다른 언어의 열거 타입보다 훨씬 강력하다.
- 열거 타입을 뒷받침하는 아이디어
  - 열거 타입 자체는 클래스다.
  - 상수 하나당 자신의 인스턴스 하나를 만들어, `public static final` 필드로 공개한다.
  - 열거 타입은 밖에서 접근할 수 있는 생성자를 공개하지 않는다. (사실상 `final`)
- 싱글턴은 원소가 하나뿐인 열거 타입이라 할 수 있고, 거꾸로 열거 타입은 싱글턴을 일반화한 형태라고 볼 수 있다.
- 열거 타입 상수 각각을 특정 데이터와 연결지으려면 생성자에서 데이터를 받아 인스턴스 필드에 저장하면 된다.
  - 열거 타입은 근본적으로 불변이라 모든 필드는 `final`이어야 한다.
  - 필드를 `public`으로 선언해도 되지만, `private`으로 두고 별도의 `public` 접근자 메소드를 두는 게 낫다.
- 추상 메소드를 선언해 각 상수별 메소드 구현을 만들 수도 있다.

```java
public enum Operation {
  
  PLUS("+")   {public double apply(double x, double y) {return x + y;}},
  MINUS("-")  {public double apply(double x, double y) {return x - y;}},
  TIMES("*")  {public double apply(double x, double y) {return x * y;}},
  DIVIDE("*") {public double apply(double x, double y) {return x / y;}},
  ;
  
  private final String symbol;
  
  Operation(String symbol) {
    this.symbol = symbol;
  }
  
  public abstract double apply(double x, double y);
  
  @Override
  public String toString() {
    return symbol;
  }
}
```

- 문자열을 받아 해당 열거 타입 상수로 변환해주는 `fromString` 메소드도 함께 제공하는 걸 고려해보라.

```java
private static final Map<String, Operation> stringToEnum = 
    Stream.of(values()).collect(Collectors.toMap(Operation::toString, e -> e));

public static Optional<Operation> fromString(String symbol) {
  return Optional.ofNullable(stringToEnum.get(symbol));
}
```

- 열거 타입의 성능은 정수 상수와 별반 다르지 않다. 열거 타입을 메모리에 올리는 공간과 초기화하는 시간이 들긴 하지만 체감될 정도는 아니다.
- 열거 타입 이럴 때 쓰자! **필요한 원소를 컴파일타임에 다 알 수 있는 상수 집합이라면 항상 열거 타입을 사용하자.**
  - 열거 타입에 정의된 상수 개수가 영원히 고정 불변일 필요는 없다.
  - 열거 타입은 나중에 상수가 추가돼도 바이너리 수준에서 호환되도록 설계되었다.

----

## item35. ordinal 메소드 대신 인스턴스 필드를 사용하라

- 열거 타입 상수는 자연스럽게 하나의 정수값에 대응된다. 그리고 모든 열거 타입은 해당 상수가 그 열거 타입에서 몇 번째 위치인지를 반환하는 `ordinal`이라는 메소드를 제공한다.
- 하지만 `ordinal`을 사용하면 프로그램이 너무 깨지기 쉬워진다.
  - **열거 타입 상수에 연결된 값은 `ordinal` 메소드로 얻지 말고, 인스턴스 필드에 저장하자.**

> #### Enum API 문서
> - `Enum`의 API 문서를 보면 `ordinal`에 대해 이렇게 쓰여 있다고 한다.
> - "대부분 프로그래머는 이 메소드를 쓸 일이 없다. 이 메소드는 `EnumSet`과 `EnumMap` 같이 열거 타입 기반의 범용 자료구조에 쓸 목적으로 설계되었다."
> - 따라서 이런 용도가 아니라면 `ordinal` 메소드는 절대 사용하지 말자.

---

## item36. 비트 필드 대신 `EnumSet`을 사용하라

- 비트 필드를 사용하면 비트별 연산을 사용해 합집합과 교집합 같은 집합 연산을 효율적으로 수행할 수 있다.
  - 비트 필드 값이 그대로 출력되면 단순한 정수 열거 상수를 출력할 때보다 해석하기가 훨씬 어렵다.
  - 비트 필드 하나에 녹아 있는 모든 원소를 순회하기도 까다롭다.
  - 마지막으로, 최대 몇 비트가 필요한지를 API 작성 시 미리 예측하여 적절한 타입(보통 `int`나 `long`)을 선택해야 한다. API를 수정하지 않고는 비트 수(32비트 or 64비트)를 더 늘릴 수 없기 때문이다.
- `java.util.EnumSet` 클래스는 열거 타입 상수의 값으로 구성된 집합을 효과적으로 표현해준다.
  - `Set` 인터페이스를 완벽히 구현하며, 타입 안전하고, 다른 어떤 `Set` 구현체와도 함께 사용할 수 있다.

---

## item36. 


<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

