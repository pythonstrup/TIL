# Defer

- `defer` 는 뒤따르는 함수 호출을 **지금 실행하지 않고, 현재 함수가 반환되는 시점으로 미뤄** 실행한다.
- "열었으면 닫기, 잡았으면 풀기"처럼 정리 작업을 깜빡하지 않도록 자원 획득 코드 바로 옆에 적어두는 용도로 주로 쓴다.

```go
func readFile(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close() // 이 함수가 어떻게 끝나든 반드시 닫힌다

	// ... f 사용 ...
	return nil
}
```

- 함수 중간에 `return` 이 여러 군데 있어도, 어느 경로로 빠져나가든 `defer` 는 실행된다. 그래서 정리 누락을 구조적으로 막아준다.

## 실행 순서는 LIFO (역순)

- `defer` 가 여러 개면 **나중에 선언한 것이 먼저** 실행된다(스택처럼 쌓였다 거꾸로 풀림).

```go
func main() {
	defer fmt.Println("1")
	defer fmt.Println("2")
	defer fmt.Println("3")
	fmt.Println("main")
}
// 출력:
// main
// 3
// 2
// 1
```

- 자원을 연 순서의 역순으로 닫고 싶을 때 자연스럽게 맞아떨어진다.

## 인자는 defer 시점에 평가된다 (중요한 함정)

- `defer` 로 미뤄지는 것은 **함수 실행**이지 **인자 평가**가 아니다. 인자는 `defer` 문을 만난 그 순간 값이 고정된다.

```go
func main() {
	i := 0
	defer fmt.Println("defer 시점 값:", i) // i는 여기서 0으로 고정
	i = 10
	fmt.Println("현재 값:", i)
}
// 출력:
// 현재 값: 10
// defer 시점 값: 0
```

- 반환 직전의 최신 값을 쓰고 싶다면 클로저로 감싼다.

```go
func main() {
	i := 0
	defer func() { fmt.Println("실행 시점 값:", i) }() // 실행될 때의 i를 읽음 → 10
	i = 10
}
```

## 명명된 반환값을 수정할 수 있다

- 반환값에 이름을 붙이면(named return), `defer` 안에서 그 값을 고쳐 최종 반환값을 바꿀 수 있다.
- 에러를 한 곳에서 감싸거나, `recover` 로 패닉을 에러로 변환할 때 유용하다.

```go
func doWork() (err error) {
	defer func() {
		if r := recover(); r != nil {
			err = fmt.Errorf("패닉 복구: %v", r) // 반환값 err를 교체
		}
	}()

	panic("뭔가 터짐")
}
// doWork()는 패닉으로 죽지 않고 error를 반환한다
```

## panic / recover 와의 관계

- 함수가 패닉으로 중단돼도 **이미 등록된 `defer` 들은 실행된다.** 그래서 자원 정리가 보장된다.
- `recover()` 는 반드시 `defer` 함수 안에서 호출해야 패닉을 붙잡아 정상 흐름으로 되돌릴 수 있다.

## 반복문 안의 defer 주의

- `defer` 는 **함수가 끝날 때** 실행된다. 루프를 도는 동안에는 실행되지 않고 계속 쌓이기만 한다.
- 루프에서 매 반복마다 자원을 열고 닫아야 한다면, 루프 안의 defer는 함수가 끝날 때까지 자원을 붙잡고 있어 누수가 된다.

```go
// 나쁜 예: 파일 핸들이 함수 끝까지 안 닫힘 (수천 개 쌓일 수 있음)
func process(names []string) {
	for _, name := range names {
		f, _ := os.Open(name)
		defer f.Close() // 루프가 다 끝난 뒤에야 한꺼번에 실행됨
		// ...
	}
}

// 좋은 예: 반복 단위를 별도 함수로 빼서 defer 범위를 좁힌다
func process(names []string) {
	for _, name := range names {
		processOne(name) // 이 함수가 끝날 때마다 defer 실행
	}
}

func processOne(name string) {
	f, _ := os.Open(name)
	defer f.Close()
	// ...
}
```

## 자주 쓰는 패턴

- 파일/연결 닫기: `defer f.Close()`, `defer conn.Close()`
- 뮤텍스 잠금 해제: `defer mu.Unlock()`
- WaitGroup 카운트 감소: `defer wg.Done()`
- 시작/종료 로깅, 시간 측정

```go
func work() {
	mu.Lock()
	defer mu.Unlock() // 함수 끝에서 자동 해제 → 중간 return 있어도 안전

	// ... 임계 구역 ...
}
```

## 실전 예시 — 연결 정리

- 함수 안에서 여러 자원을 열 때, 연 직후 `defer` 로 정리를 예약하면 어느 경로로 반환해도 누수가 없다.

```go
func (a *Agent) serveOne(ctx context.Context) error {
	relayConn, err := dialer.DialContext(ctx, "tcp", a.relayAddr)
	if err != nil {
		return err
	}
	defer relayConn.Close() // 릴레이 연결 정리 예약

	localConn, err := dialer.DialContext(ctx, "tcp", a.localAddr)
	if err != nil {
		return err // relayConn은 defer로 닫히므로 누수 없음
	}
	defer localConn.Close() // 로컬 연결 정리 예약

	return netio.Join(relayConn, localConn)
}
```

- 두 번째 dial이 실패해 중간에 `return` 해도, 먼저 등록된 `relayConn.Close()` 가 실행돼 정리된다.

## 정리

| 특징 | 내용 |
|---|---|
| 실행 시점 | 현재 함수가 반환될 때 |
| 여러 개일 때 순서 | LIFO (나중 선언이 먼저 실행) |
| 인자 평가 | `defer` 문을 만난 시점에 고정 |
| 반환값 수정 | 명명된 반환값을 defer 안에서 변경 가능 |
| 패닉 시 | 등록된 defer는 실행됨, `recover`는 defer 안에서만 |
| 루프 주의 | 함수 끝까지 안 실행됨 → 반복 단위를 함수로 분리 |
