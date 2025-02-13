# useMutation

- React-Query를 사용해 서버에 변경(insert, update, delete) 작업 요청 시 사용

## mutationFn

- promise 처리가 이뤄지는 mutation function
- axios를 이용해 서버에 API를 요청하는 부분

```javascript
const get = useMutation({
  mutationFn: (id) => axios.get(`/api/members/${id}`)
})
```

## mutate

- useMutation을 이용해 작성한 내용들이 실제로 실행될 수 있도록 돕는 trigger 역할
- useMutation을 정의해준 후, 이벤트 발생 시 사용하는 것

## 이벤트 처리

- `onSuccess`: 성공 시 실행 
- `onError` : 에러 발생 시 실행
- `onSettled`: 성공/에러 상관 없이 실행

```javascript
const get = useMutation({
  mutationFn: (id) => axios.get(`/api/members/${id}`),
  onSuccess: () => { console.log('성공') },
  onError: () => { console.log('에러') },
  onSettled: () => { console.log('무조건 실행') },
})
```

## 낙관적 업데이트 optimistic update

- 성공 시 UI를 보여준 후, 요청의 결과가 오면 성공/실패 여부에 따라 UI 업데이트

