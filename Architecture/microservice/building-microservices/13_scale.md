# Chapter13. 확장

- 시스템을 `확장 scale`하는 이유
  1. 더 많은 부하를 처리하거나 지연 시간을 개선하는 등 시스템 성능을 향상시키기 위해
  2. 시스템의 견고성을 개선하기 위해

## 1. 확장의 네 가지 축

### 1-1. 수직 확장 vertical scaling

- 더 빠른 CPU.
- 더 높은 I/O를 갖춘 큰 머신.
- 스펙이 좋은 하드웨어를 구매하는 것은 시간도 오래 걸리고, 과도한 비용을 지출할 가능성이 높다.

#### 구현

- 인프라스트럭처에 따라 다르다.
- 자체 가상화 인프라스트럭처에서 실행하는 경우 하부의 하드웨어를 더 많이 사용하도록 VM의 크기를 조정하기만 하면 된다.
  - 빠르고 위험 부담 없이 구현할 수 있다.
- 일반적으로 수직 확장을 시도하기 위해 새로운 인프라스트럭처를 구매해야 하는 상황에 도달했다면, 비용이 증가하므로 당분간 이 형태의 확장은 피하고 수평 복제를 고려할 수 있다.

#### 주요 이점

- 가상화된 인프라스트럭처 상에서는 이러한 형태의 확장을 빠르게 구현할 수 있다.

#### 제한 사항

- CPU가 실제로 빨라지는 것이 아니라 코어만 더 많아진다.
    - 애플리케이션을 4코어 시스템 상에서 8코어 시스템으로 옮겨도 기존 시스템이 CPU에 종속돼 있어 개선 효과가 거의 없는 경우도 있다.
    - 멀티코어 하드웨어를 활용하기 위해 코드를 변경하는 것은 상당한 작업이 될 수 있으며, 그에 따라 `프로그래밍 이디엄 programming idiom`을 완전히 바꿔야 할 수도 있다.

### 1-2. 수평 복제 horizontal duplication

- 시스템의 일부를 복제해 더 많은 워크로드를 처리.
- 수평 복제를 사용하려면 이러한 복제본들에게 작업을 분산하는 방법이 필요하다.

#### 구현

- 로드 밸런서를 통해 여러 기능 복제본으로 요청을 분산시키는 것이 가장 유명한 방법.
- 데이터베이스의 수평 복제
  - 읽기 복제본을 사용해 기본 데이터베이스의 읽기 부하를 줄일 수 있다.
  - 주 데이터베이스나 읽기 복제본에 대한 라우팅은 마이크로서비스에서 내부적으로 처리된다.

#### 주요 이점

- 비교적 간단하다.

#### 제한 사항

- 더 많은 인프라스트럭처가 필요하므로 당연히 더 많은 비용이 발생할 수 있다.
  - 과잉 상황: 확장 문제가 모놀리식 애플리케이션 일부에서만 발생하더라도 애플리케이션 전체 복제본을 여러 개 실행해야 한다.

### 1-3. 데이터 파티셔닝 data partitioning

- 데이터의 일부 측면을 기반으로 부하를 분산시키는 것.
- 예를 들면 사용자를 기반으로 부하를 분산시킬 수 있다.

#### 구현

- 워크로드와 관련된 키를 가져와 그 키에 함수를 적용하고 그 결과로 얻은 파티션에 부하를 분산.

#### 주요 이점

- 트랜잭션 워크로드에 매우 유용하게 확장.
  - 예를 들어 쓰기 제약이 있는 시스템의 경우 데이터 파티셔닝을 통해 큰 개선 효과를 얻을 수 있다.
- 또한 여러 파티션을 생성하면 유지 보수 활동의 영향도와 범위를 더 쉽게 줄일 수 있다.
  - 파티션 별로 롤아웃. 다운타임이 필요한 작업도 단일 파티션에만 영향을 미치므로 영향도를 낮출 수 있다.

#### 제한 사항

- 시스템 견고성을 개선하는 데 한계가 있다는 점은 지적할 필요가 있다.
  - 파티션에 장애가 발생하면 해당 부분의 요청이 실패한다.
