function addItem(cart, item) {
  return objectSet(cart, item.name, item);
}

function calcTotal(cart) {
  let total = 0;
  const names = Object.keys(cart);
  for (let i = 0; i < names.length; i++) {
    const item = cart[names[i]];
    total += item.price;
  }
  return total;
}

function setPriceByName(cart, name, price) {
  if (isInCart(cart, name)) {
    const item = cart[name];
    const copy = setPrice(item, price);
    return objectSet(cart, name, copy);
  }
  const item = makeItem(name, price);
  return objectSet(cart, name, item);
}

function removeItemByName(cart, name) {
  return objectDelete(cart, name);
}

function isInCart(cart, name) {
  return cart.hasOwnProperty(name);
}