# Elasticsearch API

## 문서 색인

- 문서를 색인하기 위해 엘라스틱에서 제공하는 API를 활용할 수 있다.

### Elasticsearch Built-In API

- 인덱스 관리 API: 인덱스 관리
- 문서 관리 API: 문서의 추가/수정/삭제
- 검색 API: 문서 조회
- 집계 API: 문서 통계

### 다시 한 번 인덱스?

- 문서를 색인하려면 기본적으로 인덱스를 생성해야 한다. 인덱스를 통해 입력되는 문서의 필드를 정의하고 각 필드에 알맞은 데이터 타입을 지정할 수 있다. 이러한 과정을 통해 좀 더 효율적인 색인이 가능해진다.

> ### Index vs Indexing vs Indices <br/>
>
> - index: 색인 데이터
> - indexing: 색인하는 과정
> - indices: 매핑 정보를 저장하는 논리적인 데이터 공간

- 색인은 `index`, 매핑 정의 공간을 `indices`라고 표현한다.

### 스키마리스 Schemaless

- 엘라스틱서치는 스키마리스라는 기능을 제공한다. 위에서 설명했다시피 문서를 색인하려면 기본적으로 인덱스를 생성하는 과정이 필요한데 인덱스를 생성하는 과정 없이 문서를 추가하더라도 문서가 색인되도록 지원하는 일종의 편의 기능이다.
- 최초 문서가 색인될 때 인덱스의 존재 여부를 확인하고 만약 인덱스가 존재하지 않는다면 문서를 분석해서 문서가 색인될 수 있게 인덱스를 자동으로 생성한다.
- 하지만 검색 결과에서 주요 정보가 누락되는 등의 문제가 발생할 수 있기 때문에 가급적이면 사용하지 않는 것이 좋다고 한다.

<br/>

## 인덱스 관리 API

### 인덱스 생성

- 인덱스를 생성할 때는 매핑이라는 세부 설정을 이용할 수 있다. 매핑은 문서와 문서에 포함된 필드, 필드 타입 등을 세세하게 지정하는 것이 가능한 설정 방식이다. 인덱스 생성 시 이러한 매핑 정보를 추가할 수 있다.
- 대신 한 번 생성된 매핑 정보는 변경이 불가능하다는 것을 알고 있어야 한다. 변경해야할 경우, 데이터를 삭제하고 다시 색인할 수밖에 없다.
- 인덱스 생성은 아래와 같이 할 수 있다. (8.7에서는 "\_doc"을 사용하지 않는다.)

```json
PUT /movie
{
  "settings": {
    "number_of_shards": 3,
    "number_of_replicas": 2
  },
  "mappings": {
    "properties": {
      "movieCd": {"type": "integer"},
      "movieNm": {"type": "text"},
      "movieNmEn": {"type": "text"},
      "prdtYear": {"type": "integer"},
      "openDt": {"type": "date"},
      "typeNm": {"type": "keyword"},
      "prdtStatNm": {"type": "keyword"},
      "nationAlt": {"type": "keyword"},
      "genreAlt": {"type": "keyword"},
      "repNationNm": {"type": "keyword"},
      "repGenreNm": {"type": "keyword"}
    }
  }
}
```

- 결과값

```json
{
  "acknowledged": true,
  "shards_acknowledged": true,
  "index": "movie"
}
```

- 단순히 문자열로 저장하고 싶다면 keyword 타입을 사용한다.
- 반면 형태소 분석을 원할 경우, text 타입을 사용하면 된다.

### 인덱스 삭제

- 삭제는 콘솔에 아래와 같이 입력해 실행하면 된다.

```
DELETE /movie
```

- 결과값

```json
{
  "acknowledged": true
}
```

- 존재하지 않는 인덱스를 삭제하려고 하면 아래와 같은 에러가 발생한다. 삭제했던 인덱스를 한 번 더 삭제해보자.

```
{
  "error": {
    "root_cause": [
      {
        "type": "index_not_found_exception",
        "reason": "no such index [movie]",
        "resource.type": "index_or_alias",
        "resource.id": "movie",
        "index_uuid": "_na_",
        "index": "movie"
      }
    ],
    "type": "index_not_found_exception",
    "reason": "no such index [movie]",
    "resource.type": "index_or_alias",
    "resource.id": "movie",
    "index_uuid": "_na_",
    "index": "movie"
  },
  "status": 404
}
```

