# 4장 클래스와 인터페이스

- 추상화의 기본 단위인 클래스와 인터페이스는 자바 언어의 심장과도 같다.

## item 15. 클래스와 멤버의 접근 권한을 최소화하라

- 어설프게 설계된 컴포넌트와 잘 설계된 컴포넌트의 가장 큰 차이는 바로 클래스 내부 데이터와 내부 구현 정보를 외부 컴포넌트로부터 얼마나 잘 숨겼느냐다.

#### 정보 은닉

- 정보 은닉의 장점
  - 시스템 개발 속도를 높인다. 여러 컴포넌트를 병렬로 개발할 수 있기 때문이다.
  - 시스템 관리 비용을 낮춘다. 각 컴포넌트를 더 빨리 파악하여 디버깅할 수 있고, 다른 컴포넌트로 교체하는 부담도 적기 때문이다.
  - 정보 은닉 자체가 성능을 높여주지는 않지만, 성능 최적화에 도움을 준다. 완성된 시스템을 프로파일링해 최적화할 컴포넌트를 정한 다음(item 67), 다른 컴포넌트에 영향을 주지 않고 해당 컴포넌트만 최적화할 수 있기 때문이다.
  - 소프트웨어 재사용성을 높인다. 외부에 거의 의존하지 않고 독자저그로 동작할 수 있는 컴포넌트라면 그 컴포넌트와 함께 개발되지 않은 낯선 환경에서도 유용하게 쓰일 가능성이 크기 때문이다.
  - 큰 시스템을 제작하는 난이도를 낮춰준다. 시스템 전체가 아직 완성되지 않은 상태에서도 개별 컴포넌트의 동작을 검증할 수 있기 때문이다.
- 자바는 정보 은닉을 위한 다양한 장치를 제공하나. 그 중 접근 제어 메커니즘은 클래스, 인터페이스, 멤버의 접근성(접근 허용 범위)을 명시한다.
  - 각 요소의 접근성은 그 요소가 선언된 위치와 접근 제한자로 정해진다.
  - **이 접근 제한자를 제대로 활용하는 것이 정보 은닉의 핵심이다.**

#### 접근성

- **기본 원칙은 간단하다. 모든 클래스와 멤버의 접근성을 가능한 한 좁혀야 한다.**
- 한 클래스에서만 사용하는 `package-private` 톱레벨 클래스나 인터페이스는 이를 사용하는 클래스 안에 `private static`으로 중첩시켜보자.
  - 톱레벨로 두면 같은 패키지의 모든 클래스가 접근할 수 있지만, private static으로 중첩시키면 바깥 클래스 하나에서만 접근할 수 있다.
- 이보다 더 중요한 일은 `public`일 필요가 없는 클래스의 접근 수준을 `package-private` 톱레벨 클래스로 좁히는 일이다.
  - `public` 클래스는 그 패키지의 API인 반면, `package-private` 톱레벨 클래스는 내부 구현에 속하기 때문이다.
- `public` 클래스에서는 멤버의 접근 수준을 `package-private`에서 `protected`로 바꾸는 순간 그 멤버에 접근할 수 있는 대상 범위가 엄청나게 넓어진다.
  - `public` 클래스의 `protected` 멤버는 공개 API이므로 영원히 지원돼야 한다.
  - 또한 내부 동작 방식을 API 문서에 적어 사용자에게 공개해야 할 수도 있다.(item 19)
  - 따라서 `protected`의 수는 적을수록 좋다.
- 테스트만을 위해 클래스, 인터페이스, 멤버를 공개 API로 만들어서는 안 된다. 그렇게 해야할 이유도 없다.
  - 적당한 수준까지는 괜찮다. 예를 들어, `public` 클래스의 멤버를 `package-private`까지 풀어주는 것은 허용할 수 있지만 그 이상은 안 된다.
  - 테스트 코드를 테스트 대상과 같은 패키지에 두면 `package-private` 요소에 접근할 수 있다.

#### 필드

- **`public` 클래스의 인스턴스 필드는 되도록 `public`이 아니어야 한다.**
- **`public` 가변 필드가 갖는 클래스는 일반적으로 thread-safe하지 않다.**
- 심지어 필드가 final이면서 불변 객체를 참조하더라도 문제는 여전히 남는다.
  - 내부 구현을 바꾸고 싶어도 그 `public` 필드를 없애는 방식으로는 리팩토링할 수 없게 된다.
