# 자주 사용하는 Map 연산

### 가장 높은 Value값을 가진 Key 찾기

```java
class Main {
  
  public int sum(Map<Integer, Integer> sums) {
    return sums.entrySet()
        .stream()
        .max(Map.Entry.comparingByValue())
        .map(Map.Entry::getKey)
        .orElse(null);
  }
}
```

### 값이 없으면 삽입, 값이 있으면 꺼내서 더하기

```java
class Main {
  
  public void put(Map<Integer, Integer> sums, int key, int value) {
    sums.put(key, sums.getOrDefault(key, 0) + value);
  }
}
```

