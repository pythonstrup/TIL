# Spring Envers 개요

## Spring Data Envers란?

- Spring Data JPA를 위한 저장소에서 Envers 쿼리를 사용할 수 있게 해주는 기능.
- 항상 Spring Data JPA와 함께 사용된다.

### Envers란?

- JPA 엔티티에 `auditing` 기능을 추가하는 Hibernate 모듈이다.

## 코드 예시

### DDL

- Spring Data Envers를 설정하고, JPA의 auto-ddl로 Revision 테이블을 만들면 아래와 같은 형태로 만들어준다.

```sql
create table REVINFO (
    REV integer not null auto_increment, 
    REVTSTMP bigint, 
    primary key (REV)
) engine=InnoDB
```

- 히스토리 테이블의 이름은 추적하고자 하는 엔티티 테이블에 접미사 `_AUD`가 붙은 형태이며, 필드에 `REV`와 `REVTYPE`이 들어가야 한다.
  - 여기서 문제점! `REV`가 `integer`로 되어 있어 ID로 할당할 수 있는 값이 금방 고갈될 수 있다. => 커스텀하는 것이 좋다. 뒤에서 언급.

```sql
create table review_AUD (
    REV integer not null, 
    REVTYPE tinyint, 
    id bigint not null, 
    content varchar(255) not null, 
    primary key (REV, id)
) engine=InnoDB
```

### 코드 설정하기

- 의존성

```kotlin
implementation("org.springframework.data:spring-data-envers")
```

- Configuration 파일에 `@EnableEnversRepositories`를 추가해주면 된다.

```java
@Configuration
@EnableEnversRepositories
public class JpaConfig {
  // ...
}
```

- 혹은 `@EnableJpaRepositories`의 `repositoryFactoryBeanClass`에 `EnversRevisionRepositoryFactoryBean.class`를 추가해줘도 되지만, `@EnableEnversRepositories`을 사용하는 걸 추천한다.

```java
@EnableJpaRepositories(repositoryFactoryBeanClass = EnversRevisionRepositoryFactoryBean.class)
```

- Properties 설정은 아래와 같이 해줄 수 있다. Hibernate의 고유 속성을 설정한다.
  - `org.hibernate.envers.audit_table_suffix`: 테이블 이름에 붙는 접미사 지정
  - `org.hibernate.envers.revision_field_name`: 엔티티의 변경이 발생할 때마다 `Revision` 테이블에 Row를 추가한다. 각 수정마다 고유한 식별자를 부여하는 데, 아래 예시에서는 `revision_id`로 부여된다.
  - `org.hibernate.envers.revision_type_field_name`: 해당 레코드가 어떤 종류의 변경(삽입, 수정, 삭제)인지 나타내는 변경 타입 필드의 이름을 지정한다. 아래 예시에서는 `revision_type`으로 지정된다.
  - `org.hibernate.envers.store_data_at_delete`: 삭제 시점의 데이터를 이력 테이블에 완전하게 보관하기 위한 설정이다. 만약 hard delete를 한다면 설정을 추천.

```yaml
spring:
  jpa:
    properties:
      org.hibernate.envers.audit_table_suffix: _history
      org.hibernate.envers.revision_field_name: revision_id
      org.hibernate.envers.revision_type_field_name: revision_type
      org.hibernate.envers.store_data_at_delete: true
```

### Revision 테이블 커스터마이징하기

- 위의 설정에서 Entity에 들어갈 요소명을 바꾸듯 Revision 테이블 또한 커스터마이징이 가능하다.
  - `@RevisionEntity`: Envers에서 리비전 정보를 저장하는 엔티티임을 표시
  - `@RevisionNumber`: Envers에서 리비전 번호를 저장하는 필드임을 지정
  - `@RevisionTimestamp`:  Envers에서 리비전이 생성된 시간을 저장하는 필드임을 지정

```java
@Entity
@RevisionEntity
public class RevisionHistory {

  @Id
  @RevisionNumber
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long revisionId;

  @RevisionTimestamp 
  private Long updatedAt;
}
```

- DDL은 아래와 같이 설정해볼 수 있겠다.

```sql
create table revision_history
(
    revision_id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'REVISION 식별자'
        PRIMARY KEY,
    updated_at  BIGINT NOT NULL COMMENT '수정 일시'
) engine = InnoDB
```

### 엔티티에 적용하기

- `@Audited` 어노테이션 붙여주면 끝이다.

```java
@Entity
@Audited
public class Review extends BaseEntity {
  
  private String content;
}
```

- 만약 `BaseEntity`와 같이 상속관계가 있고 부모 클래스가 가진 필드도 히스토리 테이블에 저장하고 싶다면, 부모 클래스에도 `@Audited`를 달아줘야 한다.

```java
@MappedSuperclass
@Audited
public abstract class BaseEntity {

  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;
}
```

> #### 연관 엔티티 객체가 존재한다면?
> - 연관 엔티티가 있을 때는 해당 필드에 `@NotAudited`를 달아주거나, 연관 엔티티에도 `@Audited`를 달아줘야 에러가 발생하지 않는다.

- 위 설정에 의하면 아래와 같은 DDL을 준비하면 될 것이다.

```sql
CREATE TABLE review_history
(
    revision_type TINYINT      NOT NULL COMMENT 'REVISION 타입',
    revision_id   BIGINT       NOT NULL COMMENT 'REVISION 식별자',
    id            BIGINT       NOT NULL COMMENT '리뷰 식별자',
    content       VARCHAR(255) NOT NULL COMMENT '리뷰 내용',
    primary key (id, revision_id)
)
```

### 테이블 구성

# 참고 자료

- [[Spring] spring-data-envers를 이용한 엔티티 변경 이력 관리](https://gengminy.tistory.com/60)
- [Hibernate Docs - Envers](https://hibernate.org/orm/envers/)
- [Hibernate User Guide - Envers](https://docs.jboss.org/hibernate/orm/current/userguide/html_single/Hibernate_User_Guide.html#envers)
- [Spring Data JPA Docs - Envers](https://docs.spring.io/spring-data/jpa/reference/envers.html)
- [Spring Data Envers Docs](https://docs.spring.io/spring-data/envers/docs/current/reference/html/)
