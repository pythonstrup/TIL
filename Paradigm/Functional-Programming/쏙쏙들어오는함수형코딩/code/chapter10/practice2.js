const validFiledList = ['size', 'quantity'];

function incrementFieldByName(cart, name, field) {
  if (!validFiledList.includes(field)) {
    throw  "Not a valid item field" + "'" + field + "'";
  }
  const item = cart[item];
  const value = item[field];
  const newValue = value + 1;
  const newItem = objectSet(item, field, newValue);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}