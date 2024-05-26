# Item 01. Static Factory Method

## 정적 팩토리 메소드를 사용해야 하는 이유?

1. 객체 생성 자체의 의미가 생기기 때문에 반환될 객체의 특성을 설명할 수 있다. (이름을 가질 수 있다.)
2. 호출될 때마다 인스턴스를 새로 생성하지 않을 수 있다. 불변 클래스는 미리 인스턴스를 만들어 놓거나 캐싱하는 방식으로 불필요한 객체 생성을 피할 수 있다.
3. 반환 타입의 하위 타입 객체를 반환할 수 있다. => 구현 클래스를 공개하지 않고도 그 객체를 반환할 수 있기 때문에 API를 작게 유지할 수 있다.
4. 입력 매개변수에 따라 매번 다른 클래스의 객체를 반환할 수 있다. (클라이언트는 팩토리가 건네주는 객체가 어느 클래스의 인스턴스인지 알 수도 없고 알 필요도 없다.)
5. 정적 팩토리 메소드를 작성하는 시점에는 반환할 객체의 클래스가 존재하지 않아도 된다. (클라이언트는 service provider framework의 serivce access API를 사용할 때 원하는 구현체의 조건을 명시할 수 있다. 조건을 명시하지 않으면 기본 구현체를 반환하거나 지원하는 구현체들을 하나씩 돌아가면서 반환한다. 이 API가 '유연한 정적 팩토리'의 실체다.)

## 하지만 단점도 있다.

1. 상속을 하려면 public이나 protected 생성자가 필요하니 정적 팩토리 메소드만 제공하면 하위 클래스를 만들 수 없다. (하지만 불변 타입[item17] 및 컴포지션 사용[item18]을 사용할 경우 이 제약을 지킨다는 것은 오히려 장점일 수 있다.)
2. 프로그래머가 찾기 어렵다. 생성자처럼 API 설명이 드러나지 않기 때문이다. (이 문제를 완화하기 위해 메소드 명명 방식을 사용한다.)

## 정적 팩토리 메소드에서 주로 사용하는 명명 방식들

### from

- 매개변수를 하나 받아서 해당 타입의 인스턴스를 반환하는 형변환 메소드

```java
Data date = Data.from(instance);
```

### of

- 여러 매개변수를 받아 적합한 타입의 인스턴스를 반환하는 집계 메소드

```java
Set<Rank> faceCards = EnumSet.of(JACK, QUEEN, KING);
```

### valueOf

- `from`과 `of`의 더 자세한 버전

```java
BigInteger prime = BigInteger.valueOf(Integer.MAX_VALUE);
```

### instance 또는 getInstance

- 매개변수로 명시한 인스턴스를 반환하지만, 같은 인스턴스임을 보장하지는 않는다.
- 매개변수를 받지 않을 수도 있다.

```java
MyObject = MyObject.getInstance(options);
```

### create 또는 newInstance

- `instance`, `getInstance`와 비슷하지만, 매번 새로운 인스턴스를 생성해 반환하는 것을 보장한다.

```java
Object newArray = Array.newInstance(classObject, arrayLen);
```

### getType

- `getInstance`와 같으나 생성할 클래스가 아닌 다른 클래스에 팩토리 메소드를 정의할 때 쓴다.
- `Type`은 팩토리 메소드가 반환할 객체의 타입이다.

```java
FileStore fs = Files.getFileStore(path);
```

### newType

- `newInstance`와 같으나 생성할 클래스가 아닌 다른 클래스에 팩토리 메소드를 정의할 때 쓴다.
- 위와 마찬가지로 `Type`은 팩토리 메소드가 반환할 객체의 타입이다.

```java
BufferedReader br = Files.newBufferedReader(path);
```

### type

- `getType`, `newType`의 간결한 버전

```java
List<MyObject> newList = Collections.list(list);
```

<br/>

## Item 02. 생성자에 매개변수가 많다면 빌더를 고려하라.

- 정적 팩토리와 생성자는 똑같은 제약이 하나 있는데, 선택적 매개변수가 많을 때 대응하기 어렵다는 점이다.
- 이 문제를 해결하기 위해 많은 프로그래머들이 점층적 생성자 패턴을 즐겨 사용했지만 확장이 어렵다는 단점이 존재한다.
  - 필드의 수가 늘어날수록 코드 작성이 어려워진다.
  - 매개변수의 순서라도 바뀌는 날에는.... 컴파일러도 문제를 잡아내지 못한다.

```java
public class Member {
  private final String name;
  private final int age;
  
  public Member() {
    return new Member("name", 0);
  }

  public Member(String name) {
    return new Member(name, 0);
  }
  
  public Member(int age) {
    return new Member("name", age);
  }
  
  public Member(String name, int age) {
    this.name = name;
    this.age = age;
  }
}
```

- 대안으로 자바빈즈 패턴을 사용한다?
  - 일관성이 깨지고 불변으로 만들 수 없다는 단점이 있다. => 객체의 일관성이 없다.
  - freezing 방식을 사용해 보완할 수 있지만. freeze를 호출한 것을 컴파일러가 보증할 방법이 없어 런타임 오류에 취약하다.

### 빌더 패턴

- 이 문제를 해결할 또 다른 대안이 있다. 빌더 패턴이다.
- Fluent API 혹은 Method Chaining을 사용한 방법이다.
- 빌더 패턴은 계층적으로 설계된 클래스와 함께 사용하기 좋다.
  - 추상 클래스는 추상 빌더를, 구체 클래스는 구체 빌더를 갖게 한다.
