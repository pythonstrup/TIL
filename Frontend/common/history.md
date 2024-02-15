# History

- 현재 브라우저 세션의 기록을 조작할 때 사용한다.

## History State

```javascript
console.log(window.history)
```

- state 요소는 아래와 같이 구성되어 있다.

```javascript
{
  state: {
    back: null
    current: "/"
    forward: null
    position: 47
    replaced: true
    scroll: false
  }
}
```

- `back`은 이전 페이지 표시
- `current`는 현재 페이지 표시


## History Prototype Method

- `window.history.back()`
  - 사이트 뒤로 가기 버튼을 눌렀을 때 동작하는 메소드 
  - 이전 path로 이동

- `window.history.forward()`
  - 사이트 앞으로 가기 버튼을 눌렀을 때 동작하는 메소드.
  - forward path로 이동

- `window.history.go()`
  - 숫자를 변수로 넘겨 그만큼 이동
  - `-1`을 입력하면 뒤로 1번 간다.
  - `2`를 입력하면 앞으로 2번 간다.

# 참고자료

- [MDN](https://developer.mozilla.org/en-US/docs/Web/API/Window/history)