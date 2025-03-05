# Gradle 빌드에서 성능 개선하기

## 버전 업데이트하기

### Gradle 

- Gradle 팀은 지속적으로 Gradle 빌드의 성능을 개선하고 있다.
- Gradle 버전 업그레이드는 리스크가 낮다. Gradle 팀이 마이너 버전 간의 하위 호환성(backwards compatibility) 을 보장하고 있기 때문이다.

### Java 

- Gradle은 Java Virtual Machine(JVM) 에서 실행된다. Java의 성능 향상이 Gradle에도 영향을 미치는 경우가 많다.
- 따라서 최신 버전의 자바를 사용하면 Gradle의 성능이 최적화될 수 있다.

### Plugins

- 플러그인도 마찬가지다.

<br/>

## 병렬 처리

- 프로젝트가 여러 개의 `subproject`로 구성되어 있고, 각 `subproject`가 독립적인 작업을 가지고 있다면 병렬로 실행하는 것을 고려해볼 수 있다.
- 아래와 같이 명령어에서 병렬 옵션을 사용할 수 있다.

```shell
$ gradle <task> ---parallel
```

- 또는 프로젝트의 root directory에 `gradle.properties` 파일을 추가하여 아래 설정을 추가해도 된다.

```properties
org.gradle.parallel=true
```

- 병렬 처리의 효과는 프로젝트 구조 및 서브 프로젝트 간 의존성에 따라 달라질 수 있다.
  - 특정 서브 프로젝트 하나의 실행 시간이 대부분을 차지하는 경우, 병렬 실행의 효과가 크지 않다.
  - 서브 프로젝트 간 의존성이 많을 경우에도 병렬 실행의 이점이 제한될 수 있다.
  - 그래도 대부분의 다중 서브 프로젝트 환경에서는 빌드 시간이 단축되는 효과를 볼 수 있다.

<br/>

## Gradle Daemon 재활성화

- Gradle Daemon이 빌드 시간을 단축하는 데 도움이 된다.
- Gradle은 기본적으로 Daemon을 활성화하지만, 일부 빌드는 이 설정을 변경하여 비활성화할 수도 있다.
- 만약 Daemon이 비활성화되어 있다면, 다시 활성화했을 때 빌드 성능이 크게 향상될 수 있다.

### 장점

- 여러 빌드 간에 프로젝트 정보를 캐싱
- 백그라운드에서 실행되어 매번 JVM을 다시 시작할 필요 없음
- JVM의 지속적인 런타임 최적화 활용
- 파일 시스템을 감시하여 빌드해야 할 항목을 정확하게 계산

### 빌드 실행 시 Daemon 활성화

- 아래와 같이 명령어 옵션을 통해 Daemon을 사용할 수 있다.

```shell
$ gradle <task> --daemon
```

- 또는, `gradle.properties`를 통해 Daemon을 기본적으로 활성화하는 방안도 존재한다.

### CI 환경에서의 Daemon 사용

- 개발자 로컬 환경에서는 Daemon을 활성화하면 빌드 성능이 크게 향상된다.
- CI 서버에서는 장시간 실행되는 에이전트의 경우 Daemon을 활성화하는 것이 좋다.
  - **Jenkins의 정적 에이전트는 지속적으로 유지되므로, Daemon의 효과를 볼 수도..?** (하지만 정적 에이전트는 규모가 커질수록 여러 문제를 발생시키기에, On-demand Agent를 지향하게 된다.)
- 사실, 짧은 수명의 CI 빌드 환경에서는 Daemon의 효과가 크지 않을 수 있다.
  - **Github Actions**는 인스턴스가 매번 새롭게 생성되므로 Deamon을 유지할 수 없다.
- Gradle 3.0 이상에서는 메모리가 부족하면 자동으로 Daemon이 종료되므로, Daemon을 활성화해도 안전하다.

<br/>

## 구성 캐시 활성화

> #### 유의사항
> - Configuration Cache 기능에는 아래와 같은 제한사항이 있다.
>   - 구성 캐시는 모든 Gradle 플러그인과 기능을 완벽히 지원하지 않는다.
>   - 빌드 및 사용하는 플러그인에 변경이 필요할 수도 있다.
>   - IDE에서의 프로젝트 가져오기 및 동기화에는 구성 캐시가 사용되지 않는다.

- 구성 캐시를 활성화하면 구성 단계(Configuration Phase)의 결과를 캐싱할 수 있다. 즉, 이전 빌드와 동일한 입력값을 사용하면 Gradle이 구성 단계를 건너뛸 수 있다는 말이다.
- 구성 캐시는 다음과 같은 입력값을 기반으로 한다.
  - 초기화(init) 스크립트
  - 설정(settings) 스크립트
  - 빌드(build) 스크립트
  - 구성 단계에서 사용되는 시스템 속성(System Properties)
  - 구성 단계에서 사용되는 Gradle 속성(Gradle Properties)
  - 환경 변수(Environment Variables)
  - 프로바이더(provider)를 통해 접근하는 구성 파일(Configuration Files)
  - buildSrc 입력값 (빌드 구성 입력값 및 소스 파일 포함)

### 구성 캐시 활성화

- 명령어

```shell
$ gradle <task> --configuration-cache
```

- `gradle.properties`

```properties
org.gradle.configuration-cache=true
```

### 추가적인 이점 

- 구성 캐시를 활성화하면 다음과 같은 추가적인 성능 향상을 얻을 수 있다.
  - 모든 태스크를 병렬로 실행 가능 (같은 서브프로젝트 내에서도 병렬 실행됨)
  - 의존성(Dependency) 해석 결과를 캐싱하여 빌드 속도 향상

<br/>

## 빌드 캐시 활성화

- 빌드 캐시는 특정 입력에 대해 작업 출력을 저장하는 최적화 기능이다. 동일한 입력으로 동일한 작업을 나중에 다시 실행하게 되면 빌드 캐시에서 작업 출력 값을 가져오고 해당 작업을 다시 실행하지 않는다.

### 활성화 방법

- 명령어

```shell
$ gradle <task> --build-cache
```

- `gradle.properties`

```properties
org.gradle.caching=true
```

<br/>

# 참고자료

- [Gradle 공식 문서](https://docs.gradle.org/current/userguide/performance.html)
