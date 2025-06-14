# Chapter01 소개

## 1. 설계와 아키텍처란?

- `설계 design`와 `아키텍처 architecture`
- 둘 사이에는 차이가 없다.
  - 저수준의 세부사항과 고수준의 구조는 모두 소프트웨어 전체 설계의 구성요소다. 
  - 실제로 이 둘을 구분짓는 경계는 뚜렷하지 않다. 고수준에서 저수준으로 향하는 의사결정의 연속성만 있을 뿐.

> 소프트웨어 아키텍처의 목표는 필요한 시스템을 만들고 유지보수하는 데 투입되는 인력을 최소화하는 데 있다.

- 설계 품질을 재는 척도 = 고객의 요구를 만족시키는 데 드는 비용을 재는 척도.
- 엉망진창이 되어 가는 신호
  - 생산성이 0으로 수렴하는 소프트웨어. 개발자의 노력은 기능 개발보다는 엉망이 된 상황에 대처하는 데 소모되기 시작.

> 진실은 다음과 같다. 엉망으로 만들면 깔끔하게 유지할 때보다 항상 더 느리다. 빨리 가는 유일한 방법은 제대로 가는 것이다.

---

## 2.  두 가지 가치에 대한 이야기

- 모든 소프트웨어 시스템은 이해관계자에게 서로 다른 두 가지 가치를 제공한다.
1. `행위 behavior`
2. `구조 structure`

### 행위

- 요구사항을 구현하고 버그를 수정하는 일.
- 이 일이 프로그래머가 해야할 전부는 아니다.

### 아키텍처

> 소프트웨어는 '부드러움을 지니도록' 만들어졌다.

- 소프트웨어는 변경하기 쉬워야 한다.
  - 이해관계자가 기능에 대한 생각을 바꾸면, 이러한 변경사항을 간단하고 쉽게 적용할 수 있어야 한다.
  - 어려움은 `범위 scope`에 비례해야 하며, 변경사항의 `형태 shape`와는 관련이 없어야 한다.

> 문제는 당연히 시스템의 아키텍처다. 아키텍처가 특정 형태를 다른 형태보다 선호하면 할수록, 새로운 기능을 이 구조에 맞추는 게 힘들어진다.
> 따라서 아키텍처는 형태에 독립적이어야 하고, 그럴수록 더 실용적이다.

### 아이젠하워 매트릭스

> 개발자가 흔히 저지르는 실수는 '긴급하지만 중요하지 않은' 일을 '긴급하고 중요한' 일로 격상시켜 버리는 일이다.
> 이러한 실패로 인해 시스템에서 중요도가 높은 아키텍처를 무시한 채 중요도가 떨어지는 기능을 선택하게 된다.
> 기능의 긴급성이 아닌 아키텍처의 중요성을 설득하는 일은 소프트웨어 개발팀이 마땅히 책임져야 한다.

### 아키텍처를 위해 투쟁하라.

- 개발자는 소프트웨어를 안전하게 보호해야 할 책임이 있으므로 이해관계에 속해있다.
- 아키텍트는 시스템이 제공하는 특성이나 기능보다는 시스템의 구조에 더 중점을 둔다.

---

# 참고자료

- 클린 아키텍처, 로버트 C. 마틴
