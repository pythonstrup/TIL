function addItem(cart, item) {
  logAddToCart(globalUserId, item);
  return objectSet(cart, item.name, item);
}

/**
 * 위의 방법보다는 아래 함수(추상화 벽 위의 함수)에 액션 함수를 사용하는 것이 좋아보입니다.
 */
function addItemToCart(name, price) {
  const item = makeCartItem(name, price);
  shoppingCart = addItem(shoppingCart, item);
  const total = calcTotal(shoppingCart);
  setCartTotalDom(total);
  updateShippingIcons(shoppingCart);
  updateTaxDom(total);
  logAddToCart();
}