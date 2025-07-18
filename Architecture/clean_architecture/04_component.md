# 4부 컴포넌트 원칙

## 12장 컴포넌트

- 컴포넌트는 배포 단위다.
  - 시스템 구성 요소로 배포할 수 있는 가장 작은 단위다.
  - 자바의 경우 jar 파일이 컴포넌트다.
- 여러 컴포넌트를 서로 링크하여 실행 가능한 단일 파일로 생성할 수 있다.
- 컴포넌트가 마지막에 어떤 형태로 배포되든, 잘 설계된 컴포넌트라면 반드시 독립적으로 배포 가능한, 따라서 독립적으로 개발 가능한 능력을 갖춰야 한다.

### 컴포넌트의 간략한 역사

- 개발 초창기, 메모리에서의 프로그램 위치와 레이아웃을 프로그래머가 직접 제어 (요즘에는 하지 않는 고민이다.)
- 라이브러리 함수에 접근하는 방법? 라이브러리 함수의 소스 코드를 애플리케이션 코드에 직접 포함시켜 단일 프로그램으로 컴파일 -> 라이브러리는 바이너리가 아니라 소스 코드 형태로 유지
  - 장치는 느리고 메모리는 너무 비싸서 자원이 한정적이었기에, 이러한 접근법은 문제가 있었다.
  - 메모리가 너무 작아, 컴파일러는 느린 장치를 이용해서 소스 코드를 여러 차례 읽어야만 했다. 대규모 프로그램은 몇 시간도 걸렸다.
- 컴파일 시간을 단축하기 위해 프로그래머는 함수 라이브러리의 소스 코드를 애플리케이션 코드로부터 분리했다.
  - 라이브러리를 개별적으로 컴파일이하고, 컴파일된 바이너리를 메모리의 특정 위치에 로드했다.
  - 하지만 이는 메모리 단편화 문제를 발생시켰고, 무언가 조치가 필요한 상황이 되었다.

### 재배치성

- 해결책은 `재배치가 가능한 바이너리 relocatable binary`였다.
  - 지능적인 로더를 이용해서 메모리에 재배치할 수 있는 형태의 바이너리를 생성하도록 컴파일러를 수정하자는 것이었다.
  - 이때 로더는 재배치 코드가 자리할 위치 정보를 전달받았다. 그리고 재배치 코드에는 로드한 데이터에서 어느 부분을 수정해야 정해진 주소에 로드할 수 있는지를 알려주는 플래그가 삽입되었다.
  - 대개 플래그는 바이너리에서 참조하는 메모리의 시작 주소였다. 
- 또한 컴파일러는 재배치 가능한 바이너리 안의 함수 이름을 메타데이터 형태로 생성하도록 수정되었다.
  - 라이브러리 함수 호출 -> `외부 참조 external reference`로 생성
  - 라이브러리 함수를 정의하는 프로그램 -> `외부 정의 external definition`로 생성
- 이렇게 함으로써 외부 정의를 로드할 위치가 정해지기만 하면 로더가 외부 참조를 외부 정의에 링크시킬 수 있게 된다.
- 이렇게 `linking loader`가 탄생.

### 링커

- 링킹 로더의 등장으로 프로그래머는 프로그램을 개별적으로 컴파일하고 로드할 수 있는 단위로 분할할 수 있게 되었다.
  - 하지만 프로그램이 커지면 링킹 로더가 너무 느려지는 문제가 발생.
- 로드와 링크가 두 단계로 분리되었다.
  - 프로그래머가 느린 부분, 즉 링크 과정을 맡았다. `링커 linker`라는 별도의 애플리케이션으로 이 작업 처리하도록 만들었다.
  - 링커는 링크가 완료된 재배치 코드를 만들어 줬고, 그 덕분에 로더의 로딩 과정이 아주 빨라졌다.
  - 비록 느린 링커를 사용해서 실행 파일을 만들었지만, 한번 만들어둔 실행 파일은 언제라도 빠르게 로드할 수 있게 되었다.
- 하지만 80년대 C가 등장하면서 프로그램 코드가 더 늘어났고, 링커에서 많은 시간이 소요되기 시작했다.
- 로드 시간은 여전히 빨랐지만, 컴파일-링크 시간이 병목 구간이었다.

> 컴파일하고 링크하는 데 사용 가능한 시간을 모두 소모할 때까지 프로그램은 커진다.

- 하지만 디스크가 작아지기 시작했고, 놀랄 만큼 빨라졌다. 메모리는 말도 안 될 정도로 저렴해져서 디스크에 저장된 많은 데이터를 모두 `RAM`에 캐싱할 수 있을 정도였다.
  - 컴퓨터 `클록 속도 clock rate` 또한 1MHz에서 100MHz까지 증가했다. (최근에 나온 애플의 M4칩의 clock rate는 4.4GHz나 된다.)
