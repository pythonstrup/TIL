# 03 엘라스틱서치 기본

- ElasticSearch를 제대로 사용하기 위해서는 저장과 인출 방식을 확실히 이해해둬야 한다.
  - 인출을 위해서는 가장 먼저 저장이 필요하며, 저장을 위해서는 스키마를 구성하고 데이터를 집어넣어야 한다.
  - 이 과정에서 데이터 타입을 배우고 전문 검색 내부 동작을 이해해야 한다.
- 저장 과정에 있어 핵심인 인덱스와 도큐먼트 개념.

---

## 1. 준비 작업

- 엘라스틱의 모든 기능은 REST API 형태다.

### 1-1. 엘라스틱서치 요청과 응답

- 모든 요청과 응답은 REST API 형태로 제공한다.

### 1-2. 키비나 콘솔

- 키바나 Dev Tools에 있는 콘솔을 사용해 REST API를 호출할 수 있다.

### 1-3. 시스템 상태 확인

- 엘라스틱서치의 현재 상태를 빠르게 확인하기 위해 `cat` API를 사용한다.
  - 여기서 `cat`은 'compact and aligned text'의 약어다.

```
GET _cat
```

- 귀여운 고양이 이모티콘과 함께 cat API가 지원하는 목록을 확인할 수 있다.

```
=^.^=
/_cat/allocation
/_cat/shards
/_cat/shards/{index}
/_cat/master
/_cat/nodes
/_cat/tasks
/_cat/indices
/_cat/indices/{index}
/_cat/segments
/_cat/segments/{index}
/_cat/count
/_cat/count/{index}
/_cat/recovery
/_cat/recovery/{index}
/_cat/health
/_cat/pending_tasks
/_cat/aliases
/_cat/aliases/{alias}
/_cat/thread_pool
/_cat/thread_pool/{thread_pools}
/_cat/plugins
/_cat/fielddata
/_cat/fielddata/{fields}
/_cat/nodeattrs
/_cat/repositories
/_cat/snapshots/{repository}
/_cat/templates
/_cat/component_templates/_cat/ml/anomaly_detectors
/_cat/ml/anomaly_detectors/{job_id}
/_cat/ml/datafeeds
/_cat/ml/datafeeds/{datafeed_id}
/_cat/ml/trained_models
/_cat/ml/trained_models/{model_id}
/_cat/ml/data_frame/analytics
/_cat/ml/data_frame/analytics/{id}
/_cat/transforms
/_cat/transforms/{transform_id}
```

- 노드, 샤드, 템플릿 등의 상태 정보나 통계 정보를 확인할 수 있다.
- 예시를 확인해보자.

```
GET _cat/indices?v
```

- 파라미터 사용
  - `v`: 컬럼의 이름을 확인할 수 있다.
  - `s`: 정렬
  - `h`: 헤더

- 사실 아래와 같이 `curl` 커맨드를 사용해 터미널에서 키바나 콘솔에서 얻은 것과 같은 결과를 얻을 수 있다.

```shell
$ curl -X GET "http://localhost:9200/_cat/indices?v"
```

### 1-4. 샘플 데이터 불러오기

- 키바나 홈페이지에서 `Try sample data`를 통해 샘플 데이터를 사용할 수 있다.

<img src="img/sample01.png">

- 해당 버튼을 누르면 `More ways to add data` 페이지로 진입하는데, 여기서 `Other sample data sets` 토클을 클릭하면 3가지 샘플이 뜬다.

<img src="img/sample02.png">

---

## 2. 인덱스와 도큐먼트

- 엘라스틱서치를 이해하기 위해서는 `인덱스 index`와 `도큐먼트 document`가 무척 중요하다.
- 인덱스는 도큐먼트를 저장하는 논리적 구분이며, 도큐먼트는 실제 데이터를 저장하는 단위다.

<img src="img/index01.png">


### 2-1. 도큐먼트

- 도큐먼트는 엘라스틱서치에서 데이터가 저장되는 기본 단위로 JSON 형태며, 하나의 도큐먼트는 여러 `필드 field`와 `값 value`을 갖는다.
- 아래는 JSON 형태의 도큐먼트 파일의 예시이다.

