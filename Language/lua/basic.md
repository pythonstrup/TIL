# 루아 스크립트 기초 문법

## 1. 주석

```lua
-- 주석 달기
```

## 2. 변수

### 글로벌 변수

```lua
a = 3
```

### 범위 변수

- `local` 키워드를 사용하면 해당 영역에서만 유효하게 만들 수 있다.

```lua
local a = 33
```

## 3. 타입

- Java처럼 매번 타입을 선언할 필요는 없다.
- 다만 타입이 있기는 하다.
- `nil`: 빈 값. (`null`과 비슷)
- `boolean`
- `number`
- `string`
- `function`
- `table: 여러 변수들의 그룹. 클래스처럼 활용할 수 있다.

## 4. 연산자

- 수식 연산자: `+`, `-`, `*`, `/`, `%`, `^`
- 관계 연산자: `==`, `~=`, `>`, `<`, `>=`, `<=`
- 논리 연산자: `and`, `or`, `not`
- `..`: 두 문장을 붙여준다. ex) "hi".."!!"
- `#`: 배열/테이블의 개수 카운트
- 아쉽게도 `++`, `--`과 같은 연산자는 없다.

```lua
local array = {"hi", "hello", "bye"}
print(#array) 
```

## 5. 반복문

### while

```lua
while (a < 3)
do
    ~~~
end
```

### for

- 아래는 1부터 시작해서 30이 될 때까지 1을 더하는 것이다.

```lua
for i = 1, 30, 1
do
    ~~~
end
```

### repeat문

```lua
repeat
    ~~~
    a = a + 1
until(a > 30)
```

## 6. 조건문

### if-else

```lua
if (a and b)
then
    ~~~
else
    ~~~
end
```

## 7. 함수

- 함수에 `local`을 붙이지 않으면 글로벌 함수가 된다.

```lua
local function add(a, b)
    return a + b
end
```

- 변수에 함수를 할당할 수 있으며, 콜백 함수를 사용할 수 있다.

```lua
myPrinter = function(target)
    print("--", target, "--")
end

function add(a, b, printer) 
    result = a + b
    printer(result)
    return result
end

add(10, 15, myPrinter)
```

## 8. 배열

- 다른 언어와 다르게 첫번째 인덱스 값이 `1`이다.

## 9. Table 자료구조

- lua의 유일한 자료구조
- 클래스처럼 사용하기도, 배열로 사용하기도 한다.

### pairs

- Table 안의 `index`, `key index` 무관하게 전체를 보여준다.

```lua
for key, value in pairs(table) do
    ~~~
end
```

### ipairs

- 테이블 안의 `index`를 분리해서 보여준다.
 
```lua
for key, value in ipairs(table) do
    ~~~
end
```
