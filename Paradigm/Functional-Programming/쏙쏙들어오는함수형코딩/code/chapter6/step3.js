// 읽기와 쓰기를 분리
function firstElement(array) {
  return array[0];
}

function dropFirst(array) {
  const copy = array.slice();
  copy.shift();
  return copy;
}

// 값을 두 개 리턴하는 함수
function shift(array) {
  const copy = array.slice();
  const first = copy.shift();
  return {
    first,
    array: copy,
  };
}

// 위 두 가지 방법을 조합하는 방법
function shift(array) {
  return {
    first: firstElement(array),
    array: dropFirst(array),
  };
}