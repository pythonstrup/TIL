# 사용자 수에 따른 규모 확장성

# 단일 서버

- 웹브라우저, 웹서버, DNS
- 사용자 요청이 어떻게 처리되는지?

1. 사용자는 도메인 주소를 입력 (like www.google.com)
2. DNS(Domain Name Server)에 도메인 주소에 따른 IP 질의
3. 클라이언트는 IP 주소를 반환받음
4. 해당 IP로 HTTP 요청
5. 웹 서버에서 HTML 페이지나 JSON 데이터를 전달

## 데이터베이스

- 관계형 데이터베이스 vs 비관계형 데이터베이스

### 비관계형 데이터베이스가 적합한 경우

- 아주 낮은 지연시간(Latency)이 요구될 때
- 다루는 데이터가 비정형(unstructured)이라 관계형 데이터가 아닐 때
- 데이터(JSON, YAML, XML)를 직렬화 혹은 역직렬화할 수 있기만 하면 될 때
- 아주 많은 양의 데이터를 저장해야할 때

## 수직적 규모 확장 vs 수평적 규모 확장

- Scale Up: 수직적 규모 확장
- Scale Out: 수평적 규모 확장

- Scale Up??
  - 장점
    - 확장이 단순
  - 단점
    - CPU나 메모리를 무한대로 늘릴 수 X
    - 장애 자동복구(failover) 방안이나 다중화(redundancy) 방안을 제시하지 않기 때문에 장애가 발생하면 웹사이트/앱이 완전히 중단
- 따라서 대규모 서비스 운영을 위해서라면 수평적 규모 확장을 고려하는 것이 좋다.

<br/>

# 부하 분산

## 로드밸런서

- 부하 분산 집합(Load Balancing Set)에 속한 웹 서버들에게 트래픽 부하를 고르게 분산하는 역할

![img.png](img/load_balancer.png)

- 자동 복구 문제를 해소할 수 있으면 웹 계층의 가용성 향상

## 데이터베이스 다중화

- 주(master)-부(slave) 관계를 설정하고 데이터 원본은 주서버에 사본은 부서버에 저장
- 주서버는 쓰기 연산
- 부서버는 읽기 연산

### 데이터베이스 다중화의 장점

- 성능이 좋아진다. 읽기 연산이 분산된다. 병렬로 처리될 수 있는 query의 수가 늘어난다.
- 안정성(reliability) 증대. DB 하나가 소실되어도 데이터가 보존되어 있기 때문에 복구가 가능하다.
- 가용성(availability) 증대. 하나의 DB에 장애가 발생하더라도 대체 가능.

## 결론

- 웹 서버 부하 분산 = 로드밸런서
- 데이터베이스 서버 부하 분산 = 데이터베이스 다중화

<br/>

# 지연시간(Latency) 개선

## 캐시

- 캐시를 사용하면 응답시간을 개선할 수 있다.
- 애플리케이션의 성능은 데이터베이스를 얼마나 자주 호출하느냐에 크게 좌우 -> 캐시가 이를 완화

### 캐시 계층

- 별도의 캐시 계층은 성능 개선에도 도움이 되지만 데이터베이스의 부하도 줄일 수 있다.
- 캐시 계층의 규모를 독립적으로 확장하는 것도 가능하다.

![read-through_caching_strategy.png](img/read-through_caching_strategy.png)

- 위는 읽기 주도형 캐시 전략이다. 이 외에도 다양한 캐시 전략이 있다.
  - 데이터 종류, 크기, 액세스 패턴에 맞는 전략은 선택

### 사용할 때의 유의점

- 데이터 갱신은 자주 일어나지 않고 참조가 빈번할 때 고려
- 영속적으로 보관할 데이터를 캐시에 담지 말자.
- 적당한 만료 기한이 중요하다. 너무 짧아도 데이터베이스를 자주 읽게 되고, 너무 길어도 원본가 차이가 발생하기 때문에 문제가 된다.
- 데이터베이스와의 일관성 유지
- 장애 대처. 캐시 서버가 하나만 존재하면 단일 장애 지점(Single Point of Failure, SPOF)가 될 가능성이 있다. 캐시 서버를 분산하자.
- 적당한 메모리 크기.
- 데이터 방출 정책. 캐시가 꽉 찼을 때, 새로운 데이터를 넣어야할 경우 기존 데이터를 내보내야 한다. LRU, LFU, FIFO 등의 정책.

<br/>

## 콘텐츠 전송 네트워크 CDN

- 이미지, 비디오, CSS, Javascript 파일 등을 캐시
- 클라이언트가 웹 사이트를 방문하면 지리적으로 가까운 CDN 서버가 정적 콘텐츠를 사용자에게 전달한다.
- 지리적으로 분산된 네트워크이기 때문에 거리가 멀면 응답이 느려진다.

### CDN 사용 시 고려사항

- 비용: CDN은 보통 제 3 사업자에 의해 운영. 데이터 전송 양에 따라 요금을 내기 때문에 자주 참조되지 않는 콘텐츠를 캐싱해두는건 비용적으로 손실이 있다.
- 적절한 만료 시간
- 장애 대처 방안. CDN이 응답하지 않을 경우 어떻게 대처할 지 생각해봐야 한다. 예를 들어, 원본 서버에서 직접 콘텐츠를 가져오도록 클라이언트를 구성할 수 있다.
- 콘텐츠 무효화.

<br/>

# 웹 계층의 수평적 확장

- 웹 계층의 수평적 확장을 위해 상태 정보(ex. 사용자 세션 데이터)를 웹 계층에서 제거해야 한다.
- 바람직한 전략은 상태 정보를 관계형 데이터베이스나 NoSQL 같은 지속성 저장소에 보관하고, 필요할 때 가져오도록 하는 것이다.
- 이렇게 구성된 웹 계층을 무상태 웹 계층이라고 부른다.

## 무상태(Stateless) 웹 계층
