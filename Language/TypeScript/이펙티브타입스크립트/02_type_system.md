# 2장 타입스크립트의 타입 시스템

## item06. 편집기를 사용하여 타입 시스템 탐색하기

- 타입스크립트로 실행할 수 있는 것 2가지
  - `tsc`: 컴파일러
  - `tsserver`: 단독으로 실행할 수 있는 타입스크립트 서버
- 편집기에서 타입스크립트 언어 서비스를 적극적으로 활용하기 바람.

----

## item07. 타입이 값들의 집합이라고 생각하기

- `타입` = 할당 가능한 값들의 집합
  - `null`이나 `undefined`는 `strictNullChecks` 설정에 따라 타입에 해당되거나 아닐 수 있다.
- 가장 작은 집합 = `never`. 아무 값도 포함하지 않는 공집합.
- 그 다음으로 작은 값은 `유닛 타입 unit type`이라고도 불리는 `리터럴 타입 literal type`

```typescript
type A = 'A';
type B = 'B';
type Twelve = 12;
```

- 여러 개를 묶는 `유니온 타입 union type`

```typescript
type AB = 'A' | 'B';
```

```typescript
const a: AB = 'A'; // 가능
const b: AB = 'C'; // error
```

- 특정 상황에서만 추가 속성을 허용하지 않는 `잉여 속성 체크 excess property checking` (item11)
- 교집합(intersection) => `&`로 표현
  - 아래 예시에서 `PersonSpan`이 `never`로 예상할 수 있겠지만, 타입 연산자는 인터페이스의 속성이 아닌, 값의 집합에 적용된다.

```typescript
interface Person {
  name: string;
}
interface Lifespan {
  birth: Date;
  death?: Date;
}
type PersonSpan = Person & Lifespan;
```

- 아래 타입은 교집합에 속한다고 친다.
  - 물론 더 많은 속성을 가진 값도 `PersonSpan`에 속한다.

```typescript
const ps: PersonSpan = {
  name: 'Alice',
  birth: new Date('1970-01-01'),
  death: new Date('2020-01-01'),
};
```

- 하지만 두 인터페이스의 유니온에서는 그렇지 않다. (`|`)

```typescript
type K = keyof (Person | Lifespan); // never
```

- 앞의 유니온 타입에 속하는 값은 어떠한 키도 없기 때문에, 유니온에 대한 `keyof`는 `never`여야만 한다.
- 정확히는 아래와 같다.

```typescript
keyof (A&B) = (keyof A) | (keyof B)
keyof (A|B) = (keyof A) & (keyof B)
```

- 타입이 집합이라는 관점에서 `extends`의 의미는 '~에 할당 가능한', `~의 부분 집합'이라는 의미로 받아들일 수 있다.
  - `서브타입 subtype`
- 아래 예시의 2개 코드는 사실 똑같다고 볼 수 있다.
  - 집합이 다르지 않기 때문이다.

```typescript
interface Vector1D { x: number; }
interface Vector2D extends Vector1D { y: number; }
interface Vector3D extends Vector2D { z: number; }
```

```typescript
interface Vector1D { x: number; }
interface Vector2D { x: number; y: number; }
interface Vector3D { x: number; y: number; z: number; }
```

- `Exclude`를 통해 일부 타입을 제외하는 것도 가능하다.

```typescript
type T = Exclude<string|Date, string|number>; // 타입은 Date
type NonZeroNumber = Exclude<number, 0>; // 타입은 number
```

---

## item08. 타입 공간과 값 공간의 심벌 구분하기

- `symbol`은 타입 공간이나 값 공간 중의 한 곳에 존재
- 아래 2개의 `Cylinder`는 각각 다르다. `타입 type`과 `값 value` 이라는 개념 => 이런 점이 에러를 야기

```typescript
interface Cylinder {}
const Cylinder = () => {};
```

- `class`와 `enum`은 상황에 따라 타입과 값 두 가지 모두 가능한 예약어다.

```typescript
class Cylinder {
  radius = 1;
  height = 1;
}

