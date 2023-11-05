# Chapter4. 액션에서 계산 빼내기

- 함수로 정보가 들어가고 나오는 과정
- 테스트하기 쉽고 재사용성이 좋은 코드 만들기
- 액션에서 계산을 빼내는 방법!

## MegaMart.com 장바구니

- 장바구니는 제품 리스트를 나열해 보여줌
- 장바구니는 항상 금액 합계를 보여줌
- ~~var가 좀 불편~~

```jsx
var shopping_cart = []; // 장바구니 제품 전역 변수
var shopping_cart_total = 0; // 금액 합계 전역 변수

function add_item_to_cart(name, price) {
	shopping_cart.push({ // 장바구니에 제품을 담기 위해 cart 배열에 레코드 추가
		name: name,
		price: price,
	});
	calc_cart_total(); // 장바구니 제품이 바뀌었기 때문에 금액 합계를 업데이트
}

function calc_cart_total() {
	shopping_cart_total = 0;
	for (var i = 0; i < shopping_cart.length; i++) {
		var item = shopping_cart[i];
		shopping_cart_total += item.price; // 모든 제품값 더하기
	}
	set_cart_total_dom(); // 금액 합계를 반영하기 위한 DOM 업데이트
}
```

## 새로운 요구 사항

- 20달러가 넘는 제품의 구매 버튼 옆에 무료 배송 아이콘 추가!!
- 세금 계산 추가!

### 절차적인 방식 적용

- 반복문과 IF 문을 사용해 로직을 작성하는 방법

```jsx
function update_shopping_icons() {
  var buy_buttons = get_but_buttons_dom(); // 페이지에 렌더링된 모든 구매 버튼을 가져와 반복문을 돌린다.
  for (var i = 0; i < buy_buttons.length; i++) {
    var button = buy_buttons[i];
    var item = button.item;
    if (item.price + shopping_cart_total >= 20) { // 무료 배송이 가능한지 확인하고 결정에 따라 무료 배송 아이콘을 보여주거나 보여주지 않음
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}
```

- 그 다음은 세금 계산 로직 ⇒ 해당 로직은 간단

```jsx
function update_tax_dom() {
  set_tax_dom(shopping_cart_total * 0.10); // 금액 합계에 10%를 계산해 DOM 업데이트 함수 호출
}
```

- 그리고 위에서 작성한 함수를 다음과 같이 변경해준다.

```jsx
function calc_cart_total() {
	shopping_cart_total = 0;
	for (var i = 0; i < shopping_cart.length; i++) {
		var item = shopping_cart[i];
		shopping_cart_total += item.price; // 모든 제품값 더하기
	}
	set_cart_total_dom(); // 금액 합계를 반영하기 위한 DOM 업데이트
	update_shopping_icons(); // 아이콘 업데이트 코드
	update_tax_dom(); // 세금 업데이트 코드
}
```

### 여기서 문제가 발생!

- 코드가 바뀔 때마다 아래와 같은 테스트를 진행해야 함..
1. 브라우저 설정하기
2. 페이지 로드하기
3. 장바구니에 제품 담기 버튼 클릭
4. DOM이 업데이트될 때까지 기다리기
5. DOM에서 값 가져오기
6. 가져온 문자열 값을 숫자로 바꾸기
7. 예상하는 값과 비교하기

### 여기서 테스트를 개선하려면 어떻게 해야할까?

- DOM을 업데이트하는 로직과 비즈니스를 분리하면!!
    - DOM 로직이 바뀌면 DOM만 테스트!
    - 비즈니스 로직이 바뀌면 비즈니스 로직 쪽만 테스트하면 된다!
- 전역변수를 제거해야 한다!
- MegeMart는 커버리지 100% 못하면 집을 못가요… ~~⇒ 나는 회사에서 살아야겠군..~~

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/f98a56bc-d567-45c8-aa71-fa8091ff2da7/625d562a-a2d9-41d3-9afb-d1f9e7d27efe/Untitled.png)

## 함수형으로 바꿔보자.

- 로직이 전역변수에 의존하지 않아야 한다.
- DOM을 사용할 수 있는 곳에서 실행된다고 가정하지 말자!
- 함수가 결과값을 리턴해야 한다.

### 현재 코드 분석

- 머리부터 발끝까지 다 액션 ~~포미닛 핫이슈도 아니구..~~
- 액션은 코드 전체로 퍼지기 때문에 발생한 현상

