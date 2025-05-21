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

<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

