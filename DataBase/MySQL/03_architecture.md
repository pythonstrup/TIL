# Architecture

## 1. MySQL 엔진 아키텍처

- MySQL 서버는 다른 DBMS에 비해 구조가 상당히 독특하다. 사용자 입장에서 보면 거의 차이가 느껴지지 않지만 사실 그 내부는 아주 독특하다.

### 1-1. 전체 구조

<img src="img/architecture01.jpg">

- MySQL은 일반 상용 RDBMS와 같이 대부분의 프로그래밍 언어로부터 접근 방법을 모두 지원한다.
- MySQL 서버는 MySQL 엔진과 스토리지 엔진으로 구분할 수 있다.
  - `쿼리 파서`나 `옵티마이저` 등과 같은 기능을 `MySQL 엔진`으로 구분하고, `InnoDB`와 `MyISAM` 그리고, `Memory`는 `스토리지 엔진`으로 구분한다.

#### 1-1-1. MySQL 엔진

- 클라이언트로부터의 접속 및 쿼리 요청을 처리하는 `커넥션 핸들러`와 `SQL 파서` 및 `전처리기`, 쿼리의 최적화된 실행을 위한 `옵티마이저` 등이 있다.
- 표준 SQL 문법을 지원하기 때문에 표준 문법에 따라 작성된 쿼리는 타 DBMS와 호환되어 사용할 수 있다.

#### 1-1-2. 스토리지 엔진

- `MySQL 엔진`은 요청된 SQL 문장을 분석하거나 최적화하는 등 DBMS의 **두뇌에 해당하는 처리를 수행**한다.
- 실제 데이터를 디스크 스토리지에 저장하거나 디스크 스토리지로부터 데이터를 읽어오는 부분은 `스토리지 엔진`이다.
- MySQL 서버는 엔진은 하나지만 스토리지 엔진은 여러 개를 동시에 사용할 수 있다.
- 아래와 같이 테이블이 사용할 스토리지 엔진을 지정하면 이후 해당 테이블의 모든 읽기 작업이나 변경 작업은 정의된 스토리지 엔진이 처리한다. 

```sql
mysql> CREATE TABLE table (column1 varchar(10), column2 int) ENGINE=INNOBD;
```

- 각 스토리지 엔진은 성능 향상을 위해 키 캐시(MyISAM 스토리지 엔진)나 InnoDB 버퍼 풀(InnoDB 스토리지 엔진)과 같은 기능을 내장하고 있다.

#### 1-1-3. 핸들러 API

- MySQL 엔진의 쿼리 실행기에서 데이터를 쓰거나 읽어야 할 때는 각 스토리지 엔진에 쓰기 또는 읽기를 요청하는데, 이러한 요청을 `핸들러 Handler` 요청이라고 한다.
  - 여기서 사용되는 API를 `Handler API`라고 한다.
- InnoDB 스토리지 엔진 또한 이 핸들러 API를 이용해 MySQL 엔진과 데이터를 주고받는다.
  - 이 핸들러 API를 통해 얼마나 많은 데이터(레코드) 작업이 있었는지는 `SHOW GLOBAL STATUS LIKE 'Handler%';` 명령으로 확인할 수 있다.

```shell
mysql> SHOW GLOBAL STATUS LIKE 'Handler%';
+----------------------------+---------+
| Variable_name              | Value   |
+----------------------------+---------+
| Handler_commit             | 1531    |
| Handler_delete             | 8       |
| Handler_discover           | 0       |
| Handler_external_lock      | 11567   |
| Handler_mrr_init           | 0       |
| Handler_prepare            | 606     |
| Handler_read_first         | 51      |
| Handler_read_key           | 2674    |
| Handler_read_last          | 0       |
| Handler_read_next          | 4738    |
| Handler_read_prev          | 0       |
| Handler_read_rnd           | 27      |
| Handler_read_rnd_next      | 231     |
| Handler_rollback           | 0       |
| Handler_savepoint          | 0       |
| Handler_savepoint_rollback | 0       |
| Handler_update             | 583     |
| Handler_write              | 4819498 |
+----------------------------+---------+
18 rows in set (0.01 sec)
```

