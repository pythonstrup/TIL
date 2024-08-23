# 모든 객체의 공통 메소드

## item 10. equals는 일반 규약을 지켜 재정의하라

- 아래의 상황 중 하나에 해당한다면 `equals`는 재정의하지 않는 것이 좋다.
  - 각 인스턴스가 본질적으로 고유하다. (ex. Thread 객체)
  - 인스턴스의 논리적 동치성(logical equality)을 검사할 일이 없다.
  - 상위 클래스에서 재정의한 equals가 하위 클래스에도 딱 들어맞는다.
  - 클래스가 private이나 package-private이고 equals 메소드를 호출할 일이 없다.

- equals를 재정의해야 할 때는 언제일까?
  - 논리적 동치성을 비교해야 하는데, 상위 클래스의 equals로 논리적 동치성을 확인할 수 없을 때

- equals 메소드를 재정의할 때는 반드시 아래의 규약을 따르도록 하자.
  - 반사성: x.equals(s)는 true
  - 대칭성: x.equals(y) == y.equals(x)
  - 추이성: x.equals(y)가 true, y.equals(z)가 true면 x.equals(z)도 true
  - 일관성: 반복해서 호출해도 항상 같은 결과
  - null 아님: 모든 참조 값 x에 대해 x.equals(null)은 false

- equals 선언하는 방법
  - `==` 연산자를 사용해 입력이 자기 자신의 참조인지 확인
  - `instanceof` 연산자로 입력이 올바른 타입인지 확인
  - 입력을 올바른 타입으로 형변환
  - 입력 객체와 자기 자신의 대응되는 '핵심' 필드들이 모두 일치하는지 하나씩 검사

- 기본 타입(Integer, String 등) 필드는 == 연산자로 비교한다. 대신 float과 double은 제외다.
  - `Float.compare(float, float)`과 `Double.compare(double, double)`로 비교
  - Float.NaN, -0.0f, 특수한 부동소수 값 등을 다뤄야 하기 때문

- 주의사항
  - `equals`를 재정의할 땐 `hashCode`도 반드시 재정의 (item11)
  - 너무 복잡하게 해결하려 들지 말자. 필드의 동치성만 검사하자. (별칭은 비교하지 않는 게 좋다.)
  - `Object` 외의 타입을 매개변수로 받는 `equals` 메소드는 선언하지 말자. 타입을 구체적으로 명시한 `equals`는 오히려 해가 된다.

<br/>

## item 11. equals를 재정의하려거든 hashCode도 재정의하라

> Object 명세 중
> - equals 비교에 사용되는 정보가 변경되지 않았다면 애플리케이션이 실행되는 동안 그 객체의 hashCode 메소드는 몇 번을 호출해도 일관되게 항상 같은 값을 반환해야 한다. 단 애플리케이션을 다시 실행한다면 이 값이 달라져도 상관없다.
> - equals(Object)가 두 객체를 같다고 판단했다면, 두 객체의 hashCode는 똑같은 값을 반환해야 한다.
> - equals(Object)가 두 객체를 다르다고 판단했더라도 두 객체의 hashCode가 서로 다른 값을 반환할 필요는 없다. 단, 다른 객체에 대해서는 다른 값을 반환해야 해시테이블의 성능이 좋아진다. (똑같은 bucket에 담기지 않아야 성능이 좋아짐)

- equals를 재정의한 클래스 모두에서 hashCode를 재정의해야 한다.
  - 해당 클래스의 인스턴스를 HashMap이나 HashSet 같은 컬렉션의 원소로 사용될 때 문제가 일어날 수 있기 때문이다.
  - 논리적으로 같은 객체는 같은 해시코드를 반환해야 한다.
- 만약 아래와 같이 `hashCode` 메소드에서 똑같은 값만 반환한다면 어떨까?
  - 모든 객체가 해시테이블의 버켓 하나에 담겨 마치 연결 리스트처럼 동작한다.
  - 해시테이블은 O(1)로 사용하려고 쓰는 자료구조인데, O(n)으로 동작해버린다.

