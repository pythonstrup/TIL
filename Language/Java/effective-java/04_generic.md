# 5장 제네릭

## item26. 로 타입은 사용하지 말라

- 클래스와 인터페이스 선언에 `타입 매개변수 type parameter`가 쓰이면, 이를 `제네릭 클래스` 혹은 `제네릭 인터페이스`라고 한다.
- `제네릭 클래스`와 `제네릭 인터페이스`를 통틀어 `제네릭 타입 generic type`이라고 한다.
- 각각의 제네릭 타입은 일련의 `매개변수화 타입 parameterized type`을 정의한다.
- 마지막으로, 제네릭 타입을 하나 정의하면 그에 딸린 `로 타입 raw type`도 함께 정의된다.
  - `List<E>`의 로 타입은 `List`다. 제네릭 정보가 모두 지워진 것처럼 동작.
- 오류는 가능한 한 발생 즉시, 이상적으로 컴파일할 때 발견하는 것이 좋다.
  - 로 타입을 쓰면 제네릭이 안겨주는 안전성과 표현력을 모두 잃게 된다. 타입 안전성이 사라진다. 런타임 에러 발생 가능성도 높다.
  - 하지만 호환성 때문에 로 타입은 존재..
  - `List<Object>`로라도 정의. 이렇게 정의하면 메소드에 `List<Object>`가 아닌 경우에는 넘길 수 없다.
- 와일드카드(`Collection<?>`)와 로 타입(`Collection`)의 차이
  - 와일드카드는 타입 안전하다. `Collection<?>`에는 어떤 원소도 넣을 수 없다. 다른 원소를 넣으려 하면 컴파일할 때 오류가 뜬다.

### 예외

1. class 리터럴에는 로 타입을 써야 한다.
  - ex) `List.class`, `String[].class`
2. `instanceof` 연산자는 비한정적 와일드카드 타입 이외의 매개변수화 타입에는 적용할 수 없다.
  - ex) `if (o instanceof Set) {...}`

----

## item27. 비검사 경고를 제거하라

- 간단한 예제

```java
Set<Long> ids = new HashSet();
```

- 아래와 같이 다이아몬드 연산자만 넣어주면 경고가 해결된다.

```java
Set<Long> ids = new HashSet<>();
```

- 경고를 제거할 수는 없지만 타입 안전하다고 확신할 수 있다면 `@SupperssWarnings("unchecked")` 어노테이션을 달아 경고를 숨기자.

```java
@SuppressWarnings("unchecked")
public <T> T[] toArray(T[] a) {
    if (a.length < size)
        // Make a new array of a's runtime type, but my contents:
        return (T[]) Arrays.copyOf(elementData, size, a.getClass());
    System.arraycopy(elementData, 0, a, 0, size);
    if (a.length > size)
        a[size] = null;
    return a;
}
```

- `elementData`는 `Object[]` 타입인데, `Arrays.copyOf`는 `Object[]`를 `T[]`로 캐스팅하려고 한다. 그래서 경고가 발생한다.
- `@SupperssWarnings("unchecked")`를 넣을 때는 그 경고를 무시해도 안전한 이유를 항상 주석으로 남겨야 한다.
- 실제로 제네릭 때문에 경고가 발생하는 예시 코드가 있었다. 
  - `@SuppressWarnings("unchecked")`을 추가하고 주석 또한 달아줬다.

```java
/*
 * 이 캐스팅은 안전함: <br>
 * 입력 매개변수 `temporal`이 T 타입이며, `with(...)` 호출은 동일한 타입을 반환하는 Temporal 구현체(LocalDate 등)에서만 사용됨. 따라서, (T) 캐스팅은 안전하게 간주됨.
 */
@SuppressWarnings("unchecked")
public static <T extends Temporal> T getStartOfWeek(final T temporal) {
  return (T) temporal.with(TemporalAdjusters.previousOrSame(DayOfWeek.SUNDAY));
}
```

--- 

## item28. 배열보다는 리스트를 사용하라

- 배열과 제네릭 타입에는 중요한 차이가 두 가지 있다.
1. 배열은 `공변 covariant`이다.
   - `Sub`이 `Super`의 하위 타입이라면, 배열 `Sub[]`은 배열 `Super[]`의 하위 타입이 된다. (공변 = 함께 변한다는 뜻)
   - 제네릭은 불공변이다. `List<Sub>`와 `List<Super>`는 완전히 다른 타입으로 취급 
2. 배열은 `실체화 reify`된다.
   - 런타임에도 자신이 담기로 한 원소의 타입을 인지하고 확인한다.
   - 반면, 제네릭은 타입 정보가 런타임에는 `소거 erasure`된다.
- `new List<E>[]`, `new E[]` 식으로 작성하면 컴파일할 때 제네릭 배열 생성 오류를 일으킨다.
- 제네릭 배열을 만들지 못하게 막은 이유?
  - 타입 안전하지 않기 때문.
