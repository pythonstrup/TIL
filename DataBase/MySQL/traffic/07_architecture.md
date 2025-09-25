# MySQL 아키텍처와 스토리지 엔진

## MySQL 정보 쿼리

### 접속자

```sql
-- 현재 누가 접속해 있는지 확인하기
SELECT
    ID as 프로세스번호,
    USER as 사용자,
    HOST as 접속위치,
    DB as 사용중인DB,
    COMMAND as 현재상태,
    TIME as 실행시간초,
    LEFT(INFO, 50) as 실행중쿼리
FROM information_schema.PROCESSLIST
WHERE COMMAND != 'Sleep'  -- 잠들어있지 않은 것만
ORDER BY TIME DESC;       -- 실행시간이 긴 순서로
```

| 프로세스번호 | 사용자             | 접속위치 | 사용중인DB | 현재상태    | 실행시간초   | 실행중쿼리                            |
| :--- |:----------------| :--- |:-------|:--------|:--------|:---------------------------------|
| 8 | event_scheduler | localhost | null   | Daemon  | 4260567 | null                             |
| 16 | system user     |  | null   | Query   | 4260567 | null                             |
| 13 | system user     |  | null   | Connect | 4260567 | Group replication applier module |

### 연결 설정 확인

```sql
-- MySQL 연결 설정 확인하기
SELECT
    '최대 연결 수' as 설정,
    @@max_connections as 현재값,
    '동시에 접속할 수 있는 최대 사용자 수' as 설명
UNION ALL
SELECT
    '현재 연결 수',
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Threads_connected'),
    '지금 접속해 있는 사용자 수'
UNION ALL
SELECT
    '활성 연결 수',
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Threads_running'),
    '실제로 뭔가 하고 있는 사용자 수';
```

| 설정      | 현재값 | 설명 |
|:--------| :--- | :--- |
| 최대 연결 수 | 1000 | 동시에 접속할 수 있는 최대 사용자 수 |
| 현재 연결 수 | 217 | 지금 접속해 있는 사용자 수 |
| 활성 연결 수 | 4 | 실제로 뭔가 하고 있는 사용자 수 |

### 스토리지 엔진

```sql
-- 어떤 스토리지 엔진을 사용하고 있는지 확인
SELECT
    TABLE_SCHEMA as 데이터베이스,
    TABLE_NAME as 테이블명,
    ENGINE as 스토리지엔진,
    ROUND((DATA_LENGTH + INDEX_LENGTH) / 1024 / 1024, 2) as 크기_MB
FROM information_schema.TABLES
WHERE TABLE_SCHEMA = DATABASE()
ORDER BY (DATA_LENGTH + INDEX_LENGTH) DESC;
```

| 데이터베이스 | 테이블명    | 스토리지엔진 | 크기_MB   |
|:-------|:--------|:-------|:--------|
| my_db  | zipcode | InnoDB | 1027.00 |


#### InnoDB 버퍼 풀 기본 정보

```sql
-- 버퍼 풀 기본 정보 확인하기
SELECT
    POOL_SIZE as 전체페이지수,
    ROUND(POOL_SIZE * 16384 / 1024 / 1024, 0) as 전체크기_MB,
    FREE_BUFFERS as 빈페이지수,
    DATABASE_PAGES as 데이터페이지수,
    MODIFIED_DATABASE_PAGES as 수정된페이지수,
    ROUND(HIT_RATE, 2) as 히트율_퍼센트
FROM information_schema.INNODB_BUFFER_POOL_STATS;
```

#### InnoDB 메모리 확인

```sql
-- 어떤 종류의 페이지들이 메모리에 있는지 확인
SELECT
    PAGE_TYPE as 페이지종류,
    COUNT(*) as 개수,
    ROUND(COUNT(*) * 16 / 1024, 1) as 크기_MB,
    ROUND(COUNT(*) / (SELECT COUNT(*) FROM information_schema.INNODB_BUFFER_PAGE) * 100, 1) as 비율_퍼센트
FROM information_schema.INNODB_BUFFER_PAGE
GROUP BY PAGE_TYPE
ORDER BY 개수 DESC
LIMIT 10;
```

#### 로그 버퍼?

- 트랜잭션 로그를 임시로 저장하는 메모리 버퍼
- 쿼리를 디스크에 직접 쓰지 않고 로그 버퍼에 기록 => 성능 보장
  - 변경 사항에 대해서 임시 저장 => 디스크가 아닌 메모리에 저장 (`WAL, Write-Ahead Logging` 원칙)
- 로그 버퍼 데이터를 디스크에 저장하는 것을 `flush`라고 한다.

#### 로그 버퍼 확인

