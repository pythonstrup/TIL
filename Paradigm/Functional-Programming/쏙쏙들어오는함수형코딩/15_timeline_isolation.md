# 15 타임라인 격리하기

- 타임라인 다이어그램을 통해 소프트웨어가 어떻게 동작하는지 이해해보자.

## 타임라인 다이어그램?

- 코드를 보거나 유스케이스 다이어그램을 봤을 땐 잘 동작하는 것 같을 때, 타임라인 다이어그램을 사용하면 큰 도움이 된다.
- 시간에 따라 실행되는 액션의 순서를 나타낸다. 타임라인을 하나씩 추가하면서 각 액션이 서로 어떻게 상호작용하고 간섭하는지 볼 수 있다.

## 타임라인 다이어그램의 기본 규칙

### 1. 두 액션이 순서대로 나타나면 같은 타임라인에 넣는다.

- 타임라인에는 액션만 그린다. 계산은 실행 시점에 영향을 받지 않기 때문에 그리지 않는다.

### 2. 두 액션이 동시에 실행되거나 순서를 예상할 수 없다면 분리된 타임라인에 넣는다.

- 액션이 서로 다른 스레드나 프로세스, 기계, 비동기 콜백에서 실행되면 서로 다른 타임라인에 표시한다.
- 예를 들어, 2개의 액션이 비동기로 실행될 때 두 액션의 실행 시점이 무작위이기 때문에 어떤 액션이 먼저 실행될 지 알 수 없다.

## 다이어그램을 그리기 위한 3단계

1. 액션을 확인한다.
2. 각 액션을 그린다.
3. 단순화한다.

## 서로 다른 언어, 서로 다른 스레드 모델

### 1. 단일 스레드, 동기

- 기본적으로 멀티스레드를 지원하지 않는 언어도 있다. (ex-PHP)
- 스레드가 하나면 타임라인도 하나라 단순하다는 점에서 장점이 있지만 네트워크를 통한 API 호출 같은 것은 다른 타임라인이 필요하다.
- 하지만 메모리를 공유하지 않기 때문에 공유 자원을 많이 없앨 수 있다.

### 2. 단일 스레드, 비동기

- 자바스크립트이 단일 스레드, 비동기 방식으로 동작한다.
- I/O 작업을 할 때 비동기 모델을 사용하고 결과를 콜백으로 받을 수 있지만 언제 끝날지 알 수 없다.

### 3. 멀티스레드

- 자바, 파이선, 루비, C, C#
- 멀티스레드는 실행 순서를 보장하지 않기 때문에 프로그래밍하기 매우 어렵다.
- 새로운 스레드가 생기면 새로운 타임라인을 그려야 한다.

### 4. 메시지 패싱 프로세스

- 엘릭서, 얼랭
- 서로 다른 프로세스를 동시에 실행할 수 있는 스레드 모델을 지원한다.
- 프로세스는 서로 메모리를 공유하지 않고 메시지로 통신한다.

## 순서대로 실행되는 코드의 종류

### 1. 순서가 섞일 수 있는 코드

- 두 액션 사이에 시간이 얼마나 걸릴지 알 수 없다. => 타임라인의 액션과 순서가 섞일 수 있다.

### 2. 순서가 섞이지 않는 코드

- 액션이 차례로 실행되기 때문에 그 사이에 다른 작업이 끼어들 수 없다.

## 좋은 타임라인의 원칙

### 1. 타임라인은 적을수록 이해하기 쉽다.

- 타임라인이 하나인 시스템이 가장 이해하기 쉽다.

### 2. 타임라인은 짧을수록 이해하기 쉽다.

- 타임라인의 단계를 줄이면 이해하기 쉽다.

### 3. 공유하는 자원이 적을수록 이해하기 쉽다.

- 타임라인이 두 개 있다면 서로 자원을 공유하는 액션을 주의 깊게 봐야 한다.

### 4. 자원을 공유한다면 서로 조율해야 한다.

- 공유 자원은 안전하게 공유해야 한다.(올바른 순서대로 자원을 쓰고 돌려준다는 의미)

### 5. 시간을 일급으로 다눈다.

## 자바스크립트의 단일 스레드

- 전역변수를 바꾸는 동기 액션은 타임라인이 서로 섞이지 않는다.
- 비동기 호출은 미래에 알 수 없는 시점에 런타임에 의해 실행된다.
- 두 동기 액션은 동시에 실행되지 않는다.
- 자바스크립트에서 어떤 동작이 동기인지 비동기인지 아는 것은 중요하다.

## 자바스크립트의 비동기 큐

- 큐는 들러온 순서대로 작업을 처리한다. 브라우저에서 동작하는 자바스크립트 엔진은 작업 큐를 가지고 있다.
- 작업 큐에 들어있는 작업은 이벤트 데이터와 이벤트를 처리할 콜백으로 구성된다.

### 작업이 큐에 들어가는 방법

1. 이벤트가 발생하면 큐에 작업이 추가된다. 여기서 이벤트란 마우스 클릭, 키보드 입력, AJAX 이벤트 값 등을 의미한다.
2. 이벤트는 예측 불가능한 시점에 작업 큐에 들어간다.

### 작업이 없을 때 엔진은 무엇을 할까?

- 이벤트 루프는 대기 상태로 들어가 전원을 아끼고, 가비지 컬렉션 같은 관리 작업을 한다.

### 이벤트 루프

- 작업 큐는 이벤트 루프에 의해 처리된다.
- 이벤트 루프는 큐에서 작업 하나를 꺼내 실행하고 완료되면 다음 작업을 꺼내 실행하는 것을 무한 반복한다.
- 이벤트 데이터를 인자로 콜백 부른다.

### AJAX와 이벤트 큐

- AJAX는 Asynchronous JavaScript And XML의 약자로, 브라우저에서 실행되는 자바스크립트에서 웹 요청을 만드는 방법이다.
- 응답은 비동기 콜백으로 처리하고, 응답은 요청 순서대로 오지 않는다는 특징이 있다.

## 타임라인 단순화하기

### 1. 하나의 타임라인에 있는 모든 액션을 하나로 통합한다.

### 2. 타임라인이 끝나는 곳에서 새로운 타임라인이 하나 생긴다면 통합한다.

```
타임라인을 쉽게 만드는 4가지 원칙
1. 적은 타임라인
2. 짧은 타임라인
3. 적은 공유자원
4. 자원을 공유한다면 조율하기
```

## 비동기 호출의 명시적 출력

- 비동기 호출에서 명시적인 출력을 위해 리턴값 대신 콜백을 사용할 수 있다!