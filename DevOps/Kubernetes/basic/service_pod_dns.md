# 서비스 및 파드용 DNS

- 쿠버네티스는 파드와 서비스를 위한 DNS 레코드를 생성한다. 이를 통해 클러스터 안에서는 IP주소가 아닌 DNS 네임을 통해 서비스에 접속할 수 있다.

## Service

### A/AAAA Record

- 같은 네임스페이스에 존재하는 서비스라면 아래와 같이 서비스 이름만 적어줘도 된다.

```shell
<service-name>
```

- 하지만 다른 네임스페이스일 경우 아래와 같이 네임스페이스를 명시해줘야 한다.

```shell
<service-name>.<namespace>
<service-name>.<namespace>.svc
<service-name>.<namespace>.svc.cluster.local
```