- 빌더 패턴은 상당히 유연하다. 빌더 하나로 여러 객체를 순회하면서 만들 수 있고, 빌더에 넘기는 매개변수에 따라 다른 객체를 만들 수도 있다.
- 적어도 4개 이상의 매개변수는 있어야 값어치를 한다. 하지만 API는 시간이 지날수록 매개변수가 많아지는 경향이 있으므로 애초에 빌더로 시작하는 편이 나을 수 있다.

### 추상 클래스 예시

- Enum

```java
public enum Topping {
  HAM, MUSHROOM, ONION, PEPER, SAUSAGE
}

public enum Size {
  SMALL, MEDIUM, LARGE
}
```

- Abstract Class

```java
public abstract class Pizza {
  final Set<Topping> toppings;

  abstract static class builder<T extends builder<T>> {
    EnumSet<Topping> toppings = EnumSet.noneOf(Topping.class);
    
    public T addTopping(Topping topping) {
      toppings.add(topping);
      return self();
    }

    abstract Pizza build();

    protected abstract T self();
  }

  Pizza(builder<?> builder) {
    toppings = builder.toppings.clone();
  }
}
```

- Class

```java
public class PepperoniPizza extends Pizza {
  private final Size size;

  public static class builder extends Pizza.builder<builder> {

    private final Size size;

    public builder(final Size size) {
      this.size = Objects.requireNonNull(size);
    }

    @Override
    public PepperoniPizza build() {
      return new PepperoniPizza(this);
    }

    @Override
    protected builder self() {
      return this;
    }
  }

  private PepperoniPizza(final builder builder) {
    super(builder);
    size = builder.size;
  }
}
```

```java
public class VegetablePizza extends Pizza {

  private final String sauce;

  public static class builder extends Pizza.builder<VegetablePizza.builder> {

    private String sauce;

    public builder sauce(String sauce) {
      this.sauce = sauce;
      return this;
    }

    @Override
    public VegetablePizza build() {
      return new VegetablePizza(this);
    }

    @Override
    protected VegetablePizza.builder self() {
      return this;
    }
  }

  private VegetablePizza(final VegetablePizza.builder builder) {
    super(builder);
    sauce = builder.sauce;
  }
}
```

- use case

```java
public class PizzaService {

  public void makePizza() {
    PepperoniPizza pepperoniPizza = new PepperoniPizza.builder(Size.SMALL)
        .addTopping(Topping.SAUSAGE)
        .addTopping(Topping.HAM)
        .build();
    VegetablePizza vegetablePizza = new VegetablePizza.builder()
        .addTopping(Topping.ONION)
        .addTopping(Topping.MUSHROOM)
        .addTopping(Topping.PEPER)
        .sauce("tomato")
        .build();
  }
}
```

### 빌더 패턴을 더 안전하게 사용하려면?

- 아래의 추가자료를 읽어보자.
- [Builder 기반으로 객체를 안전하게 생성하는 방법](https://cheese10yun.github.io/spring-builder-pattern/#google_vignette)

<br/>

## item03. private 생성자나 열거 타입으로 싱글턴임을 보증하라

- 클래스를 싱글터으로 만들면 이를 사용하는 클라이언트를 테스트하기 어려워질 수 있다.
  - 타입을 인터페이스로 정의한 다음 그 인터페이스를 구현해서 만든 싱글턴이 아니라면 이 싱글턴을 Test Double로 대체하기 어렵기 때문이다.
- 싱글턴을 만드는 방식은 3가지가 있다. (보통 위의 2가지 방식을 사용한다.)
### 1. 생성자를 private으로 감춘다. public static final 필드로 인스턴스에 접근할 수 있게 한다.

```java
public class Member {
  
  public static final Member INSTANCE = new Member();
  
  private Member() {}
}
```

### 2. 생성자를 private으로 감춘다. public static 메소드로 private static final 필드인 인스턴스에 접근할 수 있게 한다.

```java
public class Member {
  
  private static final Member INSTANCE = new Member();
  
  private Member() {}
  
  public static Member getInstance() {
    return INSTANCE;
  }
}
```

### 3. 열거 타입 선언

- 따지고 보면 첫번째 방식(public 필드)과 비슷하지만 더 간결하고 간편하게 직렬화할 수 있는 방식이다.
- 대부분 상황에서는 원소가 하나뿐인 열거 타입이 싱글턴을 만드는 가장 좋은 방법이다.
  - 하지만 enum 외에 다른 클래스를 상속해야한다면 사용할 수 없는 방식이다.

```java
public enum Member {
  INSTANCE;
  
  public void act() {...}
}
```

<br/>

# item 04. 인스턴스화를 막으려거든 private 생성자를 사용하라.

- 정적 멤버만 담은 유틸리티 클래스는 인스턴스로 만들어 사용하려고 설계한 것이 아니다.
- 하지만 생성자를 명시하지 않으면 컴파일러가 자동으로 기본 생성자를 만들어버린다.
- 즉 매개변수를 받지 않는 public 생성자가 만들어지며 사용자는 이 생성자가 자동 생성된 것인지 구분할 수 없다.
- 단순히 추상클래스로 만든다고 해서 인스턴스화를 막을 수 있는 것은 아니다. 하위 클래스를 만들어 인스턴스화가 가능하기 때문이다.
- private 생성자를 추가해 인스턴스화를 막자. 극단적으로 생각했을 때, private 생성자에서 Error를 던져 생성자를 호출했을 경우, 컴파일이 아예 되지 않도록 하는 것도 방법이 될 수 있다.