# 설계

- 성장에 따른 스키마 변경
  - 사용자 증가, 기능 확장, 데이터 분석, 성능 최적화
- 데이터 무결성(정규화) vs 성능(비정규화): 둘 사이의 균형이 필요하다.
  - JOIN은 비교적 비용이 크다. 또한 샤딩이 들어가면 완벽한 정규화는 더 어려워진다.
  - 정규화가 필요한 시점은 중복 데이터가 많을 때이다.
- `Polyglot Persistence`: 각 데이터의 특성과 용도에 맞는 최적의 저장소 사용 (다른 데이터 베이스를 사용)
  - ex) CQRS 구현을 위한 Redis 읽기 전용 저장소 활용

## 실무를 위한 데이터베이스 설계 패턴

### 읽기에 최적화된 패턴

```sql
-- 게시글 테이블 (비정규화 적용)
CREATE TABLE posts
(
    -- ...
    -- 성능을 위한 비정규화 필드들
    comment_count INT DEFAULT 0,
    like_count    INT DEFAULT 0,
    view_count    INT DEFAULT 0,
    -- ...
) ENGINE = InnoDB
  DEFAULT CHARSET = utf8mb4
  COLLATE = utf8mb4_unicode_ci;

-- 댓글 테이블 (대댓글 지원)
CREATE TABLE comments
(
    comment_id        BIGINT AUTO_INCREMENT PRIMARY KEY,
    post_id           BIGINT NOT NULL,
    user_id           BIGINT NOT NULL,
    parent_comment_id BIGINT NULL,
    content           TEXT   NOT NULL,
    -- ...
) ENGINE = InnoDB
  DEFAULT CHARSET = utf8mb4
  COLLATE = utf8mb4_unicode_ci;
```

- 비정규화가 들어갔기 때문에 `comments 댓글`을 추가할 때, 아래와 같이 `posts`의 `comment_count`를 업데이트해줘야 한다.

```sql
INSERT INTO comments (post_id, user_id, content) VALUES (123, 456, 'Good!');
UPDATE posts SET comment_count = comment_count + 1 WHERE post_id = 123;
```

- 또는 트리거로 자동화하는 것도 가능하다.

```sql
DELIMITER //
CREATE TRIGGER tr_comment_count_insert
    AFTER INSERT ON comments
    FOR EACH ROW
BEGIN
    UPDATE posts
    SET comment_count = comment_count + 1
    WHERE post_id = NEW.post_id;
END //
DELIMITER ;
```

- Column이 너무 많다면, 성능을 위한 비정규화 필드를 다른 테이블로 분리하는 것이 좋을 수 있다.
- 또한 트리거보다는 애플리케이션 레벨에서 처리하는 것이 훨씬 유연하다.
  - 트리거처럼 데이터베이스 엔진에 의해 로직을 처리하는 방식은 이벤트에 대한 규격을 고정해버리기 때문이다. 유지보수 또한 어렵다.

### 데이터 분석 최적화 패턴

```sql
-- 사용자 활동 요약 테이블 (배치로 갱신)
CREATE TABLE user_activity_summary (
   summary_date DATE NOT NULL,
   user_id BIGINT NOT NULL,

    -- 일별 활동 통계
   login_count INT DEFAULT 0,
   post_count INT DEFAULT 0,
   comment_count INT DEFAULT 0,
   like_given_count INT DEFAULT 0,
   like_received_count INT DEFAULT 0,

    -- 시간 관련
   first_activity_at TIMESTAMP NULL,
   last_activity_at TIMESTAMP NULL,
   total_active_minutes INT DEFAULT 0,

   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
   updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

   PRIMARY KEY (summary_date, user_id),
   INDEX idx_user_id (user_id),
   INDEX idx_summary_date (summary_date)
);

-- 실시간 요약 갱신 프로시저
DELIMITER //
CREATE PROCEDURE sp_update_user_activity_summary(
    IN p_user_id BIGINT,
    IN p_activity_type VARCHAR(50)
)
BEGIN
    INSERT INTO user_activity_summary (summary_date, user_id)
    VALUES (CURDATE(), p_user_id)
    ON DUPLICATE KEY UPDATE
     login_count = CASE WHEN p_activity_type = 'LOGIN' THEN login_count + 1 ELSE login_count END,
     post_count = CASE WHEN p_activity_type = 'POST' THEN post_count + 1 ELSE post_count END,
     comment_count = CASE WHEN p_activity_type = 'COMMENT' THEN comment_count + 1 ELSE comment_count END,
     last_activity_at = NOW(),
     updated_at = NOW();
END //
DELIMITER ;

CALL sp_update_user_activity_summary(123, 'LOGIN');
```