- 인덱스는 한 번 삭제하면 다시는 복구할 수 없다! 백업본을 받아놓거나, 대비책을 세워놓자!

<br/>

## 문서 관리 API

- 문서 관리 API는 실제 문서를 색인하고 조회, 수정, 삭제를 지원하는 API다. 문서를 색인하고 내용을 수정하거나 삭제할 수 있다. 엘라스틱서치는 기본적으로 검색 엔진이기 때문에 검색을 위한 다양한 검색 패턴을 지원하는 Search API를 별도로 제공한다.
- 하지만 색인된 문서의 ID를 기준으로 한 건의 문서를 다뤄야 하는 경우 문서 관리 API를 사용한다.
- 세부 기능
  - Index API: 한 건의 문서를 색인한다.
  - Get API: 한 건의 문서를 조회한다.
  - Delete API: 한 건의 문서를 삭제한다.
  - Update API: 한 건의 문서를 업데이트한다.
- 다수의 문서를 처리하려면 `Multi-document API`를 사용하면 된다.
  - Multi Get API: 다수의 문서를 조회한다.
  - Bulk API: 대량의 문서를 색인한다.
  - Delete By Query: 다수의 문서를 삭제한다.
  - Update By Query API: 다수의 문서를 업데이트한다.
  - Reindex API: 인덱스의 문서를 다시 색인한다.

### 문서 생성

- 위에서 생성한 인덱스를 바탕으로 문서를 추가해보자.

> chatGPT는 elasticsearch 8.7은 `_doc` 타입 이름을 생략할 수 있지만 이전 버전과의 호환을 위해 넣어주는 것이 좋다고 했지만... `_doc`을 빼면 `"error": "no handler found for uri [/movie/1?pretty=true] and method [POST]"` 에러와 함께 정상적으로 실행되지 않는다. <s>구라쟁이</s>

```json
POST /movie/_doc/1
{
    "movieCd": "1",
    "movieNm": "반지의제왕",
    "movieNmEn": "The Lord of the Rings",
    "prdtYear": "2001",
    "openDt": "2001-12-19",
    "typeNm": "장편",
    "prdtStatNm": "기타",
    "nationAlt": "미국",
    "genreAlt": "판타지, 드라마",
    "repNationNm": "미국",
    "repGenreNm": "드라마"
}
```

- 결과값
- `result`에 `created`라는 값을 확인해볼 수 있다.

```json
{
  "_index": "movie",
  "_id": "1",
  "_version": 1,
  "result": "created",
  "_shards": {
    "total": 3,
    "successful": 1,
    "failed": 0
  },
  "_seq_no": 0,
  "_primary_term": 1
}
```

### id 없이 생성

- 문서를 추가할 때 아이디 없이 추가해보자.

```json
POST /movie/_doc
{
    "movieCd": "1",
    "movieNm": "반지의제왕",
    "movieNmEn": "The Lord of the Rings",
    "prdtYear": "2001",
    "openDt": "2001-12-19",
    "typeNm": "장편",
    "prdtStatNm": "기타",
    "nationAlt": "미국",
    "genreAlt": "판타지, 드라마",
    "repNationNm": "미국",
    "repGenreNm": "드라마"
}
```

- 아래과 같은 결과값을 받을 수 있는데 `KI7xvIcBb2fPx25WIda6`와 같이 UUID를 통해 무작위 값을 생성해 할당해준다.

```json
{
  "_index": "movie",
  "_id": "KI7xvIcBb2fPx25WIda6",
  "_version": 1,
  "result": "created",
  "_shards": {
    "total": 3,
    "successful": 1,
    "failed": 0
  },
  "_seq_no": 4,
  "_primary_term": 1
}
```

- 이런 방식은 업데이트에서 애로사항을 만들 수 있다. 웬만하면 `_id` 값은 데이터베이스의 테이블 PK값(식별값)과 마줘줄 필요가 있다.

### 문서 조회

- 조회는 아래와 같이 할 수 있다.

```
GET /movie/_doc/1
```

- 결과값

