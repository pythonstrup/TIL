# Service Endpoints

- `Service`는 Pod들에 대한 접근을 추상화한 리소스이다.
- `Endpoint`는 실제 Pod의 네트워크 주소와 정보를 나타내는 객체다.
- `Service`는 Pod의 IP 주소를 직접 알지 못하고, 대신 `Endpoint` 객체를 통해 Pod에 접근한다.
  - `Service`에서는 `Label Selector`를 통해 연결할 Pod를 지정하며, 이 Selector와 일치하는 Pod의 IP가 `Endpoint`에 등록되며, 이 `Endpoint`는 또한 `Service`와 자동으로 연결된다.

## 원하는 Pod로만 로드밸런싱하고 싶다..?

- Server가 멈출 수도 있는 작업(ex-Heap Dump)을 실행해야 할 때 우리는 해당 Pod로 연결이 이뤄지지 않게 막아야 한다.  
- 이 때, `Service`와 연결된 Pod의 IP를 `Endpoint`에서 제거해 해당 pod로 로드밸런싱이 되지 않게 막을 수 있다.

## `spec.replicas`

- 아래와 같이 kubernetes manifest 파일에서 `spec.replicas`의 개수를 늘리면 pod가 늘어난다.

```yaml
spec:
  replicas: 2
```

> #### 참고
> - `spec.containers` 설정을 건드리면 pod 하나에 여러 컨테이너가 떠 있게 만들 수도 있다.

## pod와 svc 확인

- 위와 같이 `spec.replicas` 설정을 2로 두고 deployment로 배포를 한 뒤, pod를 확인해보면 아래와 같이 2개의 pod를 확인할 수 있을 것이다.

```shell
$ kubectl get pod -n my-namespace
 NAME                                READY   STATUS    RESTARTS        AGE
my-api-deploy-59b79c4795-q8l6p       1/1     Running   0               8d
my-api-deploy-59b79c4795-j9cxz       1/1     Running   0               8d
```

- 위 2개의 pod는 `my-svc`라는 service와 연결되어 있다.
- 이제 service의 Endpoint를 확인해보자. 아래와 같이 2개의 Endpoint가 연결되어 있는 것을 확인할 수 있다.

```shell
$ kubectl get endpoints -n my-namespace my-svc
NAME     ENDPOINTS                           AGE
my-svc   10.244.1.11:8080,10.244.2.16:8080   365d
```

- 아래와 같이 `endpoints` 대신 `ep`라는 줄임말로도 확인이 가능하다.

```shell
$ kubectl get ep -n my-namespace
```

## endpoints 편집하기 

### 먼저 service의 원리를 파악해야 한다.

- service에는 label 기능이 존재한다.

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-api-deploy
  labels:
    app: my-api
```

```yaml
apiVersion: v1
kind: Service
metadata:
  name: my-svc
spec:
  selector:
    app: my-api
```

- pod 정의의 `metadata.labels.app`과 service 정의의 `spec.selector.app`이 일치해야 자동으로 연결된다.
- 이 설정이 존재하면 아무리 endpoints를 끊어놔도 pod가 자동으로 연결되어 버린다.
- 따라서 특정 pod의 연결을 끊고 싶다면 잠시 service의 `spec.selector.app` 설정을 지워줘야 한다.

```yaml
apiVersion: v1
kind: Service
metadata:
  name: my-svc
```

- `kubectl apply` 명령어를 통해 적용해주자.

### 이제 명령어로 endpoints를 제거해보자!

- 먼저 pod의 endpoints를 확인해야한다. `describe` 명령어를 통해 확인해보자.

```shell
$ kubectl describe pod -n my-namespace
Name:             my-api-deploy-59b79c4795-q8l6p
Namespace:        my-namespace
IP:               10.244.1.11
IPs:
  IP:           10.244.1.11
  
...

Name:             my-api-deploy-59b79c4795-j9cxz
Namespace:        my-namespace
IP:               10.244.2.16
IPs:
  IP:           10.244.2.16
```

- 내가 제거하고자 하고는 pod는 `10.244.1.11`라고 가정해보자.

```shell
$ kubectl edit endpoints -n my-namespace my-svc
```

### 다시 연결해주기

- 다시 연결해주는 것은 오히려 쉽다. service 설정 파일을 복구해주면 된다. (`spec.selector.app`)

```yaml
apiVersion: v1
kind: Service
metadata:
  name: my-svc
spec:
  selector:
    app: my-api
```

## 실전 예시 - pod 연결 끊고, heap dump 떠보기!

- 이제 pod 연결이 끊어졌으니, 해당 pod의 heap dump를 떠보자!

```shell
$ kubectl exec -it <pod name> -n <namespace> -- /bin/bash
```

- bash shell에서 heap dump를 떠보자.

```shell
sh-4.4# jmap -dump:format=b,file=/tmp/<filename>.hprof 1
```

- heap dump가 만들어질 때까지는 수십 초가 걸린다.
- 덤프가 완료되면 bash shell에서 빠져나와 내 device에 dump를 저장할 차례다.

```shell
kubectl cp \
  my-namespace/my-api-deploy-59b79c4795-q8l6p:/tmp/my-api-20250226.hprof \ 
  /Users/myroot/dumps/my-api-20250226.hprof 
```

- 이제 `Selector`를 지정해 다시 `Endpoint`에 Pod의 IP를 등록해주면 끝이다.
