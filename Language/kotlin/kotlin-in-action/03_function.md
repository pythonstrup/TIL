# 3장 함수 정의와 호출

## 1. 컬렉션

```kotlin
val set = setOf(1, 7, 33)
val list = listOf(1, 7, 33)
val map = mapOf("a" to 1, "b" to 2)
```

- 여기서 구현체를 확인해보면 (여기서 `javaClass`는 자바의 `getClass()`와 같은 코틀린 문법)

```kotlin
println(set.javaClass)  // class java.util.LinkedHashSet
println(list.javaClass) // class java.util.Arrays$ArrayList
println(map.javaClass)  // class java.util.LinkedHashMap
```

## 2. 함수를 호출하기 쉽게 만들기

### 2-1. 이름 붙인 인자

- 인자로 어떤 역할인지 구문하기 어려울 때!
- 자바에서는 파라미터 이름에 주석을 넣도록 스타일을 강요하는 곳도 있다고 함.

```java
joinToString(collection, /* separator */ "");
```

- 코틀린에서는 파라미터 이름을 명시하는 문법이 있다.

```kotlin
joinToString(collection, separator = " ")
```

### 2-2. 디폴트 파라미터

```kotlin
fun <T> joinToString(
    collection: Collection<T>,
    separator: String = ", "
): String
```

- 위와 같이 디포트 파라미터를 지정한 경우, 해당 값을 생략하면 설정해둔 기본값이 들어간다.

```kotlin
joinToString(list) // separator로 ", " 값이 들어감
```

- 함수의 디폴트 파라미터 값은 함수를 호출하는 쪽이 아니라 함수 선언 쪽에 인코딩된다는 사실 주의.

> #### 참고
> 만약 자바 측에서 코틀린 함수를 편하게 호출하고 싶을 시에 `@JvmOverloads` 어노테이션을 함수에 추가할 수 있다.
> ```kotlin
> @JvmOverloads
> fun <T> joinToString(
>   collection: Collection<T>,
>   separator: String = ", "
> ): String { /* ... */ }
> ```
> 위 코드는 아래와 같은 오버로드 함수를 생성한다.
> ```java
> String joinToString(Collection<T> collection, String separator);
> String joinToString(Collection<T> collection);
> ```
> 그리고 각각의 오버로드 함수들은 시그니처에서 생략된 파라미터에 대해 코틀린 함수의 디폴트 파라미터 값을 사용한다.


### 2-3. 정적인 유틸리티 클래스 없애기: 최상위 함수와 프로퍼티

#### 최상위 함수

- 자바의 불편한 점. 최상위에 클래스가 있어야만 한다.
  - 함수를 선언하더라도 최상위 클래스가 필요하다.
- 코틀린은 최상위에 함수만 선언할 수 있다.

```kotlin
package strings

fun joinToString(/* ... */): String { /* ... */ }
```

#### 최상위 프로퍼티

- 코틀린에서는 프로퍼티도 최상위에 놓을 수 있다.

```kotlin
var opCount = 0

fun performOperation() {
    opCount++
    // ...
}
```

- 위 프로퍼티의 값은 정적 필드에 저장된다.
- 최상위 프로퍼티를 활용해 코드에서 상수를 정의할 수 있다.

```kotlin
const val UNIX_LINE_SEPARATOR = "\n"
```

- 위 코틀린 함수는 아래의 자바 코드와 동등한 바이트코드를 만든다.

```java
public static final String UNIX_LINE_SEPARATOR = "\n";
```

## 3. 메소드를 다른 클래스에 추가: 확장 함수와 확장 프로퍼티

- 기존 자바 API를 재작성하지 않고도 편리한 여러 기능을 사용할 수 있다면 좋지 않을까?
- 바로 코틀린의 `확장 함수 extension function`가 그런 역할을 해줄 수 있다.

```kotlin
package strings

fun String.lastChar(): Char = this.get(this.length - 1)
```

