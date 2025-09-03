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