```java
@Override
public int hashCode() { return 1; }
```

### 좋은 hashCode를 작성하는 요령

1. int 변수 result를 선언한 후 값 c로 초기화한다. 이때 c는 해당 객체의 첫번째 핵심 필드를 단계 2.a. 방식으로 계산한 해시코드다.
2. 해당 객체의 나머지 핵심 필드 f 각각에 대해 다음 작업을 수행한다.<br/>
  a. 해당 필드의 해시코드 c를 계산한다.<br/>
    i. 기본 타입 필드라면 Type.hashCode(f)를 수행<br/>
    ii. 참조 타입 필드면서 이 클래스의 equals 메소드가 이 필의 equals를 재귀적으로 호출해 비교한다면 이 필드의 hashCode를 재귀적으로 호출한다. 계산이 복잡해질 것 같으면 이 필드의 표준형(canonical representation)을 만들어 그 표준형의 hashCode를 호출한다. 필드의 값이 null이면 0을 사용한다.(전통적으로 0을 사용하기 때문)<br/>
    iii. 필드가 배열이라면, 핵심 원소 각각을 별도 필드처럼 다룬다. 이상의 규칙을 재귀적으로 적용해 각 핵심 원소의 해시코드를 계산한 다음, 단계 2.b. 방식으로 갱신한다. 배열에 핵심 원소가 없다면 단순히 상수(0 추천)를 사용한다. 모든 원소가 핵심 원소라면 `Arrays.hashCode()`를 사용한다.<br/>
  b. 단계 2.a.에서 계산한 해시코드 c를 result로 갱신한다. `result = 31 * result + c;`
3. result를 반환한다.

- 단계 2.b.는 필드를 곱하는 순에 따라 result 값이 달라지게 한다. => 클래스에 비슷한 필드가 여러 개일 때 해시 효과를 크게 높여준다.

### 코드 예시

```java
public class phoneNumber {
  private String areaCode;
  private String prefix;
  private String lineNum;
  
  @Override
  public int hashCode() {
    int result = Short.hashCode(areaCode);
    result = 31 * result + Short.hashCode(prefix);
    result = 31 * result + Short.hashCode(lineNum);
    return result;
  }
}
```

- 해시 충돌이 더욱 적은 방법을 사용해야 한다면 구아바의 `com.google.common.hash.Hashing`을 참고하자.
- `Obejcts`는 임의의 개수만큼 객체를 받아 해시코드를 계산하는 정적 메소드 `hash`를 제공한다.
  - 하지만 성능이 아쉽다. -> 입력 인수를 담기 위한 배열도 만들어야 한고 입력 중 기본 타입이 있다면 박싱과 언박싱도 해야 하기 때문이다.

```java
@Override
public int hashCode() {
  return Objects.hash(lineNum, prefix, areaCode);  
}
```

- 클래스가 불변이고 해시코드를 계산하기 비용이 크다면 매번 새로 계산하기 보다는 캐싱하는 방식을 고려하자.
  - 인스턴스가 만들어질 때 해시코드를 계산해두고 저장
  - 해시의 키로 사용되지 않는 경우라면 hashCode가 처음 불릴 때 계산하는 지연 초기화(lazy initialization)을 사용할 수도 있다. 대신 Thread-Safe하게 만들어야 한다. (item 83)

```java
public class phoneNumber {
  private String areaCode;
  private String prefix;
  private String lineNum;
  private int hashCode;
  
  @Override
  public int hashCode() {
    int result = hashCode;
    if (result == 0) {
      result = Short.hashCode(areaCode);
      result = 31 * result + Short.hashCode(prefix);
      result = 31 * result + Short.hashCode(lineNum);
      hashCode = result;
    }
    return result;
  }
}
```

- 성능을 높인답시고 해시코드를 계산할 때 핵심 필드를 생략해선 안된다.

<br/>

## item 12. toString을 항상 재정의하라

