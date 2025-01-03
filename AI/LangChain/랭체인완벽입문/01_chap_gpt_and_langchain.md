# Chapter01. 챗지피티와 랭체인

## 1. 챗지피티와 언어 모델

- 언어 모델이란 인간의 언어(자연어)를 컴퓨터가 이해할 수 있게 하고, 이를 바탕으로 텍스트를 생성하기 위한 알고리즘이나 프로그램을 말한다.
  - 챗지피티에는 OpenAI가 개발한 언어 모델 GPT라는 기술이 사용된다.
  - GPT는 다른 모델과 다르게 다양한 종류의 컨텍스트로 학습해 다양한 작업에 대응할 수 있다.

### OpenAI의 API에서 사용할 수 있는 대표적인 두 가지 언어 모델

- 'Chat'과 'Complete'라는 두 가지로 분류된다.
- 'Chat' 모델
  - 대화형 상호작용 생성에 특화된 모델이다.
- 'Complete' 모델
  - 주어진 텍스트에 이어 텍스트를 생성한다.
  - 어느 정도 정보나 이야기의 시작을 제공하면 이를 바탕으로 텍스트를 자동으로 보완할 수 있다.

#### Chat에서 사용할 수 있는 모델 소개

- 2023년 9월 기준 Chat에서 사용을 권장하는 버전은 GPT-3.5 계열과 GPT-4 계열 두 가지다.
  - 둘 다 대체로 동일한 작업을 수행할 수 있지만, 성능(매개변수 수)과 처리할 수 있는 입력 데이터의 종류에 차이가 있다.

#### 모델 선택 시 컨텍스트 길이를 고려해야 한다.

- gpt-3.5-turbo-16k는 16k(16,000)의 컨텍스트 길이를 처리할 수 있음을 의미한다.
  - 컨텍스트 기이는 모델이 한 번에 처리할 수 있는 텍스트의 길이(토큰 수)를 의미한다.

#### 모델 업데이트 정보

- GPT-3.5, GPT-4의 업데이트는 '증분 업데이트' 형식으로 진행된다.
  - '증분 업데이트'는 조금씩 기능을 추가하거나 수정하는 방식으로, 전체 기능 수정에 시간을 들이는 대신 단기간에 반복적으로 수정하는 개발 방식이다.
  - 사용자는 항상 최신 모델을 사용할 수 있고, 결과적으로 더 높은 품질의 경험을 제공할 수 있다.

#### Complete에서 사용 가능한 모델

- GPT-3.5에서만 제공했다고 한다.
- GPT-4에서도 `text-davinci`의 기능을 확장했다고 한다. (예전 3버전에서 사용하던 `text-davinci`와는 완전히 다르다고 함.)

### OpenAI 이외의 언어 모델

- Anthropic의 `Claude2`
  - 가장 큰 특징은 컨텍스트 길이가 길다는 것. `Claude2`는 100k(100,000) 토큰으로 매우 길다.
  - 프로젝트 전체 소스코드를 생성하고 버그를 수정하는 등 GPT에서 할 수 없는 일을 수행할 수 있다.
- Meta의 `Llama`
  - 오픈소스로 열려 있다. 원하대 대로 커스터마이징이 가능하다는 특징이 있다.
  - 덕분에 여러 파생 버전들이 개발되고 있다. (Ubuntu Linux 같은 느낌?)

## 2. 랭체인 개요

- 랭체인은 언어 모델을 이용한 애플리케이션 개발을 도와주는 라이브러리다.

### 언어 모델을 이용한 애플리케이션 개발

- 언어 모델만으로는 불가능한 작업들이 있다.
  - 예를 들어, 학습한 지식의 범위를 벗어난 정보에 대해 대답하게 할 수 없고, 논리적으로 너무 복잡한 작업에는 대응할 수 없으며, 결국 텍스트 교환만 할 수 있다.
- 이러한 한계를 극복하기 위한 방법론
  - 언어 모델이 알지 못하는 정보에 대해 대답하게 하는 RAG(Retrieval-Augmented Generation, 검색 증강 생성)
  - 추론과 행동을 언어 모델 스스로 판단해 인터넷 검색이나 파일 저장 등을 자율적으로 수행하게 하는 ReAct(Reasoning And Acting, 추론 및 행동)