function calucateVolume(shape: unknown) {
  if (shape instanceof Cynlinder) {
    shape         // 정상, 타입은 Cylinder
    shape.radius  // 정상, 타입은 number
  }
}
```

- 클래스가 타입으로 쓰일 때는 형태(property & method)가 사용되는 반면, 값으로 쓰일 때는 생성자가 사용된다.
  - 아래는 값으로 쓰인 예시

```typescript
const p2 = new Person("김철수");

// Person 생성자 함수 자체를 다른 변수에 할당 (값처럼 사용)
const PersonFactory = Person;
const p3 = new PersonFactory("이영희");
```

- 연산자 중에서도 타입에서 쓰일 때와 값에서 쓰일 때 다른 기능을 하는 것들이 있다.
- ex) `typeof`
  - 타입의 관점에서는 값을 읽어서 타입을 반환
  - 값의 관점에서는 자바스크립트 런타임의 `typeof` 연산자 => 심벌의 런타임 타입을 가리키는 문자열을 반환

```typescript
type T1 = typeof p;      // 타입은 Person
type T2 = typeof email;  // (p: Person, subject: string, body: string) => Response

const v1 = typeof p;     // 값은 "object"
const v2 = typeof email; // 값은 "function"
```

- 아래와 같이 `InstanceType` 제네릭을 사용해 생성자 타입과 인스턴스 타입을 전환할 수 있다.

```typescript
type C = InstanceType<typeof Cylinder>; // 타입이 Cylinder
```

- 속성 접근자인 `[]`는 타입으로 쓰일 때에도 동일하게 동작.
  - 그러나 `obj['field]`와 `obj.field`는 값이 동일하더라도 타입은 다를 수 있다.
  - 따라서 속성을 얻을 때는 반드시 전자의 방법(`obj['field]`)을 사용해야 한다.

```typescript
const first: Person['first'] = p['first']; // 또는 p.first
```

- 타입스크립트 코드가 잘 동작하지 않는다면 타입 공간과 값 공간을 혼동해서 잘못 작성했을 가능성이 크다.

```typescript
function email(options: {person: Person, subject: string, body: string}) {
  // ...
}
```

- 자바스크립트에서는 객체 내의 각 속성을 로컬 변수로 만들어 주는 구조 분해 할당을 사용할 수 있다.

```javascript
function email({person, subject, body}) {
  // ...
}
```

- 그런데 타입스크립트에서 구조 분해 할당을 하면, 이상한 오류가 발생한다.
  - 값의 관점에서 `Person`과 `string`이 해석되었기 때문이다.

```typescript
fucntion email({
  person: Person,   
       // ~~~~~~~ 바인딩 요소 'Person'에 암시적으로 'any' 형식이 있습니다. 
  subject: string,  
        // ~~~~~~ 'string' 식별자가 중복되었습니다.
        //        바인딩 요소 'string'에 암시적으로 'any' 형식이 있습니다.
  body: string
     // ~~~~~~ 'string' 식별자가 중복되었습니다.
     //        바인딩 요소 'string'에 암시적으로 'any' 형식이 있습니다.
}) {...}
```

- 따라서 타입스크립트에서는 아래와 같이 타입과 값을 구분해줘야 한다.

```typescript
function email({person, subject, body}: { person: Person, subject: string, body: string }) {
  // ...
}
```

---

## item09. 타입 단언보다는 타입 선언을 사용하기

- 타입 선언

```typescript
const alice: Person = {name: 'Alice'};
```

- 타입 단언

```typescript
const bob = {name: 'Bob'} as Person;
```

- 타입 선언은 할당되는 값이 인터페이스를 만족하는지 검사한다.
  - 하지만 타입 단언은 타입을 강제로 지정해서 타입 체커에게 오류를 무시하라고 하는 것이다.
  - 속성을 추가에서도 마찬가지다.
- `null`이 아니라고 단언하고 싶다면 `!`를 사용할 수 있다.
  - 다만 확신할 수 있을 때 사용해야 한다.

```typescript
const el = documnet.getElementById('foo')!;
```

- 만약 서브 타입이 아닌 것을 변환하고 싶다면 어떻게 해야하는가?
  - `unknown` 타입을 사용해볼 수 있다. `unknown` 타입은 모든 타입의 서브타입이기 때문이다.

```typescript
const el = document.body as unknown as Person;
```

---

## item10. 객체 래퍼 타입 피하기

- 객체 래퍼 타입의 자동 변환은 종종 이상하게 동작하는 것처럼 보일 때가 있음.
  - ex) 어떤 속성을 기본형에 할당하면 그 속성이 사라진다.

