# 쏙쏙 5장

# 더 좋은 액션 만들기

- 액션에서 암묵적 입력과 출력을 줄여 설계를 개선하는 방법!

## 개요

### 더 나은 추상화 단계 선택

- 액션에서 계산으로 리팩토링하는 과정은 단순하고 기계적
- 새로운 요구사항은 아래와 같다.

> “합계 금액과 제품 가격에 대한 무료 배송 여부가 아니고 주문 결과가 무료 배송인지 확인해야 합니다!”
>
- 현재 장바구니 합계를 계산하는 코드가 중복되어 있다!!

```jsx
function check_free_shipping_amount(temp_price, total) {
  return temp_price + total >= 20
}

function calc_total(cart) {
  let total = 0 ;
  for (let i = 0; i < cart.length; i++) {
    const item = cart[i];
    total += item.price;
  }
  return total;
}
```

- 위 함수를 아래와 같이 변경해주자.

```jsx
function check_free_shipping_amount(cart)
```

### 비즈니스 요구 사항과 함수를 맞추기

- 함수의 동작을 바꿨기 때문에 리팩토링이라고 할 수 있다.
- 장바구니를 인자로 받아 20보다 크거나 같은지 확인한다.

```jsx
function check_free_shipping_amount(cart) {
  return calc_total(cart) >= 20;
}

function update_shopping_icons() {
  const buy_buttons = get_but_buttons_dom();
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
    const new_cart = add_item(shopping_cart, item.name, item.price);

    if (check_free_shipping_amount(new_cart)) {
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}
```

> **생각해보기**
> 
> 방금 바꾼 코드에서 어떤 부분이 가장 인상적인가요? 원래 있던 장바구니를 직접 변경하지 않고 복사본을 만들었습니다. 이런 스타일을 함수형 프로그래밍에서 많이 사용합니다. 이 방법을 무엇이라고 부를까요?
> 
> **답변: 불변성…? 불변 유지..?**
>

> **쉬는 시간**
> 
> Q. add_item()를 부를 때마다 cart 배열을 복사하는데, 비용이 많이 들지 않는가?
> 
> A. 그럴수도 아닐수도? 배열을 바꾸는 것보다야 비용이 더 들겠지만 최신 프로그래밍 언어들은 가비지 컬렉터를 통해 불필요한 메모리를 효율적으로 처리해준다.
> 복사본을 사용할 때 잃는 것보다 얻는 것이 많다! 책을 읽으면서 알아가볼 것! 만약 복사본을 만드는 코드가 느리다면 최적화를 진행할 수 있다!
>

## 원칙

### 원칙1. 암묵적 입력과 출력은 적을수록 좋습니다.

- 암묵적 입력과 출력이 있다면 다른 컴포넌트와 강하게 연결된 컴포넌트라고 할 수 있다.
- 다른 곳에서 사용할 수 없기 때문에 모듈이 아니다. 이런 함수의 동작은 연결된 부분의 동작에 의존한다. 암묵접 입력과 출력을 명시적으로 바꿔 모듈화된 컴포넌트를 만들 수 있다.
- 암묵적 입력과 출력이 있는 함수는 아무 때나 실행할 수 없기 때문에 테스트하기 어렵다. ↔ 계산은 테스트 쉽다.
- 아래는 암묵적 입력을 명시적 입력으로 변경한 코드이다!

```jsx
function calc_cart_total() {
  shopping_cart_total = calc_total();
  set_cart_total_dom();
  update_shopping_icons(shopping_cart);
  update_tax_dom();
}

function update_shopping_icons(cart) {
  const buy_buttons = get_but_buttons_dom();
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
    const new_cart = add_item(cart, item.name, item.price);

    if (check_free_shipping_amount(new_cart)) {
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}
```

> **생각해보기**
> 
> 방금 원칙을 적용해 암묵적 입력을 없앴습니다. 하지만 아직도 액션입니다. 원칙을 적용한 후에 함수가 더 좋아졌나요? 다양한 환경에서 재사용할 수 있나요? 테스트하기 쉬워졌나요?
> 
> **답변: 함수가 get_but_buttons_dom()에 너무 의존적이라.. 흠… 좋아졌다고 할 수 있나? 거기서 거기인 것 같은데…**
>

### 연습문제1

- 전역변수를 읽는 액션 전부 바꾸기! **(암묵적 출력을 바꾸라는 의미는 아님)**