- `E`, `List<E>`와 같은 타입을 `실체화 불가 타입 non-reifiable type`이라 한다.
  - 실체화되지 않아서 런타임에는 컴파일타임보다 타입 정보를 적게 가지는 타입이다.
  - 소거 매커니즘 때문에 매개변수화 타입 가운데 실체화될 수 있는 타입은 `List<?>`와 `Map<?, ?>` 같은 비한정적 와일드카드 타입뿐이다.
  - 배열은 한정적 와일드카드 타입으로 만들 수는 있지만, 유용하게 쓰일 일은 거의 없다.

---

## item29. 이왕이면 제네릭 타입으로 만들라

- 배열을 사용하는 코드를 제네릭으로 만들려 할 때는 항상 `실체화 불가 타입 문제`가 발목을 잡을 것이다. 

```java
elements = new E[DEFAULT_INITIAL_CAPACITY]; // 에러가 발생
```

- 적절한 해결책은 두 가지다.

1. 제네릭 배열 생성을 금지하는 제약을 대놓고 우회하는 방법을 사용
  - `Object` 배열을 생성한 다음 제네릭 배열로 형변환해보자. -> 오류 대신 경고. but, 일반적으로 타입 안전하지 않다.
  - 하지만 비검사 형변환이 확실히 안전하다고 판단되면 `@SuppressWarnings("unchecked")`으로 해당 경고를 숨기자.

```java
@SuppressWarnings("unchecked")
public Stack() {
  elements = (E[]) new Object[DEFAULT_INITIAL_CAPACITY]; 
}
```

2. `elements` 필드의 타입을 `E[]`에서 `Object[]`로 바꾸는 것이다.
   - 이때는 다른 오류가 발생한다.

```java
E result = elements[--size]; // 오류 발생
```

- 아래와 같이 배열이 반환한 원소를 형변환하면 오류 대신 경고가 뜬다.

```java
E result = (E) elements[--size]; // 경고
```

- 첫 번째 방법이 더 가독성이 좋다. 코드도 더 짧다. 따라서 현업에서는 첫 번째 방식을 더 선호한다.
  - 하지만 배열의 런타임 타입이 커파일타임 타입과 달라 `힙 오염 heap pollution`을 일으킨다. 
- 이 내용은 item28과 대치되는 내용이지만, 제네릭 타입 안에서 항상 리스트를 사용하는 것이 가능하지도 않고, 꼭 더 좋은 것도 아니다.

---

## item30. 이왕이면 제네릭 메소드로 만들라

- `타입 매개변수 목록`은 메소드의 `제한자`와 `반환 타입 사이`에 온다.
  - 아래 코드에서 `<E>`는 타입 매개변수 목록이고, `Set<E>`가 반환 타입이다.

```java
public static <E> Set<E> union(Set<E> s1, Set<E> s2) {
  Set<E> result = new HashSet<>(s1);
  result.addAll(s2);
  return result;
}
```

---

## item31. 한정적 와일드카드를 사용해 API 유연성을 높이라.

- 매개변수화 타입은 불공변. (type of `List<Type1>` != type of `List<Type2>`)
- `Integer`는 `Number`의 하위 타입. 그러나 매개변수화 타입이 불공변이기 때문에 `Stack<Number>`에 `Integer`를 넣으려고 하면 오류 메시지가 뜬다.
- 이런 상황에 대처하기 위해 한정적 와일드카드 타입이라는 특별한 매개변수화 타입을 지원한다.

```java
Iterable<? extends Number>
```

- 메시지는 분명하다. 유연성을 극대화하려면 원소의 생산자나 소비자용 입력 매개변수에 와일드카드 타입을 사용하라.
  - 한편, 입력 매개변수가 생산자와 소비자 역할을 동시에 한다면 와일드카드 타입을 써도 좋을 게 없다.
  - 타입을 정확히 지정해야 하는 상황으로, 이때는 와일드카드 타입을 쓰지 말아야 한다.
- 즉, 매개변수화 타입 `T`가 생산자라면 `<? extends T>`를 사용하고, 소비자라면 `<? super T>`를 사용하라.
  - ex) `Comparable`과 `Comparator`는 모두 소비자다. => `<? super T>`사용
  - 소비자란? 데이터를 읽어서 어떤 동작을 수행하지만, 반환하거나 저장하진 않음을 뜻한다.

```java
// 생산자
public void pushAll(Iterable<? extends T> items) {
  for (E e: src)
    push(e);
}

// 소비자
public void popAll(Iterable<? super T> dst) {
  while (!isEmpty())
    dst.add(pop());
}
```

- 클래스 사용자가 와일드카드 타입을 신경 써야 한다면 그 API에 무슨 문제가 있을 가능성이 크다.
- **기본 규칙**: 메소드 선언에 타입 매개변수가 한 번만 나오면 와일드카드로 대체하라.
  - 이때 비한정적 타입 매개변수라면 비한정적 와일드카드로 바꾸고, 한정적 타입 매개변수라면 한정적 와일드 카드로 바꾸면된다.

> #### GPT에게 물어본 타입매개변수를 써야하는 시점
> ```java
> public static <T> T pickOne(T a, T b) {
>    return Math.random() > 0.5 ? a : b;
>    }
> ```
> - 반환 타입에도 쓰이고, 두 매개변수 간의 타입 일치가 중요할 때!

