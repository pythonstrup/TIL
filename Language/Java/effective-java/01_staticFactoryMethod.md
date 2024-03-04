## 정적 팩토리 메소드에서 주로 사용하는 명명 방식들

### from

- 매개변수를 하나 받아서 해당 타입의 인스턴스를 반환하는 형변환 메소드

```java
Data date = Data.from(instance);
```

### of

- 여러 매개변수를 받아 적합한 타입의 인스턴스를 반환하는 집계 메소드

```java
Set<Rank> faceCards = EnumSet.of(JACK, QUEEN, KING);
```

### valueOf

- `from`과 `of`의 더 자세한 버전

```java
BigInteger prime = BigInteger.valueOf(Integer.MAX_VALUE);
```

### instance 또는 getInstance

- 매개변수로 명시한 인스턴스를 반환하지만, 같은 인스턴스임을 보장하지는 않는다.
- 매개변수를 받지 않을 수도 있다.

```java
MyObject = MyObject.getInstance(options);
```

### create 또는 newInstance

- `instance`, `getInstance`와 비슷하지만, 매번 새로운 인스턴스를 생성해 반환하는 것을 보장한다.

```java
Object newArray = Array.newInstance(classObject, arrayLen);
```

### getType

- `getInstance`와 같으나 생성할 클래스가 아닌 다른 클래스에 팩토리 메소드를 정의할 때 쓴다.
- `Type`은 팩토리 메소드가 반환할 객체의 타입이다.

```java
FileStore fs = Files.getFileStore(path);
```

### newType

- `newInstance`와 같으나 생성할 클래스가 아닌 다른 클래스에 팩토리 메소드를 정의할 때 쓴다.
- 위와 마찬가지로 `Type`은 팩토리 메소드가 반환할 객체의 타입이다.

```java
BufferedReader br = Files.newBufferedReader(path);
```

### type

- `getType`, `newType`의 간결한 버전

```java
List<MyObject> newList = Collections.list(list);
```