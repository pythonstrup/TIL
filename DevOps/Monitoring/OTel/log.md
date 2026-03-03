## Fluent Bit

- 경량 로그 수집기(log collector/forwarder)이다. 애플리케이션이 생성한 로그를 수집하고, 가공하고, 원하는 목적지로 전달한다.
- 예를 들어 Fluent Bit이 애플리케이션의 로그를 수집하고, Open Telemetry의 Collector를 목적지 설정하는 식으로 구성할 수 있다.

### Fluent Bit를 사용해야 하는 이유

- Fluent Bit가 없다면 앱 코드에서 직접 해야할 일들
  - Elasticsearch에 HTTP 요청
  - 실패 시 재시도 로직
  - 버퍼링/배치 처리
  - 연결 관리
- 앱의 책임이 너무 커진다. Fluent Bit이 있으면 앱은 stdout 또는 파일에 JSON 로그만 찍으면 된다.
- Fluent Bit이 수집, 버퍼링, 재시도, 배치, 전달 전부 처리한다.

### Config

#### INPUT — 어디서 로그를 가져올지

```config
[INPUT]
    Name   tail              # 파일 tail -f 방식
    Path   /var/log/app/*.log
```

```config
[INPUT]
    Name   forward           # Fluentd/Fluent Bit간 전송
    Port   24224
```

```config
[INPUT]
    Name   tcp               # TCP 소켓으로 직접 수신
    Port   5170
```

#### PARSER — 원시 텍스트를 구조화

```config
[PARSER]
    Name   json              # JSON 자동 파싱 (가장 흔함)
    Format json
```

```config
[PARSER]
    Name   docker            # Docker 로그 포맷 파싱
    Format json
```

#### FILTER — 로그를 가공/필터링

```config
[FILTER]
    Name   grep              # 특정 조건만 통과
    Match  *
    Regex  level error
```

```config
[FILTER]
    Name   modify            # 필드 추가/삭제/변경
    Match  *
    Add    env production
```

#### OUTPUT — 어디로 보낼지

```config
[OUTPUT]
    Name   es                # Elasticsearch
    Host   localhost
```

```config
[OUTPUT]
    Name   loki              # Grafana Loki
```

```config
[OUTPUT]
    Name   s3                # AWS S3
```

```config
[OUTPUT]
    Name   stdout            # 콘솔 출력 (디버깅용
```

### Config 예시

```conf
[SERVICE]
    flush        1
    log_level    info
    parsers_file parsers.conf

[INPUT]
    name         http
    listen       0.0.0.0
    port         24224
    tag          app.logs

[OUTPUT]
    name            es
    match           *
    host            elasticsearch
    port            9200
    index           app-logs
    type            _doc
    suppress_type_name on
    logstash_format on
    logstash_prefix app-logs
    logstash_dateformat %Y%m%d
    generate_id     on

```

## 빅테크의 로깅 방법?

- [Google Cloud 구조화된 로깅](https://docs.cloud.google.com/logging/docs/structured-logging?hl=ko)
- [Uber Logging](https://www.uber.com/en-KR/blog/logging/)
- [AWS Log](https://docs.aws.amazon.com/lambda/latest/operatorguide/parse-logs.html)
- [AWS 구조화된 로깅](https://aws.amazon.com/blogs/mt/accelerate-troubleshooting-with-structured-logs-in-amazon-cloudwatch/)
- [Grafana Loki - Best Practices](https://grafana.com/blog/2022/05/16/all-things-logs-best-practices-for-logging-and-grafana-loki/)
- [Grafana Loki - json log](https://grafana.com/docs/loki/latest/send-data/promtail/stages/json/)
- [Netflix Skunkworks - logstash-configs](https://github.com/Netflix-Skunkworks/logstash-configs)

### 구조화된 로깅

- 로그는 주로 사람이 읽을 수 있는 형식으로 작성된다.
- 하지만 로그 관리 시스템에 입력하여 강력한 검색 및 분석 기능을 사용하려면 기계가 읽을 수 있는 구조화된 로깅이 필요하다.
- 구조화된 로깅에 가장 일반적으로 사용되는 형식 중 하나는 JSON이다.