<br>

## 1-2. 스레딩 구조

<img src="img/architecture02.jpg">

- MySQL 서버는 프로세스 기반이 아닌 스레드 기반으로 작동하며, 포그라운드(Foreground) 스레드와 백그라운드(Background) 스레드로 구분할 수 있다.
- MySQL 서버에서 실행 중인 스레드 목록은 `performance_schema` 데이터베이스의 `threads` 테이블을 통해 확인할 수 있다.

```sql
mysql> SELECT thread_id, name, type, processlist_user, processlist_host 
       FROM performance_schema.threads 
       ORDER BY type, thread_id;
+-----------+---------------------------------------------+------------+------------------+------------------+
| thread_id | name                                        | type       | processlist_user | processlist_host |
+-----------+---------------------------------------------+------------+------------------+------------------+
|         1 | thread/sql/main                             | BACKGROUND | NULL             | NULL             |
|         2 | thread/mysys/thread_timer_notifier          | BACKGROUND | NULL             | NULL             |
|         4 | thread/innodb/io_ibuf_thread                | BACKGROUND | NULL             | NULL             |
|         5 | thread/innodb/io_log_thread                 | BACKGROUND | NULL             | NULL             |
|         6 | thread/innodb/io_read_thread                | BACKGROUND | NULL             | NULL             |
|         7 | thread/innodb/io_read_thread                | BACKGROUND | NULL             | NULL             |
|         8 | thread/innodb/io_read_thread                | BACKGROUND | NULL             | NULL             |
|         9 | thread/innodb/io_read_thread                | BACKGROUND | NULL             | NULL             |
|        10 | thread/innodb/io_write_thread               | BACKGROUND | NULL             | NULL             |
|        11 | thread/innodb/io_write_thread               | BACKGROUND | NULL             | NULL             |
|        12 | thread/innodb/io_write_thread               | BACKGROUND | NULL             | NULL             |
|        13 | thread/innodb/io_write_thread               | BACKGROUND | NULL             | NULL             |
|        14 | thread/innodb/page_flush_coordinator_thread | BACKGROUND | NULL             | NULL             |
|        15 | thread/innodb/log_checkpointer_thread       | BACKGROUND | NULL             | NULL             |
|        16 | thread/innodb/log_flush_notifier_thread     | BACKGROUND | NULL             | NULL             |
|        17 | thread/innodb/log_flusher_thread            | BACKGROUND | NULL             | NULL             |
|        18 | thread/innodb/log_write_notifier_thread     | BACKGROUND | NULL             | NULL             |
|        19 | thread/innodb/log_writer_thread             | BACKGROUND | NULL             | NULL             |
|        20 | thread/innodb/log_files_governor_thread     | BACKGROUND | NULL             | NULL             |
|        25 | thread/innodb/srv_lock_timeout_thread       | BACKGROUND | NULL             | NULL             |
|        26 | thread/innodb/srv_error_monitor_thread      | BACKGROUND | NULL             | NULL             |
|        27 | thread/innodb/srv_monitor_thread            | BACKGROUND | NULL             | NULL             |
|        28 | thread/innodb/buf_resize_thread             | BACKGROUND | NULL             | NULL             |
|        29 | thread/innodb/srv_master_thread             | BACKGROUND | NULL             | NULL             |
|        30 | thread/innodb/dict_stats_thread             | BACKGROUND | NULL             | NULL             |
|        31 | thread/innodb/fts_optimize_thread           | BACKGROUND | NULL             | NULL             |
|        32 | thread/mysqlx/worker                        | BACKGROUND | NULL             | NULL             |
|        33 | thread/mysqlx/worker                        | BACKGROUND | NULL             | NULL             |
|        34 | thread/mysqlx/acceptor_network              | BACKGROUND | NULL             | NULL             |
|        38 | thread/innodb/buf_dump_thread               | BACKGROUND | NULL             | NULL             |
|        39 | thread/innodb/clone_gtid_thread             | BACKGROUND | NULL             | NULL             |
|        40 | thread/innodb/srv_purge_thread              | BACKGROUND | NULL             | NULL             |
|        41 | thread/innodb/srv_worker_thread             | BACKGROUND | NULL             | NULL             |
|        42 | thread/innodb/srv_worker_thread             | BACKGROUND | NULL             | NULL             |
|        43 | thread/innodb/srv_worker_thread             | BACKGROUND | NULL             | NULL             |
|        45 | thread/sql/signal_handler                   | BACKGROUND | NULL             | NULL             |
|        46 | thread/mysqlx/acceptor_network              | BACKGROUND | NULL             | NULL             |
|        44 | thread/sql/event_scheduler                  | FOREGROUND | event_scheduler  | localhost        |
|        48 | thread/sql/compress_gtid_table              | FOREGROUND | NULL             | NULL             |
|        49 | thread/sql/one_connection                   | FOREGROUND | root             | localhost        |
+-----------+---------------------------------------------+------------+------------------+------------------+
40 rows in set (0.00 sec)
```

