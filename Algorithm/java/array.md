# 배열

## 변환

### Array to List

- 불변 리스트 (추가는 불가, but 요소 변경은 가능)

```java
List<String> list = Arrays.asList(arr);
```

- 가변 리스트

```java
List<String> list = new ArrayList<>(Arrays.asList(arr));
```

### List to Array

- `List` 메소드

```java
list.toArray(new String[list.size()]);
```

- stream

```java
int[] array = list.stream().mapToInt(x -> x).toArray();
```