```json
{
  "name": "mike",
  "age": 25,
  "gender": "male"
}
```

<img src="img/index02.png">

- MySQL과 인덱스를 비교해보자면 아래와 같다.

| MySQL  | ElasticSearch |
|:-------|:--------------|
| Table  | Index         |
| Record | Document      |
| Column | Field         |
| Schema | Mapping       |

- 7.x로 넘어오면서 `타입 Type`이 사라짐.
  - 원래 `타입`이라는 개념이 MySQL의 테이블에 가까웠다.

### 2-2. 인덱스

- 도큐먼트를 저장하는 논리적 단위
  - 관계형 데이터베이스의 테이블과 유사한 개념이다.
  - 하나의 인덱스에 다수의 도큐먼트가 포함되는 구조. 동일한 인덱스에 있는 도큐먼트는 동일한 스키마를 갖는다.
  - 그리고 모든 도큐먼트는 반드시 하나의 인덱스를 포함해야 한다.
- 인덱스 이름에는 영어 소문자를 비롯해 `\`, `/`, `*`, `?`, `"`, `<`, `>`, `|`, `#`, 공백, 쉼표 등을 제외한 특수문자를 사용할 수 있으며 255바이트를 넘을 수 없다.

#### 스키마에 따른 그룹핑

- 일반적으로 스키마에 따라 인덱스를 구분한다.
  - ex) 회원 정보 도큐먼트 & 장바구니 도큐먼트 => 성격이 다르기 때문에 데이터 스키마도 다르다.
  - 서로 다른 스키마를 가진 도큐먼트를 하나의 인덱스에 저장하는 방법은 바람직하지 않다.
- 인덱스의 스키마는 매핑을 통해 정의한다.

#### 관리 목적의 그룹핑

- 기본적으로 인덱스는 용량이나 숫자 제한 없이 무한대의 도큐먼트를 포함할 수 있다.
  - 따라서 이론적으로는 하나의 인덱스에 수억 개의 도큐먼트도 저장될 수 있다.
  - 하지만 그만큼 검색 성능이 나빠진다. => 인덱스 용량 제한을 두게 된다.
- 특정 도큐먼트 개수에 도달하거나 특정 용량을 넘어서면 인덱스를 분리한다.
  - 혹은 일/주/월/년 단위 같은 날짜/시간 단위로 인덱스를 분리하기도 한다.

---

## 3. 도큐먼트 CRUD

### 3-1. 인덱스 생성/확인/삭제

- 생성

```http request
PUT index1
```

```json
{
  "acknowledged": true,
  "shards_acknowledged": true,
  "index": "index1"
}
```

- 확인

```http request
GET index1
```

```json
{
  "index1": {
    "aliases": {},
    "mappings": {},
    "settings": {
      "index": {
        "routing": {
          "allocation": {
            "include": {
              "_tier_preference": "data_content"
            }
          }
        },
        "number_of_shards": "1",
        "provided_name": "index1",
        "creation_date": "1744893518790",
        "number_of_replicas": "1",
        "uuid": "d7Dp2HTxTmmQZRksDA1vig",
        "version": {
          "created": "8505000"
        }
      }
    }
  }
}
```

- 삭제

```http request
DELETE index1
```

```json
{
  "acknowledged": true
}
```

### 3-2. 도큐먼트 생성

- 엘라스틱서치에서 도큐먼트를 인덱스에 포함시키는 것을 `인덱싱 indexing`이라고 한다.
- 아래와 같이 명령어를 사용하는 것을 "도큐먼트를 인덱싱한다"라고 말한다.

```http request
PUT index2/_doc/1
{
  "name": "mike",
  "age": 25,
  "gender": "male"
}
```

```json
{
  "_index": "index2",
  "_id": "1",
  "_version": 1,
  "result": "created",
  "_shards": {
    "total": 2,
    "successful": 1,
    "failed": 0
  },
  "_seq_no": 0,
  "_primary_term": 1
}
```

