# 05 엘라스틱서치: 집계

- 그룹별 통계.

## 1. 집계의 요청-응답 형태

- 집계를 위한 기본 형태

```http request
GET {index}/_search
{
    "aggs": {
        "my_aggs": {
            "agg_type": {
                ...
            }
        }
    }
}
```

- 여기서 "my_aggs"는 사용자가 지정한느 집계 이름. 그리고 "agg_type"는 집계의 타입이다.
- 두 가지 타입의 집계
  - `metric aggregations`
  - `bucket aggregations`

---

## 2. 메트릭 집계

- 필드의 최소/최대/합계/평균/중간값 같은 통계 결과를 보여준다.
  - `avg`
  - `min`
  - `max`
  - `sum`
  - `percentiles`
  - `stats`: min, max, sum, avg, count를 한 번에 볼 수 있다.
  - `cardinality`
  - `geo-centroid`

### 2-1. 평균값/중간값 구하기

- `size`를 0으로 설정하면 집계에 사용한 도큐먼트를 결과에 포함하지 않음으로써 비용을 절약할 수 있다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "stats_aggs": {
      "avg": {
        "field": "products.base_price"
      }
    }
  }
}
```

- 결과값

```json
{
  "took": 41,
  "timed_out": false,
  "_shards": {
    "total": 1,
    "successful": 1,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": {
      "value": 4675,
      "relation": "eq"
    },
    "max_score": null,
    "hits": []
  },
  "aggregations": {
    "stats_aggs": {
      "value": 34.88652318578368
    }
  }
}
```

- 아래는 백분위를 구하는 집계 요청으로 25%와 50%에 속하는 데이터를 요청한다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "stats_aggs": {
      "percentiles": {
        "field": "products.base_price",
        "percents": [
          25,
          50
        ]
      }
    }
  }
}
```

- 결과는 아래와 같다.

```json
{
  "took": 37,
  "timed_out": false,
  "_shards": {
    "total": 1,
    "successful": 1,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": {
      "value": 4675,
      "relation": "eq"
    },
    "max_score": null,
    "hits": []
  },
  "aggregations": {
    "stats_aggs": {
      "values": {
        "25.0": 17.31900364045424,
        "50.0": 26.826245672018715
      }
    }
  }
}
```

### 필드의 유니크한 값 개수 확인하기

- `precision_threshold` 파라미터는 정확도 수치이다.
  - 값이 크면 정확도가 올라가는 대신 시스템 리소스를 많이 소모하고, 값이 작으면 정확도는 떨어지는 대신 시스템 리소스를 덜 소모한다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "cardi_aggs": {
      "cardinality": {
        "field": "day_of_week",
        "precision_threshold": 100
      }
    }
  }
}
```

- 이 때 결과값은 `7`이 나온다. (일~토)

```json
{
  ...
  "aggregations": {
    "cardi_aggs": {
      "value": 7
    }
  }
}
```

- 그런데 아래처럼 `precision_threshold`를 7보다 낮게 하면 잘못된 결과가 나오게 된다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "cardi_aggs": {
      "cardinality": {
        "field": "day_of_week",
        "precision_threshold": 5
      }
    }
  }
}
```

```json
{
  ...
  "aggregations": {
    "cardi_aggs": {
      "value": 8
    }
  }
}
```

- 일반적으로 `precision_threshold`는 카디널리티의 실제 결과보다 크게 잡아야 한다.
  - 하지만 실제 결과는 모르기 때문에 변경해보면서 값이 변경되지 않는 임계점을 찾아야 한다.
  - 기본값은 3,000이며 최대 40,000까지 설정 가능
