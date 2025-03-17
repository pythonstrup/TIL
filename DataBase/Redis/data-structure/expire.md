# 레디스 자료 구조의 만료시간

## Redis의 기본 데이터 저장 구조

```c
struct redisDb {
    dict *dict;     // 실제 키-값 데이터를 저장하는 딕셔너리
    dict *expires;  // 만료 시간 정보를 저장하는 별도의 딕셔너리
    // ...
}
```

## String 

- String은 Redis의 가장 기본적인 데이터 타입.

```shell
> SET <key> <value> [EX seconds]
```

- 위의 SET 명령어가 실행되면?
1. `redisDB->dict`에 키-값 쌍이 단일 `dictEntry`로 저장
2. `EX` 옵션이 있다면 동시에 `redisDb->expries`에 만료시간이 저장
- 단순한 구조 덕분에 값 설정과 만료시간 설정을 atomic하게 수행할 수 있다.

## Collection 타입의 복잡성

- Collection 타입은 내부적으로 더 복잡한 자료구조를 사용한다.

```shell
> HSET hash <field> <value>
> EXPIRE hash 3600
```

1. `redisDB->dict`에 키-값 쌍이 저장될 때, 값 부분이 복잡한 자료구조를 포인터로 가리킨다.
2. 해당 자료구조는 여러 요소들을 포함하고 있어서, 각 요소별로 다른 만료시간을 가질 수 없다.
3. 따라서 컬렉션 전체에 대한 만료시간은 `EXPIRE` 명령어로 따로 설정해야 한다.

> #### 참고: Collection 타입별 내부 구조
> - `Hash`: `ziplist` 또는 `hashtable` 구조
> - `Set`: `intset` 또는 `hashtable` 구조
> - `Sorted Set`: `skiplist`와 `hashtable` 조합
> - `List`: `quicklist`(`ziplist`들의 연결리스트) 구조

## Redis 자료구조와 명령어 흐름 처리

### String의 SET 커맨드

```c
void setCommand(client *c) {
    robj *key = c->argv[1];
    robj *val = c->argv[2];
    
    // 1. dict에 키-값 저장
    setKey(c->db, key, val);
    
    // 2. 만료시간 옵션이 있다면 expires에 저장
    if (expire_found) {
        setExpire(c, c->db, key, expire);
    }
    
    // 3. 한 번의 명령어로 처리 완료
    addReply(c, shared.ok);
}
```

### Hash의 HSET 커맨드

```c
void hsetCommand(client *c) {
    robj *o;
    
    // 1. 해시 객체 조회 또는 생성
    if ((o = hashTypeCreate(key)) == NULL) {
        if ((o = createHashObject()) == NULL) 
            return;
    }
    
    // 2. 해시 내부의 인코딩 타입 확인
    if (hashTypeGet(o) == REDIS_ENCODING_ZIPLIST) {
        // ziplist 인코딩일 경우의 처리
        if (hashTypeLength(o) >= hash_max_ziplist_entries)
            hashTypeConvert(o, REDIS_ENCODING_HT);
    }
    
    // 3. 해시에 필드-값 추가
    hashTypeSet(o, field, value);
}
```

## Redis Collection에 개별 expire 설정이 없는 이유는 뭘까?

- Redis는 O(1), O(logN) 성능을 보장하는 고속 데이터 구조를 제공한다.
- 만약 자료 구조에서 개별 요소에 대한 TTL을 지원한다면? Redis 내부적으로 복잡한 만료 스케줄링 로직이 필요해질 수도..? => 성능과 메모리 관리 부담


# 참고자료

- [[Redis] Collection 자료구조들은 왜 만료시간 설정이 한번에 안 될까?](https://velog.io/@haron/Redis-Collection-%EC%9E%90%EB%A3%8C%EA%B5%AC%EC%A1%B0%EB%93%A4%EC%9D%80-%EC%99%9C-%EB%A7%8C%EB%A3%8C%EC%8B%9C%EA%B0%84-%EC%84%A4%EC%A0%95%EC%9D%B4-%ED%95%9C%EB%B2%88%EC%97%90-%EC%95%88-%EB%90%A0%EA%B9%8C)