- 링크 시간은 초 단위 수준이 될 정도로 감소했고, 이렇게 액티브 X와 공유 라이브러리 시대가 열렸다. (이때 jar 파일 등장.)

---

## 13장 컴포넌트 응집도

- 어떤 클래스를 컴포넌트에 포함시켜야 할까? 아래는 응집도와 관련된 세 가지 원칙이다.

1. REP: 재사용/릴리스 등가 원칙 Reuse/Release Equivalence Principle
2. CCP: 공통 폐쇄 원칙 Common Closure Principle
3. CRP: 공통 재사용 원칙 Common Reuse Principle

### 1. REP: 재사용/릴리스 등가 원칙

> 재사용 단위는 릴리스 단위와 같다.

- 재사용 가능한 컴포넌트나 컴포넌트 라이브러리가 엄청나게 많이 만들어졌으며, 우리는 소프트웨어 재사용의 시대에 살고 있다.
- 릴리스 번호가 없다면 재사용 컴포넌트들이 서로 호환되는지 보증할 방법이 전혀 없다.
  - 하지만 단순히 이 이유 때문만은 아니다. 이보다는 새로운 버전이 언제 출시되고 무엇이 변했는지를 소프트웨어 개발자들이 알아야 하기 때문이다.
- 이 원칙을 소프트웨어 설계와 아키텍처 관점에서 보면 단일 컴포넌트는 응집성 높은 클래스와 모듈들로 구성되어야 함을 뜻한다.
  - 모든 모듈은 서로 공유하는 중요한 테마나 목적이 있어야 한다.

### 2. CCP: 공통 폐쇄 원칙

> 동일한 이유로 동일한 시점에 변경되는 클래스를 같은 컴포넌트로 묶어라. 서로 다른 시점에 다른 이유로 변경되는 클래스는 다른 컴포넌트로 분리하라.

- 이 원칙은 단일 책임 원칙(SRP)를 컴포넌트 관점에서 다시 쓴 것이다.
  - 단일 컴포넌트는 변경의 이유가 여러 개 있어서는 안 된다고 말한다.
- 대다수의 애플리케이션에서 `유지보수성 maintainability`은 재사용성보다 훨씬 중요하다.
  - 애플리케이션에서 코드가 반드시 변경되어야 한다면, 이러한 변경이 여러 컴포넌트 도처에 분산되어 발생하기보다는, 차라리 변경 모두가 단일 컴포넌트에서 발생하는 편이 낫다.
  - 만약 변경을 단일 컴포넌트로 제한할 수 있다면, 해당 컴포넌트만 재배포하면 된다.
- 이 원칙은 개방 폐쇄 원칙과도 밀접하게 관련되어 있다.
  - 100% 완전한 폐쇄란 불가능하므로 전략적으로 폐쇄해야 한다.
  - 우리는 발생할 가능성이 있거나 과거에 발생했던 대다수의 공통적인 변경에 대해서 클래스가 닫혀 있도록 설계한다.
- CCP에서는 동일한 유형의 변경에 대해 닫혀 있는 클래스들을 하나의 컴포넌트로 묶음으로써 OCP에서 얻은 교훈을 확대 적용한다.
  - 따라서 변경이 필요한 요구사항이 발생했을 때, 그 변경이 영향을 주는 컴포넌트들이 최소한으로 한정될 가능성이 높아진다.

### 3. CRP: 공통 재사용 원칙

> 컴포넌트 사용자들을 필요하지 않은 것에 의존하게 강요하지 말라.

- 클래스와 모듈을 어느 컴포넌트에 위치시킬지 결정할 때 도움되는 원칙이다.
  - CRP에서는 같이 재사용되는 경향이 있는 클래스와 모듈들을 같은 컴포넌트에 포함해야 한다고 말한다.
  - 이러한 컴포넌트 내부에서는 클래스들 사이에 수많은 의존성이 있으리라고 예상할 수 있다.
- CRP는 또한 동일한 컴포넌트로 묶어서는 안 되는 클래스가 무엇인지도 말해준다.
  - 어떤 컴포넌트가 다른 컴포넌트를 사용하면, 두 컴포넌트 사이에는 의존성이 생겨난다. 어쩌면 사용하는 컴포넌트가 사용되는 컴포넌트에서 단 하나의 클래스만 사용할 수도 있다.
  - 그렇다고 해서 의존성은 조금도 약해지지 않는다. 사용하는 컴포넌트는 사용되는 컴포넌트에 여전히 의존한다.
- 의존하는 컴포넌트가 있다면 해당 컴포넌트의 모든 클래스에 대해 의존함을 확실히 인지해야 한다.
  - CRP는 강하게 결합되지 않은 클래스들을 동일한 컴포넌트에 위치시켜서는 안 된다고 말한다.

