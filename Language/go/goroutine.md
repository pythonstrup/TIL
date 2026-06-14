# Goroutine

- 고루틴(goroutine)은 Go 런타임이 관리하는 가벼운 실행 단위다.
- 함수 호출 앞에 `go` 키워드를 붙이면, 그 함수가 별도의 흐름으로 **동시에** 실행되고 호출한 쪽은 기다리지 않고 다음 줄로 넘어간다.

```go
func main() {
	go say("world") // 동시에 실행 (기다리지 않음)
	say("hello")    // 메인 흐름에서 실행
}

func say(s string) {
	for i := 0; i < 3; i++ {
		fmt.Println(s)
	}
}
```

- 일반 호출 `say("world")` 는 끝날 때까지 블록되지만, `go say("world")` 는 백그라운드로 던지고 즉시 다음으로 진행한다.

## OS 스레드와 다른 점

- 고루틴은 OS 스레드가 아니다. Go 런타임의 스케줄러가 다수의 고루틴을 소수의 OS 스레드 위에 얹어 실행한다(M:N 스케줄링).
- 초기 스택이 수 KB로 매우 작고 필요에 따라 늘어나, 수천~수만 개를 띄워도 부담이 적다.
- 생성/전환 비용이 스레드보다 훨씬 싸서, "작업마다 고루틴 하나"가 자연스러운 패턴이 된다.

## main이 끝나면 고루틴도 죽는다

- `main` 함수가 반환하면 프로그램 전체가 종료되며, 아직 도는 고루틴이 있어도 같이 사라진다.
- 따라서 고루틴이 끝날 때까지 기다리는 장치가 필요하다.

```go
func main() {
	go fmt.Println("이 줄은 출력 안 될 수도 있다")
	// main이 바로 끝나면 위 고루틴은 실행 기회를 못 얻는다
}
```

## WaitGroup — N개가 끝날 때까지 기다리기

- `sync.WaitGroup` 은 "끝나길 기다릴 작업 수"를 세는 카운터다.
- `Add(n)` 으로 카운트를 올리고, 각 고루틴이 끝날 때 `Done()` 으로 1씩 내리며, `Wait()` 가 0이 될 때까지 블록한다.

```go
func main() {
	var wg sync.WaitGroup

	for i := 0; i < 3; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done() // 끝나면 반드시 카운트 감소
			fmt.Println("작업", i)
		}()
	}

	wg.Wait() // 3개 모두 끝날 때까지 대기
}
```

- `defer wg.Done()` 로 적어두면 함수가 어떻게 끝나든 반드시 카운트를 깎는다.

## 채널 — 고루틴 사이의 데이터 전달

- 고루틴끼리 값을 주고받는 통로가 채널(`chan`)이다. "메모리를 공유해 통신하지 말고, 통신해서 메모리를 공유하라"가 Go의 철학이다.
- `<-` 연산자로 보내고(`ch <- v`) 받는다(`v := <-ch`). 받는 쪽은 값이 올 때까지 블록된다.

```go
func main() {
	ch := make(chan int)

	go func() {
		ch <- 42 // 값 보내기
	}()

	v := <-ch // 값 받기 (올 때까지 대기)
	fmt.Println(v) // 42
}
```

- 버퍼 있는 채널 `make(chan int, 16)` 은 버퍼가 찰 때까지 보내는 쪽이 블록되지 않는다. 대기열로 자주 쓰인다.

## select — 여러 채널 중 먼저 준비된 것 처리

- `select` 는 여러 채널 작업 중 **먼저 가능한 하나**를 골라 실행한다. `switch` 의 채널 버전이다.

```go
select {
case v := <-ch:
	fmt.Println("값 도착:", v)
case <-time.After(time.Second):
	fmt.Println("1초 타임아웃")
case <-ctx.Done():
	fmt.Println("취소됨")
}
```

- 타임아웃, 취소(context), 여러 입력 대기 같은 동시성 제어의 핵심 도구다.

## 동시 접근 보호 — Mutex / Once

- 여러 고루틴이 같은 변수를 동시에 건드리면 경쟁 상태(race condition)가 생긴다.
- `sync.Mutex` 로 임계 구역을 잠그거나, `sync.Once` 로 "평생 한 번만" 실행을 보장한다.

```go
var (
	once sync.Once
	mu   sync.Mutex
	n    int
)

once.Do(func() { fmt.Println("딱 한 번만 실행") })

mu.Lock()
n++ // 한 번에 한 고루틴만 진입
mu.Unlock()
```

- 경쟁 상태 의심 시 `go run -race` / `go test -race` 로 탐지할 수 있다.

## 흔한 함정

- **고루틴 누수**: 채널을 기다리며 영원히 블록되는 고루틴은 끝나지 않고 메모리에 남는다. 끝낼 경로(연결 닫기, context 취소 등)를 항상 마련한다.
- **반복 변수 캡처**: Go 1.21 이하에서는 루프 변수가 공유돼 모든 고루틴이 마지막 값을 보는 버그가 있었다. **Go 1.22부터는 반복마다 변수가 새로 생겨** 이 문제가 사라졌다(이 저장소 기준 `go 1.25`).

```go
// Go 1.22+ 에서는 의도대로 0,1,2 출력 (순서는 비결정적)
for i := 0; i < 3; i++ {
	go func() { fmt.Println(i) }()
}
```

## 실전 예시 — 양방향 연결 잇기

- 연결 두 개를 양방향으로 이어붙이려면, 두 방향 복사를 동시에 돌려야 한다.
- 고루틴 2개 + `WaitGroup`(둘 다 끝날 때까지 대기) + `Once`(첫 에러만 기록)의 조합이 자주 쓰인다.

```go
func Join(a, b io.ReadWriteCloser) error {
	var (
		once     sync.Once
		firstErr error
		wg       sync.WaitGroup
	)

	copyOneWay := func(dst, src io.ReadWriteCloser) {
		defer wg.Done()
		_, err := io.Copy(dst, src)
		if err != nil {
			once.Do(func() { firstErr = err })
		}
		// 한 방향이 끝나면 양쪽을 닫아 반대 방향 io.Copy를 깨운다
		_ = a.Close()
		_ = b.Close()
	}

	wg.Add(2)
	go copyOneWay(a, b) // A → B
	go copyOneWay(b, a) // B → A
	wg.Wait()

	return firstErr
}
```

- 핵심 흐름: 두 방향을 고루틴으로 동시에 복사 → 한쪽이 끝나면 양쪽 연결을 닫아 다른 쪽도 깨움 → 둘 다 끝날 때까지 `Wait` → 첫 에러 반환.

## 정리

| 도구 | 용도 |
|---|---|
| `go f()` | 함수를 동시에 실행 |
| `sync.WaitGroup` | N개 고루틴이 끝날 때까지 대기 |
| `chan` | 고루틴 간 값 전달 |
| `select` | 여러 채널 중 먼저 준비된 것 처리 |
| `sync.Mutex` | 임계 구역 보호 |
| `sync.Once` | 평생 한 번만 실행 |
| `go run -race` | 경쟁 상태 탐지 |
