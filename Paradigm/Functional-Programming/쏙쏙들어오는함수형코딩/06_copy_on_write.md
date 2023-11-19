# 06 Copy on Write

- 데이터가 바뀌지 않도록 하기 위해 카피-온-라이트를 적용합니다.
- 배열과 객체를 데이터에 쓸 수 있는 카피-온-라이트 동작을 만듭니다.
- 깊이 중첩된 데이터도 카피온 라이트가 잘 동작하게 만듭니다.

## 모든 동작을 불변형으로 만들 수 있는가?

- 중첩된 데이터에 대한 불변 동작을 구현할 수 있는가?

## 동작을 읽기, 쓰기 또는 둘 다로 분류하기

- 읽기 = 데이터를 바꾸지 않고 정보를 꺼내는 것입니다.
- 쓰기 = 어떻게든 데이트를 바꿉니다.
- 자바스크립트는 기본적으로 변경 가능한 데이터 구조를 사용하기 때문에 불변성 원칙을 적용하기 위해 직접 구현이 필요하다.

## 카피-온-라이트 원칙 세 단계

1. 복사본 만들기
2. 원하는만큼 복사본 변경하기
3. 복사본 리턴하기

```jsx
function addElementLast(array, elem) {
	const newArray = array.slice();
	newArray.push(elem);
	return newArray;
}
```

- 위 함수는 배열을 복사했고, 기존 배열을 변경하지 않았다.
- 복사본은 함수 범위에 있기 대문에 다른 코드에서 값을 바꾸기 위해 접근할 수 없다.
- 복사본을 변경하고 나서 함수를 나간다.
- 이 함수는 읽기일까 쓰기일까? 데이터를 바꾸지 않고 정보를 리턴했기 때문에 **읽기**이다.

## 카피-온-라이트로 쓰기를 읽기로 바꾸기

- 아래 코드는 쓰기이다. splice가 장바구니의 항목을 삭제해버리기 때문이다.

```jsx
function removeItemByName(cart, name) {
  let idx = null;
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      idx = i;
    }
  }
  if (idx !== null) {
    cart.splice(idx, 1); // cart.splice로 장바구니가 변경된다.
  }
}
```

- 하지만 장바구니가 바뀌지 않았으면 한다면? 장바구니를 불변 데이터로 써보자.
- 카피온라이트가 적용해본 함수는 아래와 같이 변경할 수 있다.

```jsx
function removeItemByName(cart, name) {
  const newCart = cart.slice(); // 1. 복사본 만들기
  let idx = null;
  for (let i = 0; i < newCart.length; i++) {
    if (newCart[i].name === name) {
      idx = i;
    }
  }
  if (idx !== null) {
    newCart.splice(idx, 1); // 2. 복사본 변경하기
  }
  return newCart; // 3. 복사본 리턴하기
}
```

## 일반적인 동작 빼내기

- splice의 동작을 일반화해보자!

```jsx
function removeItems(array, idx, count) {
  const copy = array.slice();
  copy.splice(idx, count);
  return copy;
}
```

- 일반화된 함수 동작을 적용해 아래와 같이 변경할 수 있다.

```jsx
function removeItemByName(cart, name) {
  let idx = null;
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      idx = i;
    }
  }
  if (idx !== null) {
    removeItems(cart, idx, 1);
  }
  return cart; // 값이 변경되지 않으면 그냥 cart를 반환하면 된다.
}
```

## 자바스크립트 배열?

- 자바스크립트에서 배열은 기본 컬렉션이고, 순서가 있는 값을 나타낸다.
- 자바나 C와는 다르게 크기를 늘리거나 줄일 수 있다.

# 연습문제1

- 아래 함수를 카피온라이트 방식으로 변경해보자!
    1. addContact가 전역변수에 접근하면 안 된다. mailingList를 인자로 받아 복사하고 변경한 다음 리턴!
    2. addContact 함수의 리턴값을 mailingList 전역변수에 할당해야 합니다.

```jsx
const mailingList = [];

function addContact(email) {
  mailingList.push(email);
}

function submitFormHandler(event) {
  const form = event.target;
  const email = form.elements["email"].value;
  addContact(email);
}
```

- 아래는 변경 후!

```jsx
let mailingList = [];

function addContact(mailingList, email) {
  const list = mailingList.slice();
  list.push(email);
  return list;
}

function submitFormHandler(event) {
  const form = event.target;
  const email = form.elements["email"].value;
  mailingList = addContact(mailingList, email);
}
```

## 쓰기를 하면서 읽기도 하는 동작

- 어떤 동작은 읽고 변경하는 일을 동시에 한다. 자바스크립트 배열의 `shift`가 그 예 중 하나
- `shift` 메소드는 배열의 값을 바꿈과 동시에 첫 번째 항목을 리턴한다.
- 이 동작을 카피온라이트로 바꾸려면 아래와 같은 방법을 사용할 수 있다.
    1. 읽기와 쓰기 함수로 각각 분리한다.
    2. 함수에서 값을 두 개 리턴한다.

### 1. 읽기와 쓰기 함수로 각각 분리

- 읽기 동작은 값을 단순히 리턴하는 동작이다.
- shift 메소드가 리턴하는 값은 배열에 첫 번째 항목이다. 따라서 배열의 첫 번째 항목을 리턴하는 계산 함수를 만들면 된다.

```jsx
// 읽기와 쓰기를 분리
// 읽기는 굳이 카피온라이트를 적용할 필요가 없다.
function firstElement(array) {
  return array[0];
}

// 카피온라이트 적용
function dropFirst(array) {
  const copy = array.slice();
  copy.shift();
  return copy;
}
```

### 2. 값 2개를 리턴

