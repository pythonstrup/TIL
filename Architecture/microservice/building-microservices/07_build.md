# Chapter 07 빌드

- '개발자가 변경 사항을 체크인할 준비가 될 때 어떤 일이 발생할까?'

## 1. 지속적 통합에 대한 간략한 소개

- `지속적 통합 continuous integration`, 즉 CI는 수년 동안 사용돼 왔다.
  - 특히 마이크로서비스, 빌드, 버전 제어 리포지터리 간의 매핑에 대해 생각할 대 몇 가지 고려할 사항이 있으므로 기본적인 것을 검토하는 데 약간의 시간을 할애할 가치가 있다.
- CI의 핵심 목표 = 모든 사람이 서로 조화롭게 동기화되도록 유지하는 것, 새로 체크인한 코드가 기존 코드와 적절하게 통합되는지 자주 확인해 목표를 달성.
  - 코드 커밋 감지, 체크아웃, 코드 컴파일 및 테스트 통과 확인
- 정적 분석과 테스트를 사용해 코드 품질에 대한 빠른 피드백을 얻을 수 있고 바이너리 산출물 생성을 자동화할 수도 있다.

### 1-1. 실제로 CI를 하고 있는가?

- 젠킨스, CircleCI, `트래비스 Travis` 등의 도구를 사용한다고 해서 실제로 CI를 올바르게 수행하고 있다고 장담할 수는 없다.

#### 메인라인에 하루에 한 번 체크인하는가?

- 코드가 통합되는지 확인해야 한다. 다른 사람의 변경 사항과 함께 코드를 자주 확인하지 않으면 향후 통합은 더 어려워진다.
- 단기 브랜치를 사용해 변경 사항을 관리하는 경우에도 가능한 한 자주 단일 메인라인 브랜치에 통합하라.

#### 변경 사항을 검증하는 일련의 테스트가 있는가?

- 테스트가 없다면 구성적인 면에서 통합이 됐다는 사실을 알 수 있지만 시스템의 동작을 중단시켰는지는 알 수 없다.
- 코드가 예상대로 동작하는지 확인할 수 없는 CI는 CI가 아니다.

#### 빌드가 깨졌을 때 이를 수정하는 것이 팀의 최우선 일인가?

- 빌드를 다시 통과시키려면 빌드 수정과 관련되지 않은 추가 체크인을 모두 중지해야 한다.
- 더 많은 변경 사항이 쌓이면 빌드를 수정하는 데 걸리는 시간이 크게 늘어나기 때문이다.

### 1-2. 브랜치 모델

- 기능 브랜치에서 작업할 때 변경 사항을 정기적으로 통합하지 않는 것은 문제가 된다. 근본적으로 통합을 지연하고 있는 것이다.
  - 대안은 모든 사람이 동일한 소스 코드 `트렁크 trunk`에 체크인하도록 하는 것이다.
  - 모든 사람이 동일한 트렁크에서 작업하는 이 기술을 `트렁크 기반 개발 trunk-based development`이라고 한다.
  - 빈번한 통합과 유효성 확인은 트렁크 기반 개발의 큰 이점.

> #### 브랜치 주의 사항
> 일찍 통합하고 자주 통합하라. 기능 개발을 위해 장기 브랜치를 사용하지 말고 트렁크 기반 개발을 고려하라. 브랜치를 꼭 사용해야 한다면 짧게 유지하라.

- 트렁크 기반 개발에 관한 연구

> 우리는 머지되기 전에 매우 짧은 수명의 브랜치나 포크와 총 3개 미만의 활성화된 브랜치가 지속적 전달에 중요한 측면을 차지하고 이 모든 것이 성능을 높이는 데 기여한다는 사실을 알아냈다. 매일 트렁크나 마스터에 코드를 머지하는 것도 같은 이유다.

- 오픈 소스 개발과 '오래 지속되는' 브랜치의 영향

