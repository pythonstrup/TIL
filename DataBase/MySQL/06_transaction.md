# 트랜잭션 Transaction

- InnoDB 스토리지 엔진에서 제공하는 기능 (MyIsam과 MEMORY와 같은 엔진에서는 트랜재션을 지원하지 않는다.)
- 트랜잭션은 DBMS의 커넥션과 동일하게 꼭 필요한 최소의 코드에만 적용되는 것이 좋다. (트랜잭션의 범위를 최소화)

<br/>

# 잠금 Lock

## MySQL 엔진 레벨

### MySQL 엔진 레벨에서의 잠금

- MySQL 엔진 레벨 = 스토리지 엔진 범위를 제외한 나머지 부분
- MySQL 엔진 레벨에서의 잠금은 모든 스토리지 엔진에 영향을 미친다.

### MySQL 엔진에서의 락 종류

1. 글로벌 락 Global Lock
    - `FLUSH TABLES WITH READ LOCK` 명령으로 획득할 수 있다.
    - MySQL에서 제공하는 잠금 가운데 가장 범위가 넓다.
    - 한 세션에서 글로벌 락을 획득하면 다른 세션에서 DML, DDL 문장 실행 시 글로벌 락이 해제될 때 까지 대기해야 한다.
    - MySQL 서버 전체에 영향을 미치며 작업 대상 테이블이나 데이터베이스가 다르더라도 동일한 영향을 준다.
    - InnoDB 스토리지 엔진에서는 트랜잭션을 지원하기 때문에 일관된 데이터 상태를 유지하기 위해 글로벌 락을 통한 데이터 변경 작업 잠금을 할 필요가 없다. 
2. 테이블 락 Table Lock
   - 테이블 데이터 동기화를 위한 락 
   - 개별 테이블 단위로 설정되는 잠금이다.
   - 명시적 or 묵시적으로 락을 획득할 수 있다.
   - 명시적 획득 : `LOCK TABLES table_name [READ | WRITE]`
     - 명시적으로 얻은 락은 명시적으로 반납해줘야 한다. (`UNLOCK TABLES`)
     - 글로벌 락과 마찬가지로 온라인 작업에 많은 영향을 미치기 때문에 특별한 상황이 아니면 사용할 필요가 없다.
   - 묵시적 획득 : MyISAM 혹은 MEMORY 테이블 데이터 변경 쿼리 실행 시 발생
     - 쿼리가 실행되는 동안 자동으로 획득했다가 완료되면 자동으로 해제함
     - InnoDB는 기본적으로 레코드 기반 잠금을 사용하기 때문에 단순 데이터 변경 시에는 묵시적 테이블 락이 발생하지 않는다. 더 정확히는, 테이블 락이 발생하긴 하지만 DML 쿼리에서는 무시하고 DDL의 경우에만 발생한다.
3. 사용자의 필요에 맞게 사용할 수 있는 네임드 락(Named Lock)
     - 임의의 문자열에 대해 잠금을 설정할 수 있다.
     - 테이블이나 레코드 또는 AUTO_INCREMENT와 같은 데이터베이스 객체가 대상이 아니라는 것이 특징이다.
     - 많은 레코드에 대해 복잡한 요건으로 레코드를 변경하는 트랜잭션에 유용하게 사용할 수 있다.
4. 테이블의 구조를 잠그는 메타데이터 락(Metadata Lock)
    - 데이터베이스 객체(Table, View 등)의 이름이나 구조를 변경하는 경우에 획득하는 잠금이다.

## InnoDB 스토리지 엔진 레벨

- 스토리지 엔진 레벨에서의 잠금은 스토리지 엔진 간 상호 영향을 미치지 않는다.
- InnoDB 엔진 내부에서 레코드 기반의 잠금 방식을 탑재하고 있다. (덕분에 MyISAM보다 훨씬 뛰어난 동시성 처리가 가능)

### 잠금 종류

