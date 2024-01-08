/**
 * 방법 1. 추상화 벽 사용
 */
function getsWatchDiscount(cart) {
  let total = 0;
  const names = Object.keys(cart);
  for (let i = 0; i < names.length; i++) {
    const item = cart[names[i]];
    total += item.price;
  }
  return total > 100 && cart.hasOwnProperty("watch");
}

/**
 * 방법 2. 추상화 벽 위에 만들기
 */
function getsWatchDiscount(cart) {
  const total = calcTotal(cart);
  const hasWatch = isInCart("watch");
  return total > 100 && hasWatch;
}