```jsx
// 전역 변수는 변경 가능하기 때문에 액션이다.
var shopping_cart = []; // Action
var shopping_cart_total = 0; // Action

function add_item_to_cart(name, price) { // Action
  shopping_cart.push({ // 전역 변수를 변경하기 때문에 Action
    name: name,
    price: price,
  });
  calc_cart_total();
  update_shopping_icons();
  update_tax_dom();
}

function calc_cart_total() { // Action
  shopping_cart_total = 0 ; // 전역 변수를 변경하기 때문에 Action
  for (var i = 0; i < shopping_cart.length; i++) {
    var item = shopping_cart[i];
    shopping_cart_total += item.price;
  }
  set_cart_total_dom();
}

function update_shopping_icons() { // Action
  var buy_buttons = get_but_buttons_dom(); // DOM에 읽기 때문에 Action
  for (var i = 0; i < buy_buttons.length; i++) {
    var button = buy_buttons[i];
    var item = button.item;
    if (item.price + shopping_cart_total >= 20) {
      button.show_free_shipping_icon(); // DOM을 변경하는 것도 Action
    } else {
      button.hide_free_shipping_icon(); // 마찬가지로 Action
    }
  }
}

function update_tax_dom() { // Action
  set_tax_dom(shopping_cart_total * 0.10); // DOM을 변경하는 것도 Action
}
```

### 함수에는 입력과 출력이 있다.

- 모든 함수는 입력과 출력이 있다.
- **입력**은 함수가 계산을 하기 위한 외부 정보 ⇒ 함수가 원하는 결과를 얻으려면 입력이 필요
    - 명시적 입력 ⇒ 인자
    - 암묵적 입력 → 인자 외 다른 입력 (예를 들어, 전역 변수를 읽어오는 작업)
- **출력**
    - 명시적인 출력 = 리턴값
    - 암묵적인 출력 → 리턴값 외 다른 출력 (ex - 전역변수를 변경, 입력된 인자가 변경)
- 암묵적인 입력과 출력이 생기면 부수 효과가 발생하는 것이기 때문에 액션이 된다.

### 그래서 함수 재사용성을 높이기 위해 액션을 최소화하려면

1. DOM 로직과 비즈니스 로직 분리
    - DOM 업데이트는 암묵적 출력
    - DOM 업데이트를 비즈니스 로직으로 끌고 들어오면 액션이 되어 버린다.
2. 전역변수 제거
    - 전역변수를 읽는 것은 **암묵적 입력,** 변경하는 것은 **암묵적 출력**
3. 결과값을 리턴
    - 암묵적 출력 대신 결과값을 리턴

## 액션에서 계산 빼내기

- 액션에서 계산을 빼내는 작업은 반복적인 과정
1. 계산 코드를 찾아 함수로 빼내기
2. 새 함수에 암묵적 입력과 출력을 찾기
3. 암묵적 입력은 인자로 암묵적 출력은 리턴값으로 변경

### 액션에서 계산 빼내기1 - 장바구니 금액 계산

- 먼저 계산 로직을 함수로 분리해준다.

```jsx
function calc_cart_total() {
  calc_total();
  set_cart_total_dom();
  update_shopping_icons();
  update_tax_dom();
}

function calc_total() {
  shopping_cart_total = 0 ;
  for (var i = 0; i < shopping_cart.length; i++) {
    var item = shopping_cart[i];
    shopping_cart_total += item.price;
  }
}
```

- 지역 변수를 리턴하도록 만든다.

```jsx
function calc_cart_total() {
  shopping_cart_total = calc_total();
  set_cart_total_dom();
  update_shopping_icons();
  update_tax_dom();
}

function calc_total() {
	let total = 0 ;
  for (var i = 0; i < shopping_cart.length; i++) {
    var item = shopping_cart[i];
		total += item.price;
  }
	return total;
}
```

- shopping_cart 전역 변수 읽어오는 방식으로 인자 받는 방식으로 변경한다.

```jsx
function calc_total(cart) {
  let total = 0 ;
  for (var i = 0; i < cart.length; i++) {
    var item = cart[i];
    total += item.price;
  }
  return total;
}
```

### 액션에서 계산 빼내기2 - 장바구니에 물품 추가

- 일단 새로운 함수로 분리

```jsx
function add_item_to_cart(name, price) {
  add_item(name, price);
  calc_cart_total();
}

function add_item(name, price) {
  shopping_cart.push({
    name: name,
    price: price,
  });
}
```

- 전역변수를 읽어 오기(암묵적 입력) ⇒ 인자로 읽어 오기(명시적 입력)