- 버킷 집계의 일종은 용어 집계

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "cardi_aggs": {
      "terms": {
        "field": "day_of_week"
      }
    }
  }
}
```

```json
{
  "took": 24,
  "timed_out": false,
  "_shards": {
    "total": 1,
    "successful": 1,
    "skipped": 0,
    "failed": 0
  },
  "hits": {
    "total": {
      "value": 4675,
      "relation": "eq"
    },
    "max_score": null,
    "hits": []
  },
  "aggregations": {
    "cardi_aggs": {
      "doc_count_error_upper_bound": 0,
      "sum_other_doc_count": 0,
      "buckets": [
        {
          "key": "Thursday",
          "doc_count": 775
        },
        {
          "key": "Friday",
          "doc_count": 770
        },
        {
          "key": "Saturday",
          "doc_count": 736
        },
        {
          "key": "Sunday",
          "doc_count": 614
        },
        {
          "key": "Tuesday",
          "doc_count": 609
        },
        {
          "key": "Wednesday",
          "doc_count": 592
        },
        {
          "key": "Monday",
          "doc_count": 579
        }
      ]
    }
  }
}
```

### 2-3. 검색 결과 내에서의 집계

- 예를 들어, `day_of_week`에서 월요일인 도큐먼트를 대상으로 집계 범위를 좁히는 등의 쿼리가 가능하다.

----

## 3. 버킷 집계

- 특정 기준에 맞춰서 도큐먼트를 그룹핑하는 역할.
- 여기서 버킷은 도큐먼트가 분할되는 단위로 나뉜 각 그룹을 의미한다.
- 아래는 자주 사용하는 버킷 집계 종류
  - `histogram`
  - `date_histogram`
  - `range`
  - `date_range`
  - `terms`
  - `significant_terms`
  - `filters`

### 3-1. 히스토그램 집계

- 숫자 타입 필드를 `일정 간격 interval` 기준으로 구분해주는 집계
- 이것과 비슷한 `날짜 히스토그램 집계 date histogram aggregation`도 있다.

### 3-2. 범위 집계

- 히스토그램 집계의 단점: 각 버킷의 범위를 동일하게 지정할 수밖에 없다.
  - 특정 구간에 데이터가 몰려 있거나 데이터 편차가 큰 경우 모든 데이터를 표현하는 데 비효율적인 경우가 있다.
- 이때 `범위 집계 range aggregation`을 이용하면 각 버킷의 범위를 사용자가 직접 설정할 수 있다.

### 3-3. 용어 집계

- 필드의 유니한 값을 기준으로 버킷을 나눌 때 사용.
- 기본적으로 `size` 10을 적용
- 만약 지정한 `size`보다 카디널리티가 더 높으면, 도큐먼트 개수가 많은 상위 `size` 개수만큼의 버킷만 확인할 수 있다.

#### 3-3-1. 용어 집계가 정확하지 않은 이유

- 용어 집계가 부정확도를 표시하는 이유는 분산 시스템의 집계 과정에서 발생하는 잠재적인 오류 가능성 때문이다.
- 도큐먼트가 샤드1, 샤드2로 나뉘어서 저장되어 있다고 해보자.
  - 각 샤드에서 size만큼 상위의 용어만 집계한다. 그리고 이를 다시 취합한다.
  - 상대적으로 큰 값이 저장된 샤드가 있는 경우 큰 값이 버려져 오차가 발생할 수 있다는 얘기다.

#### 3-3-2. 용어 집계 정확성 높이기

- 고속 처리를 위한 리소스와 속도 간 트레이드오프의 일환으로 리소스 소비량을 늘리면 정확도를 높일 수 있다.
- 용어를 집계할 때 `show_term_doc_count_error` 파라미터를 추가한다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "term_aggs": {
      "terms": {
        "field": "day_of_week",
        "size": 6,
        "show_term_doc_count_error": true
      }
    }
  }
}
```

- 각 버킷마다 `doc_count_error_upper_bound` 값이 나오고 0이 나오면 오류가 없다는 뜻이다.
- 샤드를 하나만 사용하고 있기 때문에 특별한 오류를 찾을 수 없었다.

```json
{
  ...
  "aggregations": {
    "term_aggs": {
      "doc_count_error_upper_bound": 0,
      "sum_other_doc_count": 579,
      "buckets": [
        {
          "key": "Thursday",
          "doc_count": 775,
          "doc_count_error_upper_bound": 0
        },
        {
          "key": "Friday",
          "doc_count": 770,
          "doc_count_error_upper_bound": 0
        },
        {
          "key": "Saturday",
          "doc_count": 736,
          "doc_count_error_upper_bound": 0
        },
        {
          "key": "Sunday",
          "doc_count": 614,
          "doc_count_error_upper_bound": 0
        },
        {
          "key": "Tuesday",
          "doc_count": 609,
          "doc_count_error_upper_bound": 0
        },
        {
          "key": "Wednesday",
          "doc_count": 592,
          "doc_count_error_upper_bound": 0
        }
      ]
    }
  }
}
```

- 확인 결과 이상값이 나올 경우에는 이를 해결하기 위해 다음과 같이 샤드 크기 파라미터를 늘릴 필요가 있다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "term_aggs": {
      "terms": {
        "field": "day_of_week",
        "size": 6,
        "shard_size": 100
      }
    }
  }
}
```

- 샤드 크기는 용어 집계 과정에서 개별 샤드에서 집계를 위해 처리하는 개수를 의미한다.
  - 정확도가 올라가는 대신 리소스 사용량이 올라간다.
  - 샤드 크기는 기본적으로 `size * 1.5 + 10`으로 게산된다. `size`는 버킷의 개수.

---

## 4. 집계의 조합

- 버킷 집계와 메트릭 집계를 조합하면 다양한 그룹별 통계를 계산할 수 있다.

### 4-1. 버킷 집계와 메트릭 집계

- 아래 에제는 메트릭 집계인 평균 집계를 용어 집계 내부에서 호출한 것이다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "term_aggs": {
      "terms": {
        "field": "day_of_week",
        "size": 5
      },
      "aggs": {
        "avg_aggs": {
          "avg": {
            "field": "products.base_price"
          }
        }
      }
    }
  }
}
```

