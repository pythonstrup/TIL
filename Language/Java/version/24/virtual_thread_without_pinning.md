# JEP 491: Synchronize Virtual Threads without Pinning

- [링크](https://openjdk.org/jeps/491)

## 요약

- 이 제안(JEP)의 핵심은 기존 `synchronized` 키워드를 가상 스레드 환경에서 아무 문제 없이 잘 작동하도록 개선하는 것이다. 
- 이를 통해 개발자들이 가상 스레드의 이점을 누리기 위해 코드를 대대적으로 수정할 필요가 없게 만든다.

## 문제점: `synchronized`와 가상 스레드의 충돌

- 가상 스레드(Virtual Thread)는 작업을 잠시 멈출 때(예: I/O 대기) 자신을 실행하던 플랫폼 스레드(Platform Thread)를 반납하여 다른 가상 스레드가 일할 수 있게 한다.
- 하지만 기존에는 가상 스레드가 `synchronized` 블록 안에서 멈추면, 플랫폼 스레드를 반납하지 못하고 `고정 Pinned` 되는 문제가 있었다.
- 이 '고정' 현상 때문에 플랫폼 스레드가 낭비되었고, 이는 가상 스레드의 가장 큰 장점인 확장성을 심각하게 저해하는 원인이었다.
- **원인**: JVM이 `synchronized`의 잠금(monitor) 소유자를 가상 스레드가 아닌 플랫폼 스레드로 기록했기 때문이었다.

## 해결책: JVM 내부 구현 변경

- JEP 491은 JVM이 잠금 소유자를 플랫폼 스레드가 아닌 가상 스레드 자체로 직접 추적하도록 변경한다.
- 이 변화 덕분에 가상 스레드는 이제 `synchronized` 블록 안에서 블로킹 되더라도 자유롭게 플랫폼 스레드에서 `언마운트 unmount` 될 수 있다.
- 결과적으로, `synchronized`를 사용해도 더 이상 `고정 pinning` 현상이 발생하지 않아 플랫폼 스레드 자원이 효율적으로 활용된다.

## 주요 변경점 및 영향

- 코드 수정 불필요: 기존에 `synchronized`를 사용하던 방대한 양의 Java 코드를 `java.util.concurrent.locks` (예: `ReentrantLock`)로 바꿀 필요가 없어졌다.
- 개발 편의성 증대: 이제 개발자는 고정 문제를 걱정하지 않고, 상황에 따라 더 간편한 `synchronized`나 더 유연한 `ReentrantLock`을 자유롭게 선택할 수 있다.
- 권장 사항: 특별한 기능(시간 제한, 인터럽트 가능 등)이 필요 없다면, 더 간단하고 실수가 적은 `synchronized`를 사용하는 것이 좋다.
- 진단 기능 개선: `jdk.VirtualThreadPinned` JFR 이벤트는 이제 `synchronized`가 아닌 다른 원인(예: 네이티브 코드 호출)으로 고정이 발생할 때만 기록되도록 용도가 변경되고 기능이 향상된다.
- 관련 시스템 속성 제거: 고정 문제 추적에 사용되던 `jdk.tracePinnedThreads` 시스템 속성은 더 이상 필요 없으므로 제거된다.
