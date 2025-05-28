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

## item36. ordinal 인덳이 대신 EnumMap을 사용하라

```java
public class Plant {
  enum LifeCycle { ANNUAL, PERNNIAL, BIENNIAL }
}
```

- 아래처럼 `ordinal()`을 인덱스로 사용하면 무엇이 문제일까?

1. 배열은 제네릭과 호환되지 않으므로 비검사 형변환을 수행해야 하고 깔끔히 컴파일되지 않을 것이다.
2. 배열은 각 인덱스의 의미를 모르니 출력 결과에 직접 레이블을 달아야 한다.
3. 가장 심각한 문제! 정확한 정수값을 사용한다는 것을 개발자가 직접 보증해야 한다. (잘못된 값이 사용될 우려 있음.)

```java
for (Plant p: garden) {
  plantByLifeCycle[p.lifeCycle.ordinal()].add(p);
}
```

- 반면 `EnumMap`을 사용하면?
1. 더 짧고 명료하며 안전하고 성능도 원래 버전과 비등.
2. 안전하지 않은 형변환은 쓰지 않고, 맵의 키인 열거 타입이 그 자체로 출력용 문자열을 제공하여 출력 결과에 직접 레이블을 달 일도 없다.
3. 나아가 배열 인덱스를 계산하는 과정에서 오류가 날 가능성도 원천봉쇄된다.

```java
Arrays.stream(garden)
  .collect(
      Collectors.groupingBy(Plant::getLifeCycle, () -> new EnumMap(LifeCycle.class), toSet()));
```

- `EnumMap`은 내부적으로 **배열 기반**을 동작하기 때문에 `HashMap`보다 훨씬 빠르고 메모리 효율적이다.
  - Enum 타입을 key로 사용할 때 무조건 `HashMap`보다 `EnumMap`이 더 낫다.

---

## item38. 확장할 수 있는 열거 타입이 필요하면 인터페이스를 사용하라

- 대부분의 상황에서 열거 타입을 확장하는 건 좋은 생각이 아니다.
  - 확장한 타입의 원소는 기반 타입의 원소로 취급하지만 그 반대는 성립하지 않는다면 이상하지 않는가?
  - 기반 타입과 확장된 타입들의 원소 모두를 순회할 방법도 마땅치 않다.
  - 마지막으로, 확장성을 높이려면 고려할 요소가 늘어나 설계와 구현이 복잡해진다.
- 그런데 확장할 수 잇는 열거 타입이 어울리는 쓰임이 최소한 하나가 있다. 바로 연산 코드다.
  - 연산 코드의 각 원소는 특정 기계가 수행하는 연산을 뜻한다.
- 다행히 열거 타입으로 이 효과를 내는 멋진 방법이 있다.
  - 기본 아이디어는 열거 타입이 임의의 인터페이스를 구현할 수 있다는 사실을 이용하는 것이다.
  - 연산 코드용 인터페이스를 정의하고 열거 타입이 이 인터페이스를 구현하게 하면 된다. 이때 열거 타입이 그 인터페이스의 표준 구현체 역할을 한다.

```java
public interface Operation {
  double apply(double x, double y);
}

public enum BasicOperation implements Operation {
  PLUS("+")   {public double apply(double x, double y) {return x + y;}},
  MINUS("-")  {public double apply(double x, double y) {return x - y;}},
  TIMES("*")  {public double apply(double x, double y) {return x * y;}},
  DIVIDE("*") {public double apply(double x, double y) {return x / y;}},
  ;
  private final String symbol;
  
  BasicOperation(String symbol) {
    this.symbol = symbol;
  }
}
```

- 열거 타입인 `BasicOperation`은 확장할 수 없지만, 인터페이스인 `Operation`은 확장할 수 있고, 이 인터페이스를 연산의 타입으로 사용하면 된다.
  - `Operation`을 구현한 또다른 열거 타입을 정의해 `BasicOperation`를 대체할 수 있다.  

```java
public enum ExtendedOperation implements Operation {
  EXP("^") {public double apply(double x, double y) {return Math.pow(x, y);}},
  ;
  private final String symbol;
  
  ExtendedOperation(String symbol) {
    this.symbol = symbol;
  }
}
```

#### 문제점

- 인터페이스르 사용해 확장 가능한 열거 타입을 흉내 내는 방식에도 한 가지 사소한 문제가 있다.
- 바로 열거 타입끼리 구현을 상속할 수 없다는 점이다.
  - 아무 상태에도 의존하지 않는다면 인터페이스의 디폴트 구현을 사용해볼 수 있을 것이다.