- 그 함수가 확장할 클래스의 이름을 덧붙이는 것이다. (내가 보기엔 자바스크립트의 함수의 프로토타입 선언과 비슷해 보인다.)
- 이런 클래스 이름을 `수신 객체 타입 receiver type`이라 부르며, 확장 함수 호출 시 호출하는 대상 값을 `수신 객체 receiver object`라고 부른다.

```kotlin
fun main() {
    println("My String".lastChar())
}
```

- 위 예시에서는 `String`이 수신 객체 타입이고, `"My String"`이 수신 객체다.

> #### 메시지 패싱 Message Passing
> - OOP의 개념
> - "함수를 호출한다" = "객체에게 메시지를 보낸다"
> - 메시지를 수신(함수 호출)하는 주체가 바로 그 객체이기 때문에 수신자라고 부르는 것이다.

### 3-1. 임포트와 확장 함수

- 확장 함수를 정의했다고 해도 자동으로 프로젝트 안의 모든 소스코드에서 그 함수를쓸 수 있지는 않다.
- 확장 함수를 쓰려면 다른 클래스나 함수와 마찬가지로 해당 함수를 임포트해야만 한다. => 이름 충졸을 막기 위함.
- 코틀린에서는 클래스를 임포트할 때와 같은 구문을 사용해 임포트가 가능하다.

```kotlin
import strings.lastChar // 개별 함수 임포트
```

```kotlin
import strings.*        // *를 사용한 와일드카드 임포트
```

```kotlin
import string.lastChar as last // as 키워드를 활용한 임포트

val c = "Kotlin".last()
```

- `as` 키워드는 여러 패키지에 이름이 같은 함수가 많은데, 함께 사용해야 할 때 사용하기 좋다.
- 일반적인 클래스나 함수라면 `전체 이름 FQN, Fully Qualifyed Name`을 써서 해당 이름을 사용할 수도 있따.

### 3-2. 자바에서 확장 함수 호출

- 내부적으로 확장 함수는 수신 객체를 첫 번째 인자로 받는 정적 메소드다. => 확장 함수를 호출해도 다른 어댑터 객체나 실행 시점 부가 비용이 들지 않음.
  - 따라서 자바에서 사용하기도 편하다. 첫 번째 인자로 수신 객체를 넘기면 된다.

```java
char c = StringUtilKt.lastChar("Java");
```

### 3-3. 확장 함수로 유틸리티 함수 정의

- 확장 함수는 단지 정적 메소드 호출에 대한 `문법적인 편의 syntatic sugar`일 뿐이다. 
  - 따라서 클래스가 아닌 더 구체적인 타입을 수신 객체 타입으로 지정할 수도 있다.
- 예를 들어, 문자열의 컬렉션에 대해서만 호출할 수 있는 `join` 함수를 정의하고 싶다면 아래와 같이 하면 된다.

```kotlin
fun Collection<String>.join(
    separator: String = ", ",
    prefix: String = "",
    postfix: String = ""
) = joinToString(separator, prefix, postfix)

fun main() {
    println(listOf("one", "two", "eight").join(""))
}
```

- 문자열 타입으로 정의했기 때문에 다른 타입의 객체로 이뤄진 리스트는 호출할 수 없다.

```kotlin
listOf(1, 2, 3).join()
```

### 3-4. 확장 함수는 오버라이드할 수 없다.

- 확장 함수는 정적 메소드와 같은 특성을 가지므로 확장 함수를 하위 클래스에서 오버라이드할 수 없다.
- 확장 함수를 첫 번째 인자가 수신 객체인 정적 자바 메소드로 컴파일한다는 사실을 기억한다면 동작을 이해할 때 도움이 될 것이다.

```java
View view = new Button();
ExtensionKt.showOff(view);
```

