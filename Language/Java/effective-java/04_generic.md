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

<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

