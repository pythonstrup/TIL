# MySQL 로그 파일

- MySQL 서버에서 서버의 상태를 진단할 수 있는 많은 도구들이 지원되지만 이러한 기능들은 많은 지식을 필요로 하는 경우가 많다.
- 하지만 로그 파일을 이용하면 깊은 내부 지식 없이도 MySQL의 상태나 부하를 일으키는 원인을 쉽게 찾아서 해결할 수 있다.

## 4-1. 에러 로그 파일

- MySQL 설정 파일(my.cnf)에서 `log_error`라는 이름의 파라미터로 정의된 경로에 생성된다.

### 4-1-1. MySQL이 시작하는 과정과 관련된 정보성 및 에러 메시지

- MySQL의 설정 파일을 변경하거나 데이터베이스가 비정상적으로 종료된 이후 다시 시작하는 경우에는 반드시 MySQL 에러 로그 파일을 통해 설정된 변수의 이름이나 값이 명확하게 설정되고 의도한 대로 적용됐는지 확인해야 한다.

### 4-1-2. 마지막으로 종료할 때 비정상적으로 종료된 경우 나타나는 InnoDB의 트랜잭션 복구 메시지

- 디스크에 기록되지 못한 데이터가 있다면 다시 기록하는 재처리 작업을 하게 된다. (리두 로그 사용)
- 이 과정에서 간단한 메시지가 출력되는데, 간혹 문제가 있어 복구되지 못할 때는 해당 에러 메시지를 출력학 MySQL이 다시 종료된다.

### 4-1-3. 쿼리 처리 도중에 발생하는 문제에 대한 에러 메시지

- 사전 예방이 어렵다. 주기적으로 에러 로그 파일을 검토하는 과정에서 알게 된다.

### 4-1-4. 비정상적으로 종료된 커넥션 메시지 Aborted Connection

- 어떤 데이터베이스 서버의 로그 파일을 보면 이 메시지가 상당히 누적돼 있는 경우가 있다.
- 종료 시에 정상 접속 종료 하지 못하고 프로그램이 종료된 경우 에러 로그가 쌓인다.
- 이런 메시지가 아주 많이 기록된다면 애플리케이션의 커넥션 종료 로직을 한 번 검토해볼 필요가 있다.
- 아니면 `max_connect_errors` 시스템 변숫값이 너무 낮은 것일 수도 있다.

### 4-1-5. InnoDB의 모니터링 또는 상태 조회 명령(SHOW ENGINE INNODB STATUS 같은)의 결과 메시지

- 모니터링을 사용한 이후에는 다시 비활성화해서 에러 로그 파일이 커지지 안헥 만들어야 한다.

### 4-1-6. MySQL 종료 메시지

- 아무도 모르게 종료돼 있거나, 때로는 아무도 모르게 재시작되는 경우.
- 서버가 종료됐는지 확인하는 유일한 방법 = 종료되면서 출력한 메시지를 확인

## 4-2. 제너럴 쿼리 로그 파일(제너럴 로그 파일, General log)

- 쿼리 로그 파일에는 시간 단위로 실행됐던 쿼리의 내용이 모두 기록된다. => 서버에서 실행되는 전체 쿼리 목록을 뽑아 검토할 때 사용.
- 슬로우 쿼리 로그와는 조금 다르게 제너럴 쿼리 로그는 실행되기 전에 MySQL이 쿼리 요청을 받으면 바로 기록하기 때문에 쿼리 실행 중에 에러가 발생해도 일단 로그 파일에 기록된다.
- 쿼리 로그 파일의 경로는 `general_log_file`이라는 이름의 파라미터에 설정돼 있다.

## 4-3. 슬로우 쿼리 로그

- 슬로우 쿼리 로그 파일에는 `long_query_time` 시스템 변수에 설정한 시간 이상의 시간이 소요된 쿼리가 모두 기록된다.
  - `long_query_time` 파라미터는 초단위로 설정하지만 소수점 값으로 설정하면 마이크로 초 단위로 설정 가능하다.

```shell
mysql> SHOW VARIABLES LIKE 'long_query_time';
```

- 반드시 쿼리가 정상적으로 실행이 완료돼야 슬로우 쿼리 로그에 기록할지 여부를 판단하기 때문에 반드시 쿼리가 정상적으로 실행히 완료돼야 슬로우 쿼리 로그에 기록될 수 있다.
- Percona에서 개발한 Percona Toolkit의 `pt-query-digest` 스크립트를 이용하면 쉽게 빈도나 처리 성능별로 쿼리를 정렬해서 살펴볼 수 있다.

```shell
# General Log 파일 분석
linux> pt-query-digest --type='genlog' general.log > parsed_general.log

# Slow Log 파일 분석
linux> pt-query-digest --type='slowlog' mysql-slow.log > parsed_mysql-slow.log
```

- 로그 파일의 분석이 완료되면 그 결과는 다음과 같이 3개의 그룹으로 나뉘어 저장된다.

### 4-3-1. 슬로우 쿼리 통계

- 분석 결과의 최상단에 표시.
- 모든 쿼리를 대상으로 슬로우 쿼리 로그의 실행 시간(Exec time), 그리고 잠금 대기 시간(Lock time) 등에 대해 평균 및 최소/최대 값을 표시한다.

### 4-3-2. 실행 빈도 및 누적 실행 시간순 랭킹

- `pt-query-digest` 명령 실행 시 `--order-by` 옵션으로 정렬 순서를 변경할 수 있다.

### 4-3-3. 쿼리 실행 횟수 및 누적 실행 시간 상세 정보

- 랭킬별 쿼리에서는 대상 테이블에 대해 어떤 쿼리인지만을 표시하는데, 실제 상세한 쿼리 내용은 개별 쿼리의 정보를 확인해보면 된다.
- 여기서는 쿼리가 얼마나 실행됐는지, 쿼리의 응답 시간에 대한 히스토그램 같은 상세한 내용을 보여준다.

<br/>

## 슬로우 쿼리에 대하여

- 아래와 같은 쿼리를 통해 MySQL 엔진에서 SQL 문을 통해 슬로우 쿼리 데이터를 찾을 수도 있다.

```mysql
SELECT *
FROM performance_schema.events_statements_summary_by_digest
WHERE SUM_TIMER_WAIT > 1000000000 -- 1초 이상 실행된 쿼리
ORDER BY SUM_TIMER_WAIT DESC
LIMIT 10;
```


<br/>

## 참고자료

- Real MySQL 8.0