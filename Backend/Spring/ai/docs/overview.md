# 개요

- Spring AI는 LangChain, LlamaIndex와 같은 Python 프로젝트에서 영감을 받음.
  - 프로젝트를 그대로 포팅하지는 않았다.
- Spring AI는 AI 애플리케이션 개발의 기반이 되는 추상화 계층을 제공하며, 이러한 추상화는 여러 구현체를 지원하여 코드를 거의 변경하지 않고도 구성 요소를 쉽게 교체할 수 있도록 돕니다.

## 주요 기능

### AI 공급자 간 이식 가능한 API

- Chat, Text-to-Image, Embedding 모델을 위한 통합 API 제공
- 동기 및 스트리밍 방식 모두 지원
- 공급자별 고유 기능 접근도 가능

### 주요 AI 모델 공급자 지원

- Anthropic, OpenAI, Microsoft, Amazon, Google, Ollama
- 지원 모델 유형
  - Chat Completion
  - Embedding
  - Text to Image
  - Audio Transcription
  - Text to Speech
  - Moderation
  - Structured Outputs – AI 출력 결과를 POJO로 매핑

### 주요 벡터 DB 공급자 지원

- Apache Cassandra, Azure Cosmos DB, Chroma, Elasticsearch, MariaDB, MongoDB Atlas, Neo4j, PineCone, Redis, Qdrant 등
- SQL 유사한 메타데이터 필터 API 포함, 다양한 Vector Store 간 이식성 제공

### 도구/함수 호출

- 모델이 클라이언트 측 함수 호출을 요청하여 실시간 정보에 접근하고 조치를 취할 수 있도록 지원

### Observability

- AI 관련 작업에 대한 인사이트 제공

### 문서 수집 ETL 프레임워크

- 데이터 엔지니어링을 위한 문서 처리 파이프라인 제공

### AI 모델 평가 도구

- 생성된 응답의 품질을 평가하고 `환각 hallucination` 응답 방지

### Spring Boot 통합

- AI 모델 및 벡터 저장소를 위한 자동 구성 및 스타터 제공

### ChatClient API

- WebClient, RestClient와 유사한 스타일의 Fluent API. 
- AI 채팅 모델과의 통신

### Advisors API

- 생성형 AI에서 반복되는 패턴을 캡슐화
- LLM과의 입출력을 변환하여 다양한 모델과 사용 사례에서 이식성 제공

### Chat Memory 및 RAG 지원(Retrieval-Augmented Generation)

- 문서 기반 질의응답 (회사에서 문서 찾기할 때 도움될지도?)
- "문서와 대화하기" 같은 시나리오 구현 가능

----

## Concept

### 1. 모델 Models

- 인간의 인지 기능을 모방하는 알고리즘
- GPT의 핵심: Pre-trained
  - 이미 학습된 상태이므로 별도의 머신러닝 지식 없이도 활용 가능.
- 입출력 유형별 모델 분류
  - Spring AI는 텍스트, 이미지, 오디오 기반 모델을 지원하며, 텍스트 -> 벡터로 변환하는 `임베딩 Embedding` 모델도 포함

### 2. 프롬프트 Prompt

- AI 모델에게 명령을 내리는 언어 기반 입력.
- 단순한 문자열이 아니라 역할이 구분된 다중 입력 구조일 수 있다.
- 언어적 유도는 모델 성능에 큰 영향을 미치므로 프롬프트는 굉장히 중요하다.

#### 프롬프트 템플릿

- 문법 예시

```
Tell me a {adjective} joke about {content}.
```

- Spring MVC에 비유하자면, 프롬프트 템플릿은 `View`와 유사하며, `Map` 객체로 값을 주입한다.

### 3. 임베딩 Embeddings

- 의미를 담은 벡터 표현
- 텍스트, 이미지, 영상 - 고차원 float 배열로 변환. 의미 기반 유사성 계산 가능
- ex) RAG, 의미 기반 검색, 추천 시스템 등

### 4. 토큰 Tokens

- 모델의 처리 단위
  - 단어를 분해해 토큰화. 영어 기준으로 한 단어는 약 0.75 토큰정도다.
- 비용 = 토큰 사용량
- 각 모델에는 토큰 제한이 있고, 문서를 쪼개어 요청해야 하는데, Spring AI는 이를 지원한다.
  - GPT-3.5: 4K
  - GPT-4: 8~32K
  - Calude: 100K
  - Meta: 1M

### 5. Structured Output

- AI는 JSON을 문자열로 출력하기 때문에 실제 데이터 객체로 변환 필요.
- Spring AI는 다중 요청을 통해 정형화된 구조 도출 및 매핑을 한다.

### 6. 외부 데이터 연동 방식

#### Fine Tuning

- 모델 자체를 변경
- 비용이 크고, 복잡도가 높다.

#### Prompt Stuffing (RAG 포함)

- 프롬프트에 데이터를 삽입
- 문서 -> 조각화 -> 벡터 DB에 저장 -> 질의 시 유사 조각을 불러와서 함께 요청

#### RAG (Retrieval Augmented Generation)

- ETL 기반 문서 처리 파이프라인
- 문서를 의미 단위로 나누고, 벡터화하여 저장
  - 질의 시 유사한 내용 검색.
- Spring AI는 이 전 과정을 자동화할 수 있도록  `ETL`, `Vector Store`, `ChatClient`, `Advisor API` 제공

### Tool Calling 메커니즘

- 모델이 외부 API 호출
- ex) 실시간 환율, 재고 정보 등 호출 가능
- 모델에게 도구 제공 -> 모델이 도구를 호출함
- 응답 생성 시 도구 호출 결과를 참고하여 최종 응답 생성
- `@Tool` 어노테이션, Schema, 자동 연결 지원

### 7. AI 응답 평가

- 정확성, 일관성, 관련성 등 메트릭으로 평가
- 벡터 DB 기반 정보와 대조하며 평가 가능
- Spring AI는 `Evaluator API`를 제공한다.

----

# 참고 자료

- [공식문서](https://docs.spring.io/spring-ai/reference/)

