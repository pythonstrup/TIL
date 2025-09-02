# 성능 지표 확인

## 1. 접속 정보 확인

```sql
SELECT ID, USER, HOST, DB, COMMAND, TIME, STATE, LEFT(INFO, 100) as QUERY_SNIPPET 
FROM information_schema.processlist
WHERE COMMAND != 'Sleep'
ORDER BY TIME DESC;
```

| ID | USER            | HOST             | DB | COMMAND | TIME | STATE                  | QUERY_SNIPPET                                                                                       |
| :--- |:----------------|:-----------------| :--- |:--------|:-----|:-----------------------|:----------------------------------------------------------------------------------------------------|
| 5 | event_scheduler | localhost        | null | Daemon  | 693  | Waiting on empty queue | null                                                                                                |
| 8 | root            | 172.17.0.1:60570 | null | Query   | 0    | executing              | /* ApplicationName=DataGrip 2025.1.3 */ SELECT ID, USER, HOST, DB, COMMAND, TIME, STATE, LEFT(INFO, |

---

## 2. 버퍼 풀 히트율 확인 (95% 이상이 목표)

```sql
SELECT ROUND(
   (1 - (
       (SELECT VARIABLE_VALUE FROM performance_schema.global_status WHERE VARIABLE_NAME = 'Innodb_buffer_pool_reads') -- 버퍼풀에 데이터가 없어 디스크에서 직접 읽은 수
        / (SELECT VARIABLE_VALUE FROM performance_schema.global_status WHERE VARIABLE_NAME = 'Innodb_buffer_pool_read_requests') -- 전체 요청 수 (디스크 + 버퍼풀)
       )) * 100, 
   2 -- 소수점 자리수
) AS buffer_pool_hit_ratio_percent;
```
| buffer_pool_hit_ratio_percent |
|:------------------------------|
| 90.7                          |

- 버퍼풀 비율이 낮으면 버퍼풀을 늘리거나, 인덱스를 사용하는 식으로 튜닝을 해주는 것이 좋다.

----

## 3. 테이블 성능 분석

```sql
SHOW TABLE STATUS LIKE 'users';
```

| Name | Engine | Version | Row_format | Rows | Avg_row_length | Data_length | Max_data_length | Index_length | Data_free | Auto_increment | Create_time | Update_time | Check_time | Collation | Checksum | Create_options | Comment |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| users | InnoDB | 10 | Dynamic | 9760 | 270 | 2637824 | 0 | 1425408 | 0 | 10000 | 2025-09-01 11:28:01 | null | null | utf8mb4_unicode_ci | null |  |  |


```sql
SHOW CREATE TABLE users;
```

| Table | Create Table |
| :--- | :--- |
| users | CREATE TABLE `users` (<br/>  `user_id` bigint NOT NULL AUTO_INCREMENT,<br/>  `email` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,<br/>  `password_hash` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,<br/>  `name` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL,<br/>  `phone` varchar(20) COLLATE utf8mb4_unicode_ci DEFAULT NULL,<br/>  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,<br/>  `updated_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,<br/>  `is_active` tinyint(1) DEFAULT '1',<br/>  PRIMARY KEY (`user_id`),<br/>  UNIQUE KEY `email` (`email`),<br/>  KEY `idx_email` (`email`),<br/>  KEY `idx_created_at` (`created_at`),<br/>  KEY `idx_is_active` (`is_active`)<br/>) ENGINE=InnoDB AUTO_INCREMENT=10001 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci |

- 인덱스에 대한 지표
  - 특히 카디널리티의 영향을 많이 받는다.

```sql
SHOW INDEX FROM users;
```

| Table | Non_unique | Key_name       | Seq_in_index | Column_name | Collation | Cardinality | Sub_part | Packed | Null | Index_type | Comment | Index_comment | Visible | Expression |
|:------| :--- |:---------------| :--- |:------------| :--- |:------------| :--- | :--- |:-----| :--- | :--- | :--- | :--- | :--- |
| users | 0 | PRIMARY        | 1 | user_id     | A | 9760        | null | null |      | BTREE |  |  | YES | null |
| users | 0 | email          | 1 | email       | A | 9760        | null | null |      | BTREE |  |  | YES | null |
| users | 1 | idx_email      | 1 | email       | A | 9760        | null | null |      | BTREE |  |  | YES | null |
| users | 1 | idx_created_at | 1 | created_at  | A | 11          | null | null | YES  | BTREE |  |  | YES | null |
| users | 1 | idx_is_active  | 1 | is_active   | A | 2           | null | null | YES  | BTREE |  |  | YES | null |

---

### 4. 실행 계획

- `type`이 `ref`가 되는 것이 좋다. `all`이 뜨면 전체 스캔 -> 성능에 문제 생길 가능성 높음.
- `possible_keys`는 키를 사용하지 않았다는 의미 => 사용할 키가 없거나, 옵티마이저가 키를 사용하지 않고 전체 스캔을 하는 것이 더 효율적이라고 판단한 경우 null이 될 수 있다.
- `key`