> #### 참고
> - 어떤 클래스를 확장한 함수와 그 클래스의 멤버 함수의 이름과 시그니처가 같다면 확장 함수가 아니라 멤버 함수가 호출된다. 클래스의 API를 확장할 경우 항상 이를 염두에 둬야 한다.
> - 만약 우리가 소유권을 가진 클래스에 대해 시그니처가 같은 확장수를 정의해서 사용하는 외부 클라이언트 프로젝트가 있다고 해보자. 
>   - 우리의 클래스 내부에 같은 시그니처 내부 함수를 추가하면 클라이언트 프로젝트를 재컴파일한 순간부터 그 클라이언트는 확장 함수가 아니라 새로 추가된 멤버 변수를 사용하게 된다. 

### 3-5. 확장 프로퍼티

- 실제로 확장 프로퍼티는아무 상태도 가질 수 없다. 따라서 커스텀 접근자를 정의한다. (2.2.2절)

```kotlin
val String.lastChar: Char
    get() = get(length - 1)
```

- 확장 프로퍼티도 단지 프로퍼티에 수신 객체 클래스가 추가됐을 뿐이다.
- 뒷받침하는 필드가 없어 기본 getter 구현을 제공할 수 없으므로 최소한 getter는 꼭 정의해야 한다.
- 마찬가지로 초기화 코드에서 계산한 값을 담을 장소가 전혀 없으므로 초기화 코드도 쓸 수 없다.
- `StringBuilder`의 맨 마지막 문자를 변경할 수 있으므로 아래와 같이 정의한다면 이를 `var`로 만들 수도 있다.

```kotlin
var StringBuilder.lastChar: Char
    get() = get(length - 1)
    set(value: Char) {
        this.setCharAt(length - 1, value)
    }
```

```kotlin
fun main() { 
  val sb = StringBuilder("Kotlin?")
  println(sb.lastChar) // ?
  sb.lastChar = '!'
  println(sb)          // "Kotlin!"
}
```

## 컬렉션 처리

- `vararg` 키워드: 인자 개수가 달라질 수 있는 함수 정의
- `중위 infix` 함수 호출 구문: 인자가 하나뿐인 메소드를 간편하게 호출
- `구조 분해 선언 destructuring declaration`: 복합적인 값을 분해해 여러 변수에 나눠 담을 수 있다.

### 4-1. 자바 컬렉션 API 확장

- 컬렉션에서도 확장 함수를 통해 API를 확장할 수 있다.

### 4-2. 가변 인자 함수

- 코틀린에서 `listOf`는 대충 아래와 같이 정의되어 있다. (`vararg`에 주목하자)

```kotlin
fun listOf<T>(vararg values: T): List<T> { /* 구현 */ }
```

> #### 자바에서는?
> - 타입 뒤에 `...`을 붙여서 표현한다.

- 스프레드 연산은 배열 앞에 *를 붙이기만 하면 된다. (자바에는 없는 기능)

```kotlin
fun main(args: Array<String>) {
  val list = listOf(*args)   
}
```

### 4-3. 튜플 tuple: 중위 호출과 구조 분해 선언

```kotlin
mapOf(1 to "one")
```

- 여기서 `to`는 코틀린 키워드가 아니다. 이 코드는 `중위 호출 infix call`이라는 특별한 방식으로 `to`라는 일반 메소드를 호출한 것이다.
- 다음 두 호출은 동일한다.

```kotlin
1.to("one")
1 to "one"
```

- 함수를 중위 호출에 사용하게 허용하고 싶으면 `infix` 변경자를 함수 선언 앞에 추가해야 한다.
- 아래는 `to` 함수의 정의를 간략하게 줄인 것이다. (실제로는 제네릭이라고 함.)

```kotlin
infix fun Any.to(other: Any) = Pair(this, other)
```

- 이제 `Pair`의 내용을 갖고 두 변수를 즉시 초기화할 수 있다.

```kotlin
val (number, name) = 1 to "one"
```