- Object의 `toString()`는 기본적으로 다음과 같은 문자열을 반환한다. `{className}@{hexadecimalHashCode}`
- toString의 일반 규약에 따르면 `간결하면서 사람이 읽기 쉬운 형태의 유익한 정보`를 반환해야 한다.
- 실전에 toString은 그 객체가 가진 주요 정보를 모두 반환하는 게 좋다.

### 개인적으로 알고 있는 것 추가

- 서로를 참조하고 있는 객체에서 `toString()`을 잘못 선언하면 순환 참조가 일어날 수 있으니 주의하자.
  - ex. JPA에서 연관관계로 묶여있는 Entity

<br/>

## item 13. clone 재정의는 주의해서 진행하라

- `Cloneable`은 복제해도 되는 클래스임을 명시하는 용도의 믹스인 인터페이스(mixin interface, item20)다.
  - 아쉽게도 의도한 목적을 이루지 못했다.
  - 가장 큰 문제는 clone 메소드가 선언된 곳이 `Cloneable`이 아닌 `Object`이고, 그마저도 protected로 선언되어 있다.
- `Cloneable`은 놀랍게도 `Object`의 protected 메소드인 `clone`의 동작 방식을 결정한다.
  - `Cloneable`을 구현한 클래스의 인스턴스에서 `clone`을 호출하면 그 객체의 필드들을 하나하나 복사한 객체를 반환하면, 그렇지 않은 클래스의 인스턴스에서 호출하면 `CloneNotSupportedException`을 던진다.
  - `Clonable`을 구현하더라도 clone을 구현하는 것이 필수가 아니다.
- 실무에서 `Cloneable`을 구현한 클래스는 clone 메소드를 public으로 제공하며 사용자는 당연히 복제가 제대로 이뤄지리라 생각한다.
  - 이 기대를 만족시키려면.. 허술하게 기술된 프로토콜을 지켜야 하는데 깨지기 쉽고, 위험하고 모순적인 매커니즘이 탄생한다.
  - 생성자를 호출하지 않고도 객체를 생성할 수 있게 되는 것이다.

> clone에 대한 Object 명세
> - 이 객체의 복사본을 생성해 반환한다. '복사'의 정확한 뜻은 그 객체를 구현한 클래스에 따라 다를 수 있다. 일반적인 의도는 다음과 같다. 어떤 객체 x에 대해 다음 식은 참이다.
> 
> `x.clone() != x`
> 
> - 또한 다음 식도 참이다.
> 
> `x.clone().getClass() == x.getClass()`
> 
> - 하지만 이상의 요구를 반드시 만족해야 하는 것은 아니다. 한편 다음 식도 일반적으로 참이지만 역시 필수는 아니다.
> 
> `x.clone().equals(x)`
> 
> - 관례상 이 메소드가 반환하는 객체는 `super.clone()`을 호출해 얻어야 한다. 이 클래스와 (Object를 제외한) 모든 상위 클래스가 이 관례를 따른다면 다음 식은 참이다.
> 
> `x.clone().getClass() == x.getClass()`
> 
> - 관례상, 반환된 객체와 원본 객체는 독립적이어야 한다. 이를 만족하려면 `super.clone()`으로 얻은 객체의 필드 중 하나 이상을 반환 전에 수정해야 할 수도 있다.
> 

- 강제성이 없다는 점만 빼면 생성자 연쇄(constructor chaining)과 유사한 매커니즘이다.
- Stack Class에 clone을 만들어보자.

