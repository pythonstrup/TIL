# Entity Mapping

### JPA는 여러 매핑 어노테이션을 지원한다.

- 객체와 테이블 매핑: `@Entity`, `@Table`
- 기본 키 매핑: `@Id`
- 필드와 컬럼 매핑: `@Column`
- 연관관계 매핑: `@ManyToOne`, `@OneToMany`, `@ManyToMany`, `@JoinColumn`

## 1. @Entity

- JPA에서 테이블과 매핑할 클래스는 `@Entity` 어노테이션을 필수로 붙여야 한다.
  - `@Entity`가 붙은 클래스는 JPA가 관리하며, 이를 엔티티라 부른다.

### @Entity의 속성

- `@Entity`는 `name` 값을 가진다.
  - default는 클래스 이름 그대로다.
  - 만약 다른 패키지에 이름이 같은 엔티티 클래스가 있다면 이름을 지정해서 충돌하지 않도록 해야 한다.

### 주의사항

- 기본 생성자는 필수다. (파라미터가 없는 public 혹은 protected 생성자)
- final 클래스, enum, interface, inner 클래스에는 사용할 수 없다.
- 저장할 필드에 final을 사용하면 안 된다.

<br/>

## 2. @Table

- `@Table`은 엔티티와 매핑할 테이블을 지정한다.

### @Table의 속성

- `name`: 매핑할 테이블 이름. 생략하면 엔티티 이름을 사용한다. (default)
- `catalog`: catalog 기능이 있는 데이터베이스에서 catalog를 매핑한다.
- `schema`: schema 기능이 있는 데이터베이스에서 schema를 매핑한다.
- `uniqueConstraints(DDL)`: DDL 생성 시에 유니크 제약 조건을 만든다. 2개 이상의 복합 유니크 제약조건도 가능.

<br/>

## 3. 스키마 자동 생성

- 아래와 같이 `application.yaml`이나 `properties`, `xml` 파일을 통해 jpa의 설정에서 ddl-auto 설정을 할 수 있다.

```yaml
spring:
  jpa:
    hibernate:
      ddl-auto: create
```

### ddl-auto 속성

- `create`: 기존 테이블을 삭제하고 새로 생성한다. (DROP + CREATE)
- `create-drop`: create 속성에 추가로 애플리케이션을 종료할 때 생성한 DDL을 제거한다. (DROP + CREATE + DROP)
- `update`: 데이터베이스 테이블과 엔티티 매핑정보를 비교해서 변경 사항만 수정한다.
- `validate`: 데이터베이스 테이블과 엔티티 매핑 정보를 비교해서 차이가 있으면 경고를 남기고 애플리케이션을 실행하지 않는다.
- `none`: 자동 생성 기능을 사용하지 않을 때 사용한다. ddl-auto 설정을 아예 하지 않아도 `none`처럼 동작한다.

<br/>

## 4. 기본 키 매핑

### JPA에서 제공하는 기본 키 생성 전략은 다음과 같다.

- **직접 할당**: 기본 키를 애플리케이션에서 직접 할당한다.
- **자동 생성**: 대리 키 사용 방식
  - `IDETITY`: 기본 키 생성을 데이터베이스에 위임
  - `SEQUENCE`: 데이터베이스 시퀀스를 사용해 기본 키를 할당
  - `TABLE`: 키 생성 테이블을 사용

> 참고
> - `자연 키 natural key`: 기본 키만을 위한 데이터가 아니라 비즈니스 모델에서 자연스레 나오는 속성으로 기본 키를 정한다고 해서 자연키라고 부른다.
> - `대체 키 (혹은 대리 키) surrogate key`: 비즈니스적인 의미가 없는 키. 인위적으로 추가하는 키. 임의의 식별자 값. (ex. 정수형 autoIncrement 식별자)

- 자동 생성 전략이 다양한 이유는 데이터베이스 벤더마다 지원하는 방식이 다르기 때문이다.
  - MYSQL의 경우 SEQUENCE를 제공하지 않고 AUTO_INCREMENT 기능만 제공한다.
  - 따라서 SEQUENCE나 IDENTITY 전략은 사용하는 데이터베이스에 의존한다.
- 기본 키를 직접 할당하려면 `@Id`만 달아주면 된다.
- 반면 자동 생성 전략을 사용하려면 `@GeneratedValue`를 추가하고 원하는 키 생성 전략을 선택하면 된다.

