function setPriceByName(cart, name, price) {
  const cartCopy = cart.slice();
  const idx = indexOfItem(cart, name);
  if (idx !== null) {
    cartCopy[idx] = setPrice(cartCopy[idx], price);
  }
  return cartCopy;
}

function indexOfItem(cart, name) {
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      return i;
    }
  }
  return null;
}