- 3개의 포그라운드 스레드를 제외하면 나머지는 모두 백그라운드 스레드임을 확인할 수 있다.
- 그런데 이 중 마지막인 `thread/sql/one_connection` 스레드만 실제 사용자의 요청을 처리하는 포그라운드 스레드다.
- 백그라운드 스레드의 개수는 MySQL 서버의 설정의 내용에 따라 가변적일 수 있다.
- 동일한 이름의 스레드가 2개 이상씩 보이는 것은 MySQL 서버의 설정 내용에 의해 여러 스레드가 동일 작업을 병렬로 처리하는 경우다.

> ##### 참고
> - 여기서 소개하는 스레드 모델은 MySQL 서버가 전통적으로 가지고 있던 스레드 모델. (MySQL Community Edition에서 사용)
> - 엔터프라이즈 에디션과 Percona MySQL 서버에서는 전통적인 스레드 모델뿐만 아니라 `스레드 풀 Thread Pool` 모델을 사용할 수 있다.
> - 스레드 풀과 전통적인 스레드 모델의 가장 큰 차이점은 포그라운드 스레드와 커넥션의 관계다.
> - 전통적인 스레드 모델에서는 커넥션별로 포그라운드 스레드가 하나씩 생성되고 할당된다.
> - 하지만 스레드 풀에서는 커넥션과 포그라운드 스레드는 1대1 관계가 아니라 하나의 스레드가 여러 개의 커넥션 요청을 전담한다.

#### 1-2-1. 포그라운드 스레드(클라이언트 스레드)

- 포그라운드 스레드는 최소한 MySQL 서버에 접속된 클라이언트의 수만큼 존재하면, 주로 각 클라이언트 사용자가 요청하는 쿼리 문장을 처리한다.
- 클라이언트 사용자가 작업을 마치고 커넥션을 종료하면 해당 커넥션을 담당하던 스레드는 다시 스레드 캐시(Thread Cache)로 되돌아간다.
- 스레드 캐시에 일정 개수 이상의 대기 중인 스레드가 있으면 스레드 캐시에 넣지 않고 스레드를 종료시켜 일정 개수의 스레드만 스레드 캐시에 존재하게 한다.
- 스레드 캐시에 유지할 수 있는 최대 스레드 개수는 `thread_cache_size` 시스템 변수로 설정한다.
- 포그라운드 스레드는 데이터 버퍼나 캐시로부터 데이터를 가져오며 버퍼나 캐시에 없는 경우 직접 디스크의 데이터나 인덱스 파일로부터 데이터를 읽어온다.
  - MyISAM 테이블은 디스크 쓰기 작업까지 포그라운드 스레드가 처리하지만!
  - InnoDB 테이블은 데이터 버퍼나 캐시까지만 포그라운드 스레드가 처리하고 나머지 버퍼로부터 디스크까지 기록하는 작업은 백그라운드 스레드가 처리한다.