- 해당 클래스가 표현하는 추상 개념을 완성하는 데 꼭 필요한 구성요소로써의 상수라면 `public static final` 필드로 공개해도 좋다.
  - 관례상 이런 상수의 이름은 대문자 알파벳과 밑줄(`_`)을 사용한다.
  - 반드시 원시 타입이나 불변 객체를 참조해야 한다.
- **클래스에서 `public static final` 배열 필드를 두거나 이 필드를 반환하는 접근자 메소드를 제공해서는 안 된다.**

```java
// 보안 허점이 숨어 있다.
public static final Thing[] VALUES = {...};
```

- 해결책은 두 가지다.
1. `public` 배열을 `private`으로 만들고 public 불변 리스트를 추가하는 것이다.

```java
private static final Thing[] PRIVATE_VALUES = {...};
public static final List<Thing> VALUES = 
    Collections.unmodifiableList(Arrays.asList(PRIVATE_VALUES));
```

2. 배열을 `private`으로 만들고 그 복사본을 반환하는 `public` 메소드를 추가하는 방법이다. (방어적 복사)

```java
private static final Thing[] PRIVATE_VALUES = {...};
public static final List<Thing> values() {
  return PRIVATE_VALUES.clone();
}
```

- 클라이언트가 무엇을 원하느냐를 판단해 둘 중 하나를 선택하면 된다. 어느 반환 타입이 더 쓰기 편할지, 성능은 어느 쪽이 나을지를 고민해 정하자.
- 자바 9에서는 모듈 시스템이라는 개념이 도입되면서 두 가지 암묵적 접근 수준이 추가되었다.
  - 패키지가 클래스들의 묶음이듯, 모듈은 패키지들의 묶음이다. 모듈은 자신에 속하는 패키지 중 공개(export)할 것들을 (관례산 module-info.java 파일) 선언한다.
  - `protected` 혹은 `public` 멤버라도 해당 패키지를 공개하지 않았다면 모듈 외부에서는 접근할 수 없다.
  - 물론 모듈 안에서는 exports로 선언했는지 여부에 아무런 영향도 받지 않는다.

<br/>

## item 16. public 클래스에서는 public 필드가 아닌 접근자 메소드를 사용하라

- `public` 필드를 사용하지 말고 `private` 필드를 사용하라.
  - 접근자 메소드(getter나 setter)를 제공함으로써 클래스 내부 표현 방식을 언제든 바꿀 수 있는 유연성을 얻을 수 있다.
- **하지만 `package-private` 클래스 혹은 `private` 중첩 클래스라면 데이터 필드를 노출한다 해도 하등의 문제가 없다.**
  - 그 클래스가 표현하려는 추상 개념만 올바르게 표현해주면 된다.
  - 클라이언트 코드 면에서 접근자 방식보다 훨씬 깔끔하다.
- `public` 클래스의 필드가 불변이라면 직접 노출할 때의 단점이 조금은 줄어들지만, 여전히 결코 좋은 생각이 아니다.
  - API를 변경하지 않고는 표현 방식을 바꿀 수 없고, 필드를 읽을 때 부수 작업을 수행할 수 없다는 단점은 여전하다.
  - 단, 불변식을 보장할 수 있게 된다. 예컨대 다음 클래스는 각 인스턴스가 유효한 시간을 표현함을 보장한다.

```java
// 불변 필드를 노출한 public 클래스 - 과연 좋은가?
public final class Time {
  private static final int HOURS_PER_DAY = 24;
  private static final int MINUTES_PER_HOUR = 60;
  
  public final int hour;
  public final int minute;
  
  public Time(int hour, int minute) {
    if (hour < 0 || hour >= HOURS_PER_DAY) 
      throw new IllegalArgumentException("시간: " + hour);
    if (minute < 0 || minute >= MINUTES_PER_HOUR)
      throw new IllegalArgumentException("분: " + minute);
    this.hour = hour;
    this.minute = minute;
  }
}
```

<br/>

## item 17. 변경 가능성을 최소화하라

