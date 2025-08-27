# MVCC

- 다중 버전 동시성 제어(Multi Version Concurrency Control)
- 동시성 제어를 하면서도 DB 더 빠르게 사용하기 위한 기술
  - 모든 동시성을 Lock으로 해결하려고 하면 병목이 생길 수밖에 없다.
  - 따라서 언두 로그와 리두 로그를 적절히 활용하여 동시성으로 인해 발생하는 데이터 정합성 문제를 해결한다.

## 리두 로그

- 리두 로그는 기본적으로 Disk에 저장된다.
- 인-메모리 구조에서 `Log Buffer`라는 저장소가 있는데, 그곳에 리두 로그를 저장해 최적화를 한다.
- 언두 데이터의 무결성과 지속성을 보장하는 장치로 사용된다.

## 언두 로그

- `Undo Tablespace`라고 디스크 구조에 영구적으로 언두 로그를 저장하기 위한 저장소가 존재
- 언두 로그를 메모리에 저장하는 것 같은 내용을 보여주는 블로그들이 있으나, MySQL 문서 어디에도 언두 로그를 메모리에 가지고 있다는 말을 발견할 수 없었다.

---

## 새로 알게된 사실

### 잠금 조회

- 아래의 경우, 트랜잭션 격리 레벨이 `REPEATABLE READ`라도 `MVCC`를 사용하지 않는다.
- 데이터베이스에 잠금을 걸고 조회를 진행하면 데이터베이스의 최신 버전을 조회한다.
- 또한 쓰기 작업(`UPDATE`, `DELETE`)을 해도 데이터베이스의 최신 버전을 토대로 적용된다.

### `REPEATABLE READ`에서 `Phantom Read` 발생시키는 방법

- 쓰기 작업을 하거나 `FOR UPDATE`와 같이 잠금 읽기를 진행하게 되면 위해서 말했다시피 데이터베이스의 최신 버전을 가져와서 보여준다.
  - 따라서 `REPEATABLE READ` 격리 레벨에서도 `Phantom Read`를 발생하게 된다.
- 아래 예시에서는 id 1~5까지만 데이터가 입력되어 있다고 가정

|                 Transaction1                  |                     Transaction2                     |
|:---------------------------------------------:|:----------------------------------------------------:|
|                                               |       SELECT * FROM USER WHERE id >= 5   // 1건       |
| INSERT INTO USER(id, name) VALUES (6, 'PARK') |                                                      |
|                                               | SELECT * FROM USER WHERE id >= 5 FOR UPDATE // 2건 조회 |

|                 Transaction1                  |                           Transaction2                           |
|:---------------------------------------------:|:----------------------------------------------------------------:|
|                                               |           SELECT * FROM USER WHERE id >= 5   // 1건 조회            |
| INSERT INTO USER(id, name) VALUES (6, 'PARK') |                                                                  |
|                                               | UPDATE USER SET name = 'KIM' WHERE id >= 5 FOR UPDATE // 2건 업데이트 |