- 헬퍼 메소드을 제네릭으로 선언해 타입 안전성을 보장하고, 외부에서는 멋진 선언을 유지할 수도 있다.

---

## item32. 제네릭과 가변인수를 함께 쓸 때는 신중하라

- 가변인수의 허점
  - 가변인수 메소드를 호출하면 가변인수를 담기 위한 배열이 자동으로 하나 만들어진다. 
  - 그런데 내부로 감춰야 했을 이 배열을 클라이언트에게 노출하는 문제가 생긴다.
  - 그 결과 `varargs` 매개변수에 제네릭이나 매개변수화 타입이 포함되면 알기 어려운 컴파일 경고가 발생한다.
- 메소드를 선언할 때 실체화 불가 타입으로 `varargs` 매개변수를 선언하면 컴파일러가 경고를 보낸다.
  - 가변인수 메소드를 호출할 때도 `varargs` 매개변수가 실체화 불가 타입으로 추론되면, 그 호출에 대해서도 경고를 낸다.

```java
static void dangerous(List<String> ...stringLists) {
  List<Integer> intList = List.of(42);
  Object[] objects = stringLists;
  objects[0] = intList;             // 힙 오염 발생
  String s = stringLists[0].get(0); // ClassCastException
}
```

- 이처럼 타입 안전성이 깨지니 제네릭 varargs 배열 매개변수에 값을 저장하는 것은 안전하지 않다.
- 그런데 여기서 생각해보자! 제네릭 배열을 직접 생성하는 건 허용하지 않으면서 제네릭 `varargs` 매개변수를 받는 메소드를 선언할 수 있게 한 이유는 무엇일까?
  - 제네릭이나 매개변수화 타입의 `varargs` 매개변수를 받는 메소드가 실무에서 매우 유용하기 때문이다.
  - 그래서 언어 설계자는 이 모순을 수용하기로 했다.
  - Java에서는 이런 메소드를 여럿 제공. (다행인 점은 타입 안전하다는 점.) 
    - `Arrays.asList(T... a)` 
    - `Collections.addAll(Collection<? super T> c, T... elements)`
    - `EnumSet.of(E first, E... rest)`
- `@SafeVarargs`는 메소드 작성자가 그 메소드가 타입 안전함을 보장하는 장치.
- 제네릭 `varargs` 매개변수 배열에 다른 메소드가 접근하도록 허용하면 안전하지 않는 점! 단 예외 두 가지!
  1. `@SafeVarargs`로 제대로 애노테이트된 또 다른 `varargs` 메소드에 넘기는 것은 안전.
  2. 그저 이 배열 내용의 일부 함수를 호출만 하는 (`varargs`를 받지 않는) 일반 메소드에 넘기는 것도 안전하다.

----

## item33. 타입 안전 이종 컨테이너를 고려하라

- 컨테이너 대신 키를 매개변수화한 다음, 컨테이너에 값을 넣거나 뺄 때 매개변수화한 키를 함께 제공하면 된다.
  - 이렇게 하면 제네릭 타입 시스템이 값의 타입이 키와 같음을 보장해줄 것이다.
  - 이러한 설계 방식을 `타입 안전 이종 컨테이너 패턴 type safe heterogeneous container pattern`dlfk gksek.

```java
public class Favorites {
  public <T> void putFavorite(Class<T> type, T instance);
  public <T> T getFavorite(Class<T> type);
}
```

```java
Favorites favorites = new Favorites();
favorites.putFavorite(String.class, "Hello");
favorites.putFavorite(Integer.class, 241);
favorites.putFavorite(Class.class, Favorites.class);
```

- `Favorites` 인스턴스는 타입 안전하다.
- 실제 구현은 아래와 같다.

```java
public class Favorites {
  private Map<Class<?>, Object> favorites = new HashMap<>();
  
  public <T> void putFavorite(Class<T> type, T instance) {
    favorites.put(Objects.requireNonNull(type), instance);
  }
  
  public <T> T getFavorite(Class<T> type) {
    return type.cast(favorites.get(type));  
  }
}
```


#### 제약1. 악의적인 클라이언트가 `Class` 객체를 로 타입으로 넘기면 `Favorites` 인스턴스의 타입 안전성이 쉽게 깨진다.
- 만약 타입 불변식을 어기는 일이 없도록 보장하려면 `putFavorite` 메소드에서 인수로 주어진 `instance`의 타입이 `type`으로 명시한 타입과 같은지 확인하면 된다.

```java
public <T> void putFavorite(Class<T> type, T instance) {
  favorites.put(Objects.requireNonNull(type), type.cast(instance));
}
```

- `java.util.Collections`에 `checkedSet`, `checkedList`, `checkedMap` 같은 메소드가 바로 이 방식을 적용한 컬렉션 래퍼들이다.

#### 제약2. 실체화 불가 타입에는 사용할 수 없다.

- `String`이나 `String[]`은 저장할 수 있어도 `List<String>`은 저장할 수 없다.

<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