```typescript
> x = 'hello';
> x.language = 'English';
'English'
> x.language;
undefined
```

- 위 예시의 동작
1. `x`가 `String` 객체로 변환
2. `language` 속성을 추가
3. `language` 속성이 추가된 객체가 버려짐.

- `String`을 사용할 때는 특히 유의해야 한다.
  - `string`은 `String`에 할당할 수 있지만, `String`은 `string`에 할당할 수 없다.
  - 타입스크립트가 제공하는 타입 선언은 전부 기본형 타입으로 되어 있다.

---

## item11. 잉여 속성 체크의 한계 인지하기

- 타입이 명시된 변수에 객체 리터럴을 할당할 때 타입스크립트는 해당 타입의 속성이 있는지, 그리고 '그 외의 속성은 없는지' 확인한다.
- 구조적 타이핑(item04)
- `잉여 속성 체크`는 `할당 가능 검사`와는 별도의 과정이라는 것을 알아야 타입스크립트 타입 시스템에 대한 개념을 잡을 수 있다.
  - `할당 가능 검사`: A가 B가 필요한 모든 것을 가지고 있는가?
  - `잉여 속성 체크`: A가 B에 알려지지 않은 추가 속성을 가지고 있는가? => 더 엄격한 추가 검사 
    - "혹시 오타 아니야?" 또는 "이 속성 의도한 거 맞아?"
- "객체 리터럴을 직접" 사용하냐에 따라 `잉여 속성 체크`가 발동 여부가 결정된다.

```typescript
interface Room {
  numDoors: number;
  ceilingHeightFt: number;
}

const r: Room = {
  numDoors: 2,
  ceilingHeightFt: 10,
  elephant: 'present',
//~~~~~~~~~~~~~~~~~~~ 개체 리터럴은 알려진 속성만 지정할 수 있으며 'Room' 형식에 'elephant'이(가) 없습니다.
}
```

- 반면 임시 변수를 만들어 재할당하는 식으로 하면 `잉여 속성 체크`를 하지 않기 때문에 오류가 사라지게 된다.

```typescript
const r1 = {
  numDoors: 2,
  ceilingHeightFt: 10,
  elephant: 'present',
}
const r2: Room = r1; // 오류 없음
```

- 만약 `잉여 속성 체크`를 의도적으로 제거하고 싶다면 아래와 같이 인덱스 시그니처를 사용할 수 있다.

```typescript
interface Options {
  darkMode: boolean;
  [otherOpitons: string]: unknown;
}
const o: Options = { darkmode: true }; // 'm'이 소문자여도 오류 발생 X
```

---

## item12. 함수 표현식에 타입 적용하기

- 자바스크립트는 함수 `문장 statement`과 함수 `표현식 expression`을 다르게 인식한다.

```typescript
function add1(a: number, b: number): number { /*...*/ }          // statement
const add2 = function(a: number, b: number): number { /*...*/ }; // expression
const add3 = (a: number, b: number): number => { /*...*/ };      // expression
```

- 타입스크립트에서는 함수 표현식을 사용하는 것이 좋다.
  - 함수의 매개변수부터 반환값까지 전체를 함수 타입으로 선언하여 함수 표현식에 재사용할 수 있다는 장점이 있기 때문이다.

