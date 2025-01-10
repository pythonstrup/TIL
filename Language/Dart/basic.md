# Dart 기초

## 변수

#### variable

- `var`를 통해 선언할 수 있다.
- 사용을 자제하자. 아래에 서술된 정확한 타입을 명시해주는 것이 좋다.
    - 어쩔 수 없을 때만 사용하자.

```dart

var name = 'name';
```

#### integer

- 정수형

```dart

int number = 10;
```

- 다른 언어와 마찬가지로 사칙연산이 가능하다.

#### dynamic

- 어떤 타입이든 다 넣을 수 있는 타입.

```dart

dynamic myVar = 1;
```

- 다른 타입으로 변경하는 것이 가능하다. (`var`는 불가능)

```dart
dynamic myVar = 1;
myVar = 'string';
```

#### `bool`

- 불리언 타입

#### `String`

- 스트링 타입
- 객체라서 앞이 대문자다.

#### nullable

- null이란 "아무런 값도 있지 않다"라는 뜻.
- null이 될 수 있다는 표시

```dart
String? name = 'string';
name = null;
```

#### non-nullable

- null이 될 수 없다.

```dart
String name2 = 'string';
name2 = null;
```

- 아래와 같이 에러가 발생하게 된다.

```shell
compileDDC
main.dart:7:11: Error: The value 'null' can't be assigned to a variable of type 'String' because 'String' is not nullable.
  name2 = null;
```

### final

- 다른 언어의 `final`와 동일하게 사용. 재선언 불가.

```dart
final DateTime now = DateTime.now();
```

- `const`의 경우 빌드 타임에 값을 알고 있어야 하기 때문에 아래의 경우 에러가 발생한다.

```dart
const DateTime now = DateTime.now(); // 런타임에 값을 알 수 있기 때문
```

## 특이한 연산

- 대부분의 연산은 다른 언어와 동일하다.

#### `??=`

- 변수가 null이면 대입되는 연산자이다.

```dart
double? number = null;
number ??= 3.0;
print(number);
```

```shell
3
```

- 아래와 같이 값이 들어가 있으면 `??=`으로 대입되지 않는 것을 확인할 수 있다. 

```dart
double? number = 2.0;
number ??= 3.0;
print(number);
```

```shell
2
```

#### `is`

- 타입을 확인할 때 사용하는 연산자

```dart
int number = 1;
print(number is int);
print(number is String);
print(number is! int);
print(number is! String);
```

```shell
true
false
false
true
```

## 컬렉션

#### `List`

- Java와 비슷

#### `Map`

```dart
Map<String, String> dictionary = {
  'Harry': '해리'
};

print(dictionary);
print(dictionary['Harry']);
print(dictionary['Ron']);
```

```shell
{Harry: 해리}
해리
null
```

#### `Set`

```dart
Set<String> names = {
  'Harry',
};

names.contains('Harry');
```

## IF-ELSE, SWITCH, FOR 문

- 다른 언어와 동일

## Enum

```dart
enum MyType {
  A,
  B,
  C
}

void main() {
  MyType myType = MyType.A;
  print(myType);
}

```

```shell
MyType.A
```

## 함수 선언

- javascript보다 더 간단하다.

```dart
void main() {
  print(addNumbers(1, 2));
}

addNumbers(int a, int b) {
  return a + b;
}
```

- 아래와 같이 함수에 타입을 안 넣을 수도 있다. 그래도 웬만하면 타입은 넣자.

```dart
addNumbers(int a, int b) {
  return a + b;
}
```

### Optional Parameter

```dart
myFunction(int a, [int? b, int? c]) {
  
}
```

- 하지만 더하기 연산을 진행하는 함수에서는 위와 같이 선언하면 문제가 생기기 때문에 아래와 같이 기본값을 넣어주는 형태로도 가능하다.

```dart
addNumbers(int a, [int b = 20, int c = 30]) {
  return a + b + c;
}
```

### Named Parameter

- 이름이 있는 파라미터 => 순서가 중요하지 않게 된다.

```dart
void main() {
  print(addNumbers(a: 1, b: 2, c: 3));
  print(addNumbers(b: 2, a: 1, c: 3));
}

addNumbers({
  required int a,
  required int b,
  required int c,
}) {
  return a + b + c;
}
```

- 아래와 같이 optional parameter와 섞어서 사용할 수도 있다.

```dart
void main() {
  print(addNumbers(b: 2, a: 1, c: 3));
  print(addNumbers(b: 2, a: 1));
}

addNumbers({
  required int a,
  required int b,
  int c = 30,
}) {
  return a + b + c;
}
```

### 반환값 타입 지정

- 아래와 같이 반환값의 타입을 지정하는 것이 가능하다.

```dart
int addNumbers({
  required int a,
  required int b,
  int c = 30,
}) {
  return a + b + c;
}

void myFunc() {}
```

### Arrow Function

```dart
void main() {
  print(addNumbers(a: 1, b: 2, c: 3));
}

addNumbers({
  required int a,
  required int b,
  required int c,
}) => a + b + c;
```

### typedef

- 함수에 body가 없다.
- 함수 타입을 선언하는 거라고 생각하면 된다!

```dart
void main() {
  Operation operation = add;

  int result = operation(10, 20, 30);
}

typedef Operation = int Function(int x, int y, int z);

// 더하기
int add(int x, int y, int z) => x + y + z;

// 빼기
int subtract(int x, int y, int z) => x - y - z;
```

