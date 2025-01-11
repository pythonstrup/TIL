# OOP

- 다트도 객체지향 프로그래밍을 지원하는 언어다.

```dart
void main() {
  Package package = Package();
  print(package.name);
  print(package.items);
  package.hello();
}

class Package {
  final String name = '책';
  final List<String> items = ['DDD', 'Spring'];

  void hello() {
    print('안녕하세요. 책 패키지 입니다.');
  }
}
```

## 생성자

- 다른 언어의 생성자와는 약간 다른 형태를 띈다.

```dart
class Package {
  final String name;
  final List<String> items;

  Package(String name, List<String> items)
      : this.name = name,
        this.items = items;
}
```

- 아래와 같이 간단하게 생성자를 만드는 방법도 있따.

```dart
class Package {
  final String name;
  final List<String> items;

  Package(this.name, this.items);
}
```

- 위 클래스의 인스턴스를 생성하려면 아래와 같이 생성자를 호출하면 된다.

```dart
Package package = Package('책', ['DDD', 'Spring']);
```

## this

- 다른 객체지향 언어와 마찬가지로 `this`를 호출해 사용할 수 있다. 

```dart
class Package {
  final String name;
  final List<String> items;

  Package(String name, List<String> items)
      : this.name = name,
        this.items = items;

  void hello() {
    print('안녕하세요. ${this.name} 패키지 입니다.');
  }
}
```

### const Constructor

- 빌드 타임에 값을 알 수 있는 경우에만 사용해야 한다.

```dart
class Package {
  final String name;
  final List<String> items;

  const Package(this.name, this.items);
}
```

- 이렇게 선언된 생성자는 아래와 같이 `const`로 인스턴스를 생성할 수 있다.
  - 기본 생성자에서는 `const`로 인스턴스를 생성할 수 없다. 
  - 이 부분은 추후에 `Flutter`에서의 효율을 올려주는 부분! 중요!

```dart
Package package = const Package('책', ['DDD', 'Spring']);
```

### const로 선언했을 때의 동일성

- `const`로 선언했을 때 값이 같으면 두 인스턴스를 동일하다!
  - **`const`로 생성하면 같은 주소를 바라보게 한다는 것을 유추할 수 있다.**

```dart
Package package1 = const Package('책', ['DDD', 'Spring']);
Package package2 = const Package('책', ['DDD', 'Spring']);
print(package1 == package2);
```

```shell
true
```

- 하지만 만약 `const`로 인스턴스를 만들지 않았다면 `false`라고 뜰 것이다.

```dart
Package package1 = Package('책', ['DDD', 'Spring']);
Package package2 = Package('책', ['DDD', 'Spring']);
print(package1 == package2);
```

```shell
false
```

## getter & setter

- dart의 setter는 파라미터를 하나만 받을 수 있다는 점에 유의하자!

```dart
void main() {
  Package package = Package('책', ['DDD', 'Spring']);
  print(package.firstItem);

  package.firstItem = 'JPA';
  print(package.firstItem);
}

class Package {
  String name;
  List<String> items;

  Package(this.name, this.items);

  void hello() {
    print('안녕하세요. ${this.name} 패키지 입니다.');
  }

  String get firstItem {
    return this.items[0];
  }

  set firstItem(String name) {
    this.items[0] = name;
  }
}
```

```shell
DDD
JPA
```

## 접근 제한자

- 따로 접근 제한자 키워드를 사용하지 않는다. `private`이라면 앞에 `_`를 붙여서 사용한다.

```dart
class Package {
  final String _name;
}
```

- 이는 함수에서도 적용된다.

```dart
class Package {

  void _hello() {
    print('안녕하세요. ${this.name} 패키지 입니다.');
  }
}
```

## 상속

- 타입의 포함관계는 다른 OOP 언어와 동일하다.

```dart
class Promotion extends Package {
  Promotion(String name, List<String> items) : super(name, items);
}
```

## 메소드 오버라이딩

- 자바와 동일하다.

```dart
void main() {
  Parent parent = Parent();
  Parent child = Child();
  parent.hello();
  child.hello();
}

class Parent {
  void hello() {
    print('안녕하세요. 부모입니다.');
  }
}

class Child extends Parent {
  @override
  void hello() {
    print('안녕하세요. 자식입니다.');
  }
}
```

```shell
안녕하세요. 부모입니다.
안녕하세요. 자식입니다.
```

## static

- `static`이란 인스턴스가 아닌 클래스에 귀속되는 것이다.
- 다른 언어와 동일하게 동작

```dart
void main() {
  Item.codeNumber = 'abcdef';
  print(Item.codeNumber);
}

class Item {
  static String? codeNumber;
}
```

## 인터페이스

- Dart에는 `interface`라는 키워드가 존재하지 않는다.
- 대신 `class`를 `interface`처럼 사용하는 방식을 사용한다.
- `implements`한 클래스의 속성을 구현하지 않으면 에러가 뜬다.

```dart
class ItemInterface {
  void hello() {}
}

class FoodItem implements ItemInterface {
  void hello() {
    print('hello');
  }
}
```

- 신기하게도 인터페이스 역할을 하는 클래스에 필드가 선언되어 있다면, 그조차도 똑같이 구현해야 한다.

```dart
class ItemInterface {
  String name;

  ItemInterface(this.name);

  void hello() {}
}

class FoodItem implements ItemInterface {
  @override
  String name;
  
  FoodItem(this.name);

  @override
  void hello() {
    print('hello');
  }
}
```

## abstract class

- 다른 oop 언어의 추상클래스와 동일하다.

## generic

```dart
void main() {
  Item listItem = Item(['a', 'b']);
  Item intItem = Item(123);
}

class Item<T> {
  final T id;

  Item(this.id);
}
```