```typescript
type Add = (a: number, b: number) => number;
const add1: Add = (a, b) => a + b;
```

### 함수 타입 선언의 장점
  
1. 불필요한 코드 반복 제거

- 만약 라이브러리를 직접 만들고 있다면, 공통 콜백 함수를 위한 타입 선언을 제공하는 것이 좋다.
  - ex) React의 `MouseEventHandler` 타입

```typescript
type BinaryFn = (a: number, b: number) => number;
const add: BinaryFn = (a, b) => a + b;
const sub: BinaryFn = (a, b) => a - b;
const mul: BinaryFn = (a, b) => a * b;
const div: BinaryFn = (a, b) => a / b;
```

2. 타입 추론

- 아래 예시는 `typeof fetch`를 적용해서 타입스크립트가 `input`과 `init`의 타입을 추론할 수 있게 해준다.

```typescript
const checkedFetch: typeof fetch = async (input, init) => {
  const response = await fetch(input, init);
  if (!response.ok) {
    throw new Error('Request failed: ' + response.status);
  }
  return response;
}
```

---

## item13. 타입과 인터페이스의 차이점 알기

- `명명된 타입 named type`을 정의하는 두 가지 방법

1. `type` 키워드
2. `interface` 키워드

- 명명된 타입을 정의할 때 인터페이스 대신 클래스를 사용할 수도 있지만, 클래스는 값으로도 쓰일 수 있는 자바스크립트 런타임의 개념이다.

### 공통점

- 추가 속성에 대한 오류
- 인덱스 시그니처 사용
- 함수 타입 정의
- 제네릭 사용 가능
- 인터페이스는 타입을 확장할 수 있고, 타입은 인터페이스를 확장할 수 있음.

### 차이점

- `유니온 타입`은 있지만 유니온 인터페이스라는 개념은 없다. (`|`)
- 인터페이스는 `보강 augment`이 가능하다.
  - 이러한 속성을 `선언 병합 declaration merging`이라고 한다.

```typescript
interface State {
  name: string;
  capital: string;
}
interface State {
  population: number;
}
const wyoming: State = {
  name: 'Wyoming',
  capital: 'Cheyenne',
  population: 500_000,
}
```

### 결론

- 복잡하면 `type` 사용
- 간단한 객체 타입이라면 일관성과 보강의 관점을 고려하여 선택
  - 보강의 가능성 => 어떤 API에 대한 타입 선언을 작성해야 한다면 인터페이스를 사용, API가 변경될 때 사용자가 인터페이스를 통해 새로운 필드를 병합할 수 있어 유용하기 때문이다.
  - 하지만 프로젝트 내부적으로 사용되는 타입에 선언 병합이 발생하는 것은 잘못된 설계다. 이럴 때는 타입을 사용해야 한다.

---

## item14. 타입 연산과 제네릭 사용으로 반복 줄이기

### 타입 연산 사용

#### 1. `extends`
#### 2. 인터섹션 연산자 `&`
#### 3. 인덱싱

```typescript
interface State {
  userId: string;
  pageTitle: string;
}

type TopNavState = {
  userId: State['userId'];
  pageTitle: State['pageTitle'];
}
```

- 아래와 같이 매핑된 타입을 사용하면 좀 더 나아진다.

```typescript
type TopNavState = {
  [k in 'userId' | 'pageTitle']: State[k];
}
```

- `Pick` 사용 (제네릭)

```typescript
type Pick<T, K> = { [k in K]: T[k] };
```

```typescript
type TopNavState = Pick<State, 'userId' | 'pageTitle'>;
```

- 유니온 인덱싱을 통한 정의

```typescript
interface SaveAcition {
  type: 'save';
}
interface LoadAction {
  type: 'load';
}

type Action = SaveAcition | LoadAction;
type ActionType = Acition['type']; // 'save' | 'load'
```

- `Pick`을 사용하면 얻게 되는 `type` 속성을 가지는 인터페이스와는 다르다.

