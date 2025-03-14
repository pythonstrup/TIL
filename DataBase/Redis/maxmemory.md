# Redis의 메모리 용량?

- 레디스에서 사용할 수 있는 용량이 부족해지면 어떻게 될까?
- 적절한 정책을 설정하지 않으면 Redis 프로세스가 죽어버릴 수도 있다.

## 메모리 확인하기

### 현재 메모리 확인

- 아래는 k8s의 파드에 접근해서 확인하는 방법

```shell
$ kubectl exec -it <redis-pod-name> -- redis-cli info memory
```

- 레디스 세션이라면 아래 명령어로도 확인할 수 있다.

```shell
> INFO memory
```

- 아래는 결과값

```shell
# Memory
used_memory:4746416
used_memory_human:4.53M
used_memory_rss:15482880
used_memory_rss_human:14.77M
used_memory_peak:4867984
used_memory_peak_human:4.64M
used_memory_peak_perc:97.50%
used_memory_overhead:2411860
used_memory_startup:946744
used_memory_dataset:2334556
used_memory_dataset_perc:61.44%
allocator_allocated:5664416
allocator_active:6135808
allocator_resident:11362304
allocator_muzzy:0
total_system_memory:67422101504
total_system_memory_human:62.79G
used_memory_lua:31744
used_memory_vm_eval:31744
used_memory_lua_human:31.00K
used_memory_scripts_eval:0
number_of_cached_scripts:0
number_of_functions:0
number_of_libraries:0
used_memory_vm_functions:32768
used_memory_vm_total:64512
used_memory_vm_total_human:63.00K
used_memory_functions:296
used_memory_scripts:296
used_memory_scripts_human:296B
maxmemory:0
maxmemory_human:0B
maxmemory_policy:noeviction
allocator_frag_ratio:1.07
allocator_frag_bytes:365536
allocator_rss_ratio:1.85
allocator_rss_bytes:5226496
rss_overhead_ratio:1.36
rss_overhead_bytes:4120576
mem_fragmentation_ratio:3.26
mem_fragmentation_bytes:10736664
mem_not_counted_for_evict:14368
mem_replication_backlog:1048580
mem_total_replication_buffers:1066208
mem_clients_slaves:17632
mem_clients_normal:150072
mem_cluster_links:0
mem_aof_buffer:896
mem_allocator:jemalloc-5.3.0
mem_overhead_db_hashtable_rehashing:0
active_defrag_running:0
lazyfree_pending_objects:0
lazyfreed_objects:0
```

### 메모리 설정 확인

```shell
> CONFIG GET maxmemory-policy
[
  {
    "field": "maxmemory-policy",
    "value": "noeviction"
  }
]
```

---

## 메모리 설정하기

### `maxmemory` 설정하기

```shell
> CONFIG SET maxmemory 2gb
```

### `maxmemory-policy` 설정하는 방법

```shell
> CONFIG SET maxmemory-policy allkeys-lfu
```

- 사용할 수 있는 정책은 아래와 같다.

| 정책                     | 동작 방식                              | 설명                                     |
|:-----------------------|:-----------------------------------|:---------------------------------------|
| `noeviction` (default) | 새로운 데이터 추가 실패                      | 삭제 없이 `ERR OOM command not allowed` 발생 |
| `volatile-lru`         | LRU(Least Recently Used) 방식으로 제거   | TTL이 있는 키 중에서 가장 오래된 키 삭제              |
| `allkeys-lru`          | 모든 키 중 LRU 방식으로 제거                 | 사용되지 않은 키부터 삭제                         |
| `volatile-lfu`         | LFU(Least Frequently Used) 방식으로 제거 | TTL이 있는 키 중 접근 빈도가 낮은 키 삭제             |
| `allkeys-lfu`          | 모든 키 중 LFU 방식으로 제거                 | 적게 사용된 키부터 삭제                          |
| `volatile-random`      | TTL이 있는 키 중 랜덤 삭제                  | 캐시 데이터에 적합                             |
| `allkeys-random`       | 모든 키 중 랜덤 삭제                       | 완전한 캐시로 사용할 때 유용                       |

### redis.conf 파일에서 수정하기

```shell
maxmemory 1gb
maxmemory-policy allkeys-lfu
```

---

## 레디스의 메모리가 부족해진다면?

### 1. `maxmemory`가 없는 경우

- `maxmemory` 설정이 없을 경우, Redis는 시스템 전체 가용 메모리를 사용할 때까지 계속 성장한다.
- 결국 `OOM`으로 프로세스가 강제 종료될 수 있다.

### 2. `maxmemory`가 있는 경우

- 설정된 용량을 초과하면 Redis가 데이터를 제거하는 정책을 따른다.
- 이 정책은 `maxmemory-policy` 옵션으로 조정 가능하다.

| 정책                     | 동작 방식                              | 설명                                     |
|:-----------------------|:-----------------------------------|:---------------------------------------|
| `noeviction` (default) | 새로운 데이터 추가 실패                      | 삭제 없이 `ERR OOM command not allowed` 발생 |
| `volatile-lru`         | LRU(Least Recently Used) 방식으로 제거   | TTL이 있는 키 중에서 가장 오래된 키 삭제              |
| `allkeys-lru`          | 모든 키 중 LRU 방식으로 제거                 | 사용되지 않은 키부터 삭제                         |
| `volatile-lfu`         | LFU(Least Frequently Used) 방식으로 제거 | TTL이 있는 키 중 접근 빈도가 낮은 키 삭제             |
| `allkeys-lfu`          | 모든 키 중 LFU 방식으로 제거                 | 적게 사용된 키부터 삭제                          |
| `volatile-random`      | TTL이 있는 키 중 랜덤 삭제                  | 캐시 데이터에 적합                             |
| `allkeys-random`       | 모든 키 중 랜덤 삭제                       | 완전한 캐시로 사용할 때 유용                       |

### 3. `maxmemory`가 있지만 Eviction 설정이 없는 경우

- 기본값은 `noeviction`이므로, 메모리가 초과되었을 때 Redis는 새로운 데이터를 저장할 수 없다.

### 4. Swap 사용

- 메모리를 초과할 때 Swap을 통해 저장량을 늘리는 방법도 있다.

### 5. AOF/RDB 동작 중 메모리 부족?

- 저장 작업에 실패하거나 Redis 프로세스가 중단될 수도 있다.
