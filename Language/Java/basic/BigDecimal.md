# BigDecimal은 문자열을 전달해야 한다.

- double은 근삿값을 담고 있기 때문에 이를 사용하여 BigDecimal을 생성하면 이상한 값이 출력되는 것을 확인할 수 있다.

```java
new BigDecimal(2.5545) // 2.55449999999999999289457264239899814128875732421875
```

- 따라서 String 생성자나 valueOf() 메서드를 사용해서 BigDecimal을 생성해야 한다.

```java
new BigDecimal("2.5545");   // 2.5545
BigDecimal.valueOf(2.5545); // 2.5545
```

<br/>

# BigDecimal은 소수점을 지정해줘야 한다. 

- 소수점을 지정하지 않으면 

```shell
Non-terminating decimal expansion; no exact representable decimal result.
```

- 이자를 계산하는 로직이다.
- 아래와 같이 큰 값을 계산할 때 소수점 자리를 지정해주지 않으면 `Non-terminating decimal expansion; no exact representable decimal result.` 에러가 발생할 수 있다. 

```java
public class Loan {

    private final BigDecimal amount;
    private final double rate;
    private final int days;
    
    public BigDecimal calculateInterest() {
        return amount
             .multiply(BigDecimal.valueOf(rate))
             .multiply(BigDecimal.valueOf(days))
             .divide(BigDecimal.valueOf(365)); 
    }
} 
```

- BigDecimal에서 나누기를 할 때 무한 소수가 생길 수 있기 때문에 몇 자리 수까지 표기할 것인지 정해야 한다.
- 소수점을 정하는 방법은 아래와 같이 두 가지다.

## 1. MathContext

```java
public class Loan {
  
  public BigDecimal calculateInterest() {
    return amount
        .multiply(BigDecimal.valueOf(rate))
        .multiply(BigDecimal.valueOf(days))
        .divide(BigDecimal.valueOf(365), MathContext.DECIMAL32);
  }
} 
```

### 기본 타입

- `MathContext.DECIMAL32`: 7번째 자릿수에서 올리거나 내림 (소수점 6자리)
- `MathContext.DECIMAL64`: 16번째 자릿수에서 올리거나 내림 (소수점 16자리)
- `MathContext.DECIMAL128`: 34번째 자릿수에서 올리거나 내림 (소수점 33자리)
- `MathContext.UNLIMITED`: 제한 없음 => 에러 발생함.

### 예시

- `MathContext`에 정의되어 있는 기본 타입 사용

```java
ten.divide(nine, MathContext.DECIMAL32); // 1.111111
ten.divide(nine, MathContext.DECIMAL64); // 1.111111111111111
ten.divide(nine, MathContext.DECIMAL128); // 1.111111111111111111111111111111111
```

- 아래와 같이 `MathContext`를 직접 생성해 사용할 수도 있다. 

```java
BigDecimal ten = BigDecimal.valueOf(10);
BigDecimal nine = BigDecimal.valueOf(9);

ten.divide(nine, new MathContext(5, RoundingMode.HALF_UP)); // 1.1111
ten.divide(nine, new MathContext(10, RoundingMode.HALF_UP)); // 1.111111111
```

## 2. RoundingMode

- `divide` 메소드에느 `scale`이라는 파라미터가 있다.
  - 몇 자리까지 보여줄 것인지를 정하는 변수다.
  - `MathContext`를 생성할 때 전달하는 `setPrecision`과의 차이점 => 몇번째 자리에서 올리거나 내릴 것인지를 결정하기 때문 

### Type

- `RoundingMode.UP`: 절대값 올림 (7.5 -> 8 / -6.5 -> -7)
- `RoundingMode.DOWN`: 절대값 내림 (7.5 -> 7 / -6.5 -> -6)
- `RoundingMode.CEILING`: 천장함수 (7.5 -> 8 / -6.5 -> -6)
- `RoundingMode.FLOOR`: 바닥함수 (7.5 -> 7 / -6.5 -> -7)
- `RoundingMode.HALF_UP`: 반올림. 사사오입.
- `RoundingMode.HALF_DOWN`: 반내림. (5를 초과해야 올림)
- `RoundingMode.HALF_EVEN`: 반올림. 오사오입, Bankers Rounding. 
  - 5 초과 시 올림. 5 미만이면 내림. 5일 경우 앞자리 숫자가 짝수면 버리고 홀수면 올림하여 짝수로 만듦.
  - 왜 이런 반올림을 사용하는가? => [은행가의 반올림](https://www.freeism.co.kr/wp/archives/1792)

### 예시

- 우리가 가장 친근한 반올림(사사오입)의 예시

```java
ten.divide(nine, 5, RoundingMode.HALF_UP); // 1.11111
ten.divide(nine, 10, RoundingMode.HALF_UP); // 1.1111111111
ten.divide(nine, 12, RoundingMode.HALF_UP); // 1.111111111111
```

- 보통은 `Bankers Rounding`을 사용한다. (`HALF_EVEN`)

```java
BigDecimal target = BigDecimal.valueOf(2.5545);
target.divide(BigDecimal.ONE, 0, RoundingMode.HALF_EVEN); // 3
target.divide(BigDecimal.ONE, 1, RoundingMode.HALF_EVEN); // 2.6 => 5: 홀수이기 때문에 가까운 짝수로 올림
target.divide(BigDecimal.ONE, 2, RoundingMode.HALF_EVEN); // 2.55 => 4: 내림
target.divide(BigDecimal.ONE, 3, RoundingMode.HALF_EVEN); // 2.554
target.divide(BigDecimal.ONE, 4, RoundingMode.HALF_EVEN); // 2.5545
```


# 참고자료

- [지마켓 기술블로그](https://dev.gmarket.com/75)
