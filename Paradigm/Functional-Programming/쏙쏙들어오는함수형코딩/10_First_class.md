# 일급함수

### 코드의 냄새: 함수 이름에 있는 암묵적 인자

- 코드의 냄새는 일급 값으로 바꾸면 표현력이 더 좋아집니다.
    - 특징1: 거의 똑같이 구현된 함수가 있다.
    - 특징2: 함수 이름이 구현에 있는 다른 부분을 가리킨다.

### 리팩터링: 암묵적 인자를 드러내기

1. 함수 이름에 있는 암묵적 인자를 확인합니다.
2. 명시적인 인자를 추가합니다.
3. 함수 본문에 하드 코딩된 값을 새로운 인자로 바꿉니다.
4. 함수를 호출하는 곳을 고칩니다.

### 리팩터링: 함수 본문을 콜백으로 나누기

1. 함수 본문에서 바꿀 부분의 앞부분과 뒷부분을 확인
2. 리팩터링할 코드를 함수로 빼냅니다.
3. 빼낸 함수의 인자로 넘길 부분을 또 다른 함수로 빼냅니다.

# 코드의 냄새: 함수 이름에 있는 암묵적 인자

- 코드의 중복
- 함수 이름에 있는 암묵적 인자(implicit argument in function name) 냄새는 아래 2가지 특성을 보입니다.
    1. 함수 구현이 거의 똑같습니다.
    2. 함수 이름이 구현의 차이를 만듭니다.

# 리팩터링: 암묵적 인자를 드러내기

- 기본적인 아이디어는 암묵적 인자를 명시적인 인자로 바꾸는 것입니다.
- 드러낸다(express)라는 의미가 암묵적인 것을 명시적으로 바꾼다는 것을 말합니다.

```jsx
// 리팩토링 전
function setPriceByName(cart, name, price) {
  const item = cart[name];
  const newItem = objectSet(item, 'price', price);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}
cart = setPriceByName(cart, 'shoe', 13);
cart = setQuantityByName(cart, 'shoe', 3);
cart = setShippingByName(cart, 'shoe', 0);
cart = setTaxByName(cart, 'shoe', 2.34);

// 리팩토링 후
function setFieldByName(cart, name, field, value) {
  const item = cart[name];
  const newItem = objectSet(item, field, value);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}
cart = setFieldByName(cart, 'shoe', 'price', 13);
cart = setFieldByName(cart, 'shoe', 'quantity', 3);
cart = setFieldByName(cart, 'shoe', 'shipping', 0);
cart = setFieldByName(cart, 'shoe', 'tax', 2.34);
```

- 리팩터링으로 비슷한 함수를 모두 일반적인 함수 하나로 바꿨습니다.
- 또한 필드명을 일급 값으로 만들었습니다.
- 필드명을 문자열로 넘기는 것이 안전하지 않다고 느낄 수 있습니다만 뒤에 살펴보는 걸로!

## 일급인 것과 일급이 아닌 것을 구별하기

- 모든 언어는 일급이 아닌 것과 일급인 것이 섞여있다.

> **자바스크립트에서 일급이 아닌 것**
1. 수식 연산자(`+`, `*` 등)
2. 반복문(for문, while문)
3. 조건문(if-else, switch)
4. try/catch 블록
>

> **일급으로 할 수 있는 것**
1. 변수에 할당
2. 함수의 인자로 넘기기
3. 함수의 리턴값으로 받기
4. 배열이나 객체에 담기
>

## 필드명을 문자열로 사용하면 버그가 발생하지 않을까요?

- 문자열에는 오타가 생길 수 있습니다. 이 문제를 해결하기 위한 방법은 2가지입니다.
    - 컴파일 타임에 검사하는 것
    - 런타임에 검사하는 것
- 컴파일 타임에 검사하기 위한 가장 대표적인 방법은 타입스크립트를 활용하는 것입니다.
- 많은 정적 타입 시스템 언어에서 필드명이 올바른지 확인하기 위해 타입 시스템을 사용할 수 있습니다. 예를 들어 자바에서는 Enum 타입을, 하스켈에서는 합 타입으로 표현할 수 있습니다.
- 런타임 검사는 함수를 실행할 때마다 동작합니다. 전달한 문자열이 올바른 문자열인지 확인합니다.
- 자바스크립트는 정적 타입 언어가 아니기 때문에 아래와 같이 런타임 검사를 실시할 수 있습니다.

```jsx
const validItemFields = ['price', 'quantity', 'shipping', 'tax'];

function setFieldByName(cart, name, field, value) {
  if (!validItemFields.includes(field)) {
    throw "Not a valid item field: " + "'" + field + "'";
  }
  const item = cart[name];
  const newItem = objectSet(item, field, value);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}
```

## 일급 필드를 사용하면 API를 바꾸기 더 어렵나요?

- 필드명은 계속 유지해야 합니다. 하지만 구현이 외부에 노출된 것은 아닙니다. 만약 내부에서 정의한 필드명이 바뀐다고 해도 사용하는 사람들이 원래 필드명을 그대로 사용하게 할 수 있습니다. 내부에서 그냥 바꿔주면 됩니다. 아래와 같이.

```jsx
const validItemFields = ['price', 'quantity', 'shipping', 'tax'];
const translations = {'quantity': 'number'};

function setFieldByName(cart, name, field, value) {
  if (!validItemFields.includes(field)) {
    throw "Not a valid item field: " + "'" + field + "'";
  }
  if (translations.hasOwnProperty(field)) {
    field = translations[field];
  }
  const item = cart[name];
  const newItem = objectSet(item, field, value);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}
```