> 우리의 연구 결과는 다음과 같은 영역의 오픈 소스 개발로 확장된다.
> - 코드 커밋을 더 빨리 할수록 좋다: 오픈 소스 프로젝트에서 많은 개발자가 리베이스를 막으려고 패치를 더 빨리 머지하면 개발자는 더 빨리 작업할 수 있다.
> - 작은 배치로 작업하는 것이 좋다: 큰 '패치 폭탄'은 유지 관리자가 변경 사항을 검토하는 데 더 많은 시간이 들어가므로 작고 읽기 쉬운 패치보다 프로젝트에 머지하기가 더 어렵고 더 느리다.
> 사유 소스 코드 기반으로 작업하든, 오픈 소스 프로젝트에서 작업하는 간에 수명이 짧은 브랜치, 작고 읽기 쉬운 패치, 변경 사항에 대한 자동화 테스트를 통해 모든 사람의 생산성을 높일 수 있다.

<br/>

## 2. 빌드 파이프라인과 지속적 제공

#### 빌드 파이프라인

- 테스트를 실행할 때 아마도 빠르고 작은 범위의 테스트는 많고, 느리고 큰 범위의 테스트는 적을지 모른다.
- 모든 테스트를 함께 실행한 후 넓은 범위의 느린 테스트가 완료되길 기다려야 한다면, 빠른 테스트가 실패할 때 빠른 피드백을 받지 못할 수 있다.
- 이 문제를 해결하려면, 빌드 과정에 `빌드 파이프라인 build pipeline`이라고 알려진 것을 생성해 다른 단계를 두도록한다.
- 각 단계를 마칠 때 소프트웨어의 진행 상황을 추적하는 좋은 방법을 제공해 소프트웨어 품질에 대한 통찰력을 제공.

#### 지속적 제공 continuous delivery

- CD는 이러한 개념에 기반해 구축되고 이후 더 많은 개념을 도입했다.

> #### 지속적 제공(continuous delivery) vs 지속적 배포(continuous deployment)
> - 지속적 제공: 각 체크인을 릴리스 후보로 취급하고 각 후보의 품질을 평가해 배포할 준비가 됐는지 결정하는 개념
> - 지속적 배포: 자동화된 메커니즘을 사용해 모든 체크인을 검증해야 하며, 이러한 검사를 통과하는 모든 소프트웨어는 사람의 개입 없이 자동으로 배포 (지속적 제공의 확장된 버전)
> - 지속적 배포가 모든 사람에게 적합하지는 않다.
> - 지속적 제공을 선택한다는 것은 운영 환경으로 가는 길을 최적화하는 데 계속 집중한다는 의미며, 향상된 가시성을 통해 어디에 최적화를 수행해야 하는지 더 쉽게 확인할 수 있다.

### 2-1. 도구

- CD를 완벽하게 지원하는 도구를 사용하면 이러한 파이프라인을 정의하고 시각화해 여러분의 소프트웨어가 운영 단계에 이르면 전체 경로를 모델링할 수 있다.
- 코드 버전이 파이프라인을 통해 이동할 때 이와 같은 자동 검증 단계 중 하나를 통과하면 다음 단계로 이동한다.

### 2-2. 절충점과 환경

- 파이프라인을 통해 마이크로서비스 산출물을 이동시키면 마이크로서비스는 다른 환경에 배포되고, 서로 다른 환경은 다른 목적을 제공하며 다른 특성을 가질 수 있다.
- 파이프라인을 구축해 필요한 환경을 정하는 것은 그 자체로 균형을 이루는 작업이다. 파이프라인 초기에는 소프트웨어의 운영 단계 준비 상태에 대한 빠른 피드백을 구한다.
  - 문제가 있다면 가능한 한 빨리 개발자에게 알리길 원하고, 문제 발생에 대한 피드백을 빨리 받을수록 문제를 더 신속하게 해결한다.
  - 운영 단계에 근접할수록 소프트웨어가 동작하는 것에 더 큰 확신을 갖길 원하므로 점차 운영 환경과 유사한 환경에 배포하게 된다.

### 2-3. 산출물 생성

- 두 가지 중요한 규칙이 있다.
1. 산출물은 한 번만 빌드해야 한다. (같은 것 반복 빌드 = 시간 낭비.)
2. 검증할 산출물이 배포할 산출물이어야 한다.