- 이런 기능을 `구조 분해 선언 destructuring declaration`이라고 부른다.
  - 순서쌍에만 해당하지 않는다.
  - 루프에서도 구조 분해 선언 가능 ex) `for ((index, element) in collection.withIndex())`
- `to` 함수는 확장 함수. `to`의 수신 객체는 제네릭.

```kotlin
fun <K, V> mapOf(vararg values: Pair<K, V>): Map<K, V>
```

## 5. 문자열과 정규식

- 코틀린 문자열 = 자바 문자열

### 5-1. 문자열 나누기

- 자바 `String`의 `split()` 메소드
  - 문제가 있다. `.` 점으로 문자열 분리가 안 된다. => 빈 배열을 반환해버린다.
  - 이유? `split`이 정규식을 구분 문자열로 받아 그 정규식에 따라 문자열을 나누기 때문이다. => `.`는 모든 문자를 나타내는 정규식으로 해석
- 코틀린에서는 여러 가지 다른 조합의 파라미터를 받는 `split` 확장 함수를 제공하여 혼동을 야기하는 메소드를 감춘다.
  - 정규식을 파라미터로 받는 함수는 `String`이 아닌 `Regex` 타입의 값을 받는다.
  - 따라서 코틀린에서는 `split` 함수에 전달하는 값의 타입에 따라 정규식이나 일반 텍스트 중 어느 것으로 문자열을 분리하는지 쉽게 알 수 있다.

```kotlin
"12.345-6.A".split("\\.|-".toRegex()) // 정규식을 명시적으로 만든다.
// 결과값 [12, 345, 6, A]
```

- 정규식 문법은 자바와 동일하다.
- 다만 위와 같이 정규식을 쓸 필요 없이 아래와 같이 문자를 넘기는 식으로 같은 결과를 얻는다.

```kotlin
"12.345-6.A".split('.', '-')
```

### 5-2. 정규식과 3중 따옴표로 묶은 문자열

```kotlin
val regex = """(.+)/(.+)\.(.+)""".Regex()
```

- 삼중 따옴표 문자열을 사용하면 백슬래시를 포함한 어떤 문자도 이스케이프할 필요가 없다.
  - 원래 마침표 기호를 이스케이프하려면 `\\.`라고 써야 하지만 3중 따옴표에서는 `\.` 라고 쓰면 된다.

### 5-3. 여러 줄 3중 따옴표

- 들여쓰기가 포함된 문자열 표현
  - 대신 줄바꿈에 특수문자 사용 불가
  - 마찬가지로 따로 이스케이프 필요 없음 ex) `\` 그대로 사용
- 하지만 `$`랑 유니코드 이스케이프를 사용하고 싶을 때는 내포식을 사용해야 한다.

### 6. 코드 깔끔하게 다듬기: 로컬 함수와 확장

- 코틀린에서는 함수에서 추출한 함수를 원래의 함수 내부에 내포시킬 수 있다. => 문법적인 부가비용 불필요

```kotlin
class User(val id: Int, val name: String, val address: String)

fun saveUser(user: User) {
  fun validate(value: String, fieldName: String) { /* 검증 구현 */}
  validate(user.name, "Name")
  validate(user.address, "Address")
}
```

- 아래와 같이 User 클래스를 확장한 함수로 검증 로직을 만들 수도 있다.

```kotlin
class User(val id: Int, val name: String, val address: String)

fun User.validateBeforeSave() {
  fun validate(value: String, fieldName: String) { /* 검증 구현 */}
  validate(name, "Name")
  validate(address, "Address")
}

fun saveUser(user: User) {
    user.validateBeforeSave() // 확장 함수 호출
}
```

- `User`에 `User` 검증 로직을 포함시키지 않으면서 `User`를 간결하게 유지하는 것에 성공한 것이다.
- 하지만 내포된 함수의 깊이가 깊어지면 코드 가독성이 떨어질 수 있으므로 주의해야 한다.
