# 계층형 설계2

- 코드를 모듈화하기 위해 추상화 벽을 만드는 법을 배웁니다.
- 좋은 인터페이스가 어떤 것이고, 어떻게 찾는지 알아봅니다.
- 설계가 이만하면 되었다고 할 수 있는 시점을 압니다.
- 왜 계층형 설계가 유지보수와 테스트, 재사용에 도움이 되는지 이해합니다.

## 계층형 설계 패턴

1. 패턴 1: 직접 구현
2. 패턴 2: 추상화 벽
3. 패턴3: 작은 인터페이스
4. 패턴4: 편리한 계층

## 패턴 2: 추상화 벽

- 추상화 벽으로 구현을 감춥니다.
- 추상화 벽이 있는 함수를 사용할 때는 구현을 전혀 몰라도 함수를 쓸 수 있습니다.

## 세부적인 것을 감추는 것은 대칭적입니다.

- 라이브러리나 API
- 추상화 벽의 한계를 시험하기 위해 장바구니 데이터 구조를 변경해보자.

## 장바구니 데이터 구조 바꾸기

- 배열을 순서대로 검색하는 것은 성능이 떨어지기 때문에 배열 ⇒ 해시맵으로 자료구조 변경

## 연습문제1

- 장바구니 데이터 구조를 변경하기 위해서는 추상화벽에 있는 함수들을 고치면 된다.
- 중간 단계에 있는 `remove_item_by_name()`, `calc_total()`, `isInCart()`, `add_item()`, `setPriceByName()`

## 장바구니를 객체로 다시 만들기

- 장바구니를 자바스크립트 객체로 다시 만들어보자.

```jsx
function addItem(cart, item) {
  return objectSet(cart, item.name, item);
}

function calcTotal(cart) {
  let total = 0;
  const names = Object.keys(cart);
  for (let i = 0; i < names.length; i++) {
    const item = cart[names[i]];
    total += item.price;
  }
  return total;
}

function setPriceByName(cart, name, price) {
  if (isInCart(cart, name)) {
    const item = cart[name];
    const copy = setPrice(item, price);
    return objectSet(cart, name, copy);
  }
  const item = makeItem(name, price);
  return objectSet(cart, name, item);
}

function removeItemByName(cart, name) {
  return objectDelete(cart, name);
}

function isInCart(cart, name) {
  return cart.hasOwnProperty(name);
}
```

## 추상화 벽이 있으면 구체적인 것을 신경쓰지 않아도 됩니다.

- 데이터 구조를 변경하기 위해 함수 다섯 개만 바꿀 수 있었던 이유는 바꾼 함수가 추상화 벽에 있는 함수이기 때문입니다.
- 추상화 벽은 필요하지 않은 것은 무시할 수 있도록 간접적인 단계를 만듭니다.
- 만약 점선 위의 함수가 장바구니를 조작하기 위해 splice() 함수를 호출한다면 추상화 벽 규칙을 어기는 것입니다.

## 추상화 벽은 언제 사용하면 좋을까요?

### 1. 쉽게 구현을 바꾸기 위해

- 구현에 대한 확신이 없는 경우 추상화 벽을 사용하면 구현을 간접적으로 사용할 수 있기 때문에 나중에 구현을 바꾸기 쉽습니다.
- 프로토타이핑 같이 최선의 구현을 확신할 수 없는 작업에 유용합니다.

### 2. 코드를 읽고 쓰기 쉽게 만들기 위해

- 추상화 벽을 사용하면 세부적인 것을 신경쓰지 않아도 됩니다.

### 3. 팀 간에 조율해야할 것을 줄이기 위해

- 추상화 벽을 사용하면 각 팀에 관한 구체적인 내용을 서로 신경쓰지 않아도 일할 수 있습니다.

### 4. 주어진 문제에 집중하기 위해

## 패턴 2 리뷰: 추상화 벽

- 추상화 벽으로 벽 아래에 있는 코드와 위에 있는 코드의 의존성을 없앨 수 있습니다.
- 서로 신경쓰지 않아도 되는 구체적인 것을 벽을 기준으로 나눠서 서로 의존하지 않게 합니다.
- 모든 추상화는 아래와 같이 동작합니다.
    - 추상화 단계 상위에 있는 코드와 하위에 있는 코드는 서로 의존하지 않게 정의합니다.
    - 추상화 단계의 모든 함수는 비슷한 세부 사항을 무시할 수 있도록 정의합니다.

