function objectSet(object, key, value) {
  const copy = Object.assign({}, object);
  copy[key] = value;
  return copy;
}

function setPrice(item, newPrice) {
  return objectSet(item, "price", newPrice);
}