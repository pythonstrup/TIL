# Chapter 02. Model I/O - 언어 모델을 다루기 쉽게 만들기

## 1. 언어 모델을 이용한 응용 프로그램 작동 방식

### 언어 모델 호출이란?

- GPT에서는 텍스트 상장에 입력한 메시지를 통해 언어 모델을 호출하고 있다고 할 수 있다.
- 언어 모델을 호출할 때 입력되는 텍스트를 '프롬프트'라고 부른다.
- 실제로 언어 모델을 활용한 애플리케이션을 개발할 때 문제가 되는 경우가 많다. 언어 모델을 활용한 애플리케이션은 모두 절차형으로 작성하는 기존 애플리케이션과 달리 좋은 결과를 얻기 위해 시행착오를 거쳐야 한다.
- 하지만 언어 모델은 OpenAI의 GPT만 있는 것이 아니다. 앤트로픽의 클로드2(Claude 2)를 사용해 결과가 어떻게 달라지는지 보고 싶을 수도 있다.
  - 이 경우, 앞서 언급한 소스코드는 OpenAI에만 대응하기 때문에 거의 모든 소스코드를 다시 작성해야 한다.
- 언어 모델을 호출하는 프롬프트를 일일이 재작성하거나 모델을 교체하는 것은 매우 번거롭다.
  - Model I/O 모듈은 이러한 번거로움을 줄일 수 있는 수단을 제공한다.

### Model I/O 랭체인의 가장 기본적인 모듈이다

- Model I/O는 모듈을 단독으로도 사용할 수 있지만, 실제 애플리케이션을 개발할 때 이 모듈만으로 모든 것을 만드는 것은 현실적으로 어렵기 때문에 다른 모듈과 조합해 사용하는 것이 일반적이다.
  - 예를 들어, Model I/O의 하위 모듈인 Prompts 모듈은 프롬프트를 최적화하기 위해 사용될 뿐만 아니라 나중에 소개할 Chains 모듈 등에서도 사용되며, 마찬가지로 하위 모듈인 Language models는 거의 모든 모듈에서 사용된다.

### Model I/O를 구성하는 3개의 서브모듈

- 랭체인의 모든 모듈은 서브모듈을 가지고 있으며, Model I/O 모듈도 예외는 아니어서 3개의 서브모듈로 구성돼 있다.

#### 1. Language models

- Language models 모듈은 다양한 언어 모델을 동일한 인터페이스에서 호출할 수 있는 기능을 제공하며, OpenAI의 모델뿐만 아니라 앤트로픽의 클로드 2와 같은 다른 모델도 동일하게 호출할 수 있다.
- 이를 통해 다른 모델을 시도할 때 기존 코드를 처음부터 다시 작성할 필요가 없다.

#### 2. Prompts

- Prompts 모듈은 언어 모델을 호출하기 위한 프롬프트를 구축하는 데 유용한 기능을 제공한다.
- 용도에 따라 다양한 손자 모듈이 제공된다.
  - 예를 들어, 프롬프트와 변수를 결합하거나 대량의 예시를 프롬프트에 효율적으로 삽입할 수 있다.
  - 다양한 처리를 통해 프롬프트를 쉽게 만들 수 있도록 하는 것이 목적이다.

#### 3. Output parsers

- Output parsers 모듈은 언어 모델에서 얻은 출력을 분석해 애플리케이션에서 사용하기 쉬운 형태로 변환하는 기능을 제공한다.
  - 출력 문자열을 정형화하거나 특정 정보를 추출하는 데 사용한다.
  - 이 모듈을 사용하면 출력을 구조화된 데이터로 쉽게 처리할 수 있다.

### Language models를 사용해 gpt-3.5-turbo 호출하기

```python
from langchain_community.chat_models import ChatOpenAI
from langchain.schema import HumanMessage

chat = ChatOpenAI(model="gpt-3.5-turbo")

result = chat([HumanMessage(content="안녕하세요!")])
print(result)
```

- 위 코드를 실행하면 아래와 같은 결과물을 받을 수 있다.

```shell
content='안녕하세요! 무엇을 도와드릴까요?' 
additional_kwargs={} 
response_metadata={
  'token_usage': {
    'completion_tokens': 21, 
    'prompt_tokens': 13, 
    'total_tokens': 34, 
    'completion_tokens_details': {
      'accepted_prediction_tokens': 0, 
      'audio_tokens': 0, 
      'reasoning_tokens': 0, 
      'rejected_prediction_tokens': 0
    }, 
    'prompt_tokens_details': {
      'audio_tokens': 0, 
      'cached_tokens': 0
    }
  }, 
  'model_name': 'gpt-3.5-turbo', 
  'system_fingerprint': None, 
  'finish_reason': 'stop', 
  'logprobs': None
} 
id='run-d3735d94-4502-40a8-b69a-e6a4b773a79b-0
```