```sql
-- 로그 버퍼 상태 확인
SELECT
    '로그 버퍼 크기(MB)' as 항목,
    @@innodb_log_buffer_size / 1024 / 1024 as 값
UNION ALL
SELECT
    '로그 대기 횟수',
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Innodb_log_waits')
UNION ALL
SELECT
    '로그 쓰기 요청',
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Innodb_log_write_requests')
UNION ALL
SELECT
    '실제 로그 쓰기',
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Innodb_log_writes');
```

---

## 메모리와 트랜잭션

- 메모리
  - 대표적인 예시: 버퍼 풀
  - 전역과 세션별 메모리로 구분
- 전역과 세션별 메모리의 차이를 알면 엔진에서 발생하는 이슈를 해결하고 성능을 최적화하는 데 큰 도움이 될 수 있음.

### 세션별 메모리

- 연결마다 할당되는 메모리
- ex) 정렬 버퍼, 조인 버퍼, 읽기 버퍼, 읽기 전용 버퍼
    - 정렬 버퍼: 정렬을 위한 버퍼, `ORDER BY`나 `GROUP BY` 같은 작업 수행. 다룰 수 있는 범위를 초과하면 디스크에 임시 테이블을 만들어 처리.
    - 조인 버퍼: 인덱스가 없는 조인할 때 사용
    - 읽기 버퍼: 테이블을 스캔할 때 사용
    - 읽기 전용 버퍼: 인덱스 기반의 읽기를 할 때 사용

### 전역 메모리

- 모든 연결이 공유되는 메모리
- ex) 버퍼 풀, 로그 버퍼, 테이블 캐시

#### 버퍼 풀

- 가장 중요.
- LRU 알고리즘 사용 (가장 최근에 사용되지 않은 데이터를 우선적으로 제거)
- 아래와 같은 경우, 데이터가 밀려나는 문제가 발생할 수 있다.
1. 풀 테이블 스캔
2. 예측 읽기

- InnoDB는 LRU를 그대로 사용하지 않고 개선해서 사용한다.
  - `Minpoint`: 새 페이지를 LRU 리스트의 중간 지점에 삽입, Old 영역과 New 영역을 구분

#### 어댑티브 해시 인덱스

- 자주 사용되는 데이터 접근을 빠르게 처리
- 일반적으로 사용되는 `B+TREE` 대신 해시를 사용하여 시간 복잡도 O(1)에 탐색

#### Change Buffer

- Secondary Index에 대한 변경(삽입, 삭제, 업데이트)에 대한 작업을 일부러 지연 처리해서 성능을 높이는 기능
- 변경하려는 페이지가 만약 버퍼 풀에 없으면 잠시 Change Buffer에 저장해뒀다가 나중에 일괄 처리 
  - 랜덤 I/O로 처리될 걸 순차 I/O로 처리되도록 할 수 있다. => 배치성 작업에서 큰 성능 향상

#### 예측 읽기

- 데이터를 미리 읽어오는 기술 (`Linear Read-Ahead`, `Random Read-Ahead`)
- 대용량 테이블 스캔을 빠르게 함.

### 버퍼 풀 히트율 확인

```sql
-- 버퍼 풀 히트율 확인하기 (95% 이상이면 좋음)
SELECT
    '버퍼 풀 성능 분석' as 분석항목,
    bp_read_requests.VARIABLE_VALUE as 전체요청수,
    bp_reads.VARIABLE_VALUE as 디스크읽기수,
    ROUND(
            (1 - bp_reads.VARIABLE_VALUE / bp_read_requests.VARIABLE_VALUE) * 100, 2
    ) as 히트율_퍼센트,
    CASE
        WHEN (1 - bp_reads.VARIABLE_VALUE / bp_read_requests.VARIABLE_VALUE) * 100 >= 95
            THEN '👍 매우 좋음'
        WHEN (1 - bp_reads.VARIABLE_VALUE / bp_read_requests.VARIABLE_VALUE) * 100 >= 90
            THEN '😊 양호'
        WHEN (1 - bp_reads.VARIABLE_VALUE / bp_read_requests.VARIABLE_VALUE) * 100 >= 80
            THEN '😐 보통'
        ELSE '😱 개선 필요'
        END as 상태
FROM
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Innodb_buffer_pool_reads') bp_reads,
    (SELECT VARIABLE_VALUE FROM performance_schema.global_status
     WHERE VARIABLE_NAME = 'Innodb_buffer_pool_read_requests') bp_read_requests;
```

| 분석항목       | 전체요청수      | 디스크읽기수   | 히트율_퍼센트 | 상태       |
|:-----------|:-----------|:---------|:--------|:---------|
| 버퍼 풀 성능 분석 | 7532590921 | 30913210 | 99.59   | 👍 매우 좋음 |

