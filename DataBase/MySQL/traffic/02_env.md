# 환경 지표

## 1. 변수 확인 (`VARIABLES`)

### 문자 관련

#### 문자 설정

```sql
SHOW VARIABLES LIKE 'character_set%';
```

- 어떤 요청(Variable_name)이 어떤 형태(Value)를 지원하는지?

| Variable_name            | Value |
|:-------------------------| :--- |
| character_set_client     | utf8mb4 |
| character_set_connection | utf8mb4 |
| character_set_database   | utf8mb4 |
| character_set_filesystem | binary |
| character_set_results    | utf8mb4 |
| character_set_server     | utf8mb4 |
| character_set_system     | utf8mb3 |
| character_sets_dir       | /usr/share/mysql-8.0/charsets/ |

#### 문자의 비교 설정

```sql
SHOW VARIABLES LIKE 'collation%';
```
| Variable_name | Value |
| :--- | :--- |
| collation_connection | utf8mb4_0900_ai_ci |
| collation_database | utf8mb4_0900_ai_ci |
| collation_server | utf8mb4_0900_ai_ci |

- `utf8mb4_0900_ai_ci`: utf8mb4를 기반으로 대소문자와 악센트를 구분하지 않고 문자열을 비교 정렬.

### 성능 관련

#### 버퍼 풀

- 중요한 설정 중 하나.
- 메모리 공간에 대한 값. -> 데이터와 인덱스를 캐싱하는 공간의 크기.
- MySQL에서 성능에 가장 큰 영향을 주는 값

```sql
SHOW VARIABLES LIKE 'innodb_buffer_pool_size';
```

| Variable_name | Value |
| :--- | :--- |
| innodb_buffer_pool_size | 134217728 |

#### 커넥션

- 애플리케이션의 DB 커넥션과 조율할 때 중요.

```sql
SHOW VARIABLES LIKE 'max_connections';
```

| Variable_name | Value |
| :--- | :--- |
| max_connections | 151 |

---

## 2. MySQL의 현재 상태를 확인 (`STATUS`)

#### 현재 MySQL 서버에 접속해 있는 클라이언트 수

```sql
SHOW STATUS LIKE 'Threads_connected';
```

| Variable_name | Value |
| :--- | :--- |
| Threads_connected | 2 |

#### 서버가 마지막으로 시작된 후부터 지금까지 경과된 시간 (초단위)

```sql
SHOW STATUS LIKE 'Uptime';
```

| Variable_name | Value |
| :--- | :--- |
| Uptime | 750 |

---

## 3. 응용

### 메모리 사용량 확인

```sql
SELECT
    (@@innodb_buffer_pool_size / 1024 / 1024) AS buffer_pool_mb,
    (@@key_buffer_size / 1024 / 1024) AS key_buffer_mb;
```

| buffer_pool_mb | key_buffer_mb |
| :--- | :--- |
| 128.00000000 | 8.00000000 |

---

## 4. 설정 변경

### 기본 설정 변경

```sql
-- 연결 수 조정 (즉시 적용)
SET GLOBAL max_connections = 1000;

-- InnoDB 설정 (재시작 필요)
SET GLOBAL innodb_buffer_pool_size = 4294967296; -- 4GB
```

### 타임존 설정

- 타임존 확인

```sql
SELECT @@global.time_zone, @@session.time_zone
```

| @@global.time_zone | @@session.time_zone |
| :--- | :--- |
| SYSTEM | SYSTEM |


- 타임존 설정

```sql
SET GLOBAL time_zone = '+09:00'; -- KST
SELECT @@global.time_zone, @@session.time_zone -- 다시 조회
```

| @@global.time_zone | @@session.time_zone |
| :--- | :--- |
| +09:00 | SYSTEM |

---

## 5. 로그 확인

### 슬로우 쿼리 로그 설정

```sql
SET GLOBAL slow_query_log = 'ON';
SET GLOBAL long_query_time  = 2; -- 2초 이상
```

- 로그 설정 확인

```sql
SHOW VARIABLES LIKE '%log%';
```


