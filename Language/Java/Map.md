# Map

## ChatGPT를 통해 알아보자!

- `SortedMap<String, String> map = new TreeMap<>();` 과 `Map<String, String> map = new TreeMap<>();`의 차이

```
SortedMap<String, String> map = new TreeMap<>();은 SortedMap 인터페이스의 객체를 생성하는 것이고, Map<String, String> map = new TreeMap<>();은 Map 인터페이스의 객체를 생성하는 것입니다.

둘 다 TreeMap 클래스의 객체를 생성하지만, SortedMap은 Map 인터페이스를 확장한 것이며, 추가적인 메소드와 기능을 가지고 있습니다. 예를 들어, SortedMap은 범위 검색(range search)과 정렬된 맵에서 작업하는 다른 메소드를 제공합니다.

따라서, SortedMap을 사용하면 추가적인 메소드와 기능을 사용할 수 있지만, Map을 사용하면 일반적인 맵에서 사용할 수 있는 메소드만 사용할 수 있습니다.
```

- `Map<String, String> map = HashMap<>();`과 `HashMap<String, String> map = HashMap<>();`의 차이

```
Map<String, String> map = HashMap<>();과 HashMap<String, String> map = HashMap<>();은 두 가지 다른 것을 생성하는 것입니다.

Map<String, String> map = new HashMap<>();은 HashMap 클래스의 객체를 생성하고, 이 객체를 Map 인터페이스의 변수에 할당하는 것입니다. 이렇게 하면 변수 map은 HashMap 클래스에서 상속한 메소드와 Map 인터페이스에서 정의한 메소드를 모두 사용할 수 있습니다. 이는 다형성(polymorphism)의 한 예입니다. 변수 map은 Map 타입으로 선언되어 있지만, 실제로는 HashMap 클래스의 객체를 참조하고 있습니다.

반면, HashMap<String, String> map = new HashMap<>();은 HashMap 클래스의 객체를 생성하고, 이 객체를 HashMap 타입의 변수에 할당하는 것입니다. 이렇게 하면 변수 map은 HashMap 클래스에서 상속한 메소드만 사용할 수 있습니다. 이 경우에는 다형성이 적용되지 않습니다.

따라서, Map<String, String> map = new HashMap<>();은 인터페이스를 활용한 다형성을 사용할 수 있습니다. 이를테면, HashMap 대신 다른 클래스(예: TreeMap)를 사용하여 Map 인터페이스를 구현하도록 변경할 수 있습니다. 이렇게 하면 코드의 유연성과 확장성이 높아집니다. 반면에 HashMap<String, String> map = new HashMap<>();은 변수 타입이 HashMap으로 고정되어 있기 때문에 이점이 없습니다.
```

## 참고자료

- ChatGPT
