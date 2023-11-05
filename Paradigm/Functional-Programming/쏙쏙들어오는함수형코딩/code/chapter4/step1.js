const shopping_cart = []; // 장바구니 제품 전역 변수
const shopping_cart_total = 0; // 금액 합계 전역 변수

function add_item_to_cart(name, price) {
  shopping_cart.push({
    // 장바구니에 제품을 담기 위해 cart 배열에 레코드 추가
    name: name,
    price: price,
  });
  calc_cart_total(); // 장바구니 제품이 바뀌었기 때문에 금액 합계를 업데이트
}

function calc_cart_total() {
  shopping_cart_total = 0;
  for (let i = 0; i < shopping_cart.length; i++) {
    const item = shopping_cart[i];
    shopping_cart_total += item.price; // 모든 제품값 더하기
  }
  set_cart_total_dom(); // 금액 합계를 반영하기 위한 DOM 업데이트
  update_shopping_icons(); // 아이콘 업데이트 코드
  update_tax_dom(); // 세금 업데이트 코드
}

function update_shopping_icons() {
  const buy_buttons = get_but_buttons_dom(); // 페이지에 렌더링된 모든 구매 버튼을 가져와 반복문을 돌린다.
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
    if (item.price + shopping_cart_total >= 20) { // 무료 배송이 가능한지 확인하고 결정에 따라 무료 배송 아이콘을 보여주거나 보여주지 않음
      button.show_free_shipping_icon();
    } else {
      button.hide_free_shipping_icon();
    }
  }
}

function update_tax_dom() {
  set_tax_dom(shopping_cart_total * 0.10); // 금액 합계에 10%를 계산해 DOM 업데이트 함수 호출
}