### 시계열 데이터 설계 패턴

- 히스토리성 시계열 데이터에는 파티셔닝이라는 기능을 활용하는 것이 좋다.
  - 데이터가 적으면 굳이 파티셔닝도 필요없다.

```sql
CREATE TABLE financial_master_class.stock_trades (
         trade_id BIGINT NOT NULL AUTO_INCREMENT,
         trade_date DATE NOT NULL,
         PRIMARY KEY (trade_id, trade_date)
);

DELIMITER //

CREATE PROCEDURE sp_manage_partitions()
BEGIN
    DECLARE current_partition_key INT;
    DECLARE next_partition_key INT;
    DECLARE partition_name VARCHAR(20);
    DECLARE sql_stmt TEXT;

    -- 현재 월 기준 파티션 키 계산
    SET current_partition_key = YEAR(NOW()) * 100 + MONTH(NOW());
    SET next_partition_key =
            CASE
                WHEN MONTH(NOW()) = 12 THEN (YEAR(NOW()) + 1) * 100 + 1
                ELSE current_partition_key + 1
                END;

    -- 다음 달 파티션 이름
    SET partition_name = CONCAT('p', next_partition_key);

    -- 파티션 존재 여부 확인
    SELECT COUNT(*) INTO @partition_exists
    FROM information_schema.partitions
    WHERE table_schema = 'financial_master_class'
      AND table_name = 'stock_trades'
      AND partition_name = partition_name;

    -- 파티션이 없으면 생성
    IF @partition_exists = 0 THEN
        SET sql_stmt = CONCAT(
                'ALTER TABLE stock_trades ',
                'REORGANIZE PARTITION p_future INTO (',
                'PARTITION ', partition_name, ' VALUES LESS THAN (', next_partition_key + 1, '),',
                'PARTITION p_future VALUES LESS THAN MAXVALUE',
                ')'
                       );

        SET @sql = sql_stmt;
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;

        SELECT CONCAT('Created partition: ', partition_name) as result;
    ELSE
        SELECT CONCAT('Partition already exists: ', partition_name) as result;
    END IF;

END //

DELIMITER ;

-- 매월 1일 자동 실행을 위한 이벤트 생성
CREATE EVENT ev_monthly_partition_maintenance
    ON SCHEDULE EVERY 1 MONTH
        STARTS '2024-01-01 00:00:00'
    DO
    CALL sp_manage_partitions();
```

---

## 파티셔닝 전략

### 파티셔닝

- 파티셔닝은 거대한 테이블 하나를 논리적으로는 하나이지만 물리적으로는 여러 개의 작은 조각으로 나누어 저장하는 기술 
  - 간단하게 테이블을 쪼개는 거라고 생각하면 된다. 쪼개는 단위 하나가 파티션.
- 큰 데이터 테이블을 다룰 때 효과적이다.
  - (날짜를 기준으로 파티션을 구성했다고 가정) 특정 날짜를 기준으로 필터링하는 쿼리는 특정 파티션에만 접근하게 된다.
- 파티션 키를 기준으로 데이터가 분배된다.
  - 하지만 키를 잘못 설정하면 특정 파티션에 가중치가 높아져 데이터가 몰리는 현상이 발생할 수도 있다.

### 파티션 분석

- 파티션의 정보를 확인할 수 있는 쿼리

```sql
-- 현재 파티션 상태 확인
SELECT
    PARTITION_NAME,
    PARTITION_EXPRESSION,
    PARTITION_DESCRIPTION,
    TABLE_ROWS,
    ROUND(DATA_LENGTH / 1024 / 1024, 2) as data_size_mb,
    ROUND(INDEX_LENGTH / 1024 / 1024, 2) as index_size_mb
FROM information_schema.partitions
WHERE table_schema = 'financial_master_class'
  AND table_name = 'stock_trades'
  AND PARTITION_NAME IS NOT NULL
ORDER BY PARTITION_ORDINAL_POSITION;
```

- 파티션별 성능 분석

```sql
-- 파티션별 성능 분석
SELECT
    PARTITION_NAME,
    ROUND(DATA_LENGTH / (1024 * 1024 * 1024), 2) as data_size_gb,
    TABLE_ROWS,
    ROUND(TABLE_ROWS / (DATA_LENGTH / 1024), 2) as rows_per_kb,
    CASE
        WHEN TABLE_ROWS > 50000000 THEN 'VERY_LARGE'
        WHEN TABLE_ROWS > 10000000 THEN 'LARGE'
        WHEN TABLE_ROWS > 1000000 THEN 'MEDIUM'
        ELSE 'SMALL'
        END as partition_size_category
FROM information_schema.partitions
WHERE table_schema = 'financial_master_class'
  AND table_name = 'stock_trades'
  AND PARTITION_NAME IS NOT NULL;
```