#### ISP와의 관계

- CRP는 ISP의 포괄적인 버전.

> 필요하지 않은 것에 의존하지 말라.

### 컴포넌트 응집도에 대한 균형 다이어그램

- REP와 CCP는 `포함 inclusive` 원칙이다. 두 원칙은 컴포넌트를 더욱 크게 만든다. CRP는 `배제 exclusive` 원칙이며, 컴포넌트를 더욱 작게 만든다.
  - 뛰어난 아키텍트라면 이 원칙들이 균형을 이루는 방법을 찾아야 한다.
- 오로지 REP와 CRP에만 중점을 두면, 사소한 변경이 생겼을 때 너무 많은 컴포넌트에 영향을 미친다.
- 반대로 CCP와 REP에만 과도하게 집중하면 불필요한 릴리스가 너무 빈번해진다.
- 프로젝트의 컴포넌트 구조는 시간과 성숙도에 따라 변한다.
  - 프로젝트 초기: CCP가 REP보다 훨씬 중요 (`개발가능성 developability` > 재사용성)

---

## 14장 컴포넌트 결합

### ADP: 의존성 비순환 원칙

> 컴포넌트 의존성 그래프에 `순환 cycle`이 있어서는 안 된다.

- 많은 개발자가 동일한 소스 파일을 수정하는 환경에서는 '숙취 증후군'이 발생할 수 있다. (숙취 증후군 -> 다른 개발자의 영향으로 분명 잘 돌아갔던 결과물이 다음 날 작동하지 않는 일)
  - 개발팀 규모가 크면 클수록 지독한 악몽이 될 수 있다. 개발팀이 프로젝트의 안정 버전을 빌드하지 못한 채 몇 주가 그냥 흘러가버리는 일도 드물지 않다.
- 해결책
1. 주 단위 빌드 weekly build
2. 의존성 비순환 원칙 Acyclic Dependencies Principle (ADP)

#### Weekly Build

- 중간 규모 프로젝트에서 흔하게 사용
  - 5일 중 4일은 고립. 하루는 통합 작업.
- 하지만 프로젝트가 커지면 하루만에 통합하는 것이 불가능해질 수 있다. -> 효율성 나빠짐.

#### 순환 의존성 제거 ADP

- 개발 환경을 릴리스 가능한 컴포넌트 단위로 분리한다!
- 릴리스가 즉각적으로 다른 팀에게 영향을 주지 않기 때문에 자신의 컴포넌트를 해당 컴포넌트에 맞게 수정할 시기를 스스로 결정할 수 있다.(하위 호환성 준수?)
  - 통합은 작고 점진적으로 이뤄진다.
  - 이 절차가 성공적으로 동작하려면 컴포넌트 사이의 의존성 구조를 반드시 관리해야 한다. 순환이 있어서는 안 된다.
- 구성요소 간의 의존성을 파악하고 있으면 시스템을 빌드하는 방법을 알 수 있다.

#### 순환이 컴포넌트 의존성 그래프에 미치는 영향

- 의존성 하나가 여러 요소를 엮어 하나의 거대한 컴포넌트로 만들어 버린다. (결합)
  - 그 요소 중 어떤 것을 개발하더라도 '숙취 증후군'을 겪게 된다.
- 모듈의 개수가 많아짐에 따라 빌드 관련 이슈는 기하급수적으로 증가하게 될 것이다.

#### 순환 끊기

- 컴포넌트 사이의 순환을 끊고 의존성을 다시 DAG로 원상복구하는 일. 아래의 주요 메커니즘 두 가지 확인
1. 의존성 역전 원칙(DIP)를 사용한다.
2. 새로운 컴포넌트를 만든다. 그리고 의존하는 두 컴포넌트가 모두 의존하는 클래스들을 새로운 컴포넌트로 이동시킨다.

#### 흐트러짐 jitters

- 애플리케이션은 성장함에 따라 의존성 구조는 서서히 흐트러지며 또 성장한다.
- 따라서 의존성 구조에 순환이 발생하는지를 항상 관찰해야 한다. 순환이 발생하면 어떤 식으로든 끊어야 한다.

### 하향식 설계 Top-Down

- 결론. 컴포넌트 구조는 하향식으로 설계될 수 없다.
- 컴포넌트는 시스템에서 가장 먼저 설계할 수 있는 대상이 아니며, 오히려 시스템이 성장하고 변경될 때 함께 진화한다.
- 컴포넌트 의존성 다이어그램의 역할 = `빌드 가능성 buildability`와 `유지보수성 maintainability`를 보여주는 지도
  - 애플리케이션의 기능을 기술하는 일과는 관련이 없다. -> 따라서 컴포넌트 구조는 프로젝트 초기에 설계할 수 없다.
  - 나중에 프로젝트가 커지면 의존성 관리에 관심을 가지게 되고, 자연스럽게 SRP나 CCP에 관심을 갖게 된다.