```java
import java.util.Arrays;
import java.util.EmptyStackException;

public class Stack implements Cloneable {

  private Object[] elements;
  private int size = 0;
  private static final int DEFAULT_INITIAL_CAPACITY = 16;

  public Stack() {
    this.elements = new Object[DEFAULT_INITIAL_CAPACITY];
  }

  public void push(Object e) {
    ensureCapacity();
    elements[size++] = e;
  }

  public Object pop() {
    if (size == 0) {
      throw new EmptyStackException();
    }
    Object result = elements[--size];
    elements[size] = null;
    return result;
  }

  private void ensureCapacity() {
    if (elements.length == size) {
      elements = Arrays.copyOf(elements, 2 * size + 1);
    }
  }

  @Override
  public Stack clone() {
    try {
      Stack result = (Stack) super.clone();
      result.elements = elements.clone();
      return result;
    } catch (CloneNotSupportedException e) {
      throw new AssertionError();
    }
  }
}
```

- `Cloneable` 아키텍처는 '가변 객체를 참조하는 필드는 final로 선언하라'는 일반 용법과 충돌한다.
  - 그래서 복제할 수 있는 클래스를 만들기 위해 일부 필드에서 final 한정자를 제거해야 할 수도 있다.
- `clone`을 재귀적으로 호출한 것만으로는 충분하지 않을 때도 있다.
  - 아래 예시는 해시테이블을 구현한 것이다. 해시테이블 내부는 버킷들의 배열이고 연결리스트로 구성되어 있다. 
  - Stack과 같이 clone을 재귀적으로 호출하면 배열이 원본과 같은 연결리스트를 참조해 원복과 복제본이 모두 예기치 않게 동작할 가능성이 생긴다.
  - 아래와 같이 노드를 deepCopy해줘야 한다.

```java
public class HashTable implements Cloneable {

  private Entry[] buckets;

  private static class Entry implements Cloneable {
    final Object key;
    Object value;
    Entry next;

    public Entry(final Object key, final Object value, final Entry next) {
      this.key = key;
      this.value = value;
      this.next = next;
    }

    // 링크드리스트를 재귀적으로 복사
    Entry deepCopy() {
      return new Entry(key, value, next == null? null: next.deepCopy());
    }
  }

  @Override
  protected Object clone() {
    try {
      HashTable result = (HashTable) super.clone();
      result.buckets = new Entry[buckets.length];
      for (int i = 0; i < buckets.length; i++) {
        if (buckets[i] != null) {
          result.buckets[i] = buckets[i].deepCopy();
        }
      }
      return result;
    } catch (CloneNotSupportedException e) {
      throw new AssertionError();
    }
  }
}
```

- 위 코드를 더 개선할 수 있다.
  - 위 코드는 간단하지만 재귀 호출로 인해 리스트의 원소 수만큼 스택 프레임을 소비하여 리스트가 길면 스택 오버플로우를 일으킬 소지가 있다.
  - 재귀 호출 대신 반복자를 사용해 개선해보자.

```java
public class HashTable implements Cloneable {

  private static class Entry implements Cloneable {
    
    ...

    Entry deepCopy() {
      Entry result = new Entry(key, value, next);
      for (Entry p = result; p.next != null; p = p.next) {
        p.next = new Entry(p.next.key, p.next.value, p.next.next);
      }
      return result;
    }
  }
  
  ... 
}
```

### 복사 생성자와 복사 팩토리를 사용하자.

- 복사 생성자/복사 팩토리는 Cloneable/clone 방식보다 나은 면이 많다.
  - 위험천만한 객체 생성 매커니즘을 사용하지 않는다.
  - 엉성하게 문서화된 규약에 기대지 않는다.
  - 정상적인 final 필드와 충돌하지 않는다.
  - 불필요한 Checked Exception을 던지지 않는다.
  - 형변환도 필요치 않다.

```java
public class MyClass {
  
  ...
  
  // 복사 생성자 (혹은 변환 생성자 Conversion Constructor)
  public MyClass(MyClass myClass) {...}
  
  // 복사 팩토리 (혹은 변환 팩토리 Conversion Factory)
  public static MyClass newInstance(MyClass myClass) {...}
}
```

<br/>

## item 14. Comparable을 구현할지 고려하라

