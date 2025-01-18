# Dart 비동기 프로그래밍

## 동기 방식

```dart
void main() {
  addNumbers(1, 1);
  addNumbers(2, 2);
}

void addNumbers(int number1, int number2) {
  print('계산중: $number1 + $number2');

  print('계산 완료: ${number1 + number2}');
}
```

- 위 코드는 동기 방식으로 실행되며, 함수는 순서대로 실행될 것이다.

```shell
계산중: 1 + 1
계산 완료: 2
계산중: 2 + 2
계산 완료: 4
```

## Future

- `Future`는 미래에 받아올 값을 저장할 수 있다.

```dart
void main() {
  Future<String> name = Future.value('코드팩토리');
  Future<int> number = Future.value(1);
  Future<bool> isTrue = Future.value(true);
}
```

### delayed 함수

- Future에는 delayed 함수가 있다.
  - 2개의 파라미터를 전달 받는다.
  - 1번 파라미터: 지연할 기간. 얼마나 지연할 것인지. `Duration`
  - 2번 파라미터: 지연 시간이 지난 후 실행할 함수.

```dart
void main() {
  print('함수 시작');
  Future.delayed(Duration(seconds: 2), () {
    print('Delay end');
  });
  print('끝?');
}
```

- delayed 함수에 의해 `print('Delay end');`가 지연되기 때문에 `print('끝?');`이 먼저 실행된다.

```shell
함수 시작
끝?
Delay end
```

## async/await

- 비동기로 실행되는 요소를 기다리려면 `await`를 걸어야 한다.
  - `await`를 사용하고 싶다면 함수에 `async`를 추가해줘야 한다.

```dart
void main() {
  addNumbers(1, 1);
  addNumbers(2, 2);
}

void addNumbers(int number1, int number2) async {
  print('계산 시작: $number1 + $number2');

  // 서버 시뮬레이션
  await Future.delayed(Duration(seconds: 2), () {
    print('계산 완료: $number1 + $number2 = ${number1 + number2}');
  });

  print('함수 완료?');
}
```

```shell
계산 시작: 1 + 1
계산 시작: 2 + 2
계산 완료: 1 + 1 = 2
함수 완료?
계산 완료: 2 + 2 = 4
함수 완료?
```

- 만약 `main`에서도 순서대로 실행하고 싶다면 아래와 같이 `main`에도 `async`를 걸어주고 `await` 키워드를 사용하면 된다.

```dart
void main() async {
  await addNumbers(1, 1);
  await addNumbers(2, 2);
}
```

- 그리고 함수는 `Future<T>`로 감싸주면 된다.

```dart
Future<void> addNumbers(int number1, int number2) async {
  print('계산 시작: $number1 + $number2');

  // 서버 시뮬레이션
  await Future.delayed(Duration(seconds: 2), () {
    print('계산 완료: $number1 + $number2 = ${number1 + number2}');
  });

  print('함수 완료?');
}
```

- 이제 아래와 같이 순서대로 실행될 것이다.

```shell
계산 시작: 1 + 1
계산 완료: 1 + 1 = 2
함수 완료?
계산 시작: 2 + 2
계산 완료: 2 + 2 = 4
함수 완료?
```

## Stream

- 비동기 프로그래밍에서 사용하는 개념
- 최근 `Future`보다는 `Stream`을 사용하는 움직을 보이고 있다고 한다.
- `Stream`은 `yield`를 통해 중간중간 값을 받아올 수 있다는 특징이 있다.

### package 불러오기

- 기본 제공 기능이 아니기 때문에 import 문이 필요하다.

```dart
import 'dart:async';

void main() {
  final controller = StreamController();
  final stream = controller.stream;
}
```

### 기본 사용법

```dart
import 'dart:async';

void main() {
  final controller = StreamController();
  final stream = controller.stream;

  final streamListener1 = stream.listen((val) {
    print('Listener 1: $val');
  });

  controller.sink.add(1);
  controller.sink.add(2);
  controller.sink.add(3);
  controller.sink.add(4);
  controller.sink.add(5);
}
```

```shell
Listener 1: 1
Listener 1: 2
Listener 1: 3
Listener 1: 4
Listener 1: 5
```

### 리스너가 여러 개인 경우

- 리스너를 추가해주기만 하면 여러 개를 사용할 수 있을까?
- 아니다. 리스너를 추가하기만 하면 아래 에러를 만나게 될 것이다.

```shell
DartPad caught unhandled StateError:
Bad state: Stream has already been listened to.
```

- 리스너를 여러 개 사용하고 싶다면 아래와 같이 브로드캐스트 `Stream`을 선언해줘야 한다.

```dart
final stream = controller.stream.asBroadcastStream();
```

- 예시를 보자

```dart
import 'dart:async';

void main() {
  final controller = StreamController();
  final stream = controller.stream.asBroadcastStream();

  final streamListener1 = stream.listen((val) {
    print('Listener 1: $val');
  });

  final streamListener2 = stream.listen((val) {
    print('Listener 2: $val');
  });

  controller.sink.add(1);
  controller.sink.add(2);
  controller.sink.add(3);
  controller.sink.add(4);
  controller.sink.add(5);
}
```

```shell
Listener 1: 1
Listener 2: 1
Listener 1: 2
Listener 2: 2
Listener 1: 3
Listener 2: 3
Listener 1: 4
Listener 2: 4
Listener 1: 5
Listener 2: 5
```

### 고차함수와의 연계

- stream은 고차함수와 같이 사용할 수 있다.

```dart
import 'dart:async';

void main() {
  final controller = StreamController();
  final stream = controller.stream;

  final streamListener1 = stream.where((x) => x % 2 == 0).listen((val) {
    print('Listener 1: $val');
  });

  controller.sink.add(1);
  controller.sink.add(2);
  controller.sink.add(3);
  controller.sink.add(4);
  controller.sink.add(5);
}
```

```shell
Listener 1: 2
Listener 1: 4
```

### yield 사용하기

```dart
import 'dart:async';

void main() {
  calculate(1).listen((val) {
    print('calculate(1): $val');
  });
}

Stream<int> calculate(int number) async* {
  for (int i = 0; i < 5; i++) {
    yield i * number;
  }
}
```

```shell
calculate(1): 0
calculate(1): 1
calculate(1): 2
calculate(1): 3
calculate(1): 4
```

### yield*

- 기본적으로 `Stream` 또한 비동기로 실행된다.
- 만약 `Stream`을 `await`하게 만들고 싶다면! `yield*`을 사용하면 된다.

```dart
import 'dart:async';

void main() {
  playAllStream().listen((val) {
    print('playAllStream(): $val');
  });
}

Stream<int> calculate(int number) async* {
  for (int i = 0; i < 5; i++) {
    yield i * number;

    await Future.delayed(Duration(seconds: 1));
  }
}

Stream<int> playAllStream() async* {
  yield* calculate(1);
  yield* calculate(5);
}
```

```shell
playAllStream(): 0
playAllStream(): 1
playAllStream(): 2
playAllStream(): 3
playAllStream(): 4
playAllStream(): 0
playAllStream(): 5
playAllStream(): 10
playAllStream(): 15
playAllStream(): 20
```