- 또한 파티션 키를 올바르게 설정하는 것은 어려울 수 있다.
  - 데이터 파티셔닝을 통해 부하가 고르게 분산하기 위한 방법을 고려해야 한다.
- 쿼리에서도 문제가 발생할 수 있다.
  - 개별 레코드는 쉽다. 해싱 함수를 적용해 데이터가 어느 인스턴스에 있는지 찾은 다음 올바른 샤드에서 조회하면 되기 때문이다.
  - 하지만 여러 노드에 산재한 데이터에 걸쳐 쿼리를 한다면? 각 개별 샤드를 쿼리하고 메모리에서 조인하거나 두 테이터 집합을 모두 사용 가능한 읽기 대체 저장소가 있어야 한다.
  - 샤드 전반을 교차하는 쿼리는 캐싱된 결과를 사용하는 비동기 메커니즘에 의해 처리되는 경우가 많다.

### 1-4. 기능 분해 functional decomposition

- 기능을 추출해 독립적으로 확장.
- 기존 시스템에서 기능을 추출해 새로운 마이크로서비스를 만드는 것은 기능 분해의 표준적인 예시.

#### 주요 이점

- 다양한 유형의 워크로드를 분리했으므로 이제 시스템에 필요한 기본 인프라스트럭처의 크기를 적절히 조정할 수 있다.
- 기능 분해 그 자체로 시스템을 더 견고하게 만들지는 못하지만 적어도 부분적인 기능 장애를 견딜 수 있는 시스템을 구축할 기회를 열어준다.

#### 제한 사항

- 복잡한 작업일 수 있으며 단기간에 이점을 제공하기는 어렵다.
- 모든 형태의 확장 중에서 프론트엔드 및 백엔드 애플리케이션 코드에 모두 가장 큰 영향을 미칠 수 있는 확장 방식이며, 또한 마이크로서비스로 전환하는 경우 데이터 계층에서 상당한 양의 작업이 필요할 수 있다.

<br/>

## 2. 결합 모델

- 여러 복제본을 분리해 확장할 수도 있으며, 서로 다른 지리적 위치에 따라 마이크로서비스의 서로 다른 샤드를 실행하기로 결정할 수도 있다.
- 한 축에 따라 확장하면 다른 축을 더 쉽게 활용할 수 있다.

## 3. 작게 시작하라.

> 진짜 문제는 프로그래머들이 엉뚱한 곳에서 엉뚱한 시간에 효율성에 대해 걱정하는 데 너무 많은 시간을 소비하는 것이며, 조기 최적화는 프로그래밍에서 악의 근원이다.

- 존재하지 않는 문제를 해결하기 위해 시스템을 최적화하는 것은 다른 활동에 더 잘 사용할 수 있는 시간을 낭비하고 불필요하게 더 복잡한 시스템을 만드는 지름길이 되고 만다.
  - 따라서 모든 형태의 최적화는 실제 필요에 의해 주도돼야 한다.
- 병목 현상을 파악했다고 생각하더라도 여러분이 옳은지, 추가 작업은 합당한지 확인하려면 실험 과정이 필수적이다.
  - 문제라고 인식했다면, 제안한 솔루션이 효과가 있는지 확인하기 위해 수행 가능한 작은 작업을 찾아보라.

## 4. 캐싱

- 흔히 사용되는 성능 최적화 방법

### 4-1. 성능을 위해

- 네트워크를 호출을 하지 않아도 되므로 다운스트림 마이크로서비스의 부하를 줄이는 데 큰 도움이 된다.
- 네트워크 홉을 피할 수 있을 뿐만 아니라 각 요청에 대한 데이터를 생성할 필요성도 줄어든다.

### 4-2. 확장성을 위해

- 읽기를 캐시로 전환할 수 있다면 시스템 일부분에서 발생하는 경합을 피해 더 나은 확장성을 확보할 수 있다.
- 확장성을 위해 캐싱은 원본이 경합 지점이 되는 모든 상황에서 유용하다.
- 클라이언트와 원본 사이에 캐시를 배치하면 원본의 부하를 줄여 확장성을 높일 수 있다.

