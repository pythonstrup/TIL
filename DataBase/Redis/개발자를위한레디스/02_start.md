# 2장 레디스 시작하기

## 레디스 환경 구성

### 서버 환경 설정 변경

- 레디스의 기본 `maxclients` 설정값은 10,000
  - 레디스 프로세스에서 받아들일 수 있는 최대 클라이언트이 개수 의미 => 파일 디스크립터 수에 영향을 받는다.
  - 레디스 프로세스 내부적으로 사용하기 위해 예약한 파일 디스크립터 수는 32개. 
  - `maxclients` 값에 32를 더한 값보다 서버의 최대 파일 디스크립터 수가 작으면 레디스는 실행될 때 자동으로 그 수에 맞게 조정된다.
- 만약 레디스의 최대 클라이언트 수를 기본값이 10,000으로 지정하고 싶으면 서버의 파일 디스크립터 수를 최소 10032 이상으로 지정해야 한다.
- 확인방법

```shell
$ ulimit -a | grep open
open files  (-n) 1048576
```

- 만약 10032보다 낮다면 /etc/security/limits.conf에서 아래와 같은 구문 추가

```shell
*    soft    nofile    100000
*    hard    nofile    100000
```

### THP 비활성화

- 기본 페이지 크기 = 4096 Byte
- `THP(Transparent Huge Page)` => 페이지를 크게 만든 뒤 자동으로 관리하는 기능
  - 오히려 오버헤드를 발생시키는 경우도 있어 이 기능을 사용하지 않는 것을 추천
- 일시적 중지

```shell
$ echo never > /sys/kernel/mm/transparent_hugepage/enabled
```

- 영구적 중지. `/etc/rc.local` 파일에 구문 추가

```shell
if test -f /sys/kernel/mm/transparent_hugepage/enabled; then
  echo never > /sys/kernel/mm/transparent_hugepage/enabled
fi
```

```shell
$ chmod +x /etc/rc.d/rc.local
```

### `vm.overcommit_memory=1`로 변경

- 레디스는 디스크에 파일을 저장할 때 `fork()`를 이용해 백그라운드 프로세스를 만든다.
  - `COW(Copy On Write)`라는 메커니즘이 동작. 
  - 부모 프로세스와 자식 프로세스가 동일한 메모리 페이지를 공유하다가 레디스의 데이터가 변경될 때마다 메모리 페이지를 복사 => 데이터 변경이 많이 발생하면 메모리 사용량 빠르게 증가
- 따라서 메모리를 순간적으로 초과해 할당해야 하는 상황을 대비하기 위해 `vm.overcommit_memory=1`로 두는 것이 좋다. 
- 기본적으로 `vm.overcommit_memory`가 0으로 설정되어 있어, 필요한 메모리를 초과해 할당하는 것을 제한.
  - 그러나 이 값을 조절해 메모리의 과도한 사용이나 잘못된 동작을 예방하고, 백그라운드에서 데이터를 저장하는 과정에서의 성능 저하나 오류를 방지할 수 있게 설정해야 한다.

### `somaxconn`과 `syn_backlog` 설정 변경

- `tcp-backlog`의 기본 값인 511보다 크게 설정하도록 하자.
  - `tcp-backlog`는 클라이언트와 통신할 때 사용하는 tcp backlog 큐의 크기를 지정. 

```shell
$ sysctl -a | grep syn_backlog
net.ipv4.tcp_max_syn_backlog = 128
```

```shell
$ sysctl -a | grep somaxconn
net.core.somaxconn = 128
```

- 아래와 같이 설정 수정 가능

```shell
$ net.ipv4.tcp_max_syn_backlog = 1024
$ net.core.somaxconn = 1024
```

- 재부팅 없는 설정 변경
- 
```shell
$ sysctl net.ipv4.tcp_max_syn_backlog = 1024
$ sysctl net.core.somaxconn = 1024
```

<br/>

---

# 참고자료

- 개발자를 위한 레디스, 김가람 지음
