# 07 신뢰할 수 없는 코드를 쓰면서 불변성 지키기

- 레거시 코드나 신뢰할 수 없는 코드로부터 내 코드를 보호하기 위한 방어적 복사
- 얕은 복사와 깊은 복사
- 카피-온-라이트와 방어적 복사를 언제 사용해야 하는가?

## 레거시 코드와 불변성

- 기존 add_item_to_cart 함수에 블랙프라이데이 행사를 위한 코드를 추가한다고 가정
- `black_friday_promotion(shopping_cart);` 를 추가
- 하지만 추가된 함수를 호출할 경우 카피-온-라이트 원칙을 지킬 수 없다. 그리고 `black_friday_promotion` 함수를 고칠 수도 없다.
- 하지만 카피-온-라이트를 지키면서 안전하게 함수를 사용할 수 있는 원칙이 있다. 이 원칙을 방어적 복사(defensive copy)라고 한다.

## 신뢰할 수 없는 코드와 상호작용

- 블랙프라이데이 코드는 분석하기 어려운 레거시 코드라 어떤 일이 일어날 지 정확히 알 수 없다.
- 그래서 데이터가 바뀌는 것을 완벽히 막아주는 원칙이 필요하다. 이 원칙을 방어적 복사라고 한다.

## 방어적 복사는 원본이 바뀌는 것을 막아준다.

- 신뢰할 수 없는 코드에 있는 데이터가 안전지대로 들어올 때
    1. 신뢰할 수 없는 코드의 데이터가 안전지대로 들어온다.
    2. 안전지대에서는 깊은 복사를 한다.
- 안전지대에서 데이터를 내보낼 때
    1. 깊은 복사를 한다.
    2. 복사본을 안전지대 밖으로 반환한다.
- 이렇게 하면 안전지대의 데이터가 바뀔 염려가 없다.

## 방어적 복사 구현

- 원본

```jsx
function add_item_to_cart(name, price) {
  const item = make_cart_item(name, price);
  shopping_cart = add_item(shopping_cart, item);
  let total = calc_total(shopping_cart);
  set_cart_total_dom(total);
  update_shipping_icons(shopping_cart);
  update_tax_dom(total);

  black_friday_promotion(shopping_cart);
}
```

- 데이터를 전달하기 전에 복사

```jsx
function add_item_to_cart(name, price) {
  const item = make_cart_item(name, price);
  shopping_cart = add_item(shopping_cart, item);
  let total = calc_total(shopping_cart);
  set_cart_total_dom(total);
  update_shipping_icons(shopping_cart);
  update_tax_dom(total);

  // 블랙프라이데이
  const cart_copy = deepCopy(shopping_cart);
  black_friday_promotion(shopping_cart);
}
```

- 데이터를 전달하기 전후 복사

```jsx
function add_item_to_cart(name, price) {
  const item = make_cart_item(name, price);
  shopping_cart = add_item(shopping_cart, item);
  let total = calc_total(shopping_cart);
  set_cart_total_dom(total);
  update_shipping_icons(shopping_cart);
  update_tax_dom(total);

  // 블랙프라이데이
  const cart_copy = deepCopy(shopping_cart);
  black_friday_promotion(shopping_cart);
  shopping_cart = deepCopy(cart_copy);
}
```

## 방어적 복사 규칙

### 규칙1: 데이터가 안전한 코드에서 나갈 때 복사하기

1. 불변성 데이터를 위한 깊은 복사본을 만든다.
2. 신뢰할 수 없는 코드로 복사본을 전달한다.

### 규칙2: 안전한 코드로 데이터가 들어올 때 복사하기

1. 변경될 수도 있는 데이터가 들어오면 바로 깊은 복사본을 만들어 안전한 코드로 전달한다.
2. 복사본을 안전한 코드에서 사용한다.

## 신뢰할 수 없는 코드 감싸기

- 방어적 복사 코드를 따로 분리해 새로운 함수를 만들면 좋을 것 같다.

```jsx
function add_item_to_cart(name, price) {
  const item = make_cart_item(name, price);
  shopping_cart = add_item(shopping_cart, item);
  let total = calc_total(shopping_cart);
  set_cart_total_dom(total);
  update_shipping_icons(shopping_cart);
  update_tax_dom(total);

  shopping_cart = black_friday_promotion_safe(shopping_cart);
}

function black_friday_promotion_safe(cart) {
  const cart_copy = deepCopy(shopping_cart);
  black_friday_promotion(shopping_cart);
  return deepCopy(cart_copy);
}
```