- 의존성 구조와 관련된 최우선 관심사 = 변동성 격리
- 애플리케이션 성장 -> 재사용성, 공통 재사용 원칙. -> 순환 발생, ADP 적용.
  - 이런 과정 없이 의존성 구조부터 설계하려고 하면 상당히 큰 실패를 맛볼 수 있다.  
  - 컴포넌트 의존성 구조는 시스템 논리적 설계에 발맞춰 성장하며 또 진화해야 한다.

### SDP: 안정된 의존성 원칙

> 안정성의 방향으로 (더 안정된 쪽에) 의존하라.

- 변경이 쉽지 않은 컴포넌트가 변동이 예상되는 컴포넌트에 의존하게 만들어서는 안 된다.

#### 안정성

- `안정성 stability`란 무엇인가? -> 변환가 발생하는 빈도와 관련이 없는 특성. '쉽게 움직이지 않는'
- 의존성 = 변경을 어렵게 만드는 주범

#### 안정성 지표

- `Fan-in`: 안으로 들어오는 의존성. 컴포넌트 내부의 클래스에 의존하는 컴포넌트 외부의 클래스 개수.
- `Fan-out`: 밖으로 나가는 의존성. 외부의 클래스에 의존하는 컴포넌트 내부의 클래스 개수.
- `I(불안정성)`: `I` = `Fan-out` ÷ `Fan-in`. `I = 0`이면 최고로 안정된 컴포넌트.
- 자바의 경우 `import` 구문의 개수를 세어서 계산할 수 있다.

#### 모든 컴포넌트가 안정적이어야 하는 것은 아니다

- 모든 컴포넌트가 최고로 안정적인 시스템이라면 변경이 불가능하다. -> 바람직 X

### SAP: 안정된 추상화 원칙

> 컴포넌트는 안정된 정도만큼만 추상화되어야 한다.

#### 고수준 정책을 어디에 위치시켜야 하는가?

- 시스템에 자주 변경해서는 절대로 안 되는 소프트웨어도 있다.
  - 고수준 아키텍처나 정책 결정과 관련된 소프트웨어가 그 예다. (I=0)
- 하지만 안정된 컴포넌트에 위치시키면, 그 정책을 포함하는 소스 코드는 수정하기가 어려워 진다. 이로 인해 시스템 전체 아키텍처가 유연성을 잃는다.
  - 컴포넌트가 최고로 안정된 상태이면서도 동시에 변경에 충분히 대응할 수 있을 정도로 유연하게 만드는 방법 -> `OCP`

#### 안정된 추상화 원칙

- `안정된 추상화 원칙 Stable Abstractions Principle, SAP`은 안정성과 추상화 정도 사이의 관계를 정의한다.
  - 한편으로 안정된 컴포넌트는 추상 컴포넌트여야 하며, 이를 통해 안정성이 컴포넌트를 확장하는 일을 방해해서는 안 된다고 말한다.
  - 다른 한편으로 불안정한 컴포넌트는 구체 컴포넌트여야 한다고 한다. -> 쉽게 변경할 수 있게 하기 위해서.

#### 추상화 정도 측정하기

- `Nc`: 컴포넌트의 클래스 개수
- `Na`: 컴포넌트의 추상 클래스와 인터페이스 개수
- `A`: 추상화 정도 `A = Na ÷ Nc`
  - 0~1 사이의 값을 가짐. 0이면 추상 클래스가 없음. 1이면 추상 클래스만 존재.

#### 주계열

- 안정성(`I`)과 추상화 정도(`A`)의 관계
  - 컴포넌트마다 그 정도가 다양하다.
- 배제할 구역을 찾는 방법식으로 추론 가능

#### 고통의 구역 Zone of Pain

- (0, 0) 좌표 근처
- 매우 안정적이며 구체적인 상태.
- 추상적이지 않아 확장할 수 없고, 안정적이라 변경하기도 상당히 어렵다.
- 일부 소프트웨어 엔티티는 고통의 구역에 위치하곤 한다. (ex-데이터베이스 스키마, 유틸리티 라이브러리)

#### 쓸모없는 구역 Zone of Uselessness

- (1, 1) 좌표 근처
- 추상적이지만, 누구도 그 컴포넌트에 의존하지 않는 상태.

#### 배제 구역 벗어나기 & 주계열과의 거리

- 개인적인 생각
  - 솔직히 이걸 계산할 일이 있을까? -> 시간도 좀 들여야 하고, 시간을 쓴다고 해도 그 판단이 정확하지 않을 가능성이 농후. 
  - 차라리 내가 가진 직관으로 파악하는 게 빠르지 않을까?

---

# 참고자료

- 클린 아키텍처, 로버트 C. 마틴