```jsx
const shopping_cart = [];
let shopping_cart_total = 0;

function add_item_to_cart(name, price) {
  shopping_cart = add_item(shopping_cart, name, price);  // 암묵적 출력
  calc_cart_total(shopping_cart, shopping_cart_total);
}

function calc_cart_total(cart, cart_total) {
  shopping_cart_total = calc_total(cart); // 암묵적 출력
  set_cart_total_dom(cart_total);
  update_shopping_icons(cart);
  update_tax_dom(cart_total);
}

function set_cart_total_dom(cart_total) {
  ...
}

function update_shopping_icons(cart) {
  const buy_buttons = get_but_buttons_dom();
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
    const new_cart = add_item(cart, item.name, item.price);

    if (check_free_shipping_amount(new_cart)) {
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}

function update_tax_dom(cart_total) {
  set_tax_dom(calc_tax(cart_total));
}
```

### 코드 다시 살펴보기

- 정리할 코드 2개가 있다. 사용하지 않는 shopping_cart_total 전역변수와 과해보이는 calc_cart_total() 함수이다.

```jsx
const shopping_cart = [];

function add_item_to_cart(name, price) {
  shopping_cart = add_item(shopping_cart, name, price);  // 암묵적 출력
  const cart_total = calc_total(cart);
  set_cart_total_dom(cart_total);
  update_shopping_icons(cart);
  update_tax_dom(cart_total);
}

// calc_cart_total 함수 제거
```

> **생각해보기**
> 
> 지금까지 결과로 액션 코드 라인 수를 많이 줄였습니다. 여기까지 오는데 오래 걸렸다고 생각하나요? 처음부터 바로 할 수 있었을까요?
> 
> **답변: 설계를 잘했다면 처음부터 할 수 있지 않았을까요? 근데 일에 치이고 빨리빨리 만들다보면 신경을 못쓰니 액션으로 가득한 코드를 만들고 있을 수도??!? 아니면 오히려 만들어놓고 리팩토링하는 게 빠를 수도 있다.**


### 계산 분류하기

- 계산에 대해 분류해보자!

```jsx
function calc_total(cart) { // Cart, Item, Business rule
  let total = 0 ;
  for (let i = 0; i < cart.length; i++) {
    const item = cart[i];
    total += item.price;
  }
  return total;
}

function add_item(cart, name, price) { // Cart, Item
  const new_cart = cart.slice();
  new_cart.push({
    name: name,
    price: price,
  });
  return new_cart;
}

function check_free_shipping_amount(cart) { // Business rule
  return calc_total(cart) >= 20;
}

function calc_tax(total) { // Business rule
  return total * 0.10;
}
```

### 원칙2. 설계는 엉켜있는 코드를 푸는 것이다.

- 함수를 사용하면 관심사를 자연스럽게 분리할 수 있다.
- 함수는 인자로 넘기는 값과 그 값을 사용하는 방법을 분리한다.
- 크고 복잡한 것이 좋아보일 수 있지만 분리된 것은 언제든 쉽게 조합할 수 있기 때문에! 분리가 더 좋다는 말씀!
    - 재사용하기 쉽다.
    - 유지보수가 쉽다.
    - 테스트하기 쉽다. ⇒ 작은 함수는 한 가지 일만 하기 때문!

### add_item() 분리하기

- add_item()은 4부분으로 나눌 수 있다.

```jsx
function add_item(cart, name, price) { // Cart, Item, Business rule
  const new_cart = cart.slice(); // 1. 배열을 복사
  new_cart.push({ // 2. item 객체를 생성 / 3. 복사본에 item을 추가
    name: name, 
    price: price,
  });
  return new_cart; // 4. 복사본 반환
}
```

- 분리 후 변경된 코드는 아래와 같다.

```jsx
// --------- 액션 --------- //
function add_item_to_cart(name, price) {
  shopping_cart = add_item(shopping_cart, make_cart_item(name, price)); 변경
  const cart_total = calc_total(cart);
  set_cart_total_dom(cart_total);
  update_shopping_icons(cart);
  update_tax_dom(cart_total);
}

function update_shopping_icons(cart) {
  const buy_buttons = get_but_buttons_dom();
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
    const new_cart = add_item(cart, make_cart_item(item.name, item.price)); 변경

    if (check_free_shipping_amount(new_cart)) {
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}

// --------- 계산 --------- //
function add_item(cart, item) { // Cart, Item, Business rule
  const new_cart = cart.slice();
  new_cart.push(item);
  return new_cart;
}

function make_cart_item(name, price) {
  return { name, price };
}
```

- 1, 3, 4번은 카피-온-라이트(copy-on-write)를 구현한 부분이기 때문에 함께 두는 것이 좋아보인다!
- 함수 이름과 인자를 더 일반적인 이름으로 바꾸면? (**현재 `add_item()`의 네이밍은 뭔가 장바구니에만 쓸 수 있는 함수처럼 보이긴 한다.**)

```jsx
function add_element_last(array, item) {
  const new_array = array.slice();
  new_array.push(item);
  return new_array;
}
```

- 짜잔~ 재사용할 수 있는 유틸리티 함술로 재탄생!
- 이제 아래와 같이 분리되어 사용될 수 있다!