## 연습문제1

```jsx
function payrollCalc(employees) {
  ...
  return payrollChecks;
}

function payrollCalcSafe(employees) {
  const copy = deepCopy(employees);
  const payrollChecks = payrollCalc(copy);
  return deepCopy(payrollChecks);
}
```

## 연습문제2

```jsx
function userChanges.subscribe(function (user) {
  const copy = deepCopy(user);
  processUser(copy);
})
```

## 생각보다 익술할 수 있는 방어적 복사

- 직렬화되는 JSON은 깊은 복사본 (웹 API는 방어적 복사를 한다.)
- 얼랭과 엘릭서의 방어적 복사

## 카피-온-라이트 vs 방어적 복사

### 카피-온-라이트

- 통제할 수 있는 데이터를 바꿀 때 카피-온-라이트를 사용합니다.
- 안전지대 어디서나 사용할 수 있다.
- 복사방식 = 얕은 복사

### 방어적 복사

- 신뢰할 수 없는 코드와 데이터를 주고받아야 할 때 방어적 복사를 사용한다.
- 안전지대의 경계에서 데이터가 오고 갈 때 방어적 복사를 사용
- 복사방식 = 깊은 복사(비용이 상대적으로 높다.)

## 자바스크립트의 깊은 복사는 구현이 어렵다.

- Lodash 라이브러리에 있는 깊은 복사 함수 사용을 추천
- Lodash의 cloneDeep() 함수는 중첩된 데이터에 깊은 복사를 한다.
- 깊은 복사가 대충 어떤 식으로 동작하는 알 수 있는 예제 코드! (웬만하면 Lodash 사용하자.)

```jsx
function deepCopy(thing) {
  if (Array.isArray(thing)) {
    const copy = [];
    thing.forEach(item => {
      copy.push(deepCopy(item));
    });
    return copy;
  } else if (thing === null) {
    return null;
  } else if (typeof thing === 'object') {
    const copy = {};
    Object.keys(thing).forEach(key => {
      copy[key] = deepCopy(thing[key]);
    });
    return copy;
  }

  return thing; // 문자열, 숫자, 불리언, 함수는 불변형이기 때문에 복사할 필요가 없다.
}
```

## 연습문제3

### 깊은 복사

1. 중첩된 데이터 구조에서 모든 것을 복사합니다.
2. 공유하는 데이터 구조가 없다. ⇒ 신뢰할 수 없는 코드로부터 원본 데이터를 보호할 수 있다.
3. 비공유 아키텍처를 구현하기 좋다.

### 얕은 복사

1. 복사본과 원본 데이터 구조가 많은 부분을 공유 ⇒ 비용이 상대적으로 적게 든다.
2. 바뀐 부분만 복사

## 연습문제4

### 방어적 복사

- 깊은 복사를 한다.
- 불변성을 유지하는 데 중요하다.
- 데이터를 바꾸기 전에 복사복은 만든다.
- 신뢰할 수 없는 코드와 데이터를 주고 받을 때 사용
- 불변성을 위한 완전한 방법, 다른 원칙 없이도 사용 가능
- 신뢰할 수 없는 코드로부터 데이터를 전달받거나 신뢰할 수 없는 코드로 데이터를 전달하기 전에 데이터를 복사한다.

### 카피온라이트

- 다른 것보다 비용이 적게 든다.
- 데이터를 바꾸기 전에 복사복은 만든다.
- 안전지대 안에서 불변성 유지하기 위해 사용
- 얕은 복사

## 연습문제5

1. 맞다.
2. 아니다. 신뢰할 수 없는 코드와 데이터를 주고받을 때는 방어적 복사를 사용하는 것이 좋다.
3. ?? 맞는 거 같기도 하고 아닌 거 같기도 하고. 레거시를 완벽히 이해할 수 있나..?
4. 가능하다면 그렇게 할 수 있다. 하지만.. 현실에서는 레거시를 뜯어 고치는 일은…
5. 아니다. 불변성이 보장되는지 모른다.

## 결론, 정리

- 방어적 복사는 불변성을 구현하는 원칙. 데이터가 들어오고 나갈 때 복사본을 만든다.
- 방어적 복사는 깊은 복사를 한다. 상대적으로 비용이 많이 든다.
- 불변성 원칙을 구현하지 않은 코드로부터 데이터를 보호해준다.