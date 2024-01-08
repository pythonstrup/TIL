const validItemFields = ['price', 'quantity', 'shipping', 'tax'];
const translations = {'quantity': 'number'};

function setFieldByName(cart, name, field, value) {
  if (!validItemFields.includes(field)) {
    throw "Not a valid item field: " + "'" + field + "'";
  }
  if (translations.hasOwnProperty(field)) {
    field = translations[field];
  }
  const item = cart[name];
  const newItem = objectSet(item, field, value);
  const newCart = objectSet(cart, name, newItem);
  return newCart;
}