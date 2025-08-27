# 윈도우 함수

- 집계 함수는 컬럼 간의 연산은 잘 수행하지만, 레코드 간의 연산은 처리하지 못하는 문제가 있다.
- 따라서 레코드 간의 연산을 처리하기 위한 함수인 `윈도우 함수 Window Function`이 만들어졌다.

## 특징

- 서브쿼리는 사용 가능하지만, 중첩쿼리는 사용이 불가능하다.
- `OVER` 키워드가 필수로 사용된다.
- `BETWEEN`을 사용하는 타입과 사용하지 않는 타입. 두 가지 종류가 있다.
- 대충 아래와 같은 형태로 사용된다.

```sql
SELECCT 
    WINDOW_FUNCTION (ARGUMENTS)
        OVER ([PARTITION BY {COLUMN}] [ORDER BY] [ROWS BETWEEN ~ AND ~])
FROM TABLE
```

## 옵션

### 1. `PARTITION BY` 컬럼

- SQL에서 그룹화하는 것처럼 윈도우 함수를 적용할 데이터를 나눈다.
- ex) `PARTITION BY department`로 옵션을 주면, 부서(department)별로 윈도우를 나눠 계산한다. 

### 2. `ORDER BY` 컬럼

- 데이터 정렬 기능과 같이 윈도우 안에서 함수 적용 전에 데이터를 정렬
- `PARTIION`을 지정하면 그 안에서 데이터를 정렬

### 3. `ROW BETWEEN ~ AND ~`

- 윈도우의 프레임을 지정한다.
- `~`에 들어갈 수 있는 키워드
  - `CURRENT ROW`: 현재 행
  - `n PRECEDING`: n행 앞
  - `n FOLLOWING`: n행 뒤
  - `UNBOUNDED PRECEDING`: 앞 행 전부
  - `UNBOUNDED FOLLOWING`: 앞 행 전부



## 순위 함수

### `ROW_NUMBER()`

- 행의 출력되는 위치를 순위로써 제공하는 함수
- 중복되는 순위가 없다.

```sql
SELECT
    id,
    name,
    salary,
    ROW_NUMBER() OVER (ORDER BY salary DESC) AS number
FROM employee;
```

| id | name | salary  | number |
|:---|:-----|:--------|:-------|
| 3  | 기영이  | 5000000 | 1      |
| 1  | 김삿갓  | 3300000 | 2      |
| 4  | 이몽룡  | 3300000 | 3      |
| 2  | 홍길동  | 2100000 | 4      |

### `RANK()`

- 각 행의 순위를 반환하는 함수
  - `ROW_NUMBER`와는 다르게 공동 순위 개념이 있다.

```sql
SELECT
    id,
    name,
    salary,
    RANK() OVER (ORDER BY salary DESC) AS 순위
FROM employee;
```

| id | name | 순위      | 순위 |
|:---|:-----|:--------|:---|
| 3  | 기영이  | 5000000 | 1  |
| 1  | 김삿갓  | 3300000 | 2  |
| 4  | 이몽룡  | 3300000 | 2  |
| 2  | 홍길동  | 2100000 | 4  |

### `DENSE_RANK()`

- 각 행의 순위를 반환하는 함수
  - `RANK()`와 비슷하게 공동 순위가 있지만, 숫자를 건너뛰지 않는다.

```sql
SELECT
    id,
    name,
    salary,
    DENSE_RANK() OVER (ORDER BY salary DESC) AS 순위
FROM employee;
```

| id | name | salary  | 순위 |
|:---|:-----|:--------|:---|
| 3  | 기영이  | 5000000 | 1  |
| 1  | 김삿갓  | 3300000 | 2  |
| 4  | 이몽룡  | 3300000 | 2  |
| 2  | 홍길동  | 2100000 | 4  |

## 행순서 함수

### `FIRST_VALUE()`

- 윈도우 별 처음 나온 값을 반환

```sql
SELECT
    id,
    name,
    level,
    salary,
    FIRST_VALUE(name) OVER (PARTITION BY level ORDER BY salary DESC) AS 최고월급
FROM employee;
```

| id | name | level | salary | 최고월급 |
| :--- | :--- | :--- | :--- |:-----|
| 3 | 기영이 | 부장 | 5000000 | 기영이  |
| 1 | 김삿갓 | 대리 | 3300000 | 기영이  |
| 4 | 이몽룡 | 사원 | 3300000 | 기영이  |
| 2 | 홍길동 | 대리 | 2100000 | 기영이  |

- 하지만, 위 표는 사실상 별 의미가 없어 보인다. 그래서  직급에 따라 최고월급자를 뽑아보기로 했다고 해보자.
  - 그러면 `PARTITION BY` 키워드를 사용하면 된다.

