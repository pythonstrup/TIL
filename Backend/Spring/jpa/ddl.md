# DDL-AUTO

### 내가 정확히 알고 있는 설정

#### create

- 애플리케이션이 시작될 때 기존 스키마를 삭제하고 매번 새롭게 스키마를 생성한다.
- 데이터가 모두 삭제되며 엔티티 매핑을 기반으로 새로운 스키마가 생성된다. (프로덕션에서는 사용 X)

#### create-drop

- `create`와 같이 테이블을 생성해주고, 애플리케이션을 종료할 때 테이블을 drop 한다.
- 테스트 환경에서 일시적으로 데이터베이스 스키마가 필요한 경우 유용하고, 매 테스트 실행 시마다 깨끗한 데이터베이스 유지할 수 있다는 장점이 있다.
- 프로덕션 X

#### none

- 현재 테이블의 스키마를 그대로 사용.
- 스키마를 수동으로 관리할 때 사용.
- 프로덕션 환경에서 사용하는 것이 좋다.

### 잘 몰랐던 설정

#### update

- 엔티티 매핑이 데이터베이스 스키마를 비교하여 필요한 경우 스키마를 수정한다.
- 데이터는 유지되지만, 새로운 엔티티나 변경된 엔티티 필드는 스키마에 바로 반영된다.
- 자동으로 스키마를 업데이트하고 싶을 때 유용하다. 하지만 프로덕션에서는 의도하지 않은 변경이 일어날 수 있기 때문에 주의가 필요하다.

#### validate

- 애플리케이션이 시작될 때, 엔티티 매핑이 데이터베이스 스키마와 일치하는지 검증하며 스키마 변경은 하지 않는다.
- 프로덕션 환경에서 엔티티와 데이터베이스 스키마 설정이 같은지 확인할 때 사용한다.

### 운영 환경에서의 설정

- 데이터베이스 마이그레이션 도구(Flyway, Liquibase 등)를 사용해볼 수 있다.

# 참고자료

- [메일매일](https://www.maeil-mail.kr/question/28)