- 배포 가능한 아티팩트를 한 번만 빌드하고, 이상적으로 빌드는 파이프라인 초기에 수행하라.

<br/>

## 3. 소스 코드와 빌드를 마이크로서비스에 매핑하기

### 3-1. 거대한 레포지터리 하나와 거대한 빌드

- 모든 것을 하나로 묶어 시작
- 모든 코드를 저장하는 1개의 거대한 코드 레포지터리와 1개의 빌드.

<img src="img/build01.jpg">

- 이 모델은 한 번에 여러 서비스를 배포하는 것을 꺼리지 않는 락스텝 배포의 아이디어를 수용한다면 완벽하게 작동할 수 있다.
  - 일반적으로 절대 피해야 할 패턴.
  - 프로젝트 초기, 단일 팀이 모든 작업을 수행하는 경우 이 모델은 짧은 기간에 적합할 수 있다.
- 몇 가지 중요한 단점
  - 한 서비스에서 코드 한 줄을 변경하려 하면 다른 모든 서비스까지 검증되고 빌드된다. => 필요 이상으로 많은 시간. (작은 변경을 운영 환경에 적용하기 위해 빌드한 서비스를 모두 배포해야 할까?)
  - 코드 한 줄을 변경함으로써 빌드가 중단되는 경우 이 문제가 수정될 때까지 다른 서비스를 변경하지 못한다. (거대한 빌드를 공유하는 다수의 팀. 누가 책임자인가?)
- 최악의 모노레포 형태

### 3-2. 마이크로서비스당 하나의 레포지터리(멀티레포)

- 여러 개의 레포지터리와 레포지터리 하나당 하나의 CI 서버.
  - 마이크로서비스에 대한 강력한 소유권 모델을 고려하려는 경우 적합.
- 이 패턴의 단순한 특성으로 인해 몇 가지 문제가 발생한다.
  - 어떤 경우에는 한 번에 여러 레포지터리를 사용해 작업할 때가 있을 것.

#### 레포지토리 간 코드 재사용

- 이 패턴을 사용하면 마이크로서비스가 다른 레포지터리에서 관리되는 다른 코드에 의존하는 것을 막지 못한다.
  - 그런 상황을 방지할 간단한 메커니즘은 재사용하려는 코드를 라이브러리로 패키징한 다음 이 라이브러리를 다운스트림 마이크로서비스의 명시적 의존성이 되도록 만드는 것이다.
- 새로운 버전의 라이브러리를 롤아웃하려는 경우 라이브러리를 의존하고 있는 다른 서비스도 함께 배포해야 한다는 점이 중요하다.

#### 여러 레포지토리에서 작업

- 라이브러리를 통해 코드를 재사용하는 것 외에 2개 이상의 레포지터리에서 변경할 수 있는 다른 방법은 없을까?
- 마이크로서비스 경계 안에서 변경 사항을 적용할 가능성이 더 높아지도록 아키텍처와 마이크로서비스 경계를 최적화.
  - 여러 레포지터리에 걸쳐 작업하는 고통이 마이크로서비스의 경계를 강화하는 데 도움이 된다.
  - 경계의 위치와 경계 간 상호작용의 특성을 신중하게 생각해볼 수 있기 때문.

>  여러 마이크로 서비스를 넘나들며 지속적으로 변경하고 있다면 마이크로서비스 경계가 잘못된 위치에 있을 가능성이 높다. 이러한 일이 발생하면 마이크로서비스를 다시 합치는 것을 고려해볼 만하다.

### 3-3. 모노 레포

- 여러 마이크로서비스에 대한 코드가 동일한 소스 코드 레포지터리에 저장된다.
  - 여러 프로젝트에서 소스 코드를 `원자적 방식 atomic fashion`으로 변경할 수 있고 한 프로젝트에서 다음 프로젝트로 코드를 보다 세밀하게 재사용할 수 있다.
  - 모노 레포를 사용하는 회사의 예시 - 구글.

#### 빌드 매핑

