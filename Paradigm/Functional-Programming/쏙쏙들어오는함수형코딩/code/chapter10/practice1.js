function incrementFieldByName(cart, name, field) {
  const item = cart[item];
  const value = item[field];
  const newValue = value + 1;
  const newItem = objectSet(item, field, newValue);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}