> ##### 참고
> - MySQL에서 사용자 스레드와 포그라운드 스레드는 똑같은 의미로 사용된다.
> - 클라이언트가 MySQL 서버에 접속하게 되면 MySQL 서버는 그 클라이언트의 요청을 처리해 줄 스레드를 생성해 그 클라이언트에게 할당한다.
> - 이 스레드는 DBMS 앞단에서 사용자와 통신하기 때문에 포그라운드 스레드라고 하며, 사용자가 요청한 작업을 처리하기 때문에 사용자 스레드라고도 한다.

#### 1-2-2. 백그라운드 스레드

- MyISAM에 경우 해당 사항이 거의 없다.
- InnoDB는 아래의 작업이 백그라운드로 처리된다.
  - 인서트 버터(Insert Buffer)를 병합하는 스레드
  - 로그를 디스크로 기록하는 스레드
  - InnoDB 버퍼 풀을 데이터를 디스크에 기록하는 스레드
  - 데이터를 버퍼로 읽어 오는 스레드
  - 잠금이나 데드락을 모니터링하는 스레드
- 여기서 가장 중요한 것은 `로그 스레드 Log Thread`와 버퍼의 데이터를 디스크로 내려쓰는 작업을 수행하는 `쓰기 스레드 Write Thread`이다.
- `MySQL 5.5` 버전부터 데이터 쓰기 스레드와 데이터 읽기 스레드의 개수를 2개 이상 지정할 수 있게 됐으며, `innodb_write_io_threads`와 `innodb_read_io_threads` 시스템 변수로 스레드의 개수를 설정한다.
- InnoDB에서도 데이터를 읽는 작업은 주로 클라이언트 스레드에서 처리되기 때문에 `읽기 스레드`는 많이 설정할 필요가 없지만.
  - `쓰기 스레드`는 아주 많은 작업을 백그라운드로 처리하기 때문에 일반적인 내장 디스크를 사용할 때는 2~4개 정도로 둔다. 
  - DAS나 SAN 같은 스토리지를 사용할 때는 디스크를 최적으로 사용할 수 있을만큼 충분히 설정하는 것이 좋다.
- 사용자의 요청을 처리하는 도중 데이터의 쓰기 작업은 지연(버퍼링)되어 처리될 수 있지만 **데이터의 읽기 작업은 절대 지연될 수 없다.**
  - 예를 들어 SELECT 쿼리를 실행했는데 요청된 SELECT는 10분 뒤에 결과를 돌려주겠다라고 응답하는 DBMS는 없다.

|InnoDB|MyISAM|
|:--:|:--:|
|일반적인 상용 DBMS는 대부분 쓰기 작업을 버퍼링해서 일괄 처리하는 기능이 탑재되어 있으며 InnoDB 또한 이러한 방식으로 처리한다.| 사용자(포그라운드) 스레드가 쓰기 작업까지 함께 처리하도록 설계되어 있기 때문에 기본적으로 쓰기 작업을 버퍼링해서 일괄 처리하는 기능이 없다.|
|`INSERT`, `UPDATE`, `DELETE` 쿼리로 데이터가 변경되는 경우 데이터가 디스크의 데이터 파일로 완전히 저장될 때까지 기다리지 않아도 된다.|일반적인 쿼리는 쓰기 버퍼링 기능을 사용할 수 없다.|

<br>

## 메모리 할당 및 사용 구조

<img src="img/architecture03.png">


## 참고자료

- Real MySQL 8.0