```sql
SELECT
    id,
    name,
    level,
    salary,
    FIRST_VALUE(name) OVER (PARTITION BY level ORDER BY salary DESC) AS 최고월급
FROM employee;
```

| id | name | level | salary | 최고월급 |
| :--- | :--- | :--- | :--- | :--- |
| 1 | 김삿갓 | 대리 | 3300000 | 김삿갓 |
| 2 | 홍길동 | 대리 | 2100000 | 김삿갓 |
| 3 | 기영이 | 부장 | 5000000 | 기영이 |
| 4 | 이몽룡 | 사원 | 3300000 | 이몽룡 |

- 반대로 `ORDER BY`의 정렬을 바꾸면 값이 반환값이 달라지게 된다.

```sql
SELECT
    id,
    name,
    level,
    salary,
    FIRST_VALUE(name) OVER (PARTITION BY level ORDER BY salary) AS 최저월급
FROM employee;
```

| id | name | level | salary | 최저월급 |
| :--- | :--- | :--- | :--- |:-----|
| 2 | 홍길동 | 대리 | 2100000 | 홍길동  |
| 1 | 김삿갓 | 대리 | 3300000 | 홍길동  |
| 3 | 기영이 | 부장 | 5000000 | 기영이  |
| 4 | 이몽룡 | 사원 | 3300000 | 이몽룡  |


### `LAST_VALUE()`

- `FIRST_VALUE`와 반대다. 가장 마지막에 나오는 값을 반환한다.

### `LAG()`

- 파티션 별로 지정한 수만큼 앞에 있는 행을 가져올 수 있다.
- LAG의 인자 3가지
1. 출력할 행
2. 앞 행 개수
3. null일 경우 반환값

```sql
SELECT
    id,
	name,
	LAG(name, 2, '없음') OVER(ORDER BY name) AS 앞이름 -- 2행 앞에 있는 사원 이름을 가져오고, null일 경우 '없음' 반환
FROM employee
```

| id | name | 앞이름 |
| :--- | :--- | :--- |
| 3 | 기영이 | 없음 |
| 1 | 김삿갓 | 없음 |
| 4 | 이몽룡 | 기영이 |
| 2 | 홍길동 | 김삿갓 |


### `LEAD()`

- `LAG` 함수와 반대.
- 파티션 별로 지정한 수만큼 뒤에 있는 행을 가져온다.

```sql
SELECT
    id,
	name,
	LEAD(name, 2, '없음') OVER(ORDER BY name) AS 앞이름 -- 2행 뒤에 있는 사원 이름을 가져오고, null일 경우 '없음' 반환
FROM employee
```

| id | name | 앞이름 |
| :--- | :--- | :--- |
| 3 | 기영이 | 이몽룡 |
| 1 | 김삿갓 | 홍길동 |
| 4 | 이몽룡 | 없음 |
| 2 | 홍길동 | 없음 |


## 비율함수

### `PERCENT_RANK()`

- 파티션 별 윈도우에서 제일 먼저 나오는 것을 0, 제일 늦게 나오는 것을 1로 하여 행의 순서를 기준으로 백분율을 구한다.

```sql
SELECT
    id,
	name,
	salary,
	PERCENT_RANK() OVER(ORDER BY salary) AS 백분율
FROM employee
```

| id | name | salary | 백분율                |
| :--- | :--- | :--- |:-------------------|
| 2 | 홍길동 | 2100000 | 0                  |
| 1 | 김삿갓 | 3300000 | 0.3333333333333333 |
| 4 | 이몽룡 | 3300000 | 0.3333333333333333 |
| 3 | 기영이 | 5000000 | 1                  |


### `CUME_DIST()`

- 현재 행보다 작거나 같은 데이터에 대해 누적 백분율을 구한다.

```sql
SELECT
    id,
	name,
    salary,
	CUME_DIST() OVER(ORDER BY salary) AS 누적백분율
FROM emp
```

| id | name | salary | 누적백분율 |
| :--- | :--- | :--- | :--- |
| 2 | 홍길동 | 2100000 | 0.25 |
| 1 | 김삿갓 | 3300000 | 0.75 |
| 4 | 이몽룡 | 3300000 | 0.75 |
| 3 | 기영이 | 5000000 | 1 |

### `NTILE()`

- 인자로 전달된 개수만큼 그룹으로 나눈다.

```sql
SELECT
    id,
	name,
	salary,
	NTILE(2) OVER(ORDER BY salary) AS 백분위
FROM emp
```

| id | name | salary | 백분위 |
| :--- | :--- | :--- | :--- |
| 2 | 홍길동 | 2100000 | 1 |
| 1 | 김삿갓 | 3300000 | 1 |
| 4 | 이몽룡 | 3300000 | 2 |
| 3 | 기영이 | 5000000 | 2 |

