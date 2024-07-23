# 실행 계획

1. `npm init`하고 필요한 의존성 모두 설치
2. AWS 의존성 떼어내기
    - `aws-sdk`
    - `s3-append`
3. 실행해보기

### 1. 의존성 설치

```shell
$ npm init
$ npm i bluebird
$ npm i moment
$ npm i json-stringify-pretty-compact
$ npm i npm-pkg-searchby-dependency 
$ npm i stream
$ npm i underscore
$ npm i xml2js
$ npm i node-fetch # request-promise 대용
$ npm i npm-registry-fetch # silent-npm-registry-client 대용
```

### 내장 라이브러리

- `url`
- `util`

# getMaven

### 이슈 1. `requset-promise`가 deprecated

- `node-fetch`로 수정해야함.

### 이슈 2. delay

- `requset-promise`에서 delay라는 체이닝 메소드를 지원했었는데 이제는 지원하지 않음.
- 직접 delay 코드를 만들어 주입.

### 이슈 3. HTTP 에러

```shell
501 HTTPS Required. 
Use https://repo1.maven.org/maven2/
More information at https://links.sonatype.com/central/501-https-required
```

- pomUri를 입력할 때 위와 같은 에러가 발생
- `https`로 요청으로 변경해줌

```javascript
const pomUri = ver.pomUri.replace('http', 'https')
```

# fetchHelium

### 이슈 1. `npm-pkg-searchby-dependency` deprecated

- npm-registry로 대체해보자.
