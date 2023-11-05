// 전역 변수는 변경 가능하기 때문에 액션이다.
const shopping_cart = []; // Action
const shopping_cart_total = 0; // Action

function add_item_to_cart(name, price) { // Action
  shopping_cart.push({ // 전역 변수를 변경하기 때문에 Action
    name: name,
    price: price,
  });
  calc_cart_total();
}

function calc_cart_total() { // Action
  shopping_cart_total = 0 ; // 전역 변수를 변경하기 때문에 Action
  for (let i = 0; i < shopping_cart.length; i++) {
    const item = shopping_cart[i];
    shopping_cart_total += item.price;
  }
  set_cart_total_dom();
  update_shopping_icons();
  update_tax_dom();
}

function update_shopping_icons() { // Action
  const buy_buttons = get_but_buttons_dom(); // DOM에 읽기 때문에 Action
  for (let i = 0; i < buy_buttons.length; i++) {
    const button = buy_buttons[i];
    const item = button.item;
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