# Architecture

## MySQL 엔진 아키텍처

- MySQL 서버는 다른 DBMS에 비해 구조가 상당히 독특하다. 사용자 입장에서 보면 거의 차이가 느껴지지 않지만 사실 그 내부는 아주 독특하다.

<img src="img/architecture1.png">

- MySQL은 일반 상용 RDBMS와 같이 대부분의 프로그래밍 언어로부터 접근 방법을 모두 지원한다.
- MySQL 서버는 MySQL 엔진과 스토리지 엔진으로 구분할 수 있다.

### 1. MySQL 엔진

- 클라이언트로부터의 접속 및 쿼리 요청을 처리하는 커넥션 핸들러와 SQL 파서 및 전처리기, 쿼리의 최적화된 실행을 위한 옵티마이저 등이 있다.
- 표준 SQL 문법을 지원하기 때문에 표준 문법에 따라 작성된 쿼리는 타 DBMS와 호환되어 사용할 수 있다.

### 2. 스토리지 엔진

- 실제 데이터를 디스크 스토리지에 저장하거나 디스크 스토리지로부터 데이터를 읽어오는 부분이다.
- MySQL 서버는 엔진은 하나지만 스토리지 엔진은 여러 개를 동시에 사용할 수 있다.
- 아래와 같이 테이블이 사용할 스토리지 엔진을 지정할 수 있다.

```sql
mysql> CREATE TABLE table (column1 varchar(10), column2 int) ENGINE=INNOBD;
```

### 3. 핸들러 API

- MySQL 엔진의 쿼리 실행기에서 데이터를 쓰거나 읽어야 할 때는 각 스토리지 엔진에 쓰기 또는 읽기를 요청하는데, 이러한 요청을 핸들러 요청이라고 한다.
- 또 여기에 사용되는 API를 핸들러 API라고 한다.

<br>

## 스레딩 구조

- MySQL 서버는 프로세스 기반이 아닌 스레드 기반으로 작동하면 포그라운드(Foreground) 스레드와 백그라운드(Background) 스레드로 구분할 수 있다.
- 아래 명령어를 사용하면 MySQL 서버에서 실행 중인 스레드 목록을 확인할 수 있다.

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

### 1. 포그라운드 스레드(클라이언트 스레드)

- 포그라운드 스레드는 최소한 MySQL 서버에 접속된 클라이언트의 수만큼 존재하면, 주로 각 클라이언트 사용자가 요청하는 쿼리 문장을 처리한다.
- 클라이언트 사용자가 작업을 마치고 커넥션을 종료하면 해당 커넥션을 담당하던 스레드는 다시 스레드 캐시(Thread Cache)로 되돌아간다.
- 스레드 캐시에 일정 개수 이상의 대기 중인 스레드가 있으면 스레드 캐시에 넣지 않고 스레드를 종료시켜 일정 개수의 스레드만 스레드 캐시에 존재하게 한다.
- 스레드 캐시에 유지할 수 있는 최대 스레드 개수는 `thread_cache_size` 시스템 변수로 설정한다.
- 포그라운드 스레드는 데이터 버퍼나 캐시로부터 데이터를 가져오며 버퍼나 캐시에 없는 경우 직접 디스크의 데이터나 인덱스 파일로부터 데이터를 읽어온다.
- MyISAM 테이블은 디스크 쓰기 작업까지 포그라운드 스레드가 처리하지만 InnoDB 테이블은 데이터 버퍼나 캐시까지만 포그라운드 스레드가 처리하고 나머지 버퍼로부터 디스크까지 기록하는 작업은 백그라운드 스레드가 처리한다.

### 2. 백그라운드 스레드

- MyISAM에 경우 해당 사항이 거의 없다.
- InnoDB는 아래의 작업이 백그라운드로 처리된다.
  - 인서트 버터(Insert Buffer)를 병합하는 스레드
  - 로그를 디스크로 기록하는 스레드
  - InnoDB 버퍼 풀을 데이터를 디스크에 기록하는 스레드
  - 데이터를 버퍼로 읽어 오는 스레드
  - 잠금이나 데드락을 모니터링하는 스레드
- 여기서 가장 중요한 것은 로그 스레드(Log Thread)와 버퍼의 데이터를 디스크로 내려쓰는 작업을 수행하는 쓰기 스레드(Write Thread)이다.
- 쓰기 스레드는 아주 많은 작업을 백그라운드로 처리하기 때문에 일반적인 내장 디스크를 사용할 때는 2~4개 정도로, DAS나 SAN 같은 스토리지를 사용할 때는 디스크를 최적으로 사용할 수 있을만큼 충분히 설정하는 것이 좋다.
- 사용자의 요청을 처리하는 도중 데이터의 쓰기 작업은 지연(버퍼링)되어 처리될 수 있지만 데이터의 읽기 작업은 절대 지연될 수 없다.
  - 예를 들어 SELECT 쿼리를 실행했는데 요청된 SELECT는 10분 뒤에 결과를 돌려주겠다라고 응답하는 DBMS는 없다.
- 일반적인 사용 DBMS는 대부분 쓰기 작업을 버퍼링해서 일괄 처리하는 기능이 탑재되어 있으며 InnoDB 또한 이러한 방식으로 처리한다.
- 하지만 MyISAM은 그렇지 않다. 사용자(포그라운드) 스레드가 쓰기 작업까지 함께 처리하도록 설계되어 있기 때문이다.

<br>

## 메모리 할당 및 사용 구조

<img src="img/architecture2.png">


## 참고자료

- Real MySQL 8.0
