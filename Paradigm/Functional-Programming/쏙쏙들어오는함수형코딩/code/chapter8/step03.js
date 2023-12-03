function freeTieClip(cart) {
  let hasTie = false;
  let hasTieClip = false;
  for (let i = 0; i < cart.length; i++) {
    const item = cart[i];
    if (item.name === 'tie') {
      hasTie = true;
    }
    if (item.name === 'tie clip') {
      hasTieClip = true;
    }
  }

  if (hasTie && !hasTieClip) {
    const tieClip = makeItem('tie clip', 0);
    return addItem(cart, tieClip);
  }
  return cart;
}

function addItem(cart, item) {
  return addElementLast(cart, item);
}

function removeItemByName(cart, name) {
  let idx = indexOfItem(cart, name);
  if (idx !== null) {
    return removeItems(cart, idx, 1);
  }
  return cart;
}

function indexOfItem(cart, name) {
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      return i;
    }
  }
  return null;
}

function calcTotal(cart) {
  let total = 0;
  for (let i = 0; i < cart.length; i++) {
    const item = cart[i];
    total += item.price;
  }
  return total;
}

function setPriceByName(cart, name, price) {
  const cartCopy = cart.slice();
  for (let i = 0; i < cartCopy.length; i++) {
    if (cartCopy[i].name === name) {
      cartCopy[i] = setPrice(cartCopy[i], price);
    }
  }
  return cartCopy;
}

function cartTax(cart) {
  return calcTax(calcTotal(cart));
}

function getsFreeShipping(cart) {
  return calcTotal(cart) >= 20;
}