- 불변 클래스란 간단히 말해 인스턴스의 내부 값을 수정할 수 없는 클래스다.
  - 객체가 파괴되는 순간까지 절대 달라지지 않는다.
  - `String`, 기본 타입의 박싱된 클래스들, `BigInteger`, `BigDecimal` 등은 불변 클래스다.

#### 불변 클래스로 만들기 위한 다섯 가지 규칙

1. **객체의 상태를 변경하는 메소드(변경자)를 제공하지 않는다.**
2. **클래스를 확장할 수 없도록 한다.** 하위 클래스에서 부주의하게 혹은 나쁜 의도로 객체의 상태를 변하게 만드는 사태를 막아준다.
3. **모든 필드를 `final`로 선언한다.** 시스템이 강제하는 수단을 사용해 설계자의 의도를 명확히 드러내는 방법이다.
4. **모든 필드를 `private`으로 선언한다.** 필드가 참조하는 가변 객체를 클라이언트에서 직접 접근해 수정하는 일을 막아준다. (`public final`은 비추천)
5. **자신 외에는 내부의 가변 컴포넌트에 접근할 수 없도록 한다.** 클래스에 가변 객체를 참조하는 필드가 하나라도 있다면 클라이언트에서 그 객체의 참조를 얻을 수 없도록 해야 한다. 이런 필드는 절대 클라이언트가 제공한 객체 참조를 가리키게 해서는 안 되며, 접근자 메소드가 그 필드를 그대로 반환해서도 안 된다. 생성자, 접근자, readObject 메소드(item 88) 모두에서 방어적 복사를 수행하라.

#### Complex 예시

- 아래 클래스는 복소수를 표현한다.
  - 함수형 프로그래밍 방식 - 피연산자에 함수를 적용해 그 결과를 반환하지만, 피연산자 자체는 그대로인 프로그래밍 패턴
- `BigInteger`와 `BigDecimal` 클래스를 사람들이 잘못 사용해 오류가 발생하는 일이 자주 있다.

```java
// 불변 복소수 클래스
public final class Complex {
  private final double re;
  private final double im;
  
  private Complex(double re, double im) {
    this.re = re;
    this.im = im;
  }
  
  public double realPart() { return re; }
  public double imaginaryPart() { return im; }
  
  public Complex plus(Complex c) {
    return new Complex(re + c.re, im + c.im);
  }

  public Complex minus(Complex c) {
    return new Complex(re - c.re, im - c.im);
  }

  public Complex times(Complex c) {
    return new Complex(re * c.re, im * c.im);
  }

  public Complex devidedBy(Complex c) {
    double tmp = c.re * c.re + c.im * c.im;
    return new Complex(
            (re * c.re + im * c.im) / tmp,
            (im * c.re - re * c.im) / tmp);
  }
  
  @Override
  public boolean equals(Object o) {
    if (o == this)
      return true;
    if (!(o instanceof Complex))
      return false;
    return Double.compare(c.re, re) == 0
            && Double.compare(c.im, im) == 0;
  }

  @Override
  public int hashCode() {
    return 31 * Double.hashCode(re) + Double.hashCode(im);
  }

  @Override
  public String toString() {
    return "(" + re + " + " + im + "i)";
  }
}
```

#### 불변 객체의 장점

- **불변 객체는 단순하다.** 불변 객체는 생성된 시점의 상태를 파괴할 때까지 그대로 간직한다.
  - 모든 생성자가 클래스 불변식(class invariant)를 보장한다면 그 클래스를 사용하는 프로그래머가 다른 노력을 들이지 않더라도 영원히 불변으로 남는다.
  - 반면 가변 객체는 임의의 복잡한 상태에 놓일 수 있다. 변경자 메소드가 일으키는 상태 전이를 정밀하게 문서로 남겨놓지 않은 가변 클래스는 믿고 사용하기 어려울 수도 있다.
- **불변 객체는 근본적으로 thread-safe하여 따로 동기화할 필요가 없다.**
  - 여러 스레드가 동시에 사용해도 절대 훼손되지 않는다. => 불변 객체는 thread-safe를 만드는 가장 쉬운 방법!
  - **안심하고 공유할 수 있다.** 따라서 불변 클래스라면 한 번 만든 인스턴스를 최대한 재활용하기를 권한다.