```typescript
type AcitionRec = Pick<Action, 'type'>; // { type: "save" | "load" }
```

#### 4. `typeof`

```typescript
const INIT_OPTIONS = {
  width: 800,
  height: 600,
}

type Options = typeof INIT_OPTIONS; // { width: number, height: number }
```

### 5. `ReturnType`

```typescript
function getUserInfo(name: string) {
  return {
    name,
    age: 30,
  }
}

type UserInfo = ReturnType<typeof getUserInfo>;
```

----

## item15. 동적 데이터에 인덱스 시그니처 사용하기

- 자바스크립트의 장점. 객체를 생성하는 문법이 간단.
- 특히 '인덱스 시그니처'를 명시해 유연하게 매핑을 표현할 수 있다.

```typescript
type Rocket = {[property: string]: string};
const rocket: Rocket = {
  name: 'Falcon 9',
  variant: 'v1.0',
};
```

- `[property: string]: string`라는 인덱스 시그니처의 의미
1. 키의 이름: 키의 위치만 표시하는 용도. 타입 체커에서는 사용하지 않는다.
2. 키의 타입: `string`이나 `number` 또는 `symbol`의 조합이어야 하지만, 보통은 `string`을 사용한다.
3. 값의 타입: 어떤 것이든 될 수 있다.

- 이렇게 타입 체크가 수행되면 네 가지 단점이 드러난다.
1. 잘못된 키를 포함해 모든 키를 허용. ex) 오타 문제. `name`을 `Name`으로 적을 수 있음.
2. 특정 키가 필요하지 않다. `{}`도 `Rocket` 타입이다.
3. 키미다 다른 타입을 가질 수 없다. 
4. 자동 완성 기능이 동작하지 않는다.

- 인덱스 시그니처는 동적 데이터를 표현할 때 사용된다.
  - ex) CSV 파일에서 열 이름을 받아올 때, 일반적인 상황에서는 열 이름이 무엇인지 미리 알 방법이 없다. 이때 사용.
- `연관 배열 associative array`의 경우, 객체에 인덱스 시그니처를 사용하는 대신 `Map` 타입을 사용하는 것을 고려할 수 있다. (item58)
- 어떤 타입에 가능한 필드가 제한되어 있는 경우라면 인덱스 시그니처로 모델링하지 말아야 한다.
- `string` 타입이 너무 광범위해서 인덱스 시그니처를 사용하는 데 문제가 있다면, 두 가지 다른 대안을 생각해볼 수 있다.

### 1. `Record`

- 키 타입에 유연성을 제공하는 제네릭 타입

```typescript
type Vec3D = Record<'x' | 'y' | 'z', number>;
```

### 2. 매핑된 타입을 사용하는 방법

```typescript
type Vec3D = {[k in 'x' | 'y' | 'z']: number}; 
```

---

## item16. `number` 인덱스 시그니처보다는 `Array`, 튜플, `ArrayLike`를 사용하기

- 이상하게 동작하기로 악명 높은 것 => 암시적 타입 강제와 관련된 부분

```typescript
"0" == 0 // true
```

- 자바스크립트에는 자바에 있는 `해시 가능 객체`라는 표현이 없다.
  - 따라서 복잡한 객체를 키로 사용하려고 하면, `toString` 메소드가 호출되어 객체가 문자열로 변환된다.

```typescript
x[[1, 2, 3]] = 2;
console.log(x); // { '1,2,3': 1 } 
```

- 특히 숫자는 키로 사용할 수 없다. -> 숫자를 사용하더라도 자바스크립트 런타임에서 문자열로 뱉어 버린다.
- 타입스크립트는 이러한 혼란을 바로잡기 위해 숫자 키를 허용하고, 문자열 키와 다른 것으로 인식한다.
  - ex) `Array`에 대한 타입 선언

```typescript
interface Array<T> {
  // ...
  [n: number]: T;
}
```