- Comparable의 `compareTo()` 다른 공통 메소드와는 다르게 `Object`의 메소드가 아니다.
  - `compareTo()`는 `Obejct`의 `equals()`와 비슷하게 동치성 비교를 할 수 있다.
  - 하지만 `equals()`와 다른 점은 (1) 순서를 비교할 수 있고 (2) 제네릭하다는 점이다. 
- Comparable을 구현한 객체들의 배열을 다음과 같이 정렬할 수 있다. `Arrays.sort(a);`
- 검색, 극단값 계산(Math.min, Math.max), 자동 정렬되는 컬렉션 관리도 역시 쉽게 할 수 있다.
- `compareTo` 메소드의 일반 규약은 `equals`와 비슷하다.

> compareTo의 규약
> 이 객체와 주어진 객체의 순서를 비교한다. 이 객체가 주어진 객체보다 작으면 음의 정수를, 같으면 0을 크면 양의 정수를 반환한다. 이 객체와 비교할 수 없는 타입의 객체가 주어지면 `ClassCastExcepion`을 던진다.
> 
> 다음 설명에서 sgn(표현식) 표기는 수학에서 말하는 부호 함수(signum function)를 뜻하며, 표현식의 값이 음수, 0, 양수일 때 -1, 0, 1을 반환하도록 정의했다.
> - `Comparable`를 구현한 클래스는 모든 x, y에 대해 `sgn(x.compareTo(y)) == -sgn(y.compareTo(x))`여야 한다. (예외를 던지는 것도 동일해야 한다.)
> - `Comparable`을 구현한 클래스는 추이성을 보장해야 한다. (`x.compareTo(y) >0 && y.compareTo(z > 0)` 이면 `x.compareTo(z) > 0` 이다.)
> - `Comparable`을 구현한 클래스는 모든 z에 대해 x.compareTo(y) == 0 이면 sgn(x.compare(z)) == sgn(y.compareTo(z))다.
> - 이 권고는 필수가 아니지만 지키는 게 좋다. (x.compareTo(y) == 0) == (x.equals(y)) 여야 한다. `Comparable`를 구현하고 이 권고를 지키지 않는 모든 클래스는 그 사실을 명시해야 한다. "주의: 이 클래스의 순서는 `equals` 메소드와 일관되지 않습니다."

### 주의할 사항

- compareTo를 사용할 때 박싱된 기본 타입 클래스의 compare를 사용하라! 
  - 자바 7 이후부터 모든 기본 타입 박싱 클래스에 `compare` 메소드가 추가되었다. (자바 7 이전에는 부등호 쓰는 것을 권했다고 함 -> 거추장스럽고 오류를 유발함.)
  - 책에서 보면 equals는 이와 다르게 `==` 사용하라고 명시되어 있다. (item10 확인해보면 나옴)
  - ex. Integer.compare()
- 기본 타입 필드가 여러 개라면 어떻게 비교해야 할까?
  - 중요한 필드 순서대로 비교한다.

```java
@Override
public int compareTo(PhoneNumber pn) {
  int result = Short.compare(areaCode, pn.areaCode);
  if (result == 0) {
    result = Short.compare(prefix, pn.prefix);
    if (result == 0) {
      result = Short.compare(lineNum, pn.lineNum);  
    }
  }
  return result;
}
```

- Java 8에서 Comparator 인터페이스가 일련의 비교자 생성 메소드와 팀을 꾸려 메소드 연쇄 방식으로 비교자를 생성할 수 있게 되었다.
  - 간결하나 약간의 성능 저하가 뒤따른다고 한다. (약 10% 정도)

```java
private static final Comparator<PhoneNumber> COMPARATOR =
        comparingInt((PhoneNumber pn) -> pn.areaCode)
              .thenComparingInt(pn -> pn.prefix) // 2번째부터는 PhoneNumber 타입으로 자동 추론
              .thenComparingInt(pn -> pn.prefix);

@Override
public int compareTo(PhoneNumber pn) {
  return COMPARATOR.compare(this, pn);
}
```

<br/>

# 참고자료

- Effective Java 3/E, Joshua Bloch