```java
public static final Complex ZERO = new Complex(0, 0);
public static final Complex ONE = new Complex(1, 0);
public static final Complex I = new Complex(0, 1);
```

- 불변 클래스는 자주 사용되는 인스턴스를 캐싱하여 같은 인스턴스를 중복 생성하지 않게 해주는 정적 팩토리를 제공할 수 있다.
  - 박싱된 기본 타입 클래스 전부와 `BigInteger`가 여기에 속한다.
  - 여러 클라이언트가 인스턴스를 공유하여 메모리 사용량과 가비지 컬렉션 비용이 줄어든다.
- 불변 객체를 자유롭게 공유할 수 있다는 점은 방어적 복사(item 50)도 필요 없다는 결론으로 자연스럽게 이어진다.
- **불변 객체는 자유롭게 공유할 수 있음은 물론, 불변 객체끼리 내부 데이터를 공유할 수 있다.**
- **객체를 만들 때 다른 불변 객체들을 구성요소로 사용하면 이점이 많다.**
- **불변 객체는 그 자체로 실패 원자성을 제공한다.**

> 실패 원자성 failure atomicity
> - 메소드에서 예외가 발생한 후에도 그 객체는 여전히 (메소드 호출 전과 똑같은) 유효한 상태여야 한다.
> - 불변 객체의 메소드는 내부 상태를 바꾸지 않으니 이 성질을 만족한다.

#### 불변 객체의 단점

- **불변 클래스에도 단점은 있다. 값이 다르면 반드시 독립된 객체로 만들어야 한다는 것이다.**
  - 값의 가짓수가 많다면 이들을 모두 만드는 데 큰 비용을 치러야 한다.
- 아래 `flipBit` 메소드는 새로운 `BigInteger` 인스턴스를 생성한다.
  - 원본과 단지 비트만 다른 백만 비트짜리 인스턴스를 말이다.
  - 이 연산은 `BigInteger`의 크리게 비례해 시간과 공간을 잡아먹는다.

```java
BigInteger moby = ...;
moby = moby.flipBit(0);
```

- `BitSet`도 임의 길이의 비트 순열을 표현 하지만, `BigInteger`와 달리 '가변'이다.
  - `BitSet` 클래스는 원하는 비트 하나만 상수 시간 안에 바꿔주는 메소드를 제공한다.

```java
BitSet moby = ...;
moby.flip(0);
```

- 원하는 객체를 완성하기까지의 단계가 많고, 그 중간 단계에서 만들어진 객체들이 모두 버려진다면 성능 문제가 더 불거진다.

#### 성능 문제 대처

- 이 문제를 대처하는 방법은 두 가지다.

1. 흔히 사용될 `다단계 연산 multistep operation`을 예측하여 기본 기능으로 제공하는 방식
- 다단계 연산을 기본으로 제공한다면 더 이상 각 단계마다 객체를 생성하지 않아도 된다.
- 불변 객체는 내부적으로 아주 영리한 방식으로 구현할 수 있기 때문이다. 
- 예를 들어 `BigInteger`는 모듈러 지수 같은 다단계 연산 속도를 높여주는 가변 동반 클래스를 `package-private`으로 가지고 있다. 그리고 내부적으로 잘 처리해준다.

2. 가변 동반 클래스를 직접 사용하는 방식
- 안타깝게도 `String`은 그렇지 않다. 그래서 `StringBuilder`를 사용해야 한다.

#### 불변 클래스를 만드는 설계 방법

- 클래스가 불변임을 보장하려면 자신을 상속하지 못하게 해야 한다.
  - 상속하지 못하게 하는 가장 쉬운 방법은 `final class`로 선언하는 것이다.
  - 더 유연한 방법은 모든 생성자를 `private` 혹은 `package-private`으로 만들고 `public` 정적 팩토리를 제공하는 방법이다.
  - 바깥에서 볼 수 없는 `package-private` 구현 클래스를 원하는 만큼 만들어 활용할 수 있으니 훨씬 유연하다.
  - 이렇게 되면 클라이언트가 바라본 불변 객체는 사실상 `final`이 된다.

#### BigInteger 이슈