- 이런 방법은 필드명이 일급이기 때문에 할 수 있는 것입니다. 필드명이 일급이라는 말은 객체나 배열에 담을 수 있다는 뜻입니다. 그리고 언어에 모든 기능을 이용해서 필드명을 처리할 수 있습니다.

## 연습문제1

- “암묵적 인자를 드러내기” 리팩토링으로 중복을 없애 봅시다.

```jsx
function multiply(x, y) {
	return x * y;
}
```

## 연습문제2

```jsx
function incrementFieldByName(cart, name, field) {
  const item = cart[item];
  const value = item[field];
  const newValue = value + 1;
  const newItem = objectSet(item, field, newValue);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}
```

## 연습문제3

```jsx
const validFiledList = ['size', 'quantity'];

function incrementFieldByName(cart, name, field) {
  if (!validFiledList.includes(field)) {
    throw  "Not a valid item field" + "'" + field + "'";
  }
  const item = cart[item];
  const value = item[field];
  const newValue = value + 1;
  const newItem = objectSet(item, field, newValue);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}
```

## 객체와 배열을 너무 많이 쓰게 됩니다.

- 데이터가 미래에 어떤 방법으로 해석될 지 미리 알 수 없기 때문에 필요할 때 알맞은 방법으로 해석할 수 있어야 한다.
- 따라서 데이터를 사용할 때 임의의 인터페이스로 감싸서 사용하는 것이 좋다.
- 이것이 데이터 지향(data orientation)이라고 하는 중요한 원칙이다.

> 데이터 지향(data orientation)은 이벤트와 엔티티에 대한 사실을 표현하기 위해 일반 데이터 구조를 사용하는 프로그래밍 형식입니다.
>

## 정책 타입 vs 동적 타입

- 컴파일할 때 타입을 검사하는 언어. 정적 타입 언어(statically typed language)
- 런타임에 타입을 확인하는 언어. 동적 타입 언어(dynamically typed language)
- 사실 양쪽을 사용해서 구성한 소프트웨어 중 어떤 것이 더 훌륭하다고 명확화게 결론내릴 순 없다.
- 정적 타입과 동적 타입에 대한 논쟁에서 언어가 가진 타입 시스템의 차이를 생각하지 않는다는 점을 생각해볼 수 있다. 좋은 정적 타입 시스템을 가진 언어가 있는 반면 나쁜 정적 타입 시스템을 가진 언어도 있다. 좋은 동적 타입 시스템을 가졌지만, 나쁜 정적 타입 시스템을 가진 언어도 있을 것이다.
- 타입 시스템의 형태만 가지고 비교하는 것은 바람직하지 않다. 한 쪽 시스템에 다른 한 쪽 시스템보다 무조건 좋다고 말할 수는 없습니다.

## 모두 문자열로 통신합니다.

- 결국은 문자열을 사용해서 통신한다. API를 주고 받다보면 이상한 문자열을 받는 경우가 무조건 생긴다. ⇒ 결국 정적 타입 언어를 사용하더라도 런타임에서 체크해줘야 한다.
- 데이터는 항상 해석이 필요하다.

## 어떤 문법이든 일급 함수로 바꿀 수 있습니다.

- 일급으로 만들면 강력한 힘이 생긴다는 것을 기억하세요.
- 이 힘을 가지고 많은 문제를 해결할 수 있습니다.

## 연습문제4

```jsx
function multiply(a, b) {
	return a * b;
}

function subtract(a, b) {
	return a - b;
}

function divide(a, b) {
	return a / b;
}
```

# 리팩터링: 함수 본문을 콜백으로 바꾸기

> 자바스크립트에서 인자로 전달하는 함수를 콜백(callback)이라고 부릅니다. 물론 자바스크립트가 아닌 다른 커뮤니티에서도 사용하는 용어입니다. 콜백으로 전달하는 함수는 나중에 호출될 것을 기대합니다. 다른 곳에서 핸들러 함수(handler function)라고도 합니다.
>
- 단계
    1. 본문과 분문의 팡부분과 뒷부분을 구분합니다.
    2. 전체를 함수로 빼냅니다.
    3. 본문 부분을 빼낸 함수의 인자로 전달한 함수로 바꿉니다.
- 콜백으로 빼내기

```jsx
// 원래 코드
function withLogging() {
	try {
		saveUserData(user);
	} carth (error) {
		logToSnapErrors(error);
	}
}

// 콜백으로 빼낸 코드
function withLogging(f) {
	try {
		f()
	} carth (error) {
		logToSnapErrors(error);
	}
}

withLogging(function() {
	saveUserData(user);
});
```

# 결론

- 주요 개념: 일급 값(first-class value), 일급 함수(first-class function), 고차 함수(high-order function)

### 요점 정리

- 일급 값은 변수에 저장할 수 있고 인자로 전달하거나 함수의 리턴값으로 사용할 수 있습니다.
- 일급 함수는 어떤 단계 이상의 함수형 프로그래밍을 하는 데 필요합니다.
- 고차 함수는 다른 함수에 인자로 넘기거나 리턴값으로 받을 수 있는 함수입니다. 고차 함수로 다양한 동작을 추상화할 수 있습니다.
- 함수 이름에 있는 암묵적 인자(implicit argument in function name)는 함수의 이름으로 구분하는 코드의 냄새입니다. 이 냄새는 코드로 다룰 수 없는 함수 이름 대신 일급 값인 인자로 바꾸는 암묵적 인자를 드러내기(express implicit argument) 리팩터링을 적용해서 없앨 수 있습니다.