- `HumanMessage`는 사람의 메시지가 있다는 것을 나타내며, content는 그 내용을 나타낸다.

#### AIMessage를 사용해 언어 모델의 응답을 표현할 수 있다.

- 랭체인에서는 대화 형식의 상호작용을 표현하기 위해 `AIMessage`도 준비돼 있다.
  - 예를 들어 '계란찜 만드는 법을 알려줘'라고 문의하면 언어 모델에서 레시피를 반환한다.

```python
result = chat([
    HumanMessage(content="계랸찜 만드는 법 알려줘"),
    AIMessage(content="ChatModel의 답변인 계란찜 만드는 법"),
    HumanMessage(content="영어로 번역해줘"),
])
```

```shell
content='How to make steamed egg custard' 
additional_kwargs={} 
response_metadata={
  'token_usage': {
    'completion_tokens': 8, 
    'prompt_tokens': 60, 
    'total_tokens': 68, 
    'completion_tokens_details': {
      'accepted_prediction_tokens': 0, 
      'audio_tokens': 0, 
      'reasoning_tokens': 0, 
      'rejected_prediction_tokens': 0
    }, 
    'prompt_tokens_details': {
      'audio_tokens': 0, 
      'cached_tokens': 0
    }
  }, 
  'model_name': 'gpt-3.5-turbo', 
  'system_fingerprint': None, 
  'finish_reason': 'stop', 
  'logprobs': None
} 
id='run-7e749350-5ecc-4b18-90ed-a4f65a19609d-0'
```

- 이처럼 Language models 모듈의 Chat models 모듈에서는 `HumanMessage`, `AIMessage`를 사용해 언어 모델과의 대화 형식의 상호작용을 표현할 수 있다.
- Language models만으로 이렇게 과거의 응답을 바탕으로 답변하게 하려면 매번 소스코드를 다시 작성해야 하므로 매우 번거롭고, 대화를 이용한 애플리케이션 개발이 어려울 수 있다.
- 랭체인에는 이러한 상호작용을 지원하기 위한 Memory 모듈이 준비돼 있다.

#### SystemMessage를 사용해 언어 모델의 성격과 설정을 정의한다.

- 이러한 대화 기능을 커스터마이징할 수 있는 `SystemMessage`도 준비돼 있다.
  - 이것은 대화를 표현하는 것이 아니라, 언어에 대한 직접적인 지시를 작성하는 기능이다.
  - ex) 언어 모델의 성격이나 설정 등을 입력하면 답변의 문체를 좀 더 솔직하게 바꿀 수 있다.

```python
result = chat([
    SystemMessage(content="당신은 친한 친구입니다. 존댓말을 쓰지 말고 솔직하게 답해주세요."),
    HumanMessage(content="안녕"),
])
```

```shell
content='안녕! 어떻게 지내? 무슨 일 있어?' 
additional_kwargs={} 
response_metadata={
  'token_usage': {
    'completion_tokens': 22, 
    'prompt_tokens': 52, 
    'total_tokens': 74, 
    'completion_tokens_details': {
      'accepted_prediction_tokens': 0, 
      'audio_tokens': 0, 
      'reasoning_tokens': 0, 
      'rejected_prediction_tokens': 0
    }, 
    'prompt_tokens_details': {
      'audio_tokens': 0, 
      'cached_tokens': 0
    }
  }, 
  'model_name': 'gpt-3.5-turbo', 
  'system_fingerprint': None, 
  'finish_reason': 'stop', 
  'logprobs': None
} 
id='run-d237eede-ca5d-4ed8-a34b-14d583999743-0'
```

#### 언어 모델 교체 가능

- 앤트로픽이 개발한 언어 모델로 대체할 경우 어떻게 변경되는지 확인.
- 앤트로픽이 개발한 대화형 언어 모델은 ChatAnthropic에서 사용할 수 있다.
  - 아래와 같이 앤트로픽의 모델을 가져오도록 변경만 해주면 된다.

```python
from langchain_community.chat_models import ChatAnthropic
# ...
chat = ChatAnthropic()
# ...
```

