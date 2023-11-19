function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}

function setQuantity(item, newQuantity) {
  return objectSet(item, "quantity", newQuantity);
}