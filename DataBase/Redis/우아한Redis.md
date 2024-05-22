# 우아한 Redis

## Redis?

- In-Memory Data Structure Store
- Support Data Structure
  - String
  - set
  - sorted-set
  - hashes
  - list
  - hyperloglog
  - bitmap
  - geospatial index
  - stream

## Cache?

- 나중에 요청올 결과를 미리 저장해두었다가 빠르게 서비스 해주는 것

### 1. Look aside Cache

- 보편적으로 많이 사용하는 방식 
- Web Server가 Cache를 먼저 확인하고 값이 없으면 DB에 접근한다.

### 2. Write Back

- 쓰기 작업이 굉장히 빈번할 때 사용
- 쓰기를 Cache에 저장해두고 특정 시점마다 Cache에 있는 값을 꺼내 DB에 쓰기 작업 수행
- DB의 쓰기 작업이 수행되면 Cache에 있는 데이터는 삭제한다.
- 치명적인 단점이 있는데, 프로그램이 죽어버리면 값이 다 날라가버릴 수 있다는 점이다.

<br/>

## 왜 Collection 이 중요한가?

- 개발의 편의성과 개발의 난이도 때문
- 개발 시간 단축, 문제 줄여줌

### 개발의 편의성

- 랭킹 서버를 직접 구현한다면??
- 가장 간단한 방법 
  - DB에 유저의 Score를 저장하고 Score로 order by로 정렬 후 읽어오기
  - 하지만, 개수가 많아지면 속도에 문제가 발생 (디스크를 사용하기 때문에)
- In-Memory 기준으로 랭킹 서버의 구현?
  - 구현이 어렵다.
- Redis의 Sorted Set을 사용
  - Redis에서 제공하는 기능이라 쉽게 사용 가능
  - 덤으로 Replication도 가능하다.
  - 다만 가져다 쓰는 것은 그 한계에 종속적일 수 밖에 없다.
    - 랭킹에 저장할 id가 1개당 100byte라고 하면? 10명(1K), 10,000명(1M), 10,000,000(1G), 10,000,000,000(1TB)

### 개발의 난이도

- 친구 리스트를 개발한다면?
- 친구 리스트를 KEY/VALUE 형태로 저장해야 한다면?
- DB를 사용했을 때, Race Condition 때문에 트랜잭션이 여러 개가 동시에 실행되기 때문에 친구가 덮어쓰기 되어버릴 수 있다.
- Redis의 자료구조는 Atmoic하기 때문에 해당 Race Condition을 피할 수 있다.
  - 그래도 잘못 짜면 Race Condition이 발생하기도 한다.

<br/>

## Redis는 어디에 써야하는가?

- Remote Data Store
  - A, B, C 서버에서 데이터를 공유하고 싶을 때
- 주로 많이 쓰는 곳들
  - 인증 토큰 등을 저장(String 또는 Hash)
  - Ranking 보드로 사용(Sorted Set)
  - 유저 API Limit
  - 잡 큐(list)

<br/>

## Redis Collection

### String

- Key/Value로 저장하는 방식
- 기본 사용법
  - Set <key> <value>
  - Get <key>
  - ex) Set token:1234567 abcdefghijklmn, Get token:1234567
- 다중 사용법
  - mset <key1> <value1> ... <keyN> <valueN>
  - mget <key1> ... <keyN>

### List

- 보통의 리스트 자료구조와 같다
- 입력
  - Lpush <key> <A> => Key: (A)
  - Rpush <key> <B> => Key: (A, B)
  - Lpush <key> <C> => Key: (C, A, B)
  - Rpush <key> <D, A> => Key: (C, A, B, D, A)
- 값 꺼내기. Key: (C, A, B, D, A)
  - Lpop <key> => Pop C, Key: (A, B, D, A)
  - Rpop <key> => Pop A, Key: (A, B, D)
  - Rpop <key> => Pop D, Key: (A, B)
- pop 심화. Key: () 비어있을 때
  - Lpop <key> => No Data
  - BLpop <key> => 누군가 데이터를 Push 하기 전까지 대기

### Set

