function deepCopy(thing) {
  if (Array.isArray(thing)) {
    const copy = [];
    thing.forEach(item => {
      copy.push(deepCopy(item));
    });
    return copy;
  } else if (thing === null) {
    return null;
  } else if (typeof thing === 'object') {
    const copy = {};
    Object.keys(thing).forEach(key => {
      copy[key] = deepCopy(thing[key]);
    });
    return copy;
  }

  return thing; // 문자열, 숫자, 불리언, 함수는 불변형이기 때문에 복사할 필요가 없다.
}