### 기본 키 직접 할당 전략

- `@Id`가 적용 가능한 자바 타입은 아래와 같다.
  - 자바 기본형
  - 자바 래퍼형
  - `String`
  - `java.util.Date`
  - `java.sql.Date`
  - `java.math.BigDecimal`
  - `java.math.BigInteger`
- 기본 키 직접 할당 전략에서 식별자 값 없이 저장하면 예외가 발생하니 주의하자.
  - 예외에 대한 JPA 표준이 정해져있지 않다.
  - Hibernate를 사용하면 `jakarta.persistence.PersistenceException` 예외가 발생한다. 내부에 하이버네이트 예외인 `org.hibernate.id.IdentifierGenerationException` 예외를 포함하고 있다.

### IDENTITY 전략

- 기본 키 생성을 데이터베이스에 위임하는 전략이다.
- 주로 MYSQL, PostgreSQL, SQL Server(MSSQL), DB2에서 사용한다.

```java
@Entity
public class Member {
  
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;
}
```

> IDENTITY 전략과 최적화
> - 엔티티에 식별자 값을 할당하려면 JPA는 추가로 데이터베이스를 조회해야 한다. (채번이 필요하기 때문에)
> - JDBC3에 추가된 Statement.getGeneratedKeys()를 사용하면 데이터를 저장하면서 동시에 생성된 기본 키 값도 얻어올 수 있다.

> 주의사항
> - 엔티티가 영속 상태가 되려면 식별자가 반드시 필요하다.
> - 그런데 IDENTITY 식별자 생성 전략은 엔티티를 데이터베이스에 저장해야 식별자를 구할 수 있으므로 em.persist()를 호출하는 즉시 INSERT SQL이 데이터베이스에 전달된다.
> - 따라서 이 전략은 쓰기 지연이 동작하지 않는다.
> - @Transaction(readOnly = true)를 걸어놔도 JpaRepository의 `save()` (혹은 EntityManger의 `em.persist()`)를 호출하면 그냥 저장이 되어버리니 주의하자.
 
### SEQUENCE 전략

- 데이터베이스 시퀀스는 유일한 값을 순서대로 생성하는 특별한 데이터베이스 오브젝트다.
- 이 전략은 시퀀스를 지원하는 Oracle, PostgreSQL, DB2, H2 데이터베이스에서 사용할 수 있다.
- 이 전략을 사용하기 전 시퀀스를 생성해야 한다.

```sql
CREATE TABLE BOARD (
    ID BIGINT NOT NULL PRIMARY KEY,
    DATA VARCHAR(255)
)

# 시퀀스 생성
CREATE SEQUENCE BOARD_SEQ START WITH 1 INCREMENT BY 1;
```

- 그리고 시퀀스를 매핑해야 한다.

```java
@Entity
@SequenceGenerator(
    name = "BOARD_SEQ_GENERATOR", // 시퀀스 생성기 이름 등록
    sequenceName = "BOARD_SEQ", // 매핑할 데이터베이스 시퀀스 이름
    initialValue = 1,
    allocationSize = 1
) // JPA는 위 설정을 통해 BOARD_SEQ 시퀀스와 BOARD_SEQ_GENERATOR 라는 시퀀스 생성기와 매핑한다. 
// 이제부터 id 식별자 값은 BOARD_SEQ_GENERATOR 시퀀스 생성기가 할당한다.
public class Board {
  
  @Id
  @GeneratedValue(strategy = GenerationType.SEQUENCE,
                  generator = "BOARD_SEQ_GENERATOR")
  private Long id;
}
```

### SEQUENCE와 IDENTITY 방식과의 차이

- `SEQUENCE`는 `IDENTITY` 전략과 유사하지만 내부 동작 방식은 다르다.
- `SEQUENCE`
  - `em.persist()`를 호출할 때 먼저 데이터베이스 시퀀스를 사용해 식별자를 조회한다.
  - 그리고 조회한 식별자를 엔티티에 할당한 후에 엔티티를 영속성 컨텍스트에 저장한다.
- `IDENTITY`
  - `SEQUENCE`랑 순서가 반대다.  
  - 엔티티를 먼저 데이터베이스에 저장한 후 식별자를 조회해서 엔티티의 식별자에 할당한다.

### @SequenceGenerator의 속성

