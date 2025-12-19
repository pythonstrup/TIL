# Chandy-Lamport Algorithm

- [Distributed Snapshots: Determining Global States of Distributed Systems](https://blog.acolyer.org/2015/04/22/distributed-snapshots-determining-global-states-of-distributed-systems/)

## 분산 시스템의 전역 상태 파악 문제

- 공통 클럭 없이 상태를 정확히 포착하는 것이 핵심적인 난제
- Chandy-Lamport 알고리즘을 통해 시스템의 계산을 방해하거나 변경하지 않고도 분산 스냅샷을 얻어낼 수 있다.

## 작동 원리
