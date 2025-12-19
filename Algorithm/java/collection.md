# Collection

## 메소드

### 최대 최소

```java
Collections.max(list);
Collections.min(list);
```

### 정렬

```java
Collections.sort(list);
Collections.sort(list, Collections.reverseOrder());
```

- 뒤집기

```java
Collections.reverse(list);
```

### 이진 탐색

- 전제 조건: 오름차순으로 먼저 정렬을 해줘야 한다.

```java
Collections.sort(list);
Collections.binarySearch(list, 10); // 10을 찾음
```

