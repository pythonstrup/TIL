function removeItemByName(cart, name) {
  const newCart = cart.slice(); // 1. 복사본 만들기
  let idx = null;
  for (let i = 0; i < newCart.length; i++) {
    if (newCart[i].name === name) {
      idx = i;
    }
  }
  if (idx !== null) {
    newCart.splice(idx, 1); // 2. 복사본 변경하기
  }
  return newCart; // 3. 복사본 리턴하기
}

function deleteHandler(name) {
  removeItemByName(shoppingCart, name);
  let total = calcTotal(shoppingCart);
  setCartTotalDom(total);
  updateShoppingIcons(shoppingCart);
  updateTaxDom(total)
}