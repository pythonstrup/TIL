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