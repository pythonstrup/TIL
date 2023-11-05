// 리팩토링 후 코드
const shopping_cart = [];

function add_item_to_cart(name, price) {
  shopping_cart = add_item(shopping_cart, name, price);  // 암묵적 출력
  const cart_total = calc_total(cart);
  set_cart_total_dom(cart_total);
  update_shopping_icons(cart);
  update_tax_dom(cart_total);
}

function set_cart_total_dom(cart_total) {

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

// --------------------- Calc --------------------- //

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