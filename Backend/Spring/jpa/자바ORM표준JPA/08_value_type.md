# 09 값 타입

- JPA의 데이터 타입을 대분류로 나누면 `엔티티 타입`과 `값 타입`으로 나눌 수 있다.

### 값 타입의 분류

1. 기본값 타입 basic value type
2. 임베디드 타입 embedded type (복합 값 타입)
3. 컬렉션 값 타입 collection value type

## 1. 기본값 타입

- 가장 단순한 기본값 타입

```java
@Entity
public class Member {
  @Id
  @GeneratedValue
  private Long id;
  
  // 대표적인 값 타입이다.
  private String name;
  private int age;
}
```

- `Member` 엔티티는 식별자 값도 가지고 생명주기도 있지만 값 타입인 `name`, `age` 속성은 식별자 값도 없고 생명주기도 `Member`에 의존한다.
- 따라서 `Member`가 제거되면 `name`, `age` 값도 제거된다.
- 그리고 값 타입인 공유하면 안 된다. (예를 들어, 다른 회원이 내 이름을 공유한다고 생각해보자.)

> 참고
> - 자바에서 `int`, `double`과 같은 기본 타입은 절대 공유되지 않는다.
> - 물론 `Integer`와 같은 래퍼 클래스나 `String` 같은 특수한 클래스도 있다.

<br/>

# 2. 임베디드 타입(복합 값 타입)

- 새로운 값 타입을 직접 정의해서 사용할 수 있는데, JPA에서는 이것을 임베디드 타입이라고 한다.
- 중요한 것은 직접 정의한 임베디드 타입도 `int`, `String` 처럼 값 타입이라는 점이다.

```java
@Entity
public class Member {
  @Id
  @GeneratedValue
  private Long id;
  
  @Embedded
  private Address address;
}
```

```java
@Embeddable
public class Address {
  // 매핑할 컬럼을 적어줄 수 있다.
  @Column(name = "zipcode")
  private String zipcode;
  
  private String street;
}
```

- 새로 정의한 값 타입들은 재사용할 수 있고 응집도도 아주 높다.
  - 해당 값 타입만 사용하는 의미 있는 메소드도 만들 수 있다.
- 임베디드 타입을 사용하려면 아래 2가지 어노테이션이 필요하다.
  - `@Embeddable`: 값 타입을 정의하는 곳에 표시
  - `@Embedded`: 값 타입을 사용하는 곳에 표시
- 임베디드 타입은 기본 생성자가 필수다.

### 임베디드 타입과 테이블 매핑

- 임베디드 타입은 엔티티의 값일 뿐이다. 따라서 값이 속한 엔티티의 테이블에 매핑한다.
- 임베디드 타입 덕분에 객체와 테이블을 아주 세밀하게 매핑하는 것이 가능하다. 잘 설계한 ORM 애플리케이션은 매핑한 테이블 수보다 클래스 수가 더 많다.
- ORM을 사용하지 않고 개발하면 테이블 컬럼과 객체 필드를 대부분 1:1로 매핑한다.
  - 주소와 같은 값을 값 타입 클래스로 만들어서 객체지향적으로 개발하고 싶어도 쉽지가 않다 
  - SQL을 직접 다루면 테이블 하나에 클래스 하나를 매핑하는 것도 힘든데.. 클래스를 더 늘리고 싶을까??

### 임베디드 타입과 연관관계

- 임베디드 타입은 같은 임베디드 타입을 포함하거나 엔티티를 참조할 수 있다.
- 아래는 임베디드 타입이 임베디드 타입을 포함한 예시다.

```java
@Embeddable
public class Address {
  @Embedded 
  private Zipcode zipcode;
  private String street;
}

@Embeddable
public class Zipcode {
  private String zip;
  private String plusFour;
}
```

- 아래 예시는 임베디드 타입이 엔티티를 포함한 경우다.

```java
@Embeddable
public class PhoneNumber {
  private String areaCode;
  private String localNumber;
  @ManyToOne
  private PhoneServiceProvider provider;
}

@Entity
public class PhoneServiceProvider {
  @Id
  private String name;
}
```

### @AttributeOverride: 속성 재정의

- 임베디드 타입에 정의한 매핑정보를 재정의하려면 엔티티에 `@AttributeOverride`를 사용하면 된다.
  - 다른 테이블과 달리 컬럼명이 달라 컬럼을 따로 매핑해줘야 하는 경우에 사용한다.
- 예를 들어 주소 2개를 사용한다고 했을 때 아래와 같이 임베디드 타입의 속성을 재정의할 수 있다.

```java
@Entity
public class Member {
  ...
  
  @Embedded 
  private Address homeAddress;
  
  @Embedded
  @AttributeOverrides({
      @AttributeOverride(name="city", column=@Column(name="company_city")),
      @AttributeOverride(name="street", column=@Column(name="company_street")),
      @AttributeOverride(name="zipcode", column=@Column(name="company_zipcode"))})
  private Address companyAddress;
}
```

### 임베디드 타입과 null

- 임베디드 타입이 `null`이면 매핑한 컬럼 값은 모두 `null`이 된다.
- 위의 Address가 null이면 거기에 속한 `city`, `street`, `zipcode`가 모두 null이 된다.