- 존재하지 않았던 `index2`라는 인덱스를 생성하면서 동시에 `index2` 인덱스에 도큐먼트를 인덱싱한다.
  - `index2`: 인덱스 이름
  - `_doc`: 엔드포인트 구분을 위한 예약어
  - `1`: 인덱싱할 도큐먼트의 고유 아이디.
- 이제 인덱스를 확인해보자. 이 중에서도 `mappings`에 집중!

```http request
GET index2
```

```json
{
  "index2": {
    "aliases": {},
    "mappings": {
      "properties": {
        "age": {
          "type": "long"
        },
        "gender": {
          "type": "text",
          "fields": {
            "keyword": {
              "type": "keyword",
              "ignore_above": 256
            }
          }
        },
        "name": {
          "type": "text",
          "fields": {
            "keyword": {
              "type": "keyword",
              "ignore_above": 256
            }
          }
        }
      }
    },
    ...
  }
}
```

- 데이터 타입을 지정하지 않아도 엘라스틱서치는 도큐먼트의 필드와 값을 보고 자동으로 지정하는데, 이런 기능을 `다이내믹 매핑 dynamic mapping`이라고 한다.
- 아래와 같이 `country`라는 새로운 이름의 필드가 추가된 도큐먼트를 인덱싱하면, 문제 없이 추가된다.

```http request
PUT index2/_doc/2
{
  "name": "jane",
  "country": "france"
}
```

- 만약 데이터 타입을 잘못 입력한 도큐먼트를 인덱싱하면 어떻게 될까?

```http request
PUT index2/_doc/3
{
  "name": "kim",
  "age": "20",
  "gender": "female"
}
```

- `age`의 타입이 이미 `"type": "long"`와 같이 설정되어 있기 때문에 3번의 도큐먼트는 인덱싱 과정에서 강제로 숫자 타입으로 변경한다.

### 3-3. 도큐먼트 읽기

- 단일 조회

```http request
GET index2/_doc/1
```

- 인덱스 내의 도큐먼트 검색

```http request
GET index2/_search
```

### 3-4. 도큐먼트 수정

- 1번 도큐먼트의 특정 필드값 변경

```http request
PUT /index2/_doc/1
{
  "name": "park",
  "age": 45,
  "gender": "male"
}
```

- 아래와 같이 업데이트가 잘 되었음을 확인할 수 있다.

```json
{
  "_index": "index2",
  "_id": "1",
  "_version": 2,
  "result": "updated",
  "_shards": {
    "total": 2,
    "successful": 1,
    "failed": 0
  },
  "_seq_no": 3,
  "_primary_term": 1
}
```

- update API를 이용해 특정 도큐먼트의 값을 업데이트할 수도 있다.
  - 아래는 1번 도큐먼트의 `name`을 `lee`로 변경

```http request
POST index2/_update/1
{
  "doc": {
    "name": "lee"
  }
}
```

- 엘라스틱서치 도큐먼트 수정 작업은 비용이 많이 들기 때문에 권장하지 않는다.
  - 특히 엘라스틱서치를 로그 수집 용도로 사용한다면 개별 도큐먼트는 수정할 일은 거의 없다.
  - 개별 도큐먼트 수정이 많은 작업이라면 엘라스틱서치가 아닌 다른 데이터베이스를 사용하는 것이 좋다.

### 3-5. 도큐먼트 삭제

- 특정 도큐먼트를 삭제하기 위해서는 인덱스명과 도큐먼트 아이디를 알고 있어야 한다.

```http request
DELETE index2/_doc/2
```

- `index2` 인덱스에서 도큐먼트 아이디가 `2`인 도큐먼트가 삭제된다.
- 도큐먼트 수정과 마찬가지로 개별 도큐먼트 삭제 또한 비용이 많이 들어가는 작업인 만큼 사용 시에 주의하자.

---

## 4. 응답 메시지

- 기본적으로 REST API를 사용하므로 HTTP 상태 코드의 각 의미는 엘라스틱서치에서도 동일한 의미로 사용된다.
- 존재하지 않는 인덱스를 조회하려고 하면 `404` 에러가 발생할 것이다.