### PromptTemplate로 변수를 프롬프트에 전개하기

- `Prompts` 모듈의 가장 기본적인 모듈인 `PromptTemplate`을 사용.

```python
from langchain import PromptTemplate

prompt = PromptTemplate(
    template="{product}는 어느 회사에서 개발한 제품인가요?",
    input_variables={
        "product"
    }
)

print(prompt.format(product="아이폰"))
print(prompt.format(product="갤럭시"))
```

- 결과값은 아래와 같다.

```shell
아이폰는 어느 회사에서 개발한 제품인가요?
갤럭시는 어느 회사에서 개발한 제품인가요?
```

### PromptTemplate에서 제공하는 다른 기능들

- 유효성 검사 기능

```python
print(prompt.format())
```

```shell
KeyError: 'product' # 필요한 입력이 있음에도 입력이 없었기 때문에
```

### Language models와 PromptTemplate의 결합

```python
from langchain_community.chat_models import ChatOpenAI
from langchain.schema import HumanMessage
from langchain import PromptTemplate

chat = ChatOpenAI(model="gpt-3.5-turbo")

prompt = PromptTemplate(
    template="{product}는 어느 회사에서 개발한 제품인가요?",
    input_variables={
        "product"
    }
)

result = chat([HumanMessage(content=prompt.format(product="아이폰"))])
print(result.content)
```

```shell
아이폰은 미국의 애플(Apple) 회사에서 개발한 제품입니다.
```

#### PromptTemplate 초기화 방법의 종류

1. `input_variables`로 초기화하기

```python
prompt = PromptTemplate(
    template="{product}는 어느 회사에서 개발한 제품인가요?",
    input_variables={
        "product"
    }
)
```

2. JSON 파일에 저장한 프롬프트 읽어오기

```python
from langchain.prompts import PromptTemplate

prompt = PromptTemplate(
    template="{product}는 어느 회사에서 개발한 제품인가요?",
)

input_variables=["product"]
prompt_json = prompt.save("prompt.json")
```

- 아래와 같은 JSON이 생성된다.

```shell
{
    "name": null,
    "input_variables": [
        "product"
    ],
    "optional_variables": [],
    "output_parser": null,
    "partial_variables": {},
    "metadata": null,
    "tags": null,
    "template": "{product}\ub294 \uc5b4\ub290 \ud68c\uc0ac\uc5d0\uc11c \uac1c\ubc1c\ud55c \uc81c\ud488\uc778\uac00\uc694?",
    "template_format": "f-string",
    "validate_template": false,
    "_type": "prompt"
}
```

- 이제 해당 JSON을 로드하여 사용할 수 있다.

```python
from langchain.prompts import load_prompt

loaded_prompt = load_prompt("prompt.json")

print(loaded_prompt.format(product="아이폰"))
```

```shell
아이폰는 어느 회사에서 개발한 제품인가요?
```

### 목록 형식으로 결과 받기

- Ouput parsers를 통해 결과를 구조화

```python
from langchain_community.chat_models import ChatOpenAI
from langchain.output_parsers import CommaSeparatedListOutputParser
from langchain.schema import HumanMessage

output_parser = CommaSeparatedListOutputParser()

chat = ChatOpenAI(model="gpt-3.5-turbo")

result = chat([
    HumanMessage(content="애플이 개발한 대표적인 제품 3개 알려주세요."),
    HumanMessage(content=output_parser.get_format_instructions()),
])

output = output_parser.parse(result.content)
for item in output:
    print("대표 상품: " + item)
```

```shell
대표 상품: 아이폰
대표 상품: 아이패드
대표 상품: 맥북
```

- `CommaSeparatedListOutputParser`에서 이뤄지는 처리는 다음과 같다.
  - 목록 형식으로 출력하도록 언어 모델에 출력 형식 지시를 추가한다.
  - 출력 결과를 분석해 목록 형식으로 변환한다.
- 언어 모델이 생성하는 출력은 기본적으로 일반 텍스트 문자열이다. 이 문자열을 그대로 활용할 수도 있지만, 애플리케이션을 개발할 때 이 문자열에서 특정 정보를 추출하거나 데이터로 구조화해야 하는 경우가 많다.
- 애플리케이션에서 활용하기 쉬운 데이터로 변홚하기 위해서는 출력 문자열을 분석해 필요한 정보를 추출하는 과정이 필수적이다.

# 참고자료

- 랭체인 완벽 입문, 타무라 하루카 저