- `name`: 식별자 생성기 이름. 필수 값이다.
- `sequenceName`: 데이터베이스에 등록되어 있는 시퀀스 이름 (기본값 hibernate_sequence)
- `initialValue`: DDL 생성 시에만 사용. 시퀀스 DDL을 생성할 때 처음 시작하는 수. (기본값 1)
- `allocationSize`: 시퀀스 한 번 호출에 증가하는 수(성능 최적화에 사용됨). (기본값 50)
- `catalog`: 데이터베이스 catalog 이름
- `schema`: 데이터베이스 schema 이름

<br/>

### TABLE 전략

- 키 생성 전용 테이블을 하나 만들고 여기에 이름과 값으로 사용할 컬럼을 만들어 데이터베이스 시퀀스를 흉내내는 전략이다.
- 먼저 테이블을 만든다. (sequence_name은 시퀀스 이름으로 사용하고, next_val 컬럼을 시퀀스 값으로 사용한다.)

```sql
CREATE TABLE MY_SEQUENCE (
    sequence_name varchar(255) not null,
    next_val bigint,
    primary key (sequence_name)
)
```

- 매핑을 해줘야 한다.

```java
@Entity
@TableGenerator(
    name = "BOARD_SEQ_GENERATOR", // 시퀀스 생성기 이름 등록
    sequenceName = "MY_SEQUENCE", // 매핑할 키 생성용 테이블
    pkColumnValue = "BOARD_SEQ",  // 실제 사용할 이름 (테이블의 sequence_name에 이름으로 들어간다.)
    allocationSize = 1
) // JPA는 위 설정을 통해 MY_SEQUENCE 키 생성용 테이블과 BOARD_SEQ_GENERATOR 라는 시퀀스 생성기와 매핑한다. 
// 이제부터 id 식별자 값은 BOARD_SEQ_GENERATOR 시퀀스 생성기가 할당한다.
public class Board {
  
  @Id
  @GeneratedValue(strategy = GenerationType.TABLE,
                  generator = "BOARD_SEQ_GENERATOR")
  private Long id;
}
```

- 시퀀스 오브젝트를 사용하지 않고 테이블을 사용한다는 것을 제외하고는 `SEQUENCE` 방식과 내부 동작이 거의 같다.

### @TableGenerator의 속성

- `name`: 식별자 생성기 이름. 필수 값이다.
- `table`: 키 생성 테이블명 (기본값: hibernate_sequences)
- `pkColumnName`: 시퀀스 컬럼명 (기본값: sequence_name)
- `valueColumnName`: 시퀀스 값 컬럼명 (기본값: next_val)
- `pkColumnValue`: 키로 사용할 값 이름 (기본값: 엔티티 이름)
- `initialValue`: 초기 값. 마지막으로 생성된 값이 기준. (기본값: 0)
- `allocationSize`: 시퀀스 한 번 호출에 증가하는 수(성능 최적화에 사용됨). (기본값: 50)
- `catalog`: 데이터베이스 catalog 이름
- `schema`: 데이터베이스 schema 이름
- `uniqueConstraints(DDL)`: 유니크 제약 조건을 지정

### AUTO 전략

- 데이터베이스 방언에 따라 `IDENTITY`, `SEQUENCE`, `TABLE` 전략 중 하나를 자동으로 선택하는 전략
- `@GeneratedValue.strategy`의 default 전략이다.
- 키 생성 전략이 확정되지 않은 개발 초기 단계나 프로토타입 개발 시 편리하게 사용!
- `AUTO`를 선택했을 때, `SEQUENCE`나 `TABLE`이 선택되면 시퀀스나 키 생성용 테이블을 미리 만들어둬야 한다.

### 권장하는 식별자 선택 전략

1. null 값은 허용하지 않는다.
2. 유일해야 한다.
3. 변해선 안 된다.

<br/>

## 5. 필드와 컬럼 매핑: 레퍼런스

- 필드와 컬럼을 매핑해주는 어노테이션
  - `@Column`: 컬럼을 매핑
  - `@Enumerated`: 자바의 enum 타입을 매핑
  - `@Temporal`: 날짜 타입을 매핑
  - `@Lob`: BLOB, CLOB 타입을 매핑
  - `@Transient`: 특정 필드를 데이터베이스에 매핑하지 않는다.
- 기타
  - `@Access`: JPA가 엔티티에 접근하는 방식을 지정

### @Column