```json
{
  ...
  "aggregations": {
    "term_aggs": {
      "doc_count_error_upper_bound": 0,
      "sum_other_doc_count": 1171,
      "buckets": [
        {
          "key": "Thursday",
          "doc_count": 775,
          "avg_aggs": {
            "value": 34.68040897713688
          }
        },
        {
          "key": "Friday",
          "doc_count": 770,
          "avg_aggs": {
            "value": 34.665464386512184
          }
        },
        {
          "key": "Saturday",
          "doc_count": 736,
          "avg_aggs": {
            "value": 34.35796178343949
          }
        },
        {
          "key": "Sunday",
          "doc_count": 614,
          "avg_aggs": {
            "value": 35.27872066570881
          }
        },
        {
          "key": "Tuesday",
          "doc_count": 609,
          "avg_aggs": {
            "value": 34.33571633511859
          }
        }
      ]
    }
  }
}
```

- 아래와 같이 물품들의 총합/최소/최대 같은 다양한 통계 데이터를 얻을 수 있다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "term_aggs": {
      "terms": {
        "field": "day_of_week",
        "size": 5
      },
      "aggs": {
        "avg_aggs": {
          "avg": {
            "field": "products.base_price"
          }
        },
        "sum_aggs": {
          "sum": {
            "field": "products.base_price"
          }
        }
      }
    }
  }
}
```

### 4-2. 서브 버킷 집계

- `서브 버킷 sub bucket`은 버킷 안에서 다시 버킷 집계를 요청하는 집계다.
  - 트리 구조를 생각하면 된다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "histogram_aggs": {
      "histogram": {
        "field": "products.base_price",
        "interval": 100
      },
      "aggs": {
        "term_aggs": {
          "terms": {
            "field": "day_of_week",
            "size": 2
          }
        }
      }
    }
  }
}
```

- 서브 버킷은 2단계를 초과해서 만들지 않는 편이 좋다.
  - 버킷의 수가 기하급수적으로 많아질 수 있기 때문.

----

## 5. 파이프라인 집계

- `pipeline aggregation`은 이전 결과를 다음 단계에서 이용하는 파이프라인 개념을 차용.
  - `parent aggregation`과 `sibling aggregation` 두 가지 유형.
- 형제 집계
  - `min_bucket`
  - `max_bucket`
  - `avg_bucket`
  - `sum_bucket`
  - `stat_bucket`
  - `percentile_bucket`
  - `moving_avg`: 기존 집계의 이동 평균 (기존 집계가 순차적인 데이터 구조여야 함.)
- 부모 집계
  - `derivative`: 기존 집계의 미분
  - `cumulative_sum`: 기존 집계의 누적합

### 5-1. 부모 집계

- 단독으로 사용할 수 없고 반드시 먼저 다른 집계가 있어야 하며 그 집계 결과를 부모 집계가 사용한다. 그리고 부모 집계는 이전 집계 내부에서 실행한다. 그리고 결과값도 기존 집계 내부에서 나타난다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "histogram_aggs": {
      "histogram": {
        "field": "products.base_price",
        "interval": 100
      },
      "aggs": {
        "sum_aggs": {
          "sum": {
            "field": "taxful_total_price"
          }
        },
        "cum_sum": {
          "cumulative_sum": {
            "buckets_path": "sum_aggs"
          }
        }
      }
    }
  }
}
```

### 5-2. 형제 집계

- 형제 집계는 기존 집계 내부가 아닌 외부에서 기존 집계를 이용해 집계 작업을 한다.

```http request
GET kibana_sample_data_ecommerce/_search
{
  "size": 0,
  "aggs": {
    "term_aggs": {
      "terms": {
        "field": "day_of_week",
        "size": 2
      },
      "aggs": {
        "sum_aggs": {
          "sum": {
            "field": "products.base_price"
          }
        }
      }
    },
    "sub_total_price": {
      "sum_bucket": {
        "buckets_path": "term_aggs>sum_aggs"
      }
    }
  }
}
```

----

# 참고 자료

- 엘라스틱 스택 개발부터 운영까지, 김준영 & 정상운 지음, 박재호 감수, 펴낸곳: 책만
