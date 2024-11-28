# 데이터 압축

- 디스크에 저장된 데이터 파일의 크기는 일반적으로 쿼리의 처리 성능과도 직결되지만 백업 및 복구 시간과도 밀접하게 연결된다.
- 파일이 크면 클수록 백업 시간이 오래 걸리며, 복구하는 데도 그만큼의 시간이 걸린다. 저장 공간 문제도 있다.
- 많은 DBMS가 이런 문제점을 해결하기 위해 데이터 압축 기능을 제공한다.ㄴ

## 1. 페이지 압축

- Transparent Page Compression
  - 디스크에 저장하는 시점에 데이터 페이지가 압축되어 저장.
  - 반대로 MySQL 서버가 디스크에서 데이터 페이를 읽어올 때 압축이 해제
  - 즉 버퍼 풀에 데이터 페이지가 한 번 적재되면 InnoDB 스토리지 엔진은 압축이 해제된 상태로만 데이터 페이지를 관리한다.
  - 그래서 MySQL 서버의 내부 코드에서는 압축 여부와 관계없이 "투명 Tranparent"하게 작동한다.
  - 문제는 16KB 데이터 페이지를 압축한 결과의 용량이 얼마나 될지 예측이 불가능한데, 적어도 하나의 테이블은 동일한 크기의 페이지(블록)로 통일돼야 한다는 것이다.
- `펀치 홀 Punch Hole`이라는 기능을 사용한다.

```
1. 16KB 페이지 압축 (결과 7KB로 가정)
2. MySQL 서버는 디스크에 압축된 결과 7KB를 기록 (압축 데이터 7에 빈 9KB의 빈 데이터를 기록)
3. 디스크에 데이터를 기록한 후, 7KB 이후의 공간 9KB에 대해 펀치 홀 생성
4. 파일 시스템은 7KB만 남기고 나머지 디스크의 9KB 공간을 다시 운영체제로 반납. 
```

- 저장할 때는 7KB로 디스크의 공간만 차지하지만, 운영체제에서 읽으면 16KB로 읽게 된다.
- 하지만 운영체제와 하드웨어에서 해당 기능을 지원해야 사용 가능하다.
  - 따라서 실제로 페이지 압축은 많이 사용되지 않는 상태다.
- 페이지 압축을 사용하려면 테이블을 생성하거나 변경할 때 다음과 같이 `COMPRESSION` 옵션을 설정하면 된다.

```shell
# 테이블 생성
mysql> CREATE TABLE t1 (c1 INT) COMPRESSION="zlib";

# 테이블 변경
mysql> ALTER TABLE t1 COMPRESSION="zlib";
mysql> OPTIMIZE TABLE t1;
```

<br/>

## 2. 테이블 압축

- 테이블 압축은 운영체제나 하드웨어에 대한 제약 없이 사용할 수 있기 때문에 일반적으로 활요도가 더 높다.
- 단점
  - 버퍼 풀 공간 활용률 낮음.
  - 쿼리 처리 성능 낮음.
  - 빈번한 데이터 변경 시 압축률 떨어짐.

### 2-1. 압축 테이블 생성

- `innodb_file_per_table` 시스템 변수가 `ON`으로 설정된 상태에서 테이블이 생성돼야 한다.
- 그리고 테이블 생성 진행 시, `ROW_FORMAT=COMPRESSED` 옵션을 명시해야 한다.
  - 페이지 크기가 16KB라면 `KEY_BLOCK_SIZE`는 4KB나 8KB만 설정할 수 있다.
  - 그리고 페이지 크기가 32KB 또는 64KB인 경우 테이블 압축을 적용할 수 없다.

```shell
mysql> SET GLOBAL innodb_file_per_table=ON;

# ROW_FORMAT 옵션과 KEY_BLOCK_SIZE 옵션을 모두 명시
mysql> CREATE TABLE compressed_table (
        c1 INT PRIMARY KEY
      )
      ROW_FORMAT=COMPRESSED
      KEY_BLOCK_SIZE=8;
```

- 페이지 블록 16KB / `KEY_BLOCK_SIZE=8` 일 때, 데이터 페이지를 압축한 용량이 얼마가 될지 알 수 없는데, 어떻게 `KEY_BLOCK_SIZE` 테이블을 생성할 때 설정할 수 있을까?
- 아래는 InnoDB 스토리지 엔진이 압축을 적용하는 방법이다.

```
1. 16KB 데이터 페이지를 압축
  1.1 압축된 결과가 8KB 이하면 그대로 디스크에 저장(압축 완료)
  1.2 압축된 결과가 8KB를 초과하면 원본 페이지를 스플릿(split)해서 2개의 페이지에 8KB씩 저장
2. 나뉘 페이지 각각에 대해 "1"번 단계를 반복 실행.
```

- 테이블 압축 방식에서 가장 중요한 것은 우너본 데이터 페이지의 압축 결과가 목표 크기(`KEY_BLOCK_SIZE`)보다 작거나 같을 때까지 반복해서 페이지를 스플릿하는 것이다.
  - 목표 크기가 잘못 설정되면 MySQL 서버의 처리 성능이 급격히 떨어질 수 있으니 주의하자.

### 2-2. KEY_BLOCK_SIZE 결정

- 샘플 데이터를 저장해보고 적절한지 판단하는 것이 좋다.
- 아래와 같이 인덱스별로 압축 횟수와 성공 횟수, 압축 실패율을 조회하자.
  - 일반적으로 압축 실패율은 3~5% 미만으로 유지할 수 있게 `KEY_BLOCK_SIZE`를 선택하는 것이 좋다.

```shell
mysql> SELECT
        table_name, index_name, compress_ops, compress_ops_ok,
        (compress_ops-compress_ops_ok)/compress_ops * 100 as compression_failure_pct
       FROM information_schema.INNODB_CMP_PER_INDEX;
```

- 성능에 민감한 서비스라면 테이블은 압축을 적용하지 않는 것이 좋다고 판단할 수 있다.

### 2-3. 압축된 페이지의 버퍼 풀 적재 및 사용

- 버퍼 풀에 적재하면 압축된 상태와 압축이 해제된 상태 2개 버전을 관리한다.
  - `LRU`: 디스크 읽은 상태 그대로 보여주는 리스트
  - `Unzip_LRU`: 압축 해제 버전 리스트
- InnoDB 스토리지 엔진은 버퍼 풀에서 압축 해제된 버전의 데이터 페이지를 적절한 수준으로 유지하기 위해 다음과 같은 어댑티브 알고리즘을 사용한다.
1. CPU 사용량이 높은 서버에서는 가능하면 압축과 압축 해제를 피하기 위해 `Unzip_LRU`의 비율을 높여서 유지하고
2. Disk IO 사용량이 높은 서버에서는 가능하면 `Unzip_LRU` 리스트의 비율을 낮춰서 InnoDB 버퍼 풀의 공간을 더 확보하도록 작동한다.

### 2-4. 테이블 압축 관련 설정

- 테이블 압축을 사용할 때 연관된 시스템 변수가 몇 가지 있다. 모두 페이지의 압축 실패율을 낮추기 위한 튜닝 포인트를 제공한다.
  - `innodb_cmp_per_index_enabled`
  - `innodb_compression_level`
  - `innodb_compression_failure_threshold_pct`와 `innodb_compression_pad_pct_max`
  - `innodb_log_compressed_pages`

<br/>

## 참고자료

- Real MySQL 8.0
