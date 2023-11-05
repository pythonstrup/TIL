// 리팩토링 후 코드
const shopping_cart = [];
let shopping_cart_total = 0;

function add_item_to_cart(name, price) {
  shopping_cart = add_item(shopping_cart, name, price);
  calc_cart_total();
}

function calc_cart_total() {
  shopping_cart_total = calc_total(shopping_cart);
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

function update_tax_dom() {
  set_tax_dom(calc_tax(shopping_cart_total));
}

// --------------------- Calc --------------------- //

function calc_total(cart) {
  let total = 0 ;
  for (let i = 0; i < cart.length; i++) {
    const item = cart[i];
    total += item.price;
  }
  return total;
}

function add_item(cart, name, price) {
  const new_cart = cart.slice();
  new_cart.push({
    name: name,
    price: price,
  });
  return new_cart;
}

function check_free_shipping_amount(cart) {
  return calc_total(cart) >= 20;
}

function calc_tax(total) {
  return total * 0.10;
}