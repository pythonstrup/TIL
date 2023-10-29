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