### 랭체인으로 언어 모델을 이용한 애플리케이션 개발이 쉬워진다.

- 랭체인은 오픈소스 라이브러리.
- 랭체인가 OpenAI와 같은 고급 언어 모델을 결합해, 간단한 텍스트를 생성하는 것뿐만 아니라 자연어를 이해하고, 특정 문제에 대한 답을 생성하거나 특정 환경에서 작동하는 애플리케이션을 개발하는 등 기존의 절차적 프로그래밍에서 어려웠던 작업을 효율적으로 수행할 수 있다.

### 6개의 모듈

#### Model I/O - 언어 모델 쉽게 다루기

- 언어 모델을 호출하기 위한 3단계
  - '프롬프트 준비', '언어 모델 호출', '결과 수신'
- 이 세 단계를 쉽게 구현할 수 있다. 다른 모듈과 함께 사용할 때 진가를 발휘한다.

#### Retrieval - 알 수 없는 데이터 다루기

- 미지의 정보를 달 때 쓴다.

#### Memory - 과거의 대화를 장/단기적으로 기억

- 사용자가 여러 개의 스레드를 가지고 각각의 대화 기록에 따라 답변할 수 있다.

#### Chains - 여러 프로세스 통합

- 여러 모듈과 다른 기능을 조합해 하나의 애플리케이션을 만들 수 있는데, 이를 쉽게 만들 수 있게 도와준다.

#### Agents - 자율적으로 외부와 상호작용해 언어 모델의 한계를 뛰어넘는다.

#### Callbacks - 다양한 이벤트를 처리한다.

> #### 참고
> - 랭체인은 파이썬 버전과 타입스크립트 버전 두 가지가 있다.
> - 사실 랭체인은 파이썬으로 개발되었고, 따라서 파이썬 버전부터 개발이 진행된다. (타입스크립트는 이에 추종)
> - 그래서 파이썬 버전에서만 사용할 수 있는 기능이 많이 존재한다.
> - 타입스크립트 덕분에 Next.js 등을 사용한 프론트엔드에 편입할 수 있다. 

## 3. 랭체인을 이용한 애플리케이션

### PDF에 대해 질문할 수 있는 챗봇.

- Retrieval 모듈을 사용하면 RAG 기법을 통해 업로드한 PDF 정보를 기반으로 응답할 수 있다.

### 파일 상호작용 및 인터넷 검색이 가능한 챗봇

- Agents 모듈 활용.
- ReAct 기법과 OpenAI Function Calling이라는 기법을 사용한다.

> #### 참고
> - ChapGPT Plus 플랜에 가입하면 GPT-4와 플러그인 기능을 사용할 수 있다.
> - 브라우징 플러그인(정보 검색 결합해 최신 정보 바탕), 익스피디아 플러그인(여행 계획), PDF를 업로드하고 그 내용에 대해 질문하는 플러그인
> - 챗지피티 플러그인과 랭체인의 차이점은 지피티는 일반 사용자도 사용할 수 있지만, 랭체인은 개발자를 위한 도구라는 점이다.
> - 랭체인을 이용하면 구글 캘린더, 지메일에서 정보를 가져와 매일 아침 Slack에서 오늘 일정과 할 일 목록을 제안하는 것도 가능해진다.

## 4. 실습 준비

#### 파이썬 환경 구축

```shell
$ brew install python3@3.13
```

#### MacOS에 OpenAI 환경 설정

- OpenAI api-key 발급받기(https://platform.openai.com/docs/overview)

```shell
$ vim ~/.zshrc
```

- OPENAI_API_KEY 환경 변수를 추가해주자.

```shell
export OPENAI_API_KEY="API키"
```

```shell
$ source ~/.zshrc
$ echo $OPENAI_API_KEY
```

#### 랭체인에 필요한 라이브러리 설치

```shell
$ brew install wget
$ wget https://raw.githubusercontent.com/wikibook/langchain/master/requirements.txt
$ python3 -m pip install --upgrade pip --break-system-packages
$ pip install --upgrade pip
$ python3 -m pip install -r requirements.txt
$ python3 -m pip install openai==0.28
```

## 5. OpenAI의 API를 호출해 작동을 확인

# 참고자료

- 랭체인 완벽 입문, 타무라 하루카 저