- 마이크로서비스 별로 하나의 소스 코드 레포지터리를 사용하면 소스 코드에서 빌드 프로세스로 매핑하는 일은 간단하다.
  - 레포지터리 변경 -> 해당 CI 빌드 시작
- 모노레포를 사용하면 족잡하다.
  - 간단한 출발점은 모노레포 내 폴더를 빌드에 매핑하는 것.
  - 관련된 폴더 구조가 많을수록 더 복잡해진다.
  - 단순하게 모든 마이크로서비스에서 사용하는 공통 폴더를 둘 수 있으며, 이 폴더를 변경하면 전체 마이크로서비스가 다시 빌드된다.
  - 더 복잡해지면 오픈 소스 도구인 `베이즐 Bazel`과 같은 그래프 기반 빌드 도구를 채택해야 할 수도 있다.

#### 소유권 정의

- 강력한 소유권에서 약한 소유권, 공동 소유권. 소유권에 대한 차등적 규모
  - https://martinfowler.com/bliki/CodeOwnership.html
- 강력한 소유권에서 코드는 특정 그룹의 사람들이 소유한다.
  - 그룹 외부의 누군가가 변경을 원하면 소유자에게 변경을 요청해야 한다.
- 약한 소유권에도 지정된 소유자 개념이 있으며, 소유권 그룹 외부의 사람들이 변경을 할 수는 있지만 변경 사항을 소유권 그룹 사람이 검토하고 허용해야 한다.
  - pull request 요청이 머지되기 전까지 검토를 위해 핵심 소유권 팀으로 전송되는 것까지 포함.
- 공동 소유권을 사용하면 모든 개발자가 모든 코드를 변경할 수 있다.

#### 도구

- 구글의 모노레포는 방대하며, 대규모로 운영하려면 상당한 양의 엔지니어링이 필요하다.
  - 여러 세대를 거친 그래프 기반 빌드 시스템
  - 빌드 시간을 단축하기 위한 분산 객체 링커
  - IDE용 플러그인
  - 의존성 파일을 동적으로 확인하는 텍스트 편집기 등
- 결국 구글은 `파이퍼 Piper`라는 자사 소유의 소스 제어 도구를 만들어야 했다.
- 구글이 모노레포를 사용하는 근거 ('구글은 왜 수십억 줄의 코드를 단일 저장소에 저장하는가?')
  - https://cacm.acm.org/research/why-google-stores-billions-of-lines-of-code-in-a-single-repository/

#### 모노의 범위는?

- 구글은 모든 코드를 모노레포에 저장하지는 않는다.
  - 2016년 기준. 95%가 모노레포에 저장돼 있다고 함.

#### 적용 대상

- 모노레포가 잘 작동하는 곳은 소수의 개발자와 팀이 있는 곳이다.
- 10~20명의 개발자가 있는 경우, 소유권 경계를 관리하고 모노레포 방식으로 빌드 프로세스를 단순하게 유지하기가 더 쉽다.
- 그러나 중간 규모의 조직에서는 어려움을 겪는 것으로 보인다.
  - 새로운 도구나 작업 방식이 필요한 문제에 직면하기 시작하지만 이러한 아이디어에 투자할 여력이 안 되기 때문이다.

### 3-4. 어떤 방식을 사용해야 할까?

- 규모가 커질수록 마이크로서비스당 하나의 레포지터리(멀티레포) 방식이 편하다.
- 필자가 반복해서 본 문제. 소규모로 시작해 공동 소유권(따라서 모노레포)이 초기에 잘 작동했던 조직이 나중에 모노레포의 개념이 너무 깊이 박혀 있어 다른 모델로 전환하는 데 어려움을 겪는다는 것이다.
  - `제공 조직 delivery organization`이 커짐에 따라 모노레포의 고충은 증가하지만 대체 방식으로 이전하는 비용도 증가한다.
  - 이는 급속도롤 성장한 조직일 경우 훨씬 더 어려운 일이다.

<br/>

# 참고 자료

- 마이크로서비스 아키텍처 구축, 샘 뉴먼 지음