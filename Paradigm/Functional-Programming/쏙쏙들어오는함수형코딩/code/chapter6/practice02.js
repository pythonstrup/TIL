// 1. 읽기와 쓰기로 분리
function lastElement(array) {
  return array[array.length - 1];
}
function dropLast(array) {
  const copy = array.slice();
  copy.pop();
  return copy;
}

// 2. 값 2개를 리턴
function pop(array) {
  const copy = array.slice();
  const last = copy.pop();
  return {
    last,
    array: copy,
  };
}