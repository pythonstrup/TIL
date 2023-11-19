function objectDelete(object, key) {
  const copy = Object.assign({}, object);
  delete copy[key];
  return copy;
}