- 객체 필드를 테이블 칼럼에 매핑하는 어노테이션
- 아래 3가지를 제외하고선 DDL 생성을 위한 속성이다.
  - `name`: 필드와 매핑할 테이블의 컬럼 이름
  - `insertable`: **거의 사용하지 않는다.** 엔티티 저장 시 이 필드도 같이 저장한다. false로 설정하면 이 필드는 데이터베이스에 저장하지 않는다. false 옵션은 읽기 전용일 때 사용한다.
  - `updateable`: **거의 사용하지 않는다.** 엔티티 수정 시 이 필드도 같이 수정한다. false로 설정하면 데이터베이스에 수정하지 않는다. false는 읽기 전용일 때 사용한다.
- 원시값(int, long 등)에 `@Column`을 생략했을 때 DDL이 자동으로 not null로 생성된다. 반면 래퍼클래스는 nullable로 생성된다. 

### @Enumerated

- 속성
  - `value`: `EnumType.ORDINAL`은 enum 순서를 데이터베이스에 저장한다. 반면 `EnumType.STRING`은 enum 이름을 데이터베이스에 저장한다. 기본값은 ORDINAL이다.
- ORDINAL 사용은 지양하자. ENUM 순서를 변경하게 되면 끔찍한 사이드 이펙트를 초래할 수 있다.

### @Temporal

- 날짜 타입(java.util.Date, java.util.Calendar)을 매핑할 때 사용한다.
- 속성은 `value` 하나 있다.
  - `TemporalType.DATE`: 날짜. Date 타입에 매핑
  - `TemporalType.TIME`: 시간. Time 타입에 매핑
  - `TemporalType.TIMESTAMP`: 날짜와 시간. Timestamp 타입에 매핑
- `@Temporal`을 생략하면 timestamp로 정의된다. (사실 방언에 따라 다름)
  - datetime: MySQL
  - timestamp: H2, Oracle, PostgreSQL

### @Lob

- 필드 타입이 문자면 CLOB 나머지는 BLOB으로 매핑한다.
  - CLOB: String, char[], java.sql.CLOB
  - BLOB: byte[], java.sql.BLOB

```java
@Lob
private String lobString;

@Lob
private byte[] lobByte;
```

### @Transient

- 이 필드느느 매핑하지 않는다. 데이터베이스에 저장하지 않고 조회하지도 않는다.
- 객체에 임시로 어떤 값을 보관하고 싶을 때 사용한다.

```java
@Trasient
private Integer myInt;
```

### @Access

- JPA가 엔티티 데이터에 접근하는 방식을 지정한다.
  - 필드 접근: `AccessType.FILED`로 지정한다. 필드에 직접 접근한다. 필드 접근 권한이 private이어도 접근할 수 있다.
  - 프로퍼티 접근: `AccessType.PROPERTY`로 지정한다. 접근자(Getter)를 사용한다.

- 필드 접근 예시

```java
@Entity
@Access(AccessType.FIELD)
public class Member {
  
  @Id
  private String id;
  
  private String name;
}
```

```java
@Entity
@Access(AccessType.PROPERTY)
public class Member {
  
  private String id;
  private String name;
  
  @Id
  public String getId() {
    return id;
  }
  
  @Column
  public String getName() {
    return name;
  }
}
```

- 필드 접근과 프로퍼티 접근을 함께 사용하는 경우도 있다.

<br/>

## 6. DDL 생성 기능

### DDL이 생성될 때만 영향을 주는 기능들

- 아래 기능은 DDL 자동 생성을 사용하지 않고 직접 만든다면 사용할 이유가 없다.
- @Column 매핑 정보의 `nullable`과 `length`는 DDL을 자동 생성할 때만 사용되고 JPA의 실행 로직에는 전혀 영향을 주지 않는다.

```java
@Entity
public class Member {
  
  @Id
  private String id;
  
  @Column(nullable = false, length = 10)
  private String username;
}
```

- `@Table`의 uniqueConstraints 속성 또한 DDL을 자동 생성할 때만 사용한다.

```java
@Entity
@Talbe(uniqueConstraints = {
    @UniqueContraint(name = "UNIQUE_MEMBER_USERNAME_AGE", columnNames = {"USERNAME", "AGE"})
})
public class Member {
  @Id
  private String id;
  
  private String username;
  
  private Integer age;
}
```

<br/>

# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음
