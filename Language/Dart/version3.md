# Dart 3.0 업데이트 주요 내용

## Record

```dart
void main() {
  final result = nameAndAge({
    'name': '민지',
    'age': 20,
  });

  print(result);
  print(result.$1);
  print(result.$2);
}

// Record
nameAndAge(Map<String, dynamic> json) {
  // tuple 형태의 자료구조 => 타입의 순서를 보장할 수 있다.
  return (json['name'] as String, json['age'] as int);
}
```

```shell
(민지, 20)
민지
20
```

- Map을 통해 매칭하는 값을 만들 수도 있지만, Record를 사용해 List에 값을 바로 넣을 수 있게 되었다.

```dart
List<Map<String, dynamic>> getNewJeans() {
  return [
    {
      'name': '민지',
      'age': 20,
    },
    {
      'name': '해린',
      'age': 18,
    }
  ];
}

List<(String, int)> getNewJeansWithType() {
  return [('민지', 20), ('해린', 18)];
}
```

- Record로 묶은 List를 출력해보면 아래와 같을 것이다.

```dart
void main() {
  final result = getNewJeansWithType();
  for (final item in result) {
    print(item.$1);
    print(item.$2);
  }
}
```

```shell
민지
20
해린
18
```

### 레코드의 키

```dart
void main() {
  final result5 = getNewJeansWithType3();
  for (final item in result5) {
    print(item.name);
    print(item.age);
  }
}

List<({String name, int age})> getNewJeansWithType3() {
  return [(name: '민지', age: 20), (name: '해린', age: 18)];
}
```

```shell
민지
20
해린
18
```

## 구조 분해 할당 Destructuring 

- `Record`를 아래와 같이 구조 분해 할당할 수 있다.

```dart
void main() {
  final (name, age) = ('민지', 20);

  print(name);
  print(age);
}
```

- 그런데 `Record`에만 추가된 것이 아니라 다른 부분에도 추가가 되었다!!

### List 형태의 구조 분해 할당

- 아래는 List의 예시이다.

```dart
void main() {
  final newJeans = ['민지', '해린'];
  final [a, b] = newJeans;
  print(a);
  print(b);
}
```

```shell
민지
해린
```

#### 리스트 구조 분해 할당 - 범위 제외 처리

- 중간을 제외 처리하는 방법이 있다.

```dart
void main() {
  final numbers = [1,2,3,4,5,6,7,8];
  final [x, y, ..., z] = numbers;
  print(x);
  print(y);
  print(z);
}
```

```shell
1
2
8
```

- 혹은 중간에 있는 값도 아래와 같이 호출할 수 있다.

```dart
void main() {
  final [xx, yy, ...rest, zz] = numbers;
  print(xx);
  print(yy);
  print(zz);
  print(rest);
}
```

```shell
1
2
8
[3, 4, 5, 6, 7]
```

#### 리스트 구조 분해 할당 - 단일 값 제외 처리

- `_`를 단일 값에 대해 제외 처리를 할 수 있다.

```dart
void main() {
  final numbers = [1, 2, 3, 4, 5, 6, 7, 8];
  final [xxx, _, yyy, ...rest, zzz, _] = numbers;
  print(xxx);
  print(yyy);
  print(zzz);
  print(rest);
}
```

- 결과 값은 아래와 같이 나온다.

```shell
1
3
7
[4, 5, 6]
```

### Map 구조 분해 할당

- `Map`의 경우 key 값에 해당하는 값을 변수에 매칭해줘야 한다. 

```dart
void main() {
  final personMap = {'name': '민지', 'age': 19};
  final {'name': name3, 'age': age3} = personMap;
  print(name3);
  print(age3);
}
```

### 객체의 구조 분해 할당

```dart
void main() {
  final person = Person(name: '민지', age: 19);
  final Person(name: name4, age: age4) = person;
  print(name4);
  print(age4);
}

class Person {
  final String name;
  final int age;

  Person({
    required this.name,
    required this.age,
  });
}
```

<br>

## Pattern Matching

### Switch

- Dart의 스위치 문이 상당히 개선되었다고 함.

