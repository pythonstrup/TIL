# 04 검색

- 실제 데이터를 인출하기 위한 쿼리.
- 대량의 데이터를 대상으로 빠르고 정확한 검색이 가능하게 만들어 줌.

## 1. 쿼리 컨텍스트와 필터 컨텍스트

- `쿼리 컨텍스트 Query Context`: 질의에 대한 유사도를 계산해 이를 기준으로 더 정확한 결과를 먼저 보여준다.
- `필터 컨텍스트 Filter Context`: 유사도를 계산하지 않고 일치 여부에 따른 결과만을 반환한다.
- 과거 1.x 버전에서 `용어 검색 term query`과 `용어 필터 term filter`처럼 쿼리 컨텍스트와 필터 컨텍스트가 명확히 구분되었으나, 논리 쿼리가 나오면서 필터 컨텍스트는 모두 논리 쿼리에 포함되게 되었다.
  - 필터 컨텍스트 단독 사용보다는 쿼리/필터 컨텍스트를 조합해 사용하는 방향으로 가는 추세

----

## 2. 쿼리 스트링과 쿼리 DSL

- 쿼리 스트링은 한 줄 정도의 간단한 쿼리에 사용.
- 쿼리 DSL은 복잡한 쿼리를 작성할 때 사용. 엘라스틱서치에서 제공하는 쿼리 전용 언어. JSON 기반으로 직관적이다.

### 2-1. 쿼리 스트링

- `/_search?q={검색어}` 형태로 사용.
- REST API의 쿼리 스트링과 같다고 보면 된다.

### 2-2. 쿼리 DSL

- REST API의 RequestBody에 JSON 형태로 쿼리를 작성하는 방식.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "query": {
    "match": {
      "customer_full_name": "Mary"
    }
  }
}
```

- 자동 완성도 지원되어 타이핑 실수도 줄일 수 있다.

----

## 3. 유사도 스코어

- 기본으로 BM25 알고리즘 사용
- 유사도 스코어는 질의문과 도큐먼트의 유사도를 표현하는 값.
  - 스코어가 높을수록 찾고자 하는 도큐먼트에 가깝다는 사실.
- 검색 시, RequestBody JSON에 `explain: true` 필드를 추가하여 쿼리 내부적인 최적화 방법과 어떤 경로를 통해 검색되었으며, 어떤 기준으로 스코어가 계산되었는지 확인 가능.

### 3-1. 스코어 알고리즘(BM25)

- 5.x 이전 버전에서는 TF-IDF 알고리즘을 사용했으나, 5.x부터 BM25이 기본 알고리즘으로 변경됨.
- BM25는 검색, 추천에 많이 사용되며, `TF(Term Frequency)`, `IDF(Inverse Document Frequency)` 개념에 문서 길이를 고려한 알고리즘.
- 검색어가 문서에서 얼마나 자주 나타나는지, 얼마나 중요한 용어인지.

### 3-2. IDF 계산

- `문서 빈도 Document frequency`는 특정 용어가 얼마나 자주 등작 했는지에 대한 지표. => 일반적으로 자주 등장하는 용어는 중요하지 않을 확률이 높다.
  - ex) 'to', 'the' 등
- 그래서 도큐먼트 내에서 발생 빈도가 적을수록 가중치를 높게 주는데 이를 `문서 빈도의 연수 Inverse Document Frequency, IDF`라고 한다.

```
"idf, computed as log(1 + (N - n + 0.5) / (n + 0.5)) from:",
```

- 요소의 의미
  - `n`은 용어가 몇 개의 도큐먼트에 있는지.
  - `N`은 인덱스의 전체 도큐먼트 수.

### 3-3. TF 계산

- 특정 용어가 도큐먼트에서 많이 반복되었다면 그 용어는 도큐먼트의 주제와 연관되어 있을 가능성이 높다.
- 하나의 도큐먼트에서 특정 용어가 많이 나오면 중요한 용어로 인식하고 가중치를 높인다.

```
"tf, computed as freq / (freq + k1 * (1 - b + b * dl / avgdl)) from:"
```

- 요소의 의미
  - `freq`: 도큐먼트에서 용어가 나온 횟수
  - `k1`, `b`: 알고리즘을 정규화하기 위한 가중치. 엘라스틱서치가 디폴트로 취하는 상수.
  - `dl`: 필드 길이
  - `avgdl`: 전체 도큐먼트에서 평균 필드 길이.
- `dl`이 작고 `avgdl`이 클수록 TF 값이 크게 나온다.

- 최종 스코어 계산식은 아래와 같다.

```
"description": "score(freq=1.0), computed as boost * idf * tf from:",
"details": [
  {
    "value": 2.2,
    "description": "boost",
    "details": []
  },
  ...
]
```

- 최종 스코어는 `IDF`와 `TF` 그리고 `boost` 변수를 곱하면 된다.
  - `boost`는 엘라스틱이 지정한 고정값으로 2.2로 정해져 있다.

---

## 4. 쿼리

- `리프 쿼리 leaf query`: 특정 필드에서 용어를 찾는 쿼리. `match`, `term`, `range`
- `복합 쿼리 compound query`: 쿼리를 조합해 사용. `논리 쿼리 bool query`

### 4-1. 전문 쿼리와 용어 수준 쿼리

- `전문 쿼리`는 전문 검색에 사용. 전문 검색을 할 필드는 인덱스 매핑 시 텍스트 타입으로 매핑해야 한다. 
  - 도큐먼트 필드: 텍스트 타입으로 매핑된 문자열 -> 분석기에 의해 토큰화
  - 검색: 검색어도 분석기에 의해 토큰으로 분리 -> 토큰화된 도큐먼트 용어들이 매칭. 스코어 계산.
- `용어 수준 쿼리`는 정확한 용어를 찾기 위해 사용. 인덱스 매핑 시 필드를 키워드 타입으로 매핑해야 한다.
  - 도큐먼트 필드: 키워드 타입으로 매핑된 문자열(분석기 사용 X. 키워드 그대로 사용)
  - 검색: 그대로 검색. 만약 `Tech`를 `tech`로 검색하면 대소문자 차이로 인해 실패한다.

### 4-2. 매치 쿼리

- 대표적인 전문 쿼리
- 전체 텍스트 중에서 특정 용어나 용어들을 검색할 때 사용

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_full_name"],
    "query": {
        "match": {
            "customer_full_name": "Mary"
        }
    }
}
```