- 보통의 Set 자료구조와 같다. 이미 존재하는 값은 입력되지 않는다.
- 데이터가 있는지 없는지만 체크하는 용도로 쓰인다.
- 기본 사용법
  - SADD <key> <value>
  - SMEMBERS <key> => 모든 value를 돌려준다. (주의! 데이터가 많을 수 있기 때문에!)
  - SISMEMBER <key> <value> => value가 존재하면 1, 없으면 0
- 어디에 사용할까?
  - 특정 유저를 Follow 하는 목록을 저장

### Sorted Sets

- Set 자료구조와 유사하지만 정렬되어 있다.
- 랭킹에 따라 순서가 바뀌길 원한다면 Sorted Set을 사용하는 것이 좋다.
- 기본 사용법
  - ZADD <key> <score> <value> => value가 이미 key에 있으면 해당 score만 변경
  - ZRANGE <key> <startIndex> <endIndex> => 해당 Index 범위 값을 모두 돌려준다.
    - Zrange <key> 0 -1 => 모든 값을 가져온다.
  - ZREVRANGE <key> <startIndex> <endIndex> => 해당 Index 범위 값을 거꾸로 돌려준다.
- score는 double 타입이기 때문에 값이 정확하지 않을 수 있다.
  - 컴퓨터에서는 실수가 표현할 수 없는 정수값들이 존재
- 어디에 사용할까?
  - 유저 랭키 보드

### Hash

- value가 key/value로 입력될 수 있는 자료구조
- 기본 사용법
  - Hmset <key> <subKey1> <value1> ... <subKeyN> <valueN>
  - Hgetall <key> => 해당 Key의 모든 subKey와 value를 가져온다.
  - Hget <key> <subKey>
  - Hmget <key> <subKey1> ... <subKeyN>

### Collection 주의 사항

- 하나의 컬렉션에 너무 많은 아이템을 담으면 좋지 않다.
  - 10,000개 이하 혹은 수천개 수준으로 유지
- Expire는 collection의 item 개별로 걸리지 않고 전체 Collection에 대해서만 걸린다.
  - 10,000개 아이템을 가진 Collection에 expire가 걸려있으면 그 시간 후에 10,000개의 아이템이 전부 삭제된다.

<br/>

## Redis 운영

### 메모리 관리를 잘하자

- Redis는 In-Memory Data Store이기 때문에 Physical Memory 이상을 사용하면 문제가 발생한다.
  - Swap이 있다면 Swap 사용으로 해당 메모리 Page에 접근할 때마다 늦어진다. (왜? 한 번 Swap을 사용하면 계속 Swap을 사용하기 때문에)
  - Swap이 없다면? 죽는다.
- Maxmemory를 설정하더라도 이보다 더 사용할 가능성이 크다.
- RSS 값을 모니터링 해야한다.
- 현재 메모리를 사용해서 Swap을 쓰고 있다는 것을 모르고 있는 경우가 많다.
- 큰 메모리를 사용하는 instance 하나보다는 적은 메모리를 사용하는 instance 여러 개가 안전하다.
- Redis는 메몸리 파편화가 발생할 수 있다. 4.x 버전부터 메모리 파편화를 줄이도록 jemalloc에 힌트를 주는 기능이 들어갔으나, jemalloc 버전에 따라서 다르게 동작할 수 있다.
- 다양한 사이즈를 가지는 데이터보다는 유사한 크기의 데이터를 사용하는 것이 메모리 파편화를 줄이는데 도움이 된다.

### 메모리를 줄이기 위한 설정

- 기본적으로 Collection은 아래와 같이 자료구조를 활용
  - Hash -> HashTable을 하나 더 사용
  - Sorted Set -> Skplist와 HashTable 사용
  - Set -> HashTable 사용
  - 따라서 해당 자료구조들은 메모리를 많이 사용한다.
- Ziplist를 활용하는 것도 방법!

### Ziplist 구조

- In-Memory 특성 상 적은 개수라면 선형 탐색을 해도 빠르다.
- Ziplist는 이 점을 노려 선형으로 데이터를 저장해버린다.
- List, Hash, Sorted Set 등을 ziplist로 대체해서 처리하는 설정이 존재한다.
  - hash-max-ziplist-entries / hash-max-ziplist-value
  - list-max-ziplist-entries / list-max-ziplist-value
  - zset-max-ziplist-entries / zset-max-ziplist-value

### O(N) 관련 명령어는 주의


### Replication


### 권장 설정 Tip


<br/>