# 성능 테스트란?

- 소프트웨어 애플리케이션의 반응 시간 안정성, 확장성, 리소스 사용량 등을 측정하는 프로세스
- 시스템이 예상 부하를 견딜 수 있는지 확인하고, 성능 병목 현상을 파악하여 최적화하는 데 목적을 둔다.

## 성능 테스트의 필요성?

- 실제 서비스. 여러 사람들이 동시에 서버에 접속해 작업을 진행하고, 사용자가 늘어나면 늘어날수록 부하가 발생한다.
    - 이 과정에서 당연하게 발생하는 문제들. Slow Query, Lock, Connection Timeout 문제 등
    - 동일 서버 사양에서 더 많은 사용자에게 최고의 서비스를 제공하는 것이 서버 개발자의 숙명
- 시스템의 응답 시간, 처리량, 안정성 등을 평가할 수 있다. 이를 통해 발견된 문제를 사전에 해결할 수 있다.
    - 사용자에게 최적의 경험 제공
- 실제 운영 환경에서의 성능을 예측하고 최적화하는 데 중요한 역할을 한다.

## 성능 테스트 분류

### 1. 부하 테스트 Load Testing

- 예상되는 최대 사용자 수를 시뮬레이션

### 2. 스트레스 테스트 Stress Testing

- 시스템이 한계치 이상으로 과부하 상태에서 어떻게 반응하는지 확인

### 3. 내구성 테스트 Endurance Testing

- 시스템이 장시간 동안 안정적으로 작동하는지를 검증

### 4. 스파이크 테스트 Spike Testing

- 짧은 시간 동안 급격한 변화를 주어 시스템의 반응을 측정

### 5. 볼륨 테스트 Volume Testing

- 대량의 데이터를 처리할 때 시스템 성능 측정

## 성능 테스트 이론

### 용어

#### Transaction (TX)

- DB 트랜잭션과는 다른 개념
- 논리적인 업무 요청의 단위를 트랜잭션이라고 함.
- 사용자가 한 번의 요청을 보내는 단위 요청

#### 총 사용자 Tatal User

- `Active User` + `InActive User` + 비접속자

#### 현재 서비스 요청자 Active User

- 현재 서비스에 요청을 보낸 상태에 있는 사용자

#### 서비스 대기자 InActive User

- 서비스에 접속은 하고 있으나 아직 요청을 보내지 않은 사용자

#### 동시 사용자 Concurrency User

- `Active User` + `InActive User`

#### 응답 시간 Response Time

- 사용자가 서버로 요청을 보낸 시점부터 응답이 오기까지의 시간

#### 생각 시간 Think Time

- 사용자가 응답을 받고 화면 등을 보면서 생각하는 시간

#### 조작 시간 Operation Time

- 작업 요청을 보내기 위해서 데이터를 입력하거나, 화면을 채우는 시간
- 일반적으로 생각 시간에 포함시킨다.

#### 처리량 Throughput

- 단위 시간당 처리되는 건수
    - TPS, Transaction Per Second
    - RPS, Request Per Second
- 처리량 계산 방법
    - `Throughput` = `ActiveUser` / `Average Response Time`
- 따라서 처리량을 통해 Active User을 계산하는 방법은 아래와 같다.
    - `ActiveUser` = `Throughput` * `Average Response Time`
- `요청 주기 Request Interval` 계산 방법
    - `Request Interval` = `Average Response Time` + `Think Time` + `Operationtime`

## 성능 테스트 툴 종류와 장단점

### JMeter

- 아파치 재단에의 오픈 소스
- Java로 구축한 오픈 소스 성능 테스트 도구

#### JMeter 장점

1. 강력한 GUI가 존재한다.
    - CLI보다 친숙하다.
    - JMeter처럼 GUI에서 다양한 기능을 사용할 수 있는 툴은 찾기 어렵다.

2. 풍부한 문서 자료 & 커뮤니티
    - JMeter의 역사는 20년 이상이기 때문에 그동안 쌓여온 수많은 사용 사례와 노하우가 있다.

3. 분산 부하 테스트 기능을 기본으로 제공한다.
    - 분산 부하 테스트란 테스트에서 가상 사용자의 수를 늘리고 다른 부하 생성기를 사용하여 다중 인스턴스를 작동시키는 것을 말한다.
    - JMeter는 컨트롤러 노드와 워커 노드들을 분리하여 분산 부하 테스트를 수행할 수 있다.

#### JMeter 단점

1. 불편한 스크립팅
    - XML로 스크립트가 작성된다.

2. 메모리를 많이 사용한다.
    - 3개의 요청을 보내는 간단한 테스트에셔도 600MB 이상을 사용한다.

### Postman

### Gatling

### k6

- Grafana Labs에서 운영하는 오픈 소스 성능 테스트 툴
    - 2017년 출시

#### k6의 장점

1. 편리한 스크립팅
    - 자바스크립트로 스크립트를 작성한다.
    - 반면 JMeter는 스크립트를 XML로 작성한다고 한다.

2. 리소스를 적게 사용한ㄷ나.
    - GUI가 없기 때문에 리소스 오버헤드가 일어나지 않는다.
    - 특히 적은 메모리 사용량을 자랑하는데 3개의 요청을 보낼 때 약 100MB 정도의 메모리를 사용한다고 함. (JMeter의 1/6 수준)

#### k6 단점

1. GUI가 없다.

### LOCUST

- 사용자가 Python으로 시나리오를 작성하여 웹 애플리케이션의 성능을 측정할 수 있게 해준다.
- 이벤트 기반 모델을 사용하여 다수의 사용자를 시뮬레이션할 수 있다.
- Locust는 `web-based` UI를 가지고 있으며, HTTP나 다른 프로토콜에 대한 부하 테스트를 실시간으로 실행할 수 있다.

#### LOCUST 장점

1. 다른 성능테스트 툴에 비해 가볍고 빠르며 간편하다.
2. python에 능숙한 개발자라면 쉽게 사용 가능하다.

### nGrinder

# 참고자료

- [성능테스트 툴 소개](https://velog.io/@dongvelop/%EC%84%B1%EB%8A%A5%ED%85%8C%EC%8A%A4%ED%8A%B8-%ED%88%B4-%EC%86%8C%EA%B0%9C)
- [k6 vs JMeter, 어느 성능 테스트 도구를 써야 할까?](https://velog.io/@yongtae923/k6-vs-JMeter)
- [IT 분야에서의 성능 테스트의 중요성과 기술](https://f-lab.kr/insight/importance-of-performance-testing)
- [서버의 성능 테스트 [필요성, 개요, 간단한 구현]](https://velog.io/@imkkuk/%EC%84%9C%EB%B2%84%EC%9D%98-%EC%84%B1%EB%8A%A5-%ED%85%8C%EC%8A%A4%ED%8A%B8-%ED%95%84%EC%9A%94%EC%84%B1-%EA%B0%9C%EC%9A%94-%EA%B0%84%EB%8B%A8%ED%95%9C-%EA%B5%AC%ED%98%84)
- [성능 Test와 K6 도구 소개](https://github.com/schooldevops/k6-tutorials/blob/main/UsingK6/99_K6_Seminar.md)