- `BigInteger`나 `BigDecimal`의 인스턴스를 인수로 받는다면 주의해야 한다.
  - 불변 객체가 final이어야 한다는 생각이 널리 퍼지지 않았던 시절의 하위 호환성 때문에 두 클래스의 메소드들은 재정의할 수 있게 설계되었다.
  - 보안을 지킬 수 있다면 인수로 받은 객체가 '진짜' `BigInteger`(혹은 `BigDecimal`)인지 반드시 확인해야 한다.

```java
public static BigInteger safeInstance(BigInteger val) {
  return val.getClass() == BigInteger.class ?
        val: new BigInteger(val.toByteArray());
}
```

<br/>

## item 18. 상속보다는 컴포지션을 사용하라

- 같은 프로그래머가 통제하는 패키지 안에서라면 상속을 사용해도 문제될 것이 없다. 확장할 목적으로 설계되었고 문서화도 잘 된 클래스도 마찬가지로 안전하다.
- 하지만 일반적인 구체 클래스를 패키지 경계를 넘어, 즉 다른 패키지의 구체 클래스를 상속하는 일은 위험하다. (`구현 상속`)
  - 상속은 캡슐화를 깨뜨린다.

### 하위 클래스가 깨지기 쉬운 이유

1. 상위 클래스의 내부 구현 방식를 정확히 이해하여 메소드 동작을 구현하는 것이 어렵다.
2. 상위 클래스가 변경되면 하위 클래스가 기대한대로 동작하지 않을 수 있다. (ex-상위 클래스에 새로운 메소드 추가)

### 돌파구

- 기존 클래스를 확장하는 대신 새로운 클래스를 만들고, private 필드로 기존 클래스의 인스턴스를 참조하게 하는 방법이다. 
- 기존 클래스가 새로운 클래스의 구성요소로 쓰인다는 뜻에서 이러한 설계를 `구성 Composition`이라고 한다.
  - 새 클래스의 인스턴스 메소드들은 기존 클래스의 대응하는 메소드를 호출해 그 결과를 반환한다. 이 전달 방식을 `forwarding`이라 하며, 새 클래스의 메소드들을 `전달 메소드 forwarding method`라 부른다.
