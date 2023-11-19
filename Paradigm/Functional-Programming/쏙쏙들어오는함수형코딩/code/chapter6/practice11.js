function setQuantityByName(cart, name, quantity) {
  const cartCopy = cart.slice();
  for (let i = 0; i < cartCopy.length; i++) {
    if (cartCopy[i].name === name) {
      cartCopy[i].quantity = setQuantity(cartCopy[i], quantity);
    }
  }
  return cartCopy;
}

function setQuantity(item, newQuantity) {
  return objectSet(item, "quantity", newQuantity);
}

function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}