1. 레코드 락 Record Lock
    - 레코드 자체만을 잠그는 것
    - InnoDB 스토리지 엔진은 레코드 자체가 아니라 인덱스의 레코드를 잠근다는 특징이 있다.
    - 인덱스가 하나도 없는 테이블이더라도 내부적으로 자동 생성된 클러스터 인덱스를 이용해 잠금을 설정한다.
2. 갭 락 Gap Lock
    - 다른 DBMS와의 차이점
    - 레코드 자체가 아니라 레코드와 인접한 레코드 사이의 간격만 잠그는 것
    - 레코드와 레코드 사이에 새로운 레코드가 생성되는 것을 제어하는 것이다.
3. 넥스트 키 락 Next Key Lock
    - 레코드 락과 갭 락을 합쳐 놓은 잠금 형태
    - 레코드 락과 갭 락은 바이너리 로그에 기록되는 쿼리가 레플리카 서버에서 실행될 때 소스 서버에서 만들어 낸 결과와 동일한 결과를 만들어내도록 보장하는 것이 주목적이다.
    - 그런데 넥스트 키 락과 갭 락으로 인해 데드락이 발생하거나 다른 트랜잭션을 기다리게 하는 일이 자주 발생하기에 바이너리 로그 포맷을 ROW 형태로 바꿔서 넥스트 키 락이나 갭락이 줄이는 것이 좋다.
4. 자동 증가 락 Auto Increment Lock
    - MySQL은 자동 증가하는 숫자 값을 추출하기 위해 AUTO_INCREMENT라는 컬럼 속성을 제공한다.
    - 만약 동시에 여러 레코드가 INSERT되는 경우, 각 레코드는 중복되지 않고 저장된 순서대로 증가하는 일련번호를 가져야한다. 이를 위해 InnoDB 스토리지 엔진에서는 내부적으로 AUTO_INCREMENT LOCK이라는 테이블 수준의 잠금을 운용한다.
    - 명시적으로 획득하거나 해제할 방법이 없으며 아주 짧은 시간 동안 걸렸다가 풀리기 때문에 대부분의 경우 문제가 되지 않는다.

<br/>

## 인덱스와 잠금

- InnoDB의 잠금과 인덱스는 상당히 중요한 연관 관계가 있다.
- InnoDB의 잠금은 레코드를 잠그는 것이 아니라 인덱스를 잠그는 방식으로 처리된다. 즉, 변경해야할 레코드를 찾기 위해 검색한 인덱스의 레코드를 모두 락을 걸어야 한다.
- 예시) my_table은 nickname에 index가 설정되어 있다. 아래 쿼리를 실행하면 총 75건이 집계된다고 가정해보자. 

```sql
SELECT COUNT(*) FROM my_table WHERE nickname='park' ;
```

- 아래의 쿼리를 실행하면 1건이 집계된다고 가정한다. 

```sql
SELECT COUNT(*) FROM my_table WHERE nickname='park' AND age=26;
```

- 그렇다면 아래의 업데이트 쿼리를 실행할 때 몇 개의 레코드가 LOCK 상태가 될까? nickname이 'park'인 75건의 레코드가 잠금 상태가 된다.

```sql
UPDATE my_table SET agree_yn='Y' WHERE nickname='park' AND age=26;
```

<br/>

## 격리 수준 Isolation Level

- 격리 수준이란 여러 트랜잭션이 동시에 처리될 때 특정 트랜잭션이 다른 트랜잭션에서 변경하거나 조회하는 데이터를 볼 수 있게 허용할지 말지를 결정하는 것이다.
- 격리 수준에는 `READ UNCOMMITTED`, `READ COMMITTED`, `REPEATABLE READ`, `SERIALIZABEL` 4가지가 있다.

