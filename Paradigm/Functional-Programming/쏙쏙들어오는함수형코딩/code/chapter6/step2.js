function removeItems(array, idx, count) {
  const copy = array.slice();
  copy.splice(idx, count);
  return copy;
}

function removeItemByName(cart, name) {
  let idx = null;
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      idx = i;
    }
  }
  if (idx !== null) {
    removeItems(cart, idx, 1);
  }
  return cart; // 값이 변경되지 않으면 그냥 cart를 반환하면 된다.
}

function deleteHandler(name) {
  removeItemByName(shoppingCart, name);
  let total = calcTotal(shoppingCart);
  setCartTotalDom(total);
  updateShoppingIcons(shoppingCart);
  updateTaxDom(total)
}