## 패턴 3: 작은 인터페이스

- 인터페이스를 최소화하면 하위 계층에 불필요한 기능이 쓸데없이 커지는 것을 막을 수 있습니다.
- 시계 할인 마케팅을 구현하는 방법은 2가지가 있습니다.
    - 하나는 추상화 벽에 구현
    - 다른 하나는 추상화 벽 위에 있는 계층에 구현하는 방법

### 1. 추상화 벽에 만들기

```jsx
/**
 * 방법 1. 추상화 벽 사용
 */
function getsWatchDiscount(cart) {
  let total = 0;
  const names = Object.keys(cart);
  for (let i = 0; i < names.length; i++) {
    const item = cart[names[i]];
    total += item.price;
  }
  return total > 100 && cart.hasOwnProperty("watch");
}
```

### 2. 추상화 벽 위에 만들기

```jsx
/**
 * 방법 2. 추상화 벽 위에 만들기
 */
function getsWatchDiscount(cart) {
  const total = calcTotal(cart);
  const hasWatch = isInCart("watch");
  return total > 100 && hasWatch;
}
```

### 추상화 벽 위에 있는 계층에 구현하는 것이 더 좋습니다.

- 추상화 벽 위에 있는 계층에 만드는 것이 더 직접 구현에 가깝습니다.
- 추상화 벽에 구현하면 시스템 하위 계층 코드가 늘어나기 때문에 좋지 않습니다.
- 새로운 기능을 만들 때 하위 계층에 기능을 추가하거나 고치는 것보다 상위 계층에 만드는 것이 작은 인터페이스 패턴이라고 할 수 있습니다.
- 작은 인터페이스 패턴을 사용하면 하위 계층을 고치지 않고 상위 계층에서 문제를 해결할 수 있습니다.

### 로그를 추가?

- 아래와 같이 해당 함수에 로그 남기는 함수를 호출하는 것이 좋은 위치일까요?

```jsx
function addItem(cart, item) {
  logAddToCart(globalUserId, item);
  return objectSet(cart, item.name, item);
}
```

### 코드 위치에 대한 설계 결정

- logAddToCart() 함수는 액션입니다. addItem()에서 함수를 호출하면 해당 함수도 액션이 되어버립니다. ⇒ 테스트하기가 어려워집니다.
- LogAddToCart는 추상화 벽 위에 있는 계층에서 호출하는 것이 좋아보입니다.

### 장바구니 로그를 남기 더 좋은 위치

- addItemToCart() 함수가 로그를 남길 좋은 곳 같아 보입니다.
    - 장바구니에 제품을 담을 때 호출하는 핸들러 함수입니다.

```jsx
function addItemToCart(name, price) {
  const item = makeCartItem(name, price);
  shoppingCart = addItem(shoppingCart, item);
  const total = calcTotal(shoppingCart);
  setCartTotalDom(total);
  updateShippingIcons(shoppingCart);
  updateTaxDom(total);
  logAddToCart();
}
```

- 유일한 정답은 아니지만 addItem() 보다는 나아 보입니다.

## 패턴 3 리뷰: 작은 인터페이스

1. 추상화 벽에 코드가 많을수록 구현이 변경되었을 때 고쳐야할 것이 많습니다.
2. 추상화 벽에 있는 코드는 낮은 수준의 코드이기 때문에 더 많은 버그가 있을 수 있습니다.
3. 낮은 수준의 코드는 이해하기 어렵습니다.
4. 추상화 벽에 코드가 많을수록 팀 간에 조율할 것도 많아집니다.
5. 추상화 벽에 인터페이스가 많으면 알아야할 것이 많아 사용하기 어렵습니다.

## 패턴 4: 편리한 계층

- 다른 패턴과 다르게 조금 더 현실적이고 실용적인 측면을 다루고 있습니다.
- 만약 현재 작업하는 코드가 편리하다고 느껴진다면 설계는 조금 멈춰도 됩니다. 반복문은 감싸지 않고 그대로 두고 호출 화살표가 조금 길어지거나 계층이 다른 계층과 섞여도 그대로 두세요. 구체적인 것을 너무 많이 알아야 하거나 코드가 지저분하다고 느껴진다면 다시 패턴을 적용하세요.