- 여기서 `_source` 파라미터는 `customer_full_name` 필드만 보여달라는 요청.
  - 파라미터를 넣지 않으면 모든 필드를 보여준다.
- 전문 쿼리는 토큰화되기 때문에 검색어 `Mary` => [mary]로 토큰화. 대문자를 소문자로 변경.
- 매치 쿼리는 용어들간의 공백을 `OR`로 인식한다.
- 그런데 여기서 `operator`라는 파라미터를 통해 조건을 변경할 수 있다.

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_full_name"],
    "query": {
        "match": {
            "customer_full_name": {
                "query": "Mary bailey",
                "operator": "and"
            }
        }
    }
}
```

### 4-3. 매치 프레이즈 쿼리

- `구 phrase`를 검색할 때 사용한다.
  - `구`는 동사가 아닌 2개 이상의 단어가 연결되어 만들어지는 단어.
- 단어 순서도 중요하다. ex. 빨간색 바지(O) <-> 바지 빨간색(X)

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_full_name"],
    "query": {
        "match_phrase": {
            "customer_full_name": "mary bailey"
        }
    }
}
```

- 토큰화되는 것가지는 같지만, 검색어에 사용된 용어들이 모두 포함되면서 순서까지 맞아야 한다. (더 깐깐한 `and` 조건)

### 4-4. 용어 쿼리

- 대표적인 용어 수준 쿼리
- 토큰화하지 않기 때문에 정확한 용어가 있는 경우만 매칭된다.

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_full_name"],
    "query": {
        "term": {
            "customer_full_name": "Mary Bailey"
        }
    }
}
```

- 정확히 "Mary Bailey"인 것들만 찾는다.
- 현업에서는 용도를 명확히 하는 것이 좋다.
  - 키워드 타입으로 매핑된 필드를 대상으로 주로 키워드 검색이나 범주형 데이터를 검색하는 용도로 사용하자.

### 4-5. 용어들 쿼리

- 용어 수준 쿼리의 일종.

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["day_of_week"],
    "query": {
        "terms": {
            "day_of_week": ["Monday", "Sunday"]
        }
    }
}
```

- `OR` 조건으로 검색하게 된다.

### 4-6. 멀티 매치 쿼리

- 정확히 어떤 필드가 있는지 모르는 경우 사용.

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_first_name", "customer_last_name", "customer_full_name"],
    "query": {
        "multi_match": {
            "query": "mary",
            "fields": ["customer_first_name", "customer_last_name", "customer_full_name"]
        }
    }
}
```

- 3개의 필드에서 개별 스코어를 구한 다음에 그중 가장 큰 값을 대표 스코어로 구한다.
  - 사용자가 결정할 수도 있음. but, 기본적으로 가장 큰 스코어를 대표 스코어로 한다.
- 아래와 같이 와일드카드를 사용해 유사한 복수의 필드를 선택 가능

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_first_name", "customer_last_name", "customer_full_name"],
    "query": {
        "multi_match": {
            "query": "mary",
            "fields": "customer_*_name"
        }
    }
}
```