<br/>

## 3. 값 타입과 불변 객체

### 값 타입 공유 참조

- 임베디드 타입 같은 값 타입을 여러 엔티티에서 공유하면 위험하다.
- 회원1, 회원2가 같은 주소 객체를 참조하고 있다면 회원1에서 주소를 바꾸는 경우 회원2에서도 변경되어 버린다.

### 값 타입 복사

- 객체를 대입할 때마다 인스턴스를 복사해서 대입하면 공유 참조는 피할 수 있다.
- 문제는 복사하지 않고 원본의 참조 값을 직접 넘기는 것을 막는 강제성이 없다는 점이다.
  - 내가 복사해서 넘기는 것을 의도했다 하더라도 다른 개발자는 그렇게 하지 않을 가능성이 다분하다.

### 불변 객체

- 객체를 불변하게 만들면 값을 수정할 수 없으므로 값이 바뀌는 부작용을 원천 차단할 수 있고, 복사를 강제할 수 있다.
- 따라서 값 타입은 될 수 있으면 불변 객체(immutable Object)로 설계해야 한다.
  - `setter`를 제공하지 않는 방법
  - `final` 키워드를 사용하는 방법

<br/>

## 4. 값 타입의 비교

- 보통 동등성 비교를 통해 값을 비교할 것이다.
- 임베디드 타입에서는 `equals()`와 `hashCode()`를 오버라이딩하자!
  - 물론 부모 타입의 `equals()`나 `hashCode()`로 충분하다면 오버라이딩할 필요는 없다.

<br/>

## 5. 값 타입 컬렉션

- 값 타입을 컬렉션에 보관하려면 컬렉션 필드 값에 `@ElementCollection`, `@CollectionTable` 어노테이션을 사용하면 된다.

```java
@Entity
public class Member {
  ...
  
  @ElementCollection
  @CollectionTable(
      name = "favorite_foods",
      joinColumns = @JoinColumn(name = "member_id"))
  @Column(name = "food_name")
  private Set<String> favoriteFoods = new HashSet<>();
  
  @ElementCollection
  @CollectionTable(
      name = "address",
      joinColumns = @JoinColumn(name = "member_id"))
  private List<Address> addressHistory = new ArraryList<>();
}
```

- 관계형 데이터베이스 테이블은 컬럼 안에 컬렉션을 포함할 수 없다.
- 따라서 별도의 테이블을 추가하고 `@CollectionTable`을 사용해서 추가한 테이블을 매핑해야 한다.
- 그리고 `favoriteFoods` 처럼 값으로 사용되는 컬럼이 하나면 `@Column`을 사용해서 컬럼명을 지정할 수 있다.

### 값 타입 컬렉션 사용

- 값 타입 컬렉션은 영속성 전이와 고아 객체 제거 기능을 필수로 가지고 있다.
- 조회할 때의 페치 전략은 LAZY가 기본이다.
- 값 타입 컬렉션을 가지고 있는 엔티티를 영속화하면 엔티티 1개 + 값 타입 컬렉션의 개수만큼 INSERT가 이뤄진다.
  - N+1 문제..
- 반면 조회할 때는 값타입 컬렉션을 한 번에 조회해온다. (엔티티 1회 + 값 타입 컬렉션 1회)

### 값 타입 컬렉션의 제약 사항

- 값 타입은 식별자라는 개념이 없고 단순한 값들의 모음이므로 값을 변경해버리면 데이터베이스에 저장된 원본 데이터를 찾기 어렵다.
- 특히 값 타입 컬렉션의 경우 값이 변경되면 데이터베이스에 있는 원본 데이터를 찾기란 더욱 어렵다.
- 따라서, JPA 구현체들은 값 타입 컬렉션에 변경 사항이 발생하면 값 타입 컬렉션에 매핑된 모든 데이터를 삭제하고 현재 값 타입 컬렉션 객체에 있는 모든 값을 데이터베이스에 다시 저장한다.
- 추가로 값 타입 컬렉션을 매핑하는 테이블은 모든 컬럼을 묶어서 기본 키를 구성해야 한다.
  - 기본 키 제약으로 인해 컬럼에 null을 입력할 수 없다.
  - 값은 값은 중복해서 저장할 수 없는 제약도 있다.
- 차라리.. 일대다 관계의 새로운 엔티티를 만들자.

<br/>

## 6. 정리

- 값 타입은 정말 값 타입이라 판단될 때만 사용해야 한다.
  - 값 타입과 엔티티를 혼동해서 엔티티를 값 타입으로 만들면 안 된다.

### 엔티티의 특징

- 식별자(`@Id`)가 있다.
- 생명주기가 있다. 
  - 생성하고 영속화하고 소멸한다.
- 공유할 수 있다. 
  - 참조 값을 공유할 수 있다.

### 값 타입의 특징

- 식별자가 없다.
- 생명 주기를 엔티티에 의존한다.
- 공유하지 않는 것이 안전하다.
  - 공유되면 사이드 이펙트가 생긴다.
  - 불변 객체로 만드는 것이 좋다.

<br/>

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