### 4-3. 견고성을 위해

- 로컬 캐시에 전체 데이터 집합이 들어 있다면 원본이 가용하지 않을 경우에도 작동할 수 있으므로 시스템의 견고성이 향상될 수 있다.
- 주의 사항
  - 가장 중요한 것은 오래된 데이터를 자동으로 제거하지 않도록 캐시 무효화 메커니즘을 구성하고 업데이트할 수 있을 때까지 캐시에 보관해야 한다는 것이다.
  - 그렇지 않으면 데이터가 무효화되면서 캐시에서 제거돼 캐시 미스가 발생하고 원본을 사용할 수 없으므로 데이터를 가져오는 데 실패하게 된다.

### 4-4. 캐싱 위치

- 캐시를 저장할 수 있는 다양한 장소가 있다.

#### 클라이언트 측 캐시

- 원본의 범위를 벗어난 곳에 캐싱
- 일반적으로 클라이언트 캐싱은 다운스트림 마이크로서비스에 대한 네트워크 호출을 피할 수 있기 때문에 매우 효과적인 측면이 있다.
- 따라서 클라이언트 측 캐싱은 지연 시간을 개선하기 위한 캐싱뿐만 아니라 견고성을 위한 캐싱에도 적합하다.
- 단점
  1. 무효화 메커니즘과 관련된 선택 범위가 제한되기 더 쉽다.
  2. 클라이언트 측 캐싱이 많이 늘어나면 클라이언트 사이에 어느 정도 불일치가 발생할 수 있다. 
- 단점을 완화하는 방법
  - 클라이언트 측 캐시를 공유하거나 `레디스 redis`나 `멤캐시드 memcached`와 같은 전용 캐싱 도구를 사용하는 것이다.
  - 서로 다른 클라이언트 간에 불일치가 발생하는 상황을 피할 수 있다.
  - 또한 관리해야 할 데이터 복사본 수가 줄어들기 때문에 리소스 사용 측면에서도 더 효율적이다. 

#### 서버 측 캐시

- ex. 판매 Top 10 예시
- 소비자 입장에서 이 캐시는 내부 구현이므로 보이지 않지만, 그렇다고 해서 마이크로서비스 인스턴스의 코드에서 캐싱을 구현해야 한다는 의미는 아니다.
  - 역방향 프록시, 숨겨진 레디스 노드, 읽기 쿼리를 데이터베이스의 복제본으로 전환 등
- 이러한 형태의 캐싱에서 나타나는 가장 큰 문제! 소비자가 마이크로서비스까지 왕복해야 하므로 지연 시간을 최적화할 수 있는 범위가 줄어든다는 점이다.

#### 요청 캐시

- 원본 요청에 대한 응답을 캐싱해 저장.
  - ex. 상위 10개 베스트셀러 항목
- 매우 효율적이지만 이러한 형태의 캐싱은 매우 한정적이라는 점을 알고 있어야 한다.
  - 특정 요청의 결과만 캐싱한다.

### 4.5 무효화

> 컴퓨터 과학에서 어려운 것은 캐시 무효화와 작명. 이 두 가지뿐이다.
> - 필 칼튼

- 캐시에서 데이터를 제거하는 과정.
- 개념은 간단하지만 실행은 복잡하다. 무효화는 구현 방법이 다양하고 오래된 캐시 테이블을 활용할 때 고려해야 할 절충점이 많다는 것 외에 다른 이유는 없다.

#### Time To Live, TTL

- 가장 간단한 메커니즘
- 특정 기간 동안만 유효한 것으로 간주.
- 단점
  - 그다지 정교하지 못하다.

#### 조건부 GET

- HTTP로 조건부 GET 요청을 전송
  - ETag: 리소스 값이 변경됐는지 확인하는 데 사용.
  - 추가 헤더를 지정해 특정 기준이 충족되는 경우에만 클라이언트에게 자원을 내려보내도록 서비스에 지시할 수 있다.

#### 알림 기반

