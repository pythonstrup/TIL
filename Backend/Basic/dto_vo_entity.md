# DTO, VO, Entity

## DTO

- 순수하게 데이터를 담아 계층 간으로 전달하는 객체
- 로직을 갖고 있지는 않는 순수한 데이터 객체이며 `getter`/`setter` 메소드만 가진다.
- `setter`가 없다면 불변 객체로 활용될 수 있지만, 존재하면 가변 객체로 활용한다.

### 사용 계층

- Client <=> Controller
- Controller <=> Service

<br/>

## VO

- 특정 값을 나타내는 객체를 의미한다.
- `DTO`와는 다르게 `getter` 메소드와 함께 비즈니즈 로직을 포함할 수 있다.
- `VO`의 경우 특정 값 자체를 표현하기 때문에 불변성의 보장을 위해 필드를 `final`로 선언하고 생성자를 사용해야 한다.

### 주요특징 1. 값 비교를 위한 `equals`와 `hashCode` 메소드의 오버라이딩이 필요하다.

- 타입도 동일하고 내부 속성값이 같은 두 객체를 동일한 객체로 취급하기 위함이다.

### 주요특징 2. `setter` 메소드를 가진다. (불변 보장)

- 속성값 자체가 식별값인 `VO`는 값이 바뀌면 다른 값이 되어 추적이 불가능하다.
- 예상치 못한 사이드 이펙트를 피하기 위해 불변 객체로 만드는 것이 좋다.

### VO를 사용함으로써 얻을 수 있는 이점

- 객체가 생성될 때 객체 안에 제약사항을 추가할 수 있다.
- `Entity`의 원시값을 `VO`로 포장해 `Entity`가 지나치게 커는 것을 막을 수 있다.

### 사용 계층

- Controller <=> Service

<br/>

## Entity

- `Entity`는 실제 DB 테이블과 매핑되는 객체다.
- 비즈니스 로직을 포함할 수 있고 가변 객체로 사용된다.

### 사용 계층

- Repository <=> Service

<br/>

## Entity와 DTO를 분리해야 하는 이유?

### 1. 관심사의 분리

- DTO와 Entity는 엄연히 다른 관심사를 가지고 있기에 분리하는 것이 합리적이다.
- DTO(Data Transfer Object)의 핵심 관심사는 이름 그대로 데이터의 전달이다.
- 반면, Entity는 핵심 비즈니스 로직을 담는 비즈니스 도메인 영역의 일부로 활용될 수 있다.

### 2. Validation 로직 및 불필요한 코드 등과의 분리

- Spring Validation의 `@Valid` 처리를 하기 위해 어노테이션(ex-`@NotBlank`)을 필드에 붙여줘야 한다.
- 그런데 Entity와 DTO가 분리되어 있지 않다면 Valid 관련된 어노테이션과 Entity 관련된 어노테이션이 하나의 클래스에 붙어야 하기 때문에 코드가 복잡해질 수 있다.
- 또 Entity 요소 중 Client에게 보여서는 안되는 필드도 존재하기 때문에 추가적으로 `@JsonIgnore`와 같은 어노테이션 필요해질 수 있다.

### 3. API 스펙의 유지

- Entity와 DTO가 분리되지 않으면 테이블에 새로운 필드가 추가되거나 필드명을 수정할 때마다 API 스펙이 변경될 것이다.
- DTO를 분리하면 API 스펙을 유지하는 것이 한결 간편해진다. 

### 4. API 스펙 파악의 용이성

- Entity가 곧 API 스펙이 되면 각 API에서 내려주는 핵심 자원이 무엇인지 파악하기 어려워진다.

## 참고자료

- [DTO vs VO vs Entity](https://tecoble.techcourse.co.kr/post/2021-05-16-dto-vs-vo-vs-entity/)

- [VO: Value Object란 무엇인가](https://tecoble.techcourse.co.kr/post/2020-06-11-value-object/)

- [DTO와 VO의 차이](https://maenco.tistory.com/entry/Java-DTO%EC%99%80-VO%EC%9D%98-%EC%B0%A8%EC%9D%B4)

- [[Spring] 엔티티(Entity) 또는 도메인 객체(Domain Object)와 DTO를 분리해야 하는 이유](https://mangkyu.tistory.com/m/192)