<table>
    <thead>
        <th></th>
        <th>DIRTY READ</th>
        <th>NON-REPEATABLE READ</th>
        <th>PHANTOM READ</th>
    </thead>
    <tbody>
        <th>READ UNCOMMITTED</th>
        <td>O</td>
        <td>O</td>
        <td>O</td>
    </tbody>
    <tbody>
        <th>READ COMMITTED</th>
        <td>X</td>
        <td>O</td>
        <td>O</td>
    </tbody>
    <tbody>
        <th>REPEATABLE READ</th>
        <td>X</td>
        <td>X</td>
        <td>O (InnoDB는 없다.)</td>
    </tbody>
    <tbody>
        <th>SERIALIZABEL</th>
        <td>X</td>
        <td>X</td>
        <td>X</td>
    </tbody>
</table>

### READ UNCOMMITTED

- `DIRTY READ`라고도 불리며 일반적인 데이터베이스에서는 거의 사용하지 않는다.
- 커밋되지 않은 상태의 내용을 그대로 조회할 수 있다. 이렇게 처리한 작업이 완료되지 않았는데도 다른 트랜잭션에서 볼 수 있는 현상을 더티 리드(Dirty Read)라고 한다.
- `READ UNCOMMITTED`는 RDBMS 표준에서도 트랜잭션 격리 수준으로 인정하지 않을 정도로 정합성에 문제가 많은 격리 수준이다.

### READ COMMITTED

- 오라클 DBMS에서의 기본 격리수준이다. 또한, 온라인 서비스에서 가장 많이 선택하는 격리 수준이다.
- 트랜잭션에 의해 변경되고 있는 테이블을 조회할 때 변경한 내용이 담겨있는 테이블이 아니라 언두 영역에 백업된 레코드를 가져온다.
  - MVCC(Multi Version Concurrency Control)를 이용해 COMMIT되기 전의 데이터를 보여줄 수 있다.
- 하지만 하나의 트랜잭션에서 반복적인 SELECT를 한다고 가정했을 때 항상 같은 결과가 반환되지 않는 NON-REPEATABLE READ 문제가 발생할 수 있다.
- 만약 해당 문제로 인해 부정합이 발생했을 때, 금전적인 처리와 연관되어 있다면 치명적인 문제가 될 수 있다.

### REPEATABLE READ 

- MySQL InnoDB의 기본 설정 수준이다.
- InnoDB 스토리지 엔진은 트랜잭션이 ROLLBACK될 가능성에 대비해 변경되기 전 레코드를 언두 영역에 백업하고 실제 레코드 값을 변경한다.
  - `READ COMMITTED`는 COMMIT되기 전 언두 영역에 백업된 레코드를 보여주지만 커밋되면 테이블의 내용을 그대로 조회한다. 
  - `REPEATABLE READ`는 MVCC를 통해 언두 영역에 백업된 이전 데이터를 이용하고 동일 트랜잭션 내에서는 동일한 결과를 보여줄 수 있게 보장한다.
- Undo 영역에 백업된 모든 레코드에는 변경을 발생시킨 트랜잭션의 번호가 포함돼 있다. 그리고 백업된 데이터는 InnoDB 스토리지 엔진이 불필요하다고 판단하는 시점에 주기적으로 삭제한다.
- `REPEATABLE READ` 격리 수준에서는 MVCC를 보장하기 위해 실행 중인 트랜잭션 중 가장 오래된 트랜잭션 번호보다 트랜잭션의 번호가 앞서는 언두 영역의 데이터를 삭제할 수 없다.
    
### SERIALIZABLE

- 가장 단순하고 엄격한 격리 수준이다. 그만큼 속도가 많이 느리다. 동시성이 중요한 데이터베이스에서는 거의 사용되지 않는다.
- 그래도 InnoDB 테이블에서 순수한 SELECT 문을 실행했을 때 잠금을 하지는 않는다. (Non-locking consistent read: 잠금이 필요 없는 일관된 읽기)
- InnoDB 스토리지 엔진에서는 갭 락과 넥스트 키락 덕분에 `REPEATABLE READ` 격리 수준에서도 `PHANTOM READ`가 발생하지 않기 때문에 굳이 `SERIALIZABLE`를 사용할 이유가 없는 것 같다.

<br/>
