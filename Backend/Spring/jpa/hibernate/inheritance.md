# @Inheritance 

- `default`는 `SINGLE_TABLE`로 지정되어 있다.

```java
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
public @interface Inheritance {
  InheritanceType strategy() default InheritanceType.SINGLE_TABLE;
}
```

## 종류

### JOINED

- 엔티티 각각을 모두 테이블로 만들고, 자식 테이블이 부모 테이블의 기본 키를 받아서 기본 키 + 외래 키로 사용하는 전략

### SINGLE_TABLE

- 테이블 하나만 사용하고, 구분 칼럼(`DTYPE`)을 통해 자식 데이터를 구분하는 전략

### TABLE_PER_CLASS

- 구현 클래스마다 테이블 전략
- 일반적으로 추천하지 않는 전략


# 참고자료

- 자바 ORM 표준 JPA 프로그래밍, 김영한 지음

