function setPriceByName(cart, name, price) {
  const idx = indexOfItem(cart, name);
  if (idx !== null) {
    return arraySet(cart, idx, setPrice(cart[idx], price));
  }
  return cart;
}

function arraySet(array, idx, value) {
  const copy = array.slice();
  copy[idx] = value;
  return copy;
}

function indexOfItem(cart, name) {
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      return i;
    }
  }
  return null;
}