```jsx
function add_item(cart, item) {
  return add_element_last(cart, item);
}

function make_cart_item(name, price) {
  return { name, price };
}
```

### 계산 재분류해보기!

```jsx
function calc_total(cart) { // Cart, Item, Business rule
  let total = 0 ;
  for (let i = 0; i < cart.length; i++) {
    const item = cart[i];
    total += item.price;
  }
  return total;
}

function add_item(cart, item) { // Cart
  return add_element_last(cart, item);
}

function make_cart_item(name, price) { // Cart
  return { name, price };
}

function check_free_shipping_amount(cart) { // Business rule
  return calc_total(cart) >= 20;
}

function calc_tax(total) { // Business rule
  return total * 0.10;
}

// ------- Utils ------- // 
function add_element_last(array, item) {
  const new_array = array.slice();
  new_array.push(item);
  return new_array;
}
```

> **쉬는 시간**
> 
>Q1. 왜 계산을 유틸리티와 장바구니, 비즈니스 규칙으로 다시 나누는 것인가요?
> 
>A1. 나중에 다룰 설계 기술을 보여주기 위함! 최종적으로 코드는 구분된 그롭과 분리된 계층으로 구성!
> 
>Q2. 그럼 비즈니스 규칙과 장바구니 기능은 어떤 차이가 있나요? 전자상거래를 만드는 것이라면 장바구니에 관한 것은 모두 비즈니스 규칙이 아닌가요?
> 
>A2. 장바구니는 대부분의 전자상거래에서 사용하는 일반적인 개념이다. 그리고 장바구니가 동작하는 방식도 모두 비슷하다. 하지만 비즈니스 규칙은 다르다. MegaMart에서 운영하는 특별한 규칙이라고 할 수 있다. 예를 들어 무료 배송 규칙이라든지!
> 
>Q3. 비즈니스 규칙과 장바구니에 대한 동작에 모두 속하는 함수도 있을 수 있나요?
> 
>A3. 지금 시점에서는 그러하다! 하지만 계층적 관점으로 보면 꾸린내가 난다. 비즈니스 규칙은 장바구니 구조와 같은 하위 계층보다 빠르게 바뀌기 때문에 이 부분은 분리되어야 한다!


⇒ 3번째 질문은 보면서 생각든 점

- 비즈니스 규칙은 장바구니 구조와 같은 하위 계층보다 빠르게 바뀐다.
    - 진짜 맞말… 현재 다니는 기업이 스타트업이라 비즈니스 규칙이 계속 변화된다. 프론트데스크나 의사가 불편하다고 하면 몇 시간만에 뒤집혀버리는 게 우리 회사의 비즈니스 규칙이다.
    - 그런데 기능 분리가 제대로 되어있지 않아, 비즈니스 규칙이 변경되면 전체 기능을 다시 테스트해봐야 한다… 정말 비효율적이다.

### 연습문제2

- update_shipping_icons() 함수가 너무 크기 때문에 많은 일을 하고 있다. 이 함수가 하는 일은 아래와 같다.
    1. **모든 버튼을 가져오기** ⇒ 구매하기 버튼 관련 동작
    2. **버튼을 가지고 반복하기** ⇒ 구매하기 버튼 관련 동작
    3. **버튼에 관련된 제품을 가져오기** ⇒ 구매하기 버튼 관련 동작
    4. **가져온 제품을 가지고 새 장바구니 만들기** ⇒ Cart, Item 관련 동작
    5. **장바구니가 무료 배송이 필요한지 확인하기** ⇒ Cart, Item 관련 동작
    6. **아이콘 표시하거나 감추기** ⇒ DOM 관련 동작
- 이제 각 분류별로 함수를 나눠보자!!

```jsx
function update_shopping_icons(cart) {
  const buy_buttons = get_but_buttons_dom();
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
    update_cart_free_shipping_icon(check_free_shipping(cart, item), button);
  }
}

// Cart, Item 관련 동작
function check_free_shipping(cart, item) {
  const new_cart = add_item(cart, make_cart_item(item.name, item.price));
  return check_free_shipping_amount(new_cart);
}

// DOM 관련 동작
function update_cart_free_shipping_icon(is_free_shipping, button) {
  is_free_shipping ? button.show_free_shipping_icon(): button.hide_free_shipping_icon();
}
```

## 정리

- 일반적으로 암묵적 입력과 출력은 인자와 리턴값으로 바꿔 없애는 것이 좋습니다.
- 설계는 엉켜있는 것을 푸는 것입니다. 풀려있는 것은 언제든 다시 합칠 수 있습니다.
- 엉켜있는 것을 풀어 각 함수가 하나의 일만 하도록 하면, 개념을 중심으로 쉽게 구성할 수 있습니다.