---

## 5. 벌크 데이터

- `_bulk` API 포맷을 사용해 한 번에 여러 개의 값을 넣을 수 있다.

```http request
POST _bulk
{"index": {"_index": "test", "_id": "1"}}
{"field1": "value1"}
{"create": {"_index": "test", "_id": "3"}}
{"field1": "value3"}
{"update": {"_id": "1", "_index": "test"}}
{"doc": {"field2": "value2"}}
{"delete": {"_index": "test", "_id": "2"}}
```

- `delete`는 한 줄, `insert` & `create` & `update`는 각각 두 줄로 작성된다.
  - 각 줄 사이에는 쉼표 등 별도의 구분자가 없고 라인 사이 공백을 허용하지 않는다.
- 벌크 데이터는 파일 형태로 만들어 적용할 수도 있다.

---

## 6. 매핑

- 엘리스틱서치도 관계형 데이터베이스의 스키마와 비슷한 역할을 하는 것이 있는데 바로 `매핑 mapping`이다.
  - 쉽게 말하면 JSON 형태의 데이터를 루씬이 이해할 수 있도록 바꿔주는 작업이다.
- 엘라스틱서치가 검색 엔진으로 전문 검색과 대용량 데이터를 빠르게 실시간 검색할 수 있는 이유는 매핑이 있기 때문인데 매핑을 엘라스틱서치가 자동으로 하면 다이내믹 매핑, 사용자가 직접 설정하면 명시적 매핑이다.
- 특히 엘라스틱서치에서는 문자열을 `text`와 `keyword` 타입으로 나눌 수 있는데, 전문 검색을 활용하려면 반드시 두 가지 타입을 이해하고 있어야 한다.

### 6-1. 다이내믹 매핑

- 유연한 활용을 위해 매핑 정의를 강제하지 않는다.
  - 특별히 데이터 타입이나 스키마에 고민하지 않아도 JSON 도큐먼트의 데이터 타입에 맞춰 엘라스틱서치가 자동으로 인덱스 매핑을 해준다.

