# Chapter11. 의식적으로 지름길 사용하기

- 갚을 길이 없는 기술 부채를 쌓아가면서 항상 지름길의 유혹을 느낀다.
- 지름길을 방지하려면 지름길 자체를 파악해야 한다.

## 왜 지름길은 깨진 창문 같을까?

> 어떤 것이 멈춘 것처럼 보이고, 망가져 보이고, 쓰레기 같아 보이고, 혹은 관리되지 않는다고 여겨지면 인간의 뇌는 이를 더 멈추고, 망가뜨리고, 쓰레기로 만들어도 된다고 생각하게 된다. 

- 품질이 떨어진 코드 => 더 낮은 품질의 코드를 추가하기가 쉽다.
- 코딩 규칙을 많이 어긴 코드 => 또 다른 규칙을 어기기도 쉽다.
- 지름길을 많이 사용한 코드 => 또 다른 지름길을 추가하기도 쉽다.

## 깨끗한 상태로 시작할 책임

- 우리는 모두 깨진 창문 심리에 무의식적으로 영향을 받는다.
- 따라서 가능한 한 지름길을 거의 쓰지 않고 기술 부채를 지지 않은 채로 프로젝트를 깨끗하게 시작하는 것이 중요하다.
- 그러나 때로는 지름길을 취하는 것이 더 실용적일 때도 있다.
  - 이러한 의도적인 지름길에 대해서는 세심하게 잘 기록해줘야 한다.
  - 마이클 나이가드가 제안한 아키텍처 결정 기록(Architecture Decision Records) 참고.
  - 지름길 => 미래에 빚을 지는 것

## 지름길1. 유스케이스 간 모델 공유하기

- 유스케이스 간 다른 입출력 모델을 가져야 한다. 그런데 입력 모델을 공유한다면?
- 공유의 영향으로 두 유스케이스는 결합된다. 공유하고 있는 `Command` 객체가 변경되면 두 유스케이스 모두 영향을 받는다.
- 유스케이스 간 입출력 모델을 공유하는 것은 유스케이스들이 기능적으로 묶여 있을 때 유효하다. 즉, 특정 요구사항을 공유할 때 괜찮다는 의미다.
  - `특정 세부사항을 변경할 경우 실제로 두 유스케이스 모두에 영향을 주고 싶은 것이다.`

## 지름길2. 도메인 엔티티를 입출력 모델로 사용하기

- 인커밍 포트에서 도메인 모델을 입출력 모델로 사용.
- 엔티티를 변경할 수 있게 된다.

## 지름길3. 인커밍 포트 건너뛰기

- 인커밍 어댑터(ex. controller)가 애플리케이션 서비스에 직접 접근.
- 보통 추상화 계층을 줄이는 것은 괜찮게 느껴진다.
- 하지만 인커밍 포트는 애플리케이션 중심에 접근하는 진입점을 정의한다.
  - 이를 제거하면 특정 유스케이스를 구현하기 위해 어떤 서비스 메소드를 호출해야 할지 알아내기 위해 애플리케이션 내부 동작에 대해 더 잘 알아야 한다.
- 인커밍 포트를 유지해야 하는 또 다른 이유는 아키텍처를 쉽게 강제할 수 있기 때문이다.
  - 인커밍 어댑터에서 호출할 의도가 없던 서비스 메소드를 실수로 호출하는 일이 절대 발생할 수 없다.
- 물론, 프로젝트 규모가 작다면 인커밍 포드가 없는 것이 편하다.
  - 하지만 규모가 계속 작게 유지된다고 확신할 수 있는가?

## 지름길4. 애플리케이션 서비스 건너뛰기

- 웹 계층에서 영속성 계층 바로 호출하기.
  - 이 방법은 인커밍 어댑터와 아웃고잉 어댑터 사이에 모델을 공유해야 한다.
- 애플리케이션 서비스가 없으면 도메인 로직을 둘 곳이 없다.
  - 이렇게 되면 도메인 로직이 웹 계층과 영속성 계층에 이리저리 흩어져서 도메인 로직을 찾거나 유지보수하는 것이 어려워진다.

<br/>

# 참고자료

- 만들면서 배우는 클린 아키텍처, 톰 홈버그 지음