### 1. 레인지 파티션

### 2. 해시 파티션

```sql
-- 계좌별 거래 이력 테이블 (Hash 파티셔닝)
CREATE TABLE account_trade_history (
       history_id BIGINT UNSIGNED AUTO_INCREMENT,
       account_id BIGINT UNSIGNED NOT NULL,
       trade_id BIGINT UNSIGNED NOT NULL,
       trade_date DATE NOT NULL,
       symbol VARCHAR(10) NOT NULL,
       trade_type ENUM('BUY', 'SELL') NOT NULL,
       price DECIMAL(12,4) NOT NULL,
       volume INT UNSIGNED NOT NULL,
       trade_value DECIMAL(18,4) NOT NULL,
       created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

       PRIMARY KEY (history_id, account_id)
) ENGINE=InnoDB PARTITION BY HASH(account_id) PARTITIONS 16;  -- 16개 파티션으로 분산
```

> #### `파티션 프루닝 Partition Pruning` 
> - 쿼리를 실행할 때 옵티마이저가 불필요한 파티션을 아예 스캔 대상에서 제외하고, 필요한 파티션에만 접근하는 핵심 최적화 기술.
> - 파티션 프루닝의 핵심은 WHERE 절이다. 데이터베이스의 옵티마이저는 사용자가 실행한 쿼리의 WHERE 절 조건을 보고, 이 조건이 어떤 파티션에 해당하는지를 미리 판단한다. 그리고는 해당 파티션의 데이터만 읽도록 실행 계획을 세운다.

- 조회해보기

```sql
SELECT * FROM account_trade_history WHERE trade_date = '2025-08-04';
```

### 3. 리스트 파티션

- 특정 코드값을 기준으로 어떤 파티션에 저장할지 명시적으로 설정할 수 있다.
- 아래 예시에서는 `market_code` 값을 기준으로 파티션을 지정하고 있다. 
  - `market_code` 값이 'KOSPI'라면 `p_kospi`라는 이름의 파티션에 저장된다.

```sql
-- 시장별 거래 데이터 (List 파티셔닝)
CREATE TABLE market_specific_trades (
    trade_id BIGINT UNSIGNED AUTO_INCREMENT,
    symbol VARCHAR(10) NOT NULL,
    market_code VARCHAR(10) NOT NULL,
    trade_date DATE NOT NULL,
    trade_timestamp TIMESTAMP(6) NOT NULL,
    price DECIMAL(12,4) NOT NULL,
    volume INT UNSIGNED NOT NULL,
    trade_type ENUM('BUY', 'SELL') NOT NULL,

    PRIMARY KEY (trade_id, market_code),
    INDEX idx_symbol_timestamp (symbol, trade_timestamp),
    INDEX idx_date_volume (trade_date, volume)

) ENGINE=InnoDB
    PARTITION BY LIST COLUMNS(market_code) (
        PARTITION p_kospi VALUES IN ('KOSPI'),
        PARTITION p_kosdaq VALUES IN ('KOSDAQ'),
        PARTITION p_nasdaq VALUES IN ('NASDAQ'),
        PARTITION p_nyse VALUES IN ('NYSE'),
        PARTITION p_other VALUES IN ('OTHER', 'CRYPTO', 'FOREX')
        );
```

- 만약 파티션을 생성하지 않았는데, 새로운 `market_code`가 생기면 문제가 생길 수 있다.
  - 이 문제를 해결하기 위해 기본 파티션 키를 생성하기도 한다.
- 가장 큰 문제는 특정 `market_code`에 값이 몰려, 불균형을 초래할 수 있다.

---

## 인덱스

- `WHERE` 절을 기준으로 만든다.

### 복합 인덱스

- 복합 인덱스를 만들 때는 카디널리티가 높은 컬럼을 앞에 두는 것이 좋다. 
- 또한 `WHERE` 절을 작성할 때 인덱스 순서에 맞춰주는 것이 좋다. => 옵티마이저가 쿼리 컬럼의 순서를 자체적으로 정렬하는 `재계획` 작업이 필요해지기 때문이다.

### 접두사 인덱스

- 만약 문자열 길이가 너무 긴데 검색 최적화를 위해 인덱스가 필요한 상황이라면, 접두사 인덱스를 활용하는 것이 좋다.

```sql
-- 접두사 인덱스 생성 (20자가 적절하다면)
CREATE INDEX idx_articles_title_prefix ON articles(title(20));
```

---

## 데이터 모델링

### 단계

#### 1. 개념적인 데이터 모델링

