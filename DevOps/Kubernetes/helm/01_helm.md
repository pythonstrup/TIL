# Helm 이란?

# k8s의 패키지 매니저!

- Node의 npm, yarn
- Java에 Maven, Gradle
- k8s는 helm이다.

## Chart

- 차트는 헬름 패키지이다.
- 차트는 쿠버네티스 클러스터 내에서 애플리케이션, 도구, 서비스를 구동하는데 필요한 리소스 정의가 포함
- ex) homebrew fomula, apt dpkg, yum prm 등

## Repository

- 차트(패키지)를 모아두고 공유하는 장소이다.
- Java 진영의 maven central과 같은 곳이다.

## Release

- 쿠버네티스 클러스터에서 구동되는 차트의 인스턴스이다.
- 하나의 차트는 동일한 클러스터에서 여러 번 설치될 수 있으며, 설치될 때마다 새로운 release가 생성된다.

<br/>

# 기본 명령

### helm search - 차트 검색

- 여러 저장소에 있는 헬름 차트를 포괄하는 helm hub를 검색한다.

```bash
$ helm search hub {hub_name}
```

- `helm repo add`로 추가된 저장소들을 검색한다.

```bash
$ helm search repo {repo_name}
```

- search는 퍼지 문자열 매칭 알고리즘을 사용하기 때문에 문구의 일부분만 입력해도 찾을 수 있다.

### helm install - 패키지 설치

- 새로운 패키지를 설치하기 위해 사용하는 명령어이다.

```bash
$ helm install {release_name} {chart_name}
```

### helm list - 리스트 확인

- 현재 배포된 모든 리스트를 확인할 수 있다.

```bash
$ helm list
```

- 실패하거나 삭제된 기록도 보고 싶다면 아래 명령어를 사용하자.

```bash
$ helm list --all
```

### helm status - 정보 확인

- 릴리스의 상태를 추적하거나 구성 정보를 확인하기 위해 사용한다.

```bash
$ helm status {release_name}
```

### helm show values - 차트 옵션 확인

- 차트를 커스터마이징하기 위해 values를 설정하게 되는데 어떤 옵션이 구성 가능한지 확인할 수 있는 명령어이다.

```bash
$ helm show values {chart_name}
```

### helm get valuse - 차트 설정 확인

- 차트에 설정이 적용되었는지 확인하고 싶을 때 사용한다.

```bash
$ helm get values {release_name}
```

### helm upgrade - 릴리스 업그레이드

- 새로운 버전의 차트가 릴리스 되었을 때 릴리스 구성을 변경하고자 해당 명령어를 사용할 수 있다.

```bash
$ helm upgrade {release_name} {chart_name}
```

- 아래와 같이 yaml 파일을 적용해 업그레이드할 수 있다.

```bash
$ helm upgrade -f {file_name}.yaml {release_name} {chart_name}
```

### helm rollback - 릴리스 실패 복구

- 릴리스가 이상하게 동작한다면 rollback 명령어를 통해 복구할 수 있다.

```bash
$ helm rollback {release_name} {release_version}
```

- 릴리스의 리비전 번호를 확인하고 싶다면 아래 명령어를 통해 확인할 수 있으니 참고하자.

```bash
$ helm history {release_name}
```

### helm uninstall - 릴리스 삭제

- 릴리스를 삭제할 수 있다.

```bash
$ helm uninstall {release_name}
```

### helm repo - 저장소 명령어

- 사용하고 있는 저장소를 확인할 수 있다.

```bash
$ helm repo list
```

- 저장소를 추가할 수 있다.

```bash
$ helm repo add {repo_name} {repo_url} [flags]
```

- 삭제도 가능하다.

```bash
$ helm repo remove {repo_name}
```

### helm create - 차트 만들기

```bash
$ helm create {chart_name}
```

### helm lint - 형식 검증

- 차트를 수정했다면 형식이 올바른지 검증할 수 있다.

```bash
$ helm lint {chart_name}
```

### helm package - 차트 패키지

- 배포용으로 차트를 패키징할 수 있다. 패키징된 차트는 install로 설치 가능하다.

```bash
$ helm package {chart_name}
```

<br/>

# 저장소

- Stable 저장소링크 : https://github.com/helm/charts
- Artifact Hub: https://artifacthub.io/

# Helm 자료

[Helm | 헬름 사용하기](https://helm.sh/ko/docs/intro/using_helm/)