```jsx
function add_item_to_cart(name, price) {
  add_item(shopping_cart, name, price);
  calc_cart_total();
}

function add_item(cart, name, price) {
  cart.push({
    name: name,
    price: price,
  });
}
```

- 함수에서는 지역 변수를 사용해 리턴값을 만들고, 리턴값은 전역변수에 할당

```jsx
function add_item_to_cart(name, price) {
  shopping_cart = add_item(shopping_cart, name, price);
  calc_cart_total();
}

function add_item(cart, name, price) {
  const new_cart = cart.slice();
  new_cart.push({
    name: name,
    price: price,
  });
  return new_cart;
}
```

## 연습문제

### 연습문제1

- `add_item()` 함수에 관하여…
- [x]  DOM 업데이트와 비즈니스 규칙은 분리
- [x]  전역변수가 없어야 한다.
- [x]  전역변수에 의존하지 않아야한다.
- [x]  DOM을 사용할 수 있는 곳에서 실행된다고 가정하면 안된다.
- [x]  함수가 결과값을 리턴해야 한다.

### 연습문제2

- 함수로 빼내고, 인자를 받고 리턴값으로 돌려주기!

```jsx
function update_tax_dom() {
  set_tax_dom(calc_tax(shopping_cart_total));
}

function calc_tax(total) {
  return total * 0.10;
}
```

### 연습문제3

- `calc_tax()` 함수에 관하여
- [x]  DOM 업데이트와 비즈니스 규칙은 분리
- [x]  전역변수가 없어야 한다.
- [x]  전역변수에 의존하지 않아야한다.
- [x]  DOM을 사용할 수 있는 곳에서 실행된다고 가정하면 안된다.
- [x]  함수가 결과값을 리턴해야 한다.

### 연습문제4

- `update_shopping_icons()` 함수에서 계산 추출
    - `item.price + shopping_cart_total >= 20` 추출

```jsx
function update_shopping_icons() {
  var buy_buttons = get_but_buttons_dom();
  for (var i = 0; i < buy_buttons.length; i++) {
    var button = buy_buttons[i];
    var item = button.item;
    if (check_amount(item.price, shopping_cart_total)) {
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}

function check_amount(temp_price, total) {
  return temp_price + total >= 20
}
```

### 연습문제5

- `check_amount()` 함수에 관하여
- [x]  DOM 업데이트와 비즈니스 규칙은 분리
- [x]  전역변수가 없어야 한다.
- [x]  전역변수에 의존하지 않아야한다.
- [x]  DOM을 사용할 수 있는 곳에서 실행된다고 가정하면 안된다.
- [x]  함수가 결과값을 리턴해야 한다.

## 리팩토링 후 코드

- 코드를 바꾸니 나서 모두가 행복해졌다고 함

```jsx
// 리팩토링 후 코드
var shopping_cart = []; // Action
var shopping_cart_total = 0; // Action

function add_item_to_cart(name, price) {  // Action
  shopping_cart = add_item(shopping_cart, name, price);
  calc_cart_total();
}

function calc_cart_total() {  // Action
  shopping_cart_total = calc_total();
  set_cart_total_dom();
  update_shopping_icons();
  update_tax_dom();
}

function update_shopping_icons() {  // Action
  var buy_buttons = get_but_buttons_dom();
  for (var i = 0; i < buy_buttons.length; i++) {
    var button = buy_buttons[i];
    var item = button.item;
    if (check_amount(item.price, shopping_cart_total)) {
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}

function update_tax_dom() {  // Action
  set_tax_dom(calc_tax(shopping_cart_total));
}

//--------------------------------------------------------------------------//

function calc_total(cart) { // Calculation
  let total = 0 ;
  for (var i = 0; i < cart.length; i++) {
    var item = cart[i];
    total += item.price;
  }
  return total;
}

function add_item(cart, name, price) { // Calculation
  const new_cart = cart.slice();
  new_cart.push({
    name: name,
    price: price,
  });
  return new_cart;
}

function check_amount(temp_price, total) { // Calculation
  return temp_price + total >= 20
}

function calc_tax(total) { // Calculation
  return total * 0.10;
}
```

## 정리

- 액션은 **암묵적 입력** 또는 **암묵적 출력**을 가지고 있다.
- 계산의 정의에 따르면 계산은 암묵적 입력이나 출력이 없어야 한다.
- 공유 변수는 일반적으로 암묵적 입력 또는 출력이 된다.
- 암묵적 입력은 인자로 바꿀 수 있다.
- 암묵적 출력은 리턴값으로 바꿀 수 있다.