- 하지만 `BasicOperation`, `ExtendedOperation`의 경우 로직이 각각에 들어가야만 한다.
  - 이 경우에는 중복량이 적어 문제가 되진 않지만, 공유하는 기능이 많다면 그 부분을 별도의 도우미 클래스나 정적 메소드로 분리하는 방식으로 중복을 제거할 수 있다.

---

## item39. 명명 패턴보다 애너테이션을 사용하라

- 명명 패턴을 사용했을 때의 문제
1. 오타로 인해 부작용 발생
   - JUnit3까지는 메소드명을 `test`로 시작하게끔 했다. 하지만 `tset`와 같이 오타를 내면 테스트를 무시하는 문제가 있었다.
2. 올바른 프로그램 요소에서만 사용되리라 보증할 방법이 없다.
   - `TestSafetyMechanisms`로 클래스 이름을 지어 JUnit3에 던져줌. => 하지만 JUnit3은 개발자의 의도대로 테스트를 실행하지 않을 것이다.
3. 프로그램 요소를 매개변수로 전달할 마땅한 방법이 없다.

- annotation은 이 모든 문제를 해결해주는 멋진 개념.
  - JUnit4에서 전면 도입
- `@Test`와 같이 아무 매개변수 없이 단순이 대상에 마킹하는 어노테이션을 `마커 marker` 어노테이션이라 한다.
- 예외를 처리하기 위한 `@ExceptionTest`라는 어노테이션을 만들었다고 가정. (대신 러너 코드를 만들어야 한다.)
  - 이 경우 예외 클래스 타입을 매개변수로 받는다. => 그리고 정확히 해당 타입의 예외가 터졌는지 러너 코드에서 확인.

```java
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface ExceptionTest {
  Class<? extends Throwable>[] value();
}
```

---

## item40. `@Override` 어노테이션을 일관되게 사용하라

- `@Override` 어노테이션은 메소드 선언에만 달 수 있으며, 상위 타입의 메소드를 재정의했음을 의미.
  - 이 어노테이션을 통해 악명 높은 버그들을 예방할 수 있다.
- 예를 들면 아래와 같이 `equals`를 `오버라이딩 overriding`이 아닌 `오버로딩 overloading`을 해버리는 실수가 나올 수 있는데 `@Override`가 달려 잇으면 컴파일 오류가 발생한다.

```java
public class Bigram {
  private final char first;
  private final char second;
  
  // ...
  
  public boolean equals(Bigram b) { // 매개변수 타입이 Object여야 오버라이딩으로 동작한다.
    return first == b.first && second == b.second;
  }
} 
```

---

## item41. 정의하려는 것이 타입이라면 마커 인터페이스를 사용하라

- 이 아이템은 reversed item22(타입을 정의할 게 아니라면 인터페이스를 사용하지 말라.)이다.
- 자신이 구현하는 클래스가 특정 속성을 가짐을 표시해주는 인터페이스를 `마커 인터페이스 marker interface`라고 한다.
  - `Serializable` 인터페이스가 대표적인 예시
- '마커 어노테이션이 등장하면서 마커 인터페이스가 구식이 되었다'고 평. 하지만 사실이 아니다. 마커 인터페이스는 두 가지 면에서 마커 어노테이션보다 낫다.
1. 마커 인터페이스는 이를 구현한 클래스의 인스턴스들을 구분하는 타입으로 쓸 수 있으나, 마커 어노페이션은 그렇지 않다.
   - 어노테이션을 사용했을 때 런타임에야 발견할 오류를 인터페이슬 사용하면 컴파일 타임에 잡을 수 있다.
2. 적용 대상을 더 정밀하게 지정할 수 있다.
   - 적용 대상(`@Target`)을 `ElementType.TYPE`으로 선언한 어노테이션은 모든 타입에 달 수 있다. 부착할 수 있는 타입을 제한하지는 못한다는 것이다.  

- 반면 마커 어노테이션이 나은 점. 거대한 어노테이션 시스템의 지원을 받을 수 있다.
  - 어노테이션을 적극 활용하는 프레임워크에서는 마커 어노테이션을 사용하는 것이 일관성을 지키는 데 유리하다.
- 언제 마커 인터페이스를 적용해야 할까?
  - "이 마킹이 된 객체를 매개변수로 받는 메소드를 작성할 일이 있을까?"라고 자문해보고 "예"라면 마커 인터페이스를 써야 한다.
  - 해당 메소드의 매개변수 타입으로 사용해 컴파일 타임에 오류를 잡을 수 있기 때문이다.


<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

