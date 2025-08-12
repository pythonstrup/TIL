# 1장 타입스크립트 알아보기

- 인터프리터 방식이나 저수준 언어 컴파일 방식을 사용하는 다른 언어랑 다르다.
- 고수준 언어인 자바스크립트로 컴파일되며, 실행 역시 자바스크립트로 이뤄진다.

## item01. 자바스크립트와의 관계 이해하기

> 타입스크립트는 자바스크립트의 상위 집합이다.

> 타입스크립트는 타입이 정의된 자바스크립트의 상위집합이다.
 
- 타입스크립트는 타입 구문 없이도 오류를 잡을 수 있지만, 타입 구문을 추가한다면 훨씬 더 많은 오류를 찾아낼 수 있다.
- "타입은 코드의 의도이다."
- 타입 시스템이 정적 타입의 정확성을 보장해주지는 않는다. 애초에 그런 목적으로 만들어지지 않았다.
  - ex) `const names = ["james", "peter"]; console.log(names[5].toUpperCase());`
  - 정확성을 보장하는 언어인 Reason, Elm 같은 언어가 있다고 함

---

## item02. 설정

- `tsconfig.json`
- 타입스크립트는 대부분의 언어에서 허용하지 않는 고수준 설계의 설정을 지원한다.
- `any`를 사용하지 않는 `noImplicitAny` 설정을 추천.
  - `noImplicitAny`과 `strictNullChecks`만큼 중요한 설정은 없다.

---

## item03. 코드 생성과 타입이 관계없음을 이해하기

- 타입스크립트의 컴파일은 타입 체크와 독립적으로 동작한다.
  - 따라서 타입 오류가 있는 코드도 컴파일이 가능하다.
- 만약 오류가 있을 때 컴파일하지 않으려면 `noEmitOnError` 설정.
- 타입 연산은 런타임에 영향을 주지 않는다. (컴파일 단계의 타입 체크에서만 중요)
- 함수를 오버로드할 수 없다.
- 런타임에 오버헤드는 없지만, 빌드타임 오버헤드가 존재한다.

---

## item04. 구조적 타이핑

- 자바스크립트는 `덕 타이핑 duck typing` 기반이다. 
  - 타입스크립트를 이것을 모델링하기 위해 구조적 타이핑을 사용한다.

> #### 덕 타이핑
> - "만약 어떤 새가 오리처럼 걷고, 오리처럼 헤엄치고, 오리처럼 꽥꽥거리는 소리를 낸다면 나는 그 새를 오리라고 부를 것이다."
> - 객체의 실제 타입(자료형)이 무엇인지는 중요하지 않고, 단지 그 객체가 특정 메서드나 속성을 가지고 있는지 여부에 따라 객체의 역할을 판단하는 방식을 말한다. 
> - 즉, 객체의 '본질'보다는 '행동'에 초점을 맞추는 프로그래밍 스타일이다.

- 예를 들어, 아래와 같은 타입이 있다고 해보자.

```typescript
interface Vector2D {
    x: number;
    y: number;
}
interface Vector3D {
    x: number;
    y: number;
    z: number;
}

function calculateLength(vector: Vector2D) {
    return Math.sqrt(vector.x * vector.x + vector.y * vector.y);
}

function normalize(vector: Vector3D) {
    const length = calculateLength(vector);
    return {
        x: vector.x / length,
        y: vector.y / length,
        z: vector.z / length
    };
}
```

- 타입스크립트의 구조적 타이핑으로 인해 `calculateLength`에 `Vector3D` 타입을 넣더라고 함수가 동작한다. (x와 y를 number 필드로 가지고 있기 때문에)

----

## item05. any 타입 지양

- `any` 타입은 타입 안정성도 없고, 함수 시그니처도 무시해버린다.
  - 개발자간의 약속을 어길 수 있게 된다는 얘기다.
- 타입 설계도 감춰버리고, 타입 시스템의 신뢰도를 떨어뜨리는 아주 나쁜 녀석..

---