- 비즈니스의 핵심 데이터와 그들 간의 관계를 파악하고, 엔티티의 관계를 설계하는 단계
- 기술적인 상세 내용은 고려하지 않는다.
- 대략적인 그림을 그리는 POC와 굉장히 유사하다.

#### 2. 논리적인 데이터 모델링

- 특정 데이터베이스 기술에 종속되지 않는 상세한 설계도를 만드는 단계.
- 모든 속성, 기본키, 외래키, 관계 정의, 정규화 과정
- ERD를 그리는 단계

#### 3. 물리적 데이터 모델링

- 특정 데이터베이스에 맞게 변환하는 단계
- DDL과 스크립트 작성(정확한 데이터 타입 작성), 인덱스나 제약 조건을 고려.

### 관계 키의 종류

#### 1. 슈퍼 키 

- 테이블에서 각 행을 고유하게 식별할 수 있는 하나 이상의 속성들의 집합 
- 가장 포괄적인 개념으로, 유일성은 만족하지만 최소성은 보장 X 
- 예를 들어 학생 테이블에서 {학번}, {학번, 이름}, {학번, 이름, 전공} 모두 슈퍼 키가 될 수 있다.

#### 2. 후보 키

- 슈퍼 키 중에서 최소성까지 만족하는 키. 
- 즉, 각 튜플을 고유하게 식별하는 데 필요한 최소한의 속성들로만 구성 
- 학생 테이블에서 {학번}과 {주민등록번호}가 각각 후보 키가 될 수 있다. 더 이상 속성을 제거하면 유일성을 잃게 된다.

#### 3. 기본 키

- 후보 키 중에서 테이블의 대표로 선정된 하나의 키. 
- NULL 값을 가질 수 없고, 테이블당 오직 하나만 존재. 
- 데이터베이스 관리 시스템이 자동으로 인덱스를 생성하며, 다른 테이블과의 관계 설정 시 주로 참조. 
- 보통 변경 가능성이 적고 간단한 속성을 선택.

#### 4. 대체 키

- 후보 키 중에서 기본 키로 선택되지 않은 나머지 키. 
- 기본 키만큼 중요하진 않지만 여전히 각 행을 고유하게 식별할 수 있다. 
- 필요시 유니크 제약조건을 설정하여 중복을 방지 가능.

#### 5. 외래 키, 복합 키

- `외래 키`: 다른 테이블의 기본 키를 참조하는 속성 또는 속성들의 집합
- `복합 키`: 두 개 이상의 속성을 조합하여 만든 키

### 정규화

- 정규화는 데이터 중복을 최소화하고 데이터 무결성을 향상시키기 위해 테이블을 체계적으로 분해
- 정규화는 필수가 아니다. 실무에서는 잘 지키는 것도 어렵다.
  - 실무 데이터베이스에서는 보통 3NF 또는 BCNF까지 정규화하는 것이 일반적이다.

#### 제1정규형 (1NF: First Normal Form)

- "모든 속성의 도메인이 원자값(Atomic Value)만을 가져야 한다."
- 하나의 컬럼에 ","를 활용해 여러 값을 넣었다면 1NF를 위반한 것이다.

#### 제2정규형 (2NF: Second Normal Form)

- "1NF를 만족하면서, 모든 비주요 속성이 기본키에 완전 함수 종속이어야 한다."
- 부분 함수에 대한 종속을 제거하는 규칙. 복합 키로 구성된 테이블에서 키의 일부에만 종속되는 컬럼이 없어야 한다.
- `(order_id, product_id)`라는 복합 키 => 만약 `product_name`이라는 컬럼이 복합 키의 일부인 `product_id`에만 종속되면 2NF를 지키지 못한 것이다.

#### 제3정규형 (3NF: Third Normal Form)

- "2NF를 만족하면서, 기본키가 아닌 모든 속성이 기본키에 이행적 함수 종속이 되지 않아야 한다."
- 이행 함수에 대한 종속 제거. A->B 이고 B->C일 때 A->C인 관계를 제거.
- 주문 테이블의 `user_id`가 `user` 테이블의 `grade` 컬럼을 결정한다. => `user` 테이블의 PK가 아닌 `user_id`가 다른 컬럼을 결정하게 되기 때문에 3NF를 위반한다.

#### BCNF (Boyce-Codd Normal Form)

- "3NF를 만족하면서, 모든 결정자가 후보키여야 한다."

#### 제4정규형 (4NF: Fourth Normal Form)

- "BCNF를 만족하면서, 다치 종속(Multi-valued Dependency)이 없어야 한다."
- 

#### 제5정규형 (5NF: Fifth Normal Form)

- "4NF를 만족하면서, 조인 종속(Join Dependency)이 없어야 한다."