```dart
void main() {
  switcher('aaa');
  switcher('bbb');
  switcher(['1', '2']);
  switcher([1, 2]);
  switcher([1, 2, 3]);
  switcher([4, 5, 6]);
  switcher([4, 5, 6, 7]);
  switcher([1, '2']);
}

void switcher(dynamic anything) {
  switch (anything) {
    case 'aaa':
      print('match: aaa');
    case ['1', '2']:
      print('match: ["1", "2"]');
    case [_, _, _]:
      print('match: [_, _, _]');
    case [int a, int b]:
      print('match: [int a, int b]');
    default:
      print('no match');
  }
}
```

```shell
match: aaa
no match
match: ["1", "2"]
match: [int a, int b]
match: [_, _, _]
match: [_, _, _]
no match
no match
```

- 대소 비교 매칭도 가능하다

```dart
void main() {
  switcher(17);
  switcher(8);
}

void switcher(dynamic anything) {
  switch (anything) {
    case < 10 && > 5:
      print('match: < 10 && > 5');
    default:
      print('no match');
  }
}
```

```shell
no match
match: < 10 && > 5
```

#### Switch & Arrow Function

- 아래와 같이

```dart
void main() {
  print(switcher2(5, true));
  print(switcher2(2, true));
  print(switcher2(7, true));
  print(switcher2(7, false));
}

String switcher2(dynamic val, bool condition) => switch (val) {
  5 => 'match: 5',
  7 when condition => 'match 7 and true',
  _ => 'no match', // default
};

```

```shell
match: 5
no match
match 7 and true
no match
```

### for loop의 패턴 매칭

```dart
void main() {
  forLooper();
}

void forLooper() {
  final List<Map<String, dynamic>> members = [
    {
      'name': 'lee',
      'age': 20,
    },
    {'name': 'bell', 'age': 28},
  ];

  print('--------------------');

  for (var {'name': name, 'age': age} in members) {
    print(name);
    print(age);
  }
}
```

```shell
lee
20
bell
28
```

### If 문의 패턴 매칭

```dart
void main() {
  ifMatcher();
}

void ifMatcher() {
  final person = {
    'name': 'bell',
    'age': 28,
  };

  if (person case {'name': String name, 'age': int age}) {
    print(name);
    print(age);
  }
}
```

```shell
bell
28
```

- 만약 타입이 아래와 같이 틀리게 된다면 if문 안의 값이 실행되지 않을 것이다.

```dart
void main() {
  ifMatcher();
}

void ifMatcher() {
  final person = {
    'name': 'bell',
    'age': 28,
  };

  if (person case {'name': int name, 'age': int age}) {
    print(name);
    print(age);
  }
}
```

<br>

## 새롭게 추가된 클래스 키워드

### final class

- `final`로 클래스를 선언하면!
- `extends`, `implements`, `mixin`으로 사용이 불가능하다.

```dart
final class Person {
  final String name;
  final int age;

  Person({
    required this.name,
    required this.age,
  });
}
```

### base class

- `base`로 선언하면 `extends`는 가능하지만 `implements`는 불가능하다!

```dart
base class Person {
  final String name;
  final int age;

  Person({
    required this.name,
    required this.age,
  });
}
```

- 또한 `base`로 선언된 클래스는 `final`, `base`, `sealed`로 선언해야 `extends`할 수 있다는 특성이 있다.

```dart
final class Actor extends Person {
  Actor({
    required super.name,
    required super.age,
  });
}
```

### interface

- `implements`만 할 수 있는 클래스가 된다.

```dart
interface class Person {
  final String name;
  final int age;

  Person({
    required this.name,
    required this.age,
  });
}
```

### sealed

- `sealed`는 `abstract`이면서 `final`이다.
- 패턴 매칭을 사용할 수 있게 해준다.

```dart
void main() {
  print(whoIs(Actor()));
  print(whoIs(Engineer()));
  print(whoIs(Chef()));
}

sealed class Person {}

class Actor extends Person {}

class Engineer extends Person {}

class Chef extends Person {}

String whoIs(Person person) => switch (person) {
      Actor a => '배우',
      Engineer e => '엔지니어',
      _ => 'no match',
    };
```

```shell
배우
엔지니어
no match
```

### mixin

- `mixin` 클래스는 `extends`나 `with`을 사용할 수 없다.
- 클래스는 on 키워드를 사용할 수 없다. 따라서 mixin class에서도 on을 사용할 수 없다.

```dart
void main() {
  Dog dog = Dog();
  print(dog.bark());
}

mixin class AnimalMixin {
  String bark() {
    return '멍멍';
  }
}

class Dog with AnimalMixin {}
```