| 원본 소스 데이터 타입 | 다이내믹 매핑으로 변환된 데이터 타입                            |
|:-------------|:------------------------------------------------|
| `null`       | 필드를 추가하지 않는다.                                   |
| `boolean`    | ``boolean`                                      |
| `float`      | `float`                                         |
| `integer`    | `integer`                                       |
| `object`     | `object`                                        |
| `string`      | `string` 데이터 형태에 따라 `date`, `text`/`keyword` 필드 |

- 각 매핑은 아래 API로 확인할 수 있다.

```http request
GET {index}/_mapping
```

### 6-2. 명시적 매핑

- 인덱스 매핑을 직접 정의하는 것을 `명시적 매핑 explicit mapping`이라고 한다.
  - 인덱스를 생성할 때 `mappings` 정의를 설정하거나 mapping API를 이용해 매핑을 지정할 수 있다.
- 아래는 인덱스 생성 시 매핑 설정 방법이다.

```http request
PUT {index}
{
  "mappings": {
    "properties": {
      "필드명": "필드타입"
      ...
    }
  }
}
```

- 아래는 예제

```http request
PUT index3
{
  "mappings": {
    "properties": {
      "age": {"type": "short"},
      "name": {"type": "text"},
      "gender": {"type": "keyword"}
    }
  }
}
```

### 6-3. 매핑 타입

| 데이터 형태 | 데이터 타입                                                                                           | 설명                                                                                                                                                                                           |
|:-------|:-------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 텍스트    | `text`                                                                                           | 전문 검색이 필요한 데이터. 텍스트 분석기가 텍스트를 작은 단위로 분리                                                                                                                                                      |
|        | `keyword`                                                                                        | 정렬이나 집계에 사용되는 텍스트 데이터. 분석 X. 통째로 인덱싱.                                                                                                                                                        |
| 날짜     | `date`                                                                                           | 날짜/시간 데이터                                                                                                                                                                                    |
| 정수     | `byte`, `short`, `integer`, `long`                                                               | 각 값은 부호가 존재.<br/>- `byte`: 8비트 데이터<br/>- `short`: 16비트 데이터<br/>- `integer` 32비트 데이터<br/>- `long`: 64비트 데이터                                                                                   |
| 실수     | `scaled_float`, `half_float`, <br/>`double`, `float`                                             | - `scaled_float`: float 데이터에 특정 값을 곱해서 정수형으로 바꾼 데이터. 정확도는 떨어지나 필요에 따라 집계 등에서 효율적으로 사용 가능.<br/>- `half_float`: 16비트 부동소수점 실수 데이터<br/>- `float`: 32비트 부동소수점 데이터<br/>- `double`: 64비트 부동소수점 데이터 |
| 불리언    | `boolean`                                                                                        | `true`/`false`                                                                                                                                                                               |
| IP 주소  | `ip`                                                                                             | `ipv4`, `ipv6` 타입의 IP 주소                                                                                                                                                                     |
| 위치 정보  | `geo-point`, `geo-shape`                                                                         | - `geo-point`: 위도, 경도 값을 갖는다.<br/>- `geo-shape`: 하나의 위치 포인트가 아닌 임의의 지형                                                                                                                       |
| 범위 값   | `integer_range`, `long_range`, <br/>`float_range`, `double_range`, <br/>`ip_range`, `date_range` | 범위를 정할 수 있는 데이터. 최솟값과 최댓값을 통해 범위를 입력한다.                                                                                                                                                      |
| 객체형    | `object`                                                                                         | 계층 구조를 갖는 형태로 필드 안에 다른 필드들이 들어갈 수 있다. (자바스크립트 처럼 `object.name`과 같은 형식으로 객체에 접근 가능)                                                                                                           |
| 배열형    | `nested`                                                                                         | 배열형 객체를 저장. 객체를 따로 인덱싱하여 객체가 하나로 합쳐지는 것을 막고, 배열 내부의 객체에 쿼리로 접근할 수 있다.ㄴ                                                                                                                       |
|        | `join`                                                                                            | 부모/자식 관계 표현                                                                                                                                                                                  |

### 6-4. 멀티 필드를 활용한 문자열 처리

#### 6-4-1. `텍스트 text` 타입

- `분석기 analyzer`에 의해 `토큰 token`으로 분리되고, 이렇게 분리된 토큰들은 인덱싱되는데 이를 `역인덱싱 inverted indexing`이라 한다.
  - 이때 역인덱스에 저장된 토큰들을 `용어 term`라고 한다.
- 불필요한 토큰은 걸러내고 대소문자를 통일하는 등 가공 과정을 거쳐 용어가 된다.
  - 이런 용어들은 역인덱스에 저장되어 전문 검색을 할 수 있게 한다.
- `LIKE` 검색은 인덱싱이 되지 않아 엘라스틱서치처럼 많은 문서를 처리하기엔 무리가 있다.
- DSL 쿼리를 통한 검색

```http request
GET text_index/_search
{
  "query": {
    "match": {
      "contents": "day"
    }
  }
}
```

- 텍스트 타입으로 지정된 필드는 정렬할 경우 문장의 첫 문자열이 아닌 분해된 용어를 기준으로 정렬을 수행하므로 예상과는 다른 결과를 얻게 된다.
  - 전문 검색이 아닌 집계나 정렬은 키워드 타입을 사용해야 한다.

#### 6-4-2. `키워드 keyword` 타입

- 카테고리나 사람 이름, 브랜드 등 규칙성이 있거나 유의미한 값들의 집합, 즉 범주형 데이터에 주로 사용된다.
  - 키워드 타입은 텍스트 타입과 다르게 분석기를 거치지 않고 문자열 전체가 하나의 용어로 인덱싱된다.
- 키워드는 용어로 분리되지 않기 때문에 전문 검색처럼 단어로 찾으려고 하면 검색되지 않는다.
  - 따라서 키워드를 찾을 때는 키워드 전부를 입력해줘야 한다.

#### 6-4-3. 멀티 필드

- 문자열의 경우 전문 검색이 필요하면서 정렬도 필요한 경우가 있다.
- 아래는 멀티 필드를 갖는 인덱스를 생성한 예시다.

```http request
PUT multifield_index
{
  "mappings": {
    "properties": {
      "message": { "type": "text" },
      "contents": {
        "type": "text",
        "fields": {
          "keyword": { "type": "keyword" }
        }
      }
    }
  }
}
```

- `contents` 필드는 텍스트 타입이면서 키워드 타입을 갖는다.
- 만약 `contents` 하위에서만 검색하고 싶다면 아래와 같이 접근해볼 수 있다.

```http request
GET multifield_index/_search
{
  "query": {
    "term": {
      "contents.keyword": "day"
    }
  }
}
```

- 아래는 집계 쿼리다.

```http request
GET multifield_index/_search
{
  "size": 0,
  "aggs": {
    "contents": {
      "terms": {
        "field": "contents.keyword"
      }
    }
  }
}
```

---

## 7. 인덱스 템플릿

- 주로 설정이 동일한 복수의 인덱스를 만들 때 사용한다.
  - 관리 편의성, 성능 등을 위해 인덱스를 파티셔닝하는 일이 많은데 이때 파티셔닝되는 인덱스들은 설정이 같아야 한다.

### 7-1. 템플릿 확인

- 등록된 템플릿 확인하기

```http request
GET _index_template
```

- 특정 인덱스 템플릿만 확인하기

```http request
GET _index_template/{template_name}
```

### 7-2. 템플릿 설정

- 일반적으로 매핑과 세팅 설정을 가장 많이 한다.

#### 7-2-1. 템플릿 생성

```http request
PUT _index_template/test_template
{
  "index_patterns": ["test_*"],
  "priority": 1,
  "template": {
    "settings": {
      "number_of_shards": 3,
      "number_of_replicas": 1
    },
    "mappings": {
      "properties": {
        "name": {"type": "text"},
        "age": {"type": "short"},
        "gender": {"type": "keyword"}
      }
    }
  }
}
```

- 결과값

```json
{
  "acknowledged": true
}
```

- `index_patterns`: 새로 만들어지는 인덱스 중에 인덱스 이름이 인덱스 패턴과 매칭되는 경우 이 템플릿이 적용된다. 여기서는 `test_`로 시작되는 이름을 가진 인덱스들은 모두 `test_template`에 있는 매핑 세팅 등이 적용된다.
- `priority`: 인덱스 생성 시 이름에 매칭되는 템플릿이 둘 이상일 때 템플릿이 적용되는 우선순위를 정할 수 있다. 숫자가 가장 높은 템플릿이 먼저 적용된다.
- `template`: 새로 생성되는 인덱스에 적용되는 `settings`, `mappings` 같은 인덱스 설정을 정의한다.

#### 7-2-2. 템플릿 적용

- 템플릿을 적용할 때 기억해야 하는 것! 템플릿을 만들기 전에 이미 존재하던 인덱스는 비록 템플릿 패턴과 일치하더라도 템플릿이 적용되지 않는다.
  - 즉 `test_template`을 만들기 전부터 존재했던 인덱스들은 템플릿의 영향을 받지 않는다.
  - 오직 템플릿을 만든 이후에 새로 만들어지는 인덱스들만 템플릿의 영향을 받는다.
- 인덱스를 만들면 다이나믹 매핑과 다르게, 템플릿에서 지정한 매핑값이 적용된 것을 확인할 수 있다.

```http request
PUT test_index1/_doc/1
{
  "name": "kim",
  "age": 10,
  "gender": "male"
}
```

```json
{
  "_index": "test_index1",
  "_id": "1",
  "_version": 1,
  "result": "created",
  "_shards": {
    "total": 2,
    "successful": 1,
    "failed": 0
  },
  "_seq_no": 0,
  "_primary_term": 1
}
```

- 여기서 인덱스 만들 때, 인덱스 이름이 `test_`로 시작하지 않으면 템플릿이 적용되지 않는다.
- 그런데 만약 설정 값과 다른 타입의 도큐먼트를 입력하면 어떻게 될까?

```http request
PUT test_index2/_doc/1
{
  "name": "lee",
  "age": "19 years",
}
```

- 400 에러가 발생한다.

```json
{
  "error": {
    "root_cause": [
      {
        "type": "document_parsing_exception",
        "reason": "[3:10] failed to parse field [age] of type [short] in document with id '1'. Preview of field's value: '19 years'"
      }
    ],
    "type": "document_parsing_exception",
    "reason": "[3:10] failed to parse field [age] of type [short] in document with id '1'. Preview of field's value: '19 years'",
    "caused_by": {
      "type": "number_format_exception",
      "reason": "For input string: \"19 years\""
    }
  },
  "status": 400
}
```

#### 7-2-3. 템플릿 삭제

- 템플릿을 지워도 기존 인덱스들은 영향을 받지 않는다. 단순히 템플릿이 지워지는 것 뿐이다.

```http request
DELETE _index_template/test_template
```

```json
{
  "acknowledged": true
}
```

### 7-3. 템플릿 우선순위

- 위에서 설명했다시피 숫자가 클수록 우선순위가 높다.
- 7.8 이전 버전의 레거시 템플릿에서는 우선수위가 낮은 템플릿부터 차례로 병합되었으나, 새로운 인덱스 템플릿은 우선순위가 높은 템플릿으로 덮어쓰기 된다는 점을 기억하자.

### 7-4. 다이내믹 템플릿

- 왜 필요할까?
- 로그 시스템이나 비정형화된 데이터를 인덱싱하는 경우를 생각해보자.
  - 로그 시스템의 구조를 알지 못하기 때문에 필드 타입을 정확히 정의하기 힘들고, 필드 개수를 정할 수 없는 경우도 있다.
  - 다이내믹 템플릿은 이처럼 매핑을 정확하게 정할 수 없거나 대략적인 데이터 구조만 알고 있을 때 사용할 수 있는 방법이다.
  - 인덱스를 만들 때 `dynamic_templates`를 추가하면 된다.

```http request
PUT dynamic_index1
{
  "mappings": {
    "dynamic_templates": [ 
        {
         "my_string_fields": {
            "match_mapping_type": "string",
            "mapping": {"type": "keyword"}
        } 
      }
    ]
  }
}
```

```json
{
  "acknowledged": true,
  "shards_acknowledged": true,
  "index": "dynamic_index1"
}
```

- 여기서 `my_string_fileds`는 임의로 정의한 다이내믹 템플릿의 이름이다. 이 이름 밑으로 2개의 설정이 있다.
1. `match_mapping_type` 조건문 혹은 매핑 트리거다. 조건에 만족할 경우 트리거링이 된다.
2. `mapping`은 트리거링이 된 경우 실제 적용될 매핑이다.


- `match`와 `unmatch`

```http request
PUT dynamic_index2
{
  "mappings": {
    "dynamic_templates": [ 
        {
         "my_string_fields": {
            "match": "long_*",
            "unmatch": "*_text",
            "mapping": {"type": "long"}
        } 
      }
    ]
  }
}
```

- `match`: 정규표현식을 이용해 필드명 검사. 조건에 맞으면 `mapping`에 의해 필드들은 모두 long 타입을 갖는다.
- `unmatch`: 이 조건에 맞는 경우, `mapping`에서 제외한다.
- 도큐먼트를 입력해보자.

```http request
PUT dynamic_index2/_doc/1
{
  "long_num": "5",
  "long_text": "170"
}
```

- `long_num`은 `long_*`에 매칭하므로 long 타입으로 매핑된다.
- `long_text`는 match와 unmatch 둘 다 부합하여 다이내믹 템플릿에 제외되어 매핑에 의해 텍스트/키워드를 갖는 멀티 필드 타입이 된다.

---

# 참고 자료

- 엘라스틱 스택 개발부터 운영까지, 김준영 & 정상운 지음, 박재호 감수, 펴낸곳: 책만
