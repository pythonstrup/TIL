function isInCart(cart, name) {
  for (let i = 0; i < cart.length; i++) {
    if (cart[i].name === name) {
      return true;
    }
  }
  return false;
}

function freeTieClip(cart) {
  let hasTie = isInCart(cart, 'tie');
  let hasTieClip = isInCart(cart, 'tie clip');

  if (hasTie && !hasTieClip) {
    const tieClip = makeItem('tie clip', 0);
    return addItem(cart, tieClip);
  }
  return cart;
}