```json
{
  "_index": "movie",
  "_id": "1",
  "_version": 1,
  "_seq_no": 0,
  "_primary_term": 1,
  "found": true,
  "_source": {
    "movieCd": "1",
    "movieNm": "반지의제왕",
    "movieNmEn": "The Lord of the Rings",
    "prdtYear": "2001",
    "openDt": "2001-12-19",
    "typeNm": "장편",
    "prdtStatNm": "기타",
    "nationAlt": "미국",
    "genreAlt": "판타지, 드라마",
    "repNationNm": "미국",
    "repGenreNm": "드라마"
  }
}
```

### 문서 삭제

- 삭제 방법은 아래와 같다.

```
DELETE /movie/_doc/1
```

- 결과값을 보면 `deleted` 문구를 확인해볼 수 있다.

```json
{
  "_index": "movie",
  "_id": "1",
  "_version": 2,
  "result": "deleted",
  "_shards": {
    "total": 3,
    "successful": 1,
    "failed": 0
  },
  "_seq_no": 1,
  "_primary_term": 1
}
```

<br/>

## 검색 API

- 검색 API는 아래 2가지 방식으로 사용된다.

1. HTTP URI 형태의 파라미터를 URI에 추가해 검색하는 방법
2. RESTful API 방식인 QueryDSL을 사용해 요청 분문(Request Body)에 질의 내용을 추가해 검색하는 방법

- 대체로 두번째 방식이 제약사항이 적어, 현업에서 많이 사용된다고 한다.

### URI 질의

```
GET /movie/_doc/1?pretty=true
```

- 결과값

```json
{
  "_index": "movie",
  "_id": "1",
  "_version": 1,
  "_seq_no": 5,
  "_primary_term": 1,
  "found": true,
  "_source": {
    "movieCd": "1",
    "movieNm": "반지의제왕",
    "movieNmEn": "The Lord of the Rings",
    "prdtYear": "2001",
    "openDt": "2001-12-19",
    "typeNm": "장편",
    "prdtStatNm": "기타",
    "nationAlt": "미국",
    "genreAlt": "판타지, 드라마",
    "repNationNm": "미국",
    "repGenreNm": "드라마"
  }
}
```

- 일치하는 값이 없으면 아래 값을 던져준다.

```json
{
  "_index": "movie",
  "_id": "2",
  "found": false
}
```

### URI 질의 - q 파라미터

- q 파라미터에서 사용하는 메소는 POST이다. (근데 GET에서도 됨)

```
POST /movie/_search?q=드라마
```

- 결과값

```json
{
  {
  "took": 2,
  "timed_out": false,
  "_shards": {
    "total": 3,
    "successful": 3,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": {
      "value": 1,
      "relation": "eq"
    },
    "max_score": 0.2876821,
    "hits": [
      {
        "_index": "movie",
        "_id": "1",
        "_score": 0.2876821,
        "_source": {
          "movieCd": "1",
          "movieNm": "반지의제왕",
          "movieNmEn": "The Lord of the Rings",
          "prdtYear": "2001",
          "openDt": "2001-12-19",
          "typeNm": "장편",
          "prdtStatNm": "기타",
          "nationAlt": "미국",
          "genreAlt": "판타지, 드라마",
          "repNationNm": "미국",
          "repGenreNm": "드라마"
        }
      }
    ]
  }
}
```

- 아래와 같이 특정 필드의 값만 검색하도록 만들 수 있다.

```
POST /movie/_search?q=typeNm:장편
```

### Request Body 방식 질의

- 형식은 아래와 같다.

```json
POST /{index_name}/_search
{
    json query
}
```

- 아래와 같이 검색해보자. 정상적으로 검색되는 것을 확인할 수 있다.

```json
POST /movie/_search
{
    "query": {
        "term": {"repGenreNm": "드라마"}
    }
}
```

### 쿼리 구문에 대한 이해

- `query`와 같은 여러 개의 키를 조합해 객체의 키 값으로 사용할 수 있다.
- `size`
  - 몇 개의 결과를 반환할 지 결정(default = 10)
- `from` - 어느 위치부터 반환할 지 결정 - 기본값은 0이다. 설정하지 않으면 상위 0~10의 데이터를 반환한다. -`_source`
- `sort`
  - 특정 필드를 기준으로 정렬한다.
  - asc, desc로 오름차순 및 내림차순을 지정할 수 있다.
- `query`
  - `{}` 안에 검색될 조건을 정의한다.