- 래퍼 클래스는 단점이 거의 없지만, `콜백 callback` 프레임워크와 어울리지 않는다는 점만 주의하면 된다.
  - 콜백 프레임워크: 자기 자신의 참조를 다른 객체에 넘겨서 다음 호출 때 사용하도록 한다.
  - 내부 객체는 자신을 감싸고 있는 래퍼의 존재를 모르니 대신 자신(`this`)의 참조를 넘기고, 콜백 때는 래퍼가 아닌 내부 객체를 호출하게 된다.
  - 이를 SELF 문제라고 한다. (참고: [Stackoverflow](https://stackoverflow.com/questions/28254116/wrapper-classes-are-not-suited-for-callback-frameworks))

### 상속은 '진짜' 하위 타입인 상황에서만 쓰여야 한다.

- `is-a` 관계
- 컴포지션을 써야 할 상황에서 상속을 사용하는 건 내부 구현을 불필요하게 노출하는 꼴이다.
  - API가 내부 구현에 묶이고 그 클래스의 성능도 영원히 제한된다. (상속은 상위 클래스의 API를 '그 결함까지도' 그대로 승계한다.)
  - 더 심각한 문제는 클라이언트가 노출된 내부에 직접 접근할 수 있다는 점이다.

<br/>

## item 18. 상속을 고려해 설계하고 문서화하라. 그러지 않았다면 상속을 금지하라

- 클래스는 프로그래머의 통제권 밖에 있는 한, 언제 어떻게 변경될지 모른다.
- 상속용 클래스는 재정의할 수 있는 메소드들을 내부적으로 어떻게 이용하는지(자기사용) 문서로 남겨야 한다.
- API 문서 메소드 설명에 `Implementation Requirements`로 시작하는 절을 볼 수 있다.
  - 이는 메소드의 내부 동작 방식을 설명하는 곳이다. 메소드 주석에 `@implSpec` 태그를 붙이면 자바독 도구가 생성해준다.

```java
/**
 * @implSpec 내부 동작 방식 설명
 */
public void method() {...}
```

### API 문서?

- "좋은 API란 문서란 '어떻게'가 아닌 '무엇'을 하는지를 설명해야 한다"라는 격언이 존재.
- 하지만 클래스를 안전하게 상속할 수 있도록 하려면 내부 구현 방식을 설명할 수밖에 없다. (상속이 캡슐화를 해치기 때문에 일어나는 안타까운 현실)

### Hook

- 내부 매커니즘을 문서로 남기는 것만이 상속을 위한 설계의 전부는 아니다. 효율적인 하위 클래스를 큰 어려움 없이 만들 수 있게 하려면 클래스의 내부 동작 과정 중간에 끼어들 수 있는 `훅 hook`을 잘 선별하여 `protected` 메소드 형태로 공개해야 할 수도 있다.
- 상속용 클래스를 설계할 때 어떤 메소드를 `protected`로 노출해야 할지는 어떻게 결정할까?
  - 심사숙고해서 잘 예측해본 다음, 실제 하위 클래스를 만들어 시험해보는 것이 최선이다.
  - hook은 하나하나가 내부 구현에 해당하므로 그 수는 가능한 한 적어야 한다. 한편으로 너무 적게 노출해서 상속으로 얻는 이점마저 없애지 않도록 주의해야 한다.
- 상속용 클래스를 시험하는 방법은 직접 하위 클래스를 만들어보는 것이 `유일`하다.
  - hook을 놓쳤다면 그 빈자리가 확연히 드러날 것이다.
  - 거꾸로, 하위 클래스를 여러 개 만들 때까지 전혀 쓰이지 않는 hook은 사실 `private`이었어야 할 가능성이 크다.
  - 보통 하위 클래스 3개 정도가 적당하다고 함. 상속용으로 설계한 클래스는 배포 저네 반드시 하위 클래스를 만들어 검증하도록!!

### 금지하기

- 구체 클래스가 상속용으로 설계되거나 문서화되지 않았음에도 상속을 위해 사용될 여지는 다분하다.
- **이 문제를 해결하는 가장 좋은 방법은 상속용으로 설계하지 않은 클래스는 상속을 금지하는 것이다.**
- 방법은 아래의 2가지가 있다. (1번이 더 쉬움)
1. 클래스를 final로 선언
2. 모든 생성자를 `private`이나 `package-private`로 선언하고 `public` 정적 팩터리를 만들어주는 방법


<br/>

## item 20. 추상 클래스보다는 인터페이스를 우선하라.

> #### mixin
> - OOP에서 다중 상속의 단점을 피하면서 코드 재사용성을 높이기 위한 기법이다. 단독으로 인스턴스를 생성할 수 없으며, 다른 클래스에 기능을 `섞어 넣는 mix in` 방식으로 동작한다.
> - 클래스가 구현할 수 있는 타입으로, 믹스인을 구현한 클래스에 원래의 '주된 타입' 외에도 특정 선택적 행위를 제공한다고 선언하는 효과를 준다.

### 인터페이스의 장점 (feat. 추상 클래스의 단점)

1. 기존 클래스에도 손쉽게 새로운 인터페이스를 구현해넣을 수있다.
2. 믹스인 정의에 안성맞춤이다. (추상 클래스로는 믹스인을 정의할 수 없다. 클래스는 두 부모를 섬길 수 없고, 클래스 계층구조에는 믹스인을 삽입하기에 합리적인 위치가 없다.)
2. 계층구조가 없는 타입 프레임워크를 만들 수 있다. (클래스로 조합을 전부 만들려면 `조합 폭발 combinatorial explosion`이 발생할 수 있다.)
3. 기능을 향상시키는 안전하고 강력한 수단이 된다. (타입을 추상 클래스로 정의해두면 그 타입에 기능을 추가하는 방법은 상속뿐이다. 상속을 해서 만든 클래스는 래퍼 클래스보다 활용도가 떨어지고 깨지기는 더 쉽다.)

### 골격 구현 클래스

- 인터페이스와 `추상 골격 구현 skeletal implemeatation` 클래스를 함께 제공하는 식으로 인터페이스와 추상 클래스의 장점을 모두 취하는 방법도 있다.
- 인터페이스로는 타입을 정의하고, 필요하면 디폴트 메소드 몇 개도 함께 제공한다. 그리고 골격 구현 클래스는 나머지 메소드들까지 구현한다.
- 단순히 골격 구현을 확장하는 것만으로 인터페이스를 구현하는 데 필요한 일이 대부분 완료된다. (`템플릿 메소드 패턴`)

<br/>

## item 21. 인터페이스는 구현하는 쪽을 생각해 설계하라

- 생각할 수 있는 모든 상황에서 불변식을 해치지 않는 디폴트 메소드를 작성하기란 어려운 법이다.
- 디폴트 메소드는 (컴파일에 성공하더라도) 기존 구현체에 런타임 오류를 일으킬 수 있다.
  - 따라서 꼭 필요한 경우가 아니라면 기존 인터페이스에 디폴트 메소드로 새 메소드를 추가하는 일은 피해야 한다.
- 여기서 핵심은, 디폴트 메소드라는 도구가 생겼더라도 **인터페이스를 설계할 때는 여전히 세심한 주의를 기울여야 한다**는 것이다.

<br/>

## item 22. 인터페이스는 타입을 정의하는 용도로만 사용하라

- 인터페이스는 자신을 구현한 클래스의 인스턴스를 참조할 수 있는 타입 역할을 한다.
  - 클래스가 어떤 인터페이스를 구현 = 인스턴스로 무엇을 할 수 있는지 클라이언트에게 전달
  - 인터페이스는 이 용도로만 사용해야 한다.
- 이 지침에 맞지 않는 예시. 상수 인터페이스. (안티 패턴)
  - 메소드 없이 상수를 뜻하는 static final 필드로만 가득 찬 인터페이스
- 결론. **인터페이스를 상수 공개용 수단**으로 사용하지 말자!

<br/>

## item 23. 태그 달린 클래스보다는 클래스 계층구조를 활용하라

> #### 태그
> - 해당 클래스가 어떤 타입인지에 대한 정보를 담고 있는 멤버 변수

```java
class Figure {
  enum Shape {RECTANGLE, CIRCLE};
  
  final Shape shape; // 태그 필드 - 현재 모양을 나타낸다.
  
  // 모양별 전용 필드...
  // 모양별 전용 생성자... 
}
```

- 여러 구현이 한 클래스에 혼합돼 있어 가독성이 나쁘다. 메모리도 많이 사용한다.
- **태그 달린 클래스는 클래스 계층구조를 어설프게 흉내낸 아류일 뿐이다.**

### 계층 구조로 바꾸기

- Root 추상 클래스를 정의하고, 모양에 따라 달라지는 메소드들을 Root의 추상 메소드로 선언한다.
- 루트 클래스를 확장한 구체 클래스를 의미별로 하니씩 정의하면 끝이다!
- 태그 필드를 사용하고 있는 클래스가 있다면 계층 구조로의 리팩토링을 고려하자!

<br/>

## item 24. 멤버 클래스는 되도록 static으로 만들라

### 중첩 클래스 nested class

- 종류: 정적 멤버, (비정적) 멤버, 익명, 지역
- 자신을 감싼 클래스에서만 쓰여야 하며, 그 외의 쓰임새가 있다면 톱레벨 클래스로 만들어야 한다. 
- **만약 중첩 클래스의 인스턴스가 바깥 인스턴스와 독립적으로 존재해야 한다면 정적 멤버 클래스로 만들어야 한다.**

### static을 써야하는 이유

- 또한, 멤버 클래스에서 바깥 인스턴스에 접근할 일이 없다면 무조건 static을 붙여서 정적 클래스로 만들자.
  - static을 생략하면 바깥 인스턴스로의 숨은 외부 참조를 갖게 된다. => 참조를 저장하려면 시간과 공간이 소비된다.
  - 더 심각한 문제는 가비지 컬렉션이 바깥 클래스의 인스턴스를 수거하지 못하는 메모리 누수가 생길 수 있다는 점이다.
  - 참조가 눈에 보이지 않으니 문제의 원인을 찾기 어려줘 때대로 심각한 상황을 초래하기도 한다.

<br/>

## item 25. 톱레벨 클래스는 한 파일에 하나만 담으라

- 컴파일러가 한 클래스에 대한 정의를 여러 개 만들어내는 일이 사라진다.
- 소스 파일을 어떤 순서로 컴파일하든 바이너리 파일이나 프로그램의 동작이 달라지는 일은 결코 일어나지 않을 것이다.

<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

