# CRUD

## Insert 최적화

### Prepared Statement

- 반복 INSERT를 최적화하는 방법

```sql
PREPARE stmt_insert_comment FROM
    'INSERT INTO comments (post_id, user_id, content) VALUES (?, ?, ?)';

-- 변수 설정 후 실행 (루프에서 반복)
SET @post_id = 1;
SET @user_id = 1;
SET @content = '첫 번째 댓글';

EXECUTE stmt_insert_comment USING @post_id, @user_id, @content;
```

### 파일 입력

- `LOAD DATA INFILE`
- 실무에서 거의 사용되지 않는다. (파일 가공이 까다로움.)

### 중복 시 에러 없이 건너뛰기

- `IGNORE`
  - 유니크 키가 중복될 때.
  - 멱등성 보장할 때 사용할 수 있다.

```sql
INSERT IGNORE INTO users (username, email, password_hash, display_name)
VALUES
    ('john_doe', 'john@example.com', SHA2('pass1', 256), 'John'),
    ('jane_doe', 'jane@example.com', SHA2('pass2', 256), 'Jane'),
    ('john_doe', 'john2@example.com', SHA2('pass3', 256), 'John2');
```

- `ON DUPLICATE KEY UPDATE`
  - 조회했을 때 값이 존재하면 특정 컬럼만 업데이트하고, 없으면 INSERT 한다.  
  - 효율성이 안 좋다.

```sql
-- 중복 시 특정 컬럼만 업데이트
INSERT INTO users (username, email, password_hash, display_name, login_count)
VALUES ('john_doe', 'john@example.com', SHA2('pass', 256), 'John Doe', 1)
ON DUPLICATE KEY UPDATE
         display_name = VALUES(display_name),
         login_count = login_count + 1,  -- 기존값 + 1
         updated_at = CURRENT_TIMESTAMP;
```

- `REPLACE`
  - 물리적으로 삭제되기 때문에 위험.

```sql
-- 중복 시 기존 데이터 삭제 후 새 데이터 삽입
REPLACE INTO users (user_id, username, email, password_hash, display_name)
VALUES (1, 'john_updated', 'john_new@example.com', SHA2('newpass', 256), 'John Updated');
```

### 동적 쿼리

- `EXISTS`

```sql
-- EXISTS를 이용한 조건부 삽입
INSERT INTO comments (post_id, user_id, content)
SELECT 1, 2, '댓글 내용'
WHERE EXISTS (
    SELECT 1 FROM posts
    WHERE post_id = 1 AND status = 'PUBLISHED'
)
  AND NOT EXISTS (
    SELECT 1 FROM comments
    WHERE post_id = 1 AND user_id = 2
      AND created_at > DATE_SUB(NOW(), INTERVAL 1 MINUTE)
);
```

---

## SELECT 최적화

### 전문 인덱스 활용

- 전문 인덱스 생성

```sql
ALTER TABLE 테이블명 ADD FULLTEXT (컬럼명);
```

- 전문 인덱스 활용
  - 아래 예시에서는 `title`, `content` 2개의 필드에 `FULLTEXT`가 걸려 있어야 한다.
  - `IN NATURAL LANGUAGE MODE`: 자연어 처리 방식

```sql
SELECT post_id, title, content
FROM posts
WHERE MATCH(title, content) AGAINST('MySQL 최적화' IN NATURAL LANGUAGE MODE)
  AND status = 'PUBLISHED';
```

### 윈도우 함수

- [윈도우 함수 정리](../query/window_function.md)

### CTE

```sql
WITH 임시테이블명 AS (
    SELECT ...
)
SELECT ...
    FROM 임시테이블명;
```

---

## UPDATE & DELETE

- 생략

