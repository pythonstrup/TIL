# $nextTick()

- Javascript는 비동기로 처리되는 특성을 갖고있다. 떄문에 Data가 업데이트 되고 난 직후 UI가 갱신될 때 Vue가 DOM을 찾지 못하는 경우가 발생할수있다.
- 다음 DOM 업데이트 사이클 이후 실행하는 콜백을 연기한다. DOM 업데이트를 기다리기 위해 일부 데이터를 변경한 직후 사용해야 한다.
- nextTick으로 감싼뒤 callback을 통해 DOM을 조작하게 되면 Vue.js에서 데이터갱신 후 UI까지 완료한 뒤에 nextTick에 있는 함수를 최종적으로 수행하게 된다.

## 사용방법

```javascript
created() {
    this.$nextTick(() => {
        // 콜백으로 UI가 그려진 이후에 수행
    });
}
```

```javascript
methods: {
    async doSomething() {
        await this.$nextTick();
        // UI 완료 후 실행될 함수 ...
    }
}
```

## 참고자료

- [VueJs - $nextTick() 이란?](https://cocobi.tistory.com/52)