#### 4-6-1. 필드에 가중치 두기

- 이를 `부스팅 boosting` 기법이라고 한다. (멀티 쿼리에서 자주 사용)

```http request
GET kibana_sample_data_ecommerce/_search
{
    "_source": ["customer_first_name", "customer_last_name", "customer_full_name"],
    "query": {
        "multi_match": {
            "query": "mary",
            "fields": [
                "customer_first_name^2", 
                "customer_last_name", 
                "customer_full_name"
            ]
        }
    }
}
```

- `^2`를 통해 2배의 효과를 준다.

### 4-7. 범위 쿼리

- 날짜나 숫자의 범위

```http request
GET kibana_sample_data_ecommerce/_search
{
    "query": {
        "range": {
            "timestamp": {
                "gte": "2025-04-20",
                "lt": "2025-05-21"
        }
    }
}
```

- 날짜와 시간 포맷은 무조건 맞추자. 안 그러면 에러가 발생한다.
  - timestamp 타입은 형식이 'yyyy-mm-dd'인데, range에 '2025/05/21'과 같은 형식으로 넣으면 에러 발생
- 4가지 파라미터 지원
  - `gte`
  - `gt`
  - `lte`
  - `lt`

#### 4-7-1. 날짜/시간 데이터 타입

- 일주일 전, 하루 전 도큐먼트들을 골라내고 싶은 경우

```http request
GET kibana_sample_data_ecommerce/_search
{
    "query": {
        "range": {
            "timestamp": {
                "gte": "now-1M",
        }
    }
}
```

- 위는 현재 시각 기준으로 한 달 전까지의 모든 데이터를 가져온다.
- 표현식 예시
  - `now+1d`: 현재 시각 + 1일
  - `now+1h+30m+10s`: 현재 시각 + 1시 + 30분 + 10초

#### 4-7-2. 범위 데이터 타입

- 숫자 범위. ex) `integer_range` 등
- 날짜 범위. ex) `date_range`
- ip 범위. ex) `ip_range`
- 이 범위값은 `relation`이라는 파라미터로 검색하게 됨
  - `intersects`(기본값): 일부라도 포함하기만 하면 된다.
  - `contains`: 도큐먼트 범위 데이터가 범위 값을 모두 포함.
  - `within`: 도큐먼트 범위 데이터가 범위 값 내에 전부 속해야 함.

### 4-8. 논리 쿼리

- 논리 쿼리는 복합 쿼리다.
- `query.bool`로 호출하며, 4가지 타입으로 가능
  - `must`: 쿼리 컨텍스트. 반드시 일치.
  - `must_not`: 필터 컨텍스트. 일치하면 결과에서 제외.
  - `should`: 쿼리 컨텍스트. 하나의 쿼리만 사용하면 `must`와 동일하게 동작. but, 여러 개를 하면 `OR` 조건.
  - `filter`: 필터 컨텍스트. `must`와 같은 동작을 하지만 필터 컨텍스트로 동작하기 때문에 유사도 스코어에 영향을 미치지 않는다. (예/아니요 두 가지 결과만 제공)

### 4-9. 패턴 검색

- `와일드카드 쿼리 wildcard query`
- `정규식 쿼리 regexp query`

#### 4-9-1. 와일드카드 쿼리

- `*` & `?`를 사용한 패턴 검색
- `*`는 공백까지 포함하여 글자 수에 상관없이 모든 문자를 매칭
  - `aabbbcd`와 `aa*cd`는 매칭
- `?`는 오직 한 문자만 매칭.
  - `aabbbcd`와 `aa???cd`는 매칭
- 맨 앞에 사용하면 속도가 매우 느려짐.

#### 4-9-2. 정규식 쿼리

- 정규식을 사용.

----

## 정리

- 쿼리 컨텍스트와 필터 컨텍스트의 차이를 잘 알아야 함.
- 둘을 잘 조합해야 검색 효율을 높일 수 있다. 쿼리 속도를 높이고 싶다면 필터 컨텍스트 사용.
- BM25 알고리즘의 원리를 알면 유사도 스코어링 품질을 높이는 아이디어를 얻을 수 있다.

----

# 참고 자료

- 엘라스틱 스택 개발부터 운영까지, 김준영 & 정상운 지음, 박재호 감수, 펴낸곳: 책만