- 알림 기반 무효화에서는 이벤트를 사용해 구독자가 로컬 캐시 항목을 무효화할지 여부를 알 수 있도록 해준다.
  - 가장 우아한 메커니즘이지만 TTL과 비교하면 상대적으로 복잡하다.
- 장점
  - 오래된 데이터를 제공할 가능성을 줄인다.

#### 연속 쓰기 write-through

- 연속 쓰기 캐시를 사용하면 원본의 상태와 동시에 캐시가 업데이트된다.
- 물론 '동시에'라는 표현 때문에 연속 쓰기 캐싱이 까다로워지는 부분이 있다.
- 서버 측 캐싱에서 연속 쓰기 메커니즘을 구현하면 동일한 트랜잭션 내에서 데이터베이스와 인메모리 캐시를 큰 어려움 없이 업데이트할 수 있으므로 비교적 간단하다.

### 나중 쓰기 write-behind

- 캐시 자체가 먼저 업데이트된 다음 원본이 업데이트되며, 개념적으로 캐시를 버퍼로 생각할 수 있다.
  - 캐시에 쓰는 것이 원본을 업데이트하는 것보다 빠르다.
- 주요 우려 사항
  - 데이터 손실 가능성.
  - 캐시 자체의 내구성이 떨어지면 데이터가 원본에 기록되기 전에 손실될 수 있다.

### 4-6. 캐싱의 황금 법칙

- 너무 많은 곳에 캐시를 만들지 않도록 주의하라.
- 새로운 데이터의 출처와 여러분 사이에 캐시가 많을수록 데이터가 더 오래될 수 있으며, 클라이언트가 최종적으로 보게 되는 데이터가 최신인지 판단하기가 더 어려워질 수 있다.
- 또한 데이터를 무효화해야 할 위치를 추론하기도 더 어려워질 것이다.

> #### TIP
> 캐싱은 주로 성능 최적화를 위한 것으로 고려하라. 또한 데이터의 신선도를 더 쉽게 추론할 수 있도록 가능한 한 적은 위치에 캐시를 저장하라.

### 4-7. 신선도 대 최적화

- 힘의 균형을 맞추는 것은 최종 사용자와 더 광범위한 시스템의 요구 사항을 이해하는 데 달려 있다.
- 캐시가 적을수록 시스템에 대한 추론이 더 쉬워지므로, 우리가 가능한 한 단순함을 유지해야 하는 또 다른 이유가 된다.

### 4-8 캐시 중독: 주의해야 할 이야기

- 캐싱은 실제로 매우 강력할 수 있지만 소스에서 목적지까지 캐싱되는 데이터의 전체 경로를 이해해야 캐싱의 복잡성과 발생하게 될 문제를 제대로 인식할 수 있다.

<br/>

## 5. 자동 확장

- 확장을 트리거하는 다양한 부하를 재현할 수 있는 테스트가 없다면, 확장 규칙의 오류 여부를 운영 환경에서야 알 수 있다.
  - 테스트는 필수.
- 반응형 확장과 예측적 확장은 둘 다 매우 유용하며, 사용한 컴퓨팅 자원에 대해서만 비용을 지불하는 플랫폼을 사용하는 경우 훨씬 더 비용 효율적으로 사용할 수 있다.

## 6. 다시 시작하기

- 특정 시점에서는 다음 단계의 성장을 지원하기 위해 시스템 아키텍처를 변경하려면 상당히 급진적인 조치를 취해야 한다.
- 특정한 확장 임계값에 도달하면 사람들은 처음부터 대규모로 구축해야 한다는 이유로 재설계가 필요하다고 인식할 위험이 있다.
  - 이는 재앙이 될 수 있다.
  - 처음부터 대규모로 구축하려고 하면 결국 발생하지 않을지도 모를 부하를 대비하기 위해 엄청난 양의 작업을 미리 수행해야 하고, 그로 인해 더 중요한 활동에 공을 들이지 못하게 된다.

<br/>

# 참고 자료

- 마이크로서비스 아키텍처 구축, 샘 뉴먼 지음