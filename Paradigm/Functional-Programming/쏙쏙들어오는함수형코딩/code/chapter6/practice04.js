function push(array, elem) {
  const copy = array.slice();
  copy.push(elem)
  return copy;
}

function addContact(mailingList, email) {
  return push(mailingList, email);
}