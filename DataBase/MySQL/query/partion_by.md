# PARTITION BY

- SQL의 분석 함수인 `OVER` 절과 윈도우 함수와 함께 사용되어, 데이터를 특정 기준으로 분할하여 각 그룹별로 집계 함수를 적용하는 기능
- `GROUP BY` 절과 유사하다.

```sql
SELECT
    t.id,
    COUNT(*) OVER(PARTITION BY t.id ORDER BY t.id) AS routes
FROM train AS t
    INNER JOIN journey j ON t.id = j.train_id
    INNER JOIN route r ON j.route_id = r.id 
```

- 모든 행을 그대로 유지하며, 집계 함수를 사용해 구한 값을 모든 행에 넣어준다.

## GROUP BY와 다른 점이 무엇인가?

### GROUP BY

- `GROUP BY` 절은 특정 기준으로 데이터를 정의하고자 할 때 사용한다.
  - 분석을 할 때 분류 기준으로 삼는 것들로 그룹화
- 보통 `COUNT`, `SUM`, `AVG`, `MIN`, `MAX` 등의 집계 함수와 함께 사용된다.

### 둘의 차이점

- `GROUP BY`를 사용하면 데이터를 합쳐 중복되는 기존 데이터는 지우고, 기준이 될 수 있는 행 하나만 남겨둔다.
- 하지만 `PARTITION BY`는 기존 데이터들이 그대로 있다. (중복되는 데이터를 지우지 않는다는 의미) 
  - 그래서 집계 함수를 통해 구한 값이 모든 행마다 부여된다.