```jsx
// 값을 두 개 리턴하는 함수
function shift(array) {
  const copy = array.slice();
  const first = copy.shift();
  return {
    first,
    array: copy
  };
}
```

### 3. 두 가지 방법을 조합하는 방법

```jsx
// 위 두 가지 방법을 조합하는 방법
function shift(array) {
  return {
    first: firstElement(array),
    array: dropFirst(array),
  };
}
```

# 연습문제2

- pop을 카피온라이트 버전으로 변경해보자!

```jsx
// 1. 읽기와 쓰기로 분리
function lastElement(array) {
  return array[array.length - 1];
}
function dropLast(array) {
  const copy = array.slice();
  copy.pop();
  return copy;
}

// 2. 값 2개를 리턴
function pop(array) {
  const copy = array.slice();
  const last = copy.pop();
  return {
    last,
    array: copy,
  };
}
```

# 연습문제3

```jsx
function push(array, elem) {
  const copy = array.slice();
  copy.push(elem)
  return copy;
}
```

# 연습문제4

```jsx
function push(array, elem) {
  const copy = array.slice();
  copy.push(elem)
  return copy;
}

function addContact(mailingList, email) {
  return push(mailingList, email);
}
```

# 연습문제5

```jsx
function arraySet(array, idx, value) {
  const copy = array.slice();
  copy[idx] = value;
  return copy;
}
```

## 불변 데이터 구조를 읽는 것은 계산이다.

- 변경 가능한 데이터를 읽는 것은 **액션** ↔ 불변 데이터 구조를 읽는 것은 **계산**
- 쓰기는 데이터를 변경 가능한 구조로 만든다.
- 어떤 데이터에 쓰기가 없다면 데이터는 변경 불가능한 데이터입니다.
- 쓰기를 읽기로 바꾸면 코드에 계산이 많아집니다.

## 애플리케이션에는 시간에 따라 변하는 상태가 있다.

- 당연히 변경 가능한 데이터는 필요하다.
- 변경이 필요할 때 요소를 바꾸는 것이 아니라 새로운 값으로 교체해버린다.

## 불변 데이터 구조는 충분히 빠르다.

- 불변 데이터 구조는 변경 가능한 데이터 구조보다 메모리를 더 많이 쓰고 느리다.
- 하지만 아래의 이유로 충분히 효율적이다.
    - 하지만 언제든 최적화 할 수 있다는 점
    - 가비지 콜렉터가 충분히 빠르다는 점
    - 생각보다 많이 복사가 일어나지 않는다는 점
    - 함수형 프로그래밍 언어에서 불변 데이터 구조를 지원하고 더 효율적인 구현체를 지원한다는 점

## 객체에 대한 카피온라이트

- 배열에 대한 카피온라이트와 똑같은 단계를 거치면 된다.
    1. 복사본 만들기
    2. 복사본 변경하기
    3. 복사본 리턴하기
- 자바스크립트에서는 Object.assign() 스태틱 메소드를 통해 객체를 복사할 수 있다.

```jsx
const object = {a: 1, b: 2};
const copy = Object.assign({}, object);
```

### 자바스크립트 객체

- 해시 맵과 연관 배열과 비슷하다.
- 키/값 쌍

# 연습문제6

```jsx
function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}
```

# 연습문제7

```jsx
function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}

function setPrice(item, newPrice) {
  return objectSet(item, "price", newPrice);
}
```

# 연습문제8

```jsx
function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}

function setQuantity(item, newQuantity) {
  return objectSet(item, "quantity", newQuantity);
}
```

# 연습문제9

```jsx
function objectDelete(object, key) {
  const copy = Object.assign({}, object);
  delete copy[key];
  return copy;
}
```

## 중첩된 쓰기를 읽기로 바꾸기

- 카피온라이트 버전의 setPrice()

```jsx
function setPriceByName(cart, name, price) {
  const cartCopy = cart.slice();
  for (let i = 0; i < cartCopy.length; i++) {
    if (cartCopy[i].name === name) {
      cartCopy[i] = setPrice(cartCopy[i], price);
    }
  }
  return cartCopy;
}
```

## 복사본 파악

- 배열 하나와 객체 세 개가 있다. 이 중에 복사되는 것은 배열 하나(장바구니)와 객체 하나(아이템)이다.

```jsx
function setPriceByName(cart, name, price) {
  const cartCopy = cart.slice(); // 배열 복사
  for (let i = 0; i < cartCopy.length; i++) {
    if (cartCopy[i].name === name) {
      cartCopy[i] = setPrice(cartCopy[i], price);
    }
  }
  return cartCopy;
}

function setPrice(item, newPrice) {
  return objectSet(item, "price", newPrice);
}

function objectSet(object, key, value) {
  const copy = Object.assign({}, object); // 객체 복사
  copy[key] = value;
  return copy;
}
```

- 나머지 2개의 객체를 복사하지 않은 이유는 무엇일까? 중첩된 데이터에 얕은 복사를 했기 때문이다.
- 결과적으로 구조적 공유가 되었다.

# 연습문제10

- 복사되는 데이터가 무엇일까?
- 정답
    - 객체 주소값을 가지고 있는 배열
    - name을 “socks”로 가지고 있는 객체

# 연습문제11

```jsx
function setQuantityByName(cart, name, quantity) {
  const cartCopy = cart.slice();
  for (let i = 0; i < cartCopy.length; i++) {
    if (cartCopy[i].name === name) {
      cartCopy[i].quantity = setQuantity(cartCopy[i], quantity);
    }
  }
  return cartCopy;
}

function setQuantity(item, newQuantity) {
  return objectSet(item, "quantity", newQuantity);
}

function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}
```