- `filter`
  - `{}` 안에 조건을 넣는다.
  - 검색 결과 중 특정한 값을 다시 보여주도록 한다.
  - 결과 내에서 재검색할 때 사용하는 기능 중 하나다.
  - 다만 필터를 사용하면 자동으로 score 값이 정렬되지 않는다.

<br/>

## 집계 API

- 쿼리에서 사용하는 집계에 따라 수치를 계산하고 동적으로 카운팅하거나 히스토그램 같은 작업도 할 수 있다.
- 각종 통계 데이터를 실시간으로 제공할 수 있는 강력한 도구다!

### 데이터 집계

- 아래 집계 쿼리는 terms 키워드를 이용해 genreAlt라는 필드의 데이터를 그룹화한다.

```json
POST /movie/_search?size=0
{
    "aggs": {
        "genre": {
            "terms": {
                "field": "genreAlt"
            }
        }
    }
}
```

- 값이 없어서 아래와 같이 '판타지, 드라마' 하나로만 그룹화되었다.

```json
{
  "took": 3,
  "timed_out": false,
  "_shards": {
    "total": 3,
    "successful": 3,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": {
      "value": 1,
      "relation": "eq"
    },
    "max_score": null,
    "hits": []
  },
  "aggregations": {
    "genre": {
      "doc_count_error_upper_bound": 0,
      "sum_other_doc_count": 0,
      "buckets": [
        {
          "key": "판타지, 드라마",
          "doc_count": 1
        }
      ]
    }
  }
}
```

- 집계 결과에 `bucket`이라는 구조 안에 그룹화된 데이터가 포함되어 있는데, 버킷 안에 다른 버킷의 결과를 추가할 수 있다.

```json
POST /movie/_search?size=0
{
    "aggs": {
        "genre": {
            "terms": {
                "field": "genreAlt"
            },
            "aggs": {
                "nation": {
                    "terms": {
                        "field": "nationAlt"
                    }
                }
            }
        }
    }
}
```

- 결과값

```json
{
  "took": 2,
  "timed_out": false,
  "_shards": {
    "total": 3,
    "successful": 3,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": {
      "value": 1,
      "relation": "eq"
    },
    "max_score": null,
    "hits": []
  },
  "aggregations": {
    "genre": {
      "doc_count_error_upper_bound": 0,
      "sum_other_doc_count": 0,
      "buckets": [
        {
          "key": "판타지, 드라마",
          "doc_count": 1,
          "nation": {
            "doc_count_error_upper_bound": 0,
            "sum_other_doc_count": 0,
            "buckets": [
              {
                "key": "미국",
                "doc_count": 1
              }
            ]
          }
        }
      ]
    }
  }
}
```

### 데이터 집계 타입

- 4가지 종류가 있다. 각각의 API는 서로 조합해 사용할 수 있다.
- 버킷 집계 Bucket Aggregation
  - 집계 중 가장 많이 사용한다.
  - 문서의 필드를 기준으로 버킷을 집계한다.
- 메트릭 집계 Metric Aggregation
  - 문서에서 추출된 값을 가지고 Sum, Max, Min, Avg를 계산한다.
- 매트릭스 집계 Matrix Aggregation
  - 행렬의 값을 합하거나 곱한다.
- 파이프라인 집계 Pipeline Aggregation
  - 버킷에서 도출된 결과 문서를 다른 필드 값으로 재분류한다.
  - 다른 집계에 의해 생성된 출력 결과를 다시 한번 집계한다는 말이다.
  - 집계가 패싯보다 강력한 이유다.

> ### 루신의 패싯 API
>
> - 패싯은 같은 항목의 총 개수를 표시하는 기능이다.
> - 인기 게시물이나 카테코리 별 시간당 총 페이지 뷰 등을 반환받을 수 있지만 이를 시간순으로 정렬할 수 없다는 큰 단점이 있다.
> - 때문에 엘라스틱서치는 루씬의 패싯 API를 사용해 통계 기능을 제공하다가 자체 집계 API를 만들게 되었다.

<br/>

## 출처

- 엘라스틱서치 실무 가이드: 한글 검색 시스템 구축부터 대용량 클러스터 운영까지
- [Elasticsearch Index 생성 및 주의할 내용](https://velog.io/@yundleyundle/ElasticSearch-Index%EC%83%9D%EC%84%B1-%EB%B0%8F-%EC%A3%BC%EC%9D%98%ED%95%A0%EB%82%B4%EC%9A%A9)