- 객체에서 인덱스에 신경쓰지 않는다면 `for-of` 구절을 쓰는 것이 좋다.
  - `for-in`을 통해 키를 순회하는 것은 배열을 순회하기에 좋은 방법은 아니라고 함.

```typescript
for (const x of xs) {
  x;
}
```

- 만약 인덱스의 타입이 중요하다면, `Array.prototype.forEach`를 사용하면 된다.
- 루프를 중간에 멈춰야 한다면, `for(;;)` 루프를 사용하는 것이 좋다.
  - 타입이 불확실하다면, `for-in` 루프는 `for-of` 또는 `for(;;)` 루프에 비해 몇 배나 느리다.
- 어떤 길이를 가지는 배열과 비슷한 형태의 튜플을 사용하고 싶다면 타입스크립트의 `ArrayLike`를 사용한다.
  - key는 여전히 문자열이라는 점을 잊지 말아야 한다.

```typescript
function checkedAddress<T>(xs: ArrayLike<T>, i: number) {
  if (i < xs.length) {
    return xs[i];
  }
  throw new Error("...");
}
```

---

## item17. 변경 관련된 오류 방지를 위해 `readonly` 사용하기

- 만약 배열을 파라미터로 받았을 때, 그 배열을 변경하지 않고 싶다는 생각이 들어 `readonly` 키워드를 사용했다고 가정해보자.
  - 하지만 아래 코드에서는 에러가 발생할 것이다.

```typescript
function arraySum(arr: readonly number[]) {
  let sum = 0, num;
  while ((num = arr.pop()) !== undefined) {
                // ~~~~~~ 'readonly number[]' 형식에 'pop' 속성이 없습니다.
    sum += num;
  }
  return sum;
}
```

- `readonly number[]`는 `number[]`와 구분되는 몇 가지 특징이 있다.

1. 배열의 요소를 읽을 수 있지만, 쓸 수는 없다.
2. `length`를 읽을 수 있지만, 바꿀 수 없다.
3. 배열을 변경하는 pop을 비롯한 다른 메소드를 호출할 수 없다.

- 따라서 `number[]`의 기능이 더 많기 때문에 `number[]`는 `readonly number[]`의 서브 타입이다.
- 매개변수를 `readonly`로 선언하면 다음과 같은 일이 생긴다.

1. 타입스크립트는 매개변수가 함수 내에서 변경이 일어나는지 체크한다.
2. 호출하는 쪽에서 함수가 매개변수를 변경하지 않는다는 보장을 받게 된다.
3. 호출하는 쪽에서 함수에 `readonly` 배열을 매개변수로 넣을 수도 있다.

- 앞의 예제의 arraySum을 고치는 방법은 간단하다. 배열을 변경하지 않도록 하면 된다.

```typescript
function arraySum(arr: readonly number[]) {
  let sum = 0;
  for (const num of arr) {
    sum += num;
  }
  return sum;
}
```

- 만약 함수가 매개변수를 변경하지 않는다면 `readonly`로 선언해야 한다.
  - 더 넓은 타입으로 호출할 수 있고(`readonly`는 그냥 원시값의 슈퍼 타입이기 때문), 의도치 않은 변경은 방지된다.

### `readonly` 수정 가능한 값으로 바꾸는 법

- `readonly`로 받아도 스프레드 연산자를 사용하여 할당하면 수정이 가능한 값이 된다!

```typescript
const readonlyArr: readonly string[] = ['a', 'b'];
const updatableArr = [...readonlyArr];
```

- 아니면 단언문(`as`)를 통해 `readonly` 속성을 제거해버릴 수도 있다.

### `Readonly` 제네릭

```typescript
type T = Readonly<Outer>;
```

- `deep readonly` 타입이 기본적으로 지원되지 않지만, `Readonly` 제네릭을 사용하면 `deep readonly` 타입을 사용할 수 있다.
  - 그러나 제네릭은 만들기 까다롭기 때문에 라이브러리를 사용하는 것이 낫다고 한다.
  - ex) `ts-essentials`의 `DeepReadonly` 제네릭

