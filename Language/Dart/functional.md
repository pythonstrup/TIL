# Dart와 함수형 프로그래밍

## 간단한 예제

```dart
void main() {
  List<String> blackPink = ['로제', '지수', '리사', '제니', '제니'];

  print(blackPink);
  print(blackPink.asMap());
  print(blackPink.toSet());
}
```

```shell
[로제, 지수, 리사, 제니, 제니]
{0: 로제, 1: 지수, 2: 리사, 3: 제니, 4: 제니}
{로제, 지수, 리사, 제니}
```

- 아래 예제에서  `keys`와 `values`는 `iterable`이다.

```dart
void main() {
  List<String> blackPink = ['로제', '지수', '리사', '제니', '제니'];

  Map blackPinkMap = blackPink.asMap();
  print(blackPinkMap.keys);
  print(blackPinkMap.values);
}
```

```shell
(0, 1, 2, 3, 4)
(로제, 지수, 리사, 제니, 제니)
```

- 만약 `iterable`을 `List`로 변경하고 싶다면 `toList()`를 호출하면 된다.

```dart
print(blackPinkMap.keys.toList());
print(blackPinkMap.values.toList());
```

```shell
[0, 1, 2, 3, 4]
[로제, 지수, 리사, 제니, 제니]
```

- `List`는 아래와 같이 `Set.from()`을 통해서 Set으로 바꿀 수도 있다.

```dart
Set blackPinkSet = Set.from(blackPink);
```

## 다트의 고차함수 Higher-order function

### map

- 기능은 java, javascript의 `map`과 동일하다.
  - `map`은 새로운 `List`를 만들어 반환한다.
  - `MapEntry`로 감싸면 `Map` 자료구조로 반환하는 것도 가능하다.

```dart
void main() {
  List<String> blackPink = ['로제', '지수', '리사', '제니'];

  final newBlackPink = blackPink.map((x) {
    return '블랙핑크 $x';
  });

  print(blackPink);
  print(newBlackPink);
}
```

```shell
[로제, 지수, 리사, 제니]
(블랙핑크 로제, 블랙핑크 지수, 블랙핑크 리사, 블랙핑크 제니)
```

- arrow function도 사용할 수 있다.

```dart
final newBlackPink = blackPink.map((x) => '블랙핑크 $x');
```

- 다른 언어와 다르게 `Map` 자료구조에서 `map`을 호출하는 것이 가능하다!

```dart
void main() {
  Map<String, String> harryPotter = {
    'Harry Potter': '해리 포터',
    'Ron Weasley': '론 위즐리',
    'Hermione Granger': '헤르미온느 그레인저',
  };

  final result = harryPotter
      .map((key, value) => MapEntry('Harry Potter Character $key', '$value'));
  print(result);
}
```

```shell
{Harry Potter Character Harry Potter: 해리 포터, Harry Potter Character Ron Weasley: 론 위즐리, Harry Potter Character Hermione Granger: 헤르미온느 그레인저}
```

- `iterable`, `Set`도 `map` 호출이 가능하다.

### where

- javascript의 `filter()`와 동일한 기능을 한다.

```dart
void main() {
  List<Map<String, String>> people = [
    {'name': 'Lose', 'group': 'blackPink'},
    {'name': 'Jisu', 'group': 'blackPink'},
    {'name': 'RM', 'group': 'BTS'},
    {'name': 'V', 'group': 'BTS'},
  ];
  
  final result = people.where((x) => x['group'] == 'blackPink');
  print(result);
}
```

```shell
({name: Lose, group: blackPink}, {name: Jisu, group: blackPink})
```

### reduce

- javascript의 `reduce`와 비슷하다.
  - dart의 `reduce`는 초기값 세팅이 없다.

```dart
void main() {
  List<int> numbers = [1,3,5,7,9];
  final result = numbers.reduce((prev, next) {
    return prev + next;
  });
  print(result);
}
```

```shell
25
```

- 다른 점이 있다면, List의 타입 값과 동일한 타입만 return할 수 있다는 점에 유의해야 한다.

### fold

- javascript의 `reduce` 고차함수와 동일한 기능을 한다.
  - 초기값을 함수의 첫번째 인자로 전달한다.
  - 타입은 fold에서 제네릭으로 알려준다.

```dart
void main() {
  List<int> numbers = [1,3,5,7,9];
  final sum = numbers.fold<int>(0, (prev, next) => prev + next);
  print(sum);
}
```

## Cascade Operator

- 점 2개를 사용하는 연산자다. `..`
  - 중복되는 코드를 줄일 수 있는 연산자다.
  - 객체를 반환하는 함수에서만 사용할 수 있다!

```dart
class User {
  String name = '';
  int age = 0;
}

void main() {
  User user1 = User();
  user1.name = '홍길동';
  user1.age = 23;

  User user2 = User()
    ..name = '이몽룡'
    ..age = 23;

  print(user1.name);
  print(user2.name);
}
```

## Spread Operator

- javascript의 스프레드 연산자와 동일한 기능이다.

```dart
void main() {
  List<int> even = [2,4,6,8];
  List<int> odd = [1,3,5,7];
  print([...even, ...odd]);
}
```

```shell
[2, 4, 6, 8, 1, 3, 5, 7]
```
