function userChanges.subscribe(function (user) {
  const copy = deepCopy(user);
  processUser(copy);
})