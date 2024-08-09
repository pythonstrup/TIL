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

### 이슈 2. artifactId가 zeppelin-interpreter인 값만 가져오는 것이 맞는가?

- helium.json에는 maven의 여러 의존성을 가지고 있는데 4가지 값만 가져옴

```shell
{
  'zeppelin-jupyter-interpreter': {
    latest: {
      type: 'INTERPRETER',
      name: 'zeppelin-jupyter-interpreter',
      version: '0.11.1',
      published: '2024-03-29T15:27:07+09:00',
      artifact: 'zeppelin-jupyter-interpreter@0.11.1',
      description: 'Zeppelin: Jupyter Interpreter',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-jupyter-interpreter'
    },
    '0.11.0': {
      type: 'INTERPRETER',
      name: 'zeppelin-jupyter-interpreter',
      version: '0.11.0',
      published: '2024-02-16T21:23:35+09:00',
      artifact: 'zeppelin-jupyter-interpreter@0.11.0',
      description: 'Zeppelin: Jupyter Interpreter',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-jupyter-interpreter'
    }
  }
}
{
  'zeppelin-zengine': {
    latest: {
      type: 'INTERPRETER',
      name: 'zeppelin-zengine',
      version: '0.11.1',
      published: '2024-03-29T15:29:09+09:00',
      artifact: 'zeppelin-zengine@0.11.1',
      description: 'Zeppelin Zengine',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-zengine'
    },
    '0.11.0': {
      type: 'INTERPRETER',
      name: 'zeppelin-zengine',
      version: '0.11.0',
      published: '2024-02-16T21:26:44+09:00',
      artifact: 'zeppelin-zengine@0.11.0',
      description: 'Zeppelin Zengine',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-zengine'
    }
  }
}
{
  'zeppelin-interpreter-parent': {
    latest: {
      type: 'INTERPRETER',
      name: 'zeppelin-interpreter-parent',
      version: '0.11.1',
      published: '2024-03-29T15:25:35+09:00',
      artifact: 'zeppelin-interpreter-parent@0.11.1',
      description: 'Zeppelin: Interpreter Parent',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-interpreter-parent'
    },
    '0.11.0': {
      type: 'INTERPRETER',
      name: 'zeppelin-interpreter-parent',
      version: '0.11.0',
      published: '2024-02-16T21:21:32+09:00',
      artifact: 'zeppelin-interpreter-parent@0.11.0',
      description: 'Zeppelin: Interpreter Parent',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-interpreter-parent'
    }
  }
}
{
  'zeppelin-cassandra': {
    latest: {
      type: 'INTERPRETER',
      name: 'zeppelin-cassandra',
      version: '0.11.1',
      published: '2024-03-29T15:22:26+09:00',
      artifact: 'zeppelin-cassandra@0.11.1',
      description: 'Zeppelin cassandra support',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-cassandra'
    },
    '0.11.0': {
      type: 'INTERPRETER',
      name: 'zeppelin-cassandra',
      version: '0.11.0',
      published: '2024-02-16T21:17:47+09:00',
      artifact: 'zeppelin-cassandra@0.11.0',
      description: 'Zeppelin cassandra support',
      license: 'Apache-2.0',
      icon: '<i class="fa fa-rocket"></i>',
      groupId: 'org.apache.zeppelin',
      artifactId: 'zeppelin-cassandra'
    }
  }
}
```

# fetchHelium

### 이슈 1. `npm-pkg-searchby-dependency` deprecated

- API가 deprecated되었다.
- 아래의 직접 API를 호출하는 방법으로 수정

```
https://api.npms.io/v2/search?q=zeppelin-vis
https://api.npms.io/v2/search?q=zeppelin-spell
```


# 중간 점검

### 멘토님 질문

1. https://github.com/apache/zeppelin/blob/master/conf/interpreter-list 링크의 모든 의존성을 가져올 수 있으면 됨.
2. helium.json을 굳이 가져와서 업데이트하는 방식이 아니라 새로운 파일을 구성하는 방식으로 해야함!


# helium.json 테스트

```xml
<property>
  <name>zeppelin.helium.registry</name>
  <value>helium,file://${ZEPPELIN_HOME}/conf/my-helium.json</value>
  <description>Location of external Helium Registry</description>
</property>
```

### helium.json이 배열이어서 에러가 발생한다.

- `enabled`, `packageConfig`와 `bundleDisplayOrder` 키 값으로 가지고 있는 객체가 필요하다.
- 현재 그냥 `helium.json`을 받아오면 에러가 발생한다. 
- 알고보니 `conf/helium.json`과 `zeppelinServer`의 `resources`에서 사용하는 `helium.json`의 형태가 다른듯 하다.

### helium 설치 안 되는 이슈

- helium.json 작업 진행 중에 NPM 의존성이 설치되지 않는 이슈를 일요일에 공유해드렸었는데요. 아래와 같은 과정을 거쳐 설치에 성공했습니.
- helium의 node version과 개발 환경인 Mac OS(M1)과 호환이 되지 않아 발생한 것으로 보입니다. 

```java
private static final String NODE_VERSION = "v6.9.1";
private static final String NPM_VERSION = "3.10.8";
private static final String YARN_VERSION = "v0.21.3";
```

- helium에서 사용하는 라이브러리를 확인해보니 현재 사용하고 있는 OS를 파악해서 자동으로 설치할 노드의 url을 구성하고 있었습니다.
- `https://nodejs.org/dist/v6.9.1/node-v6.9.1-darwin-arm64.tar.gz`라는 경로로 접근하려고 하는데 실제로 해당 경로에 파일이 존재하지 않습니다.
   - 해당 버전에서는 darwin-arm64를 지원하지 않는 것 같습니다.
- 따라서 아래와 같이 버전을 올려봤습니다.

```java
private static final String NODE_VERSION = "v16.16.0";
private static final String NPM_VERSION = "8.11.0";
private static final String YARN_VERSION = "v1.22.0";
```

- 버전을 올리니 다른 예외가 발생했습니다. 
  - `HeliumBundleFactory.java`에서 result가 null 값으로 반환되어 에러가 발생하길래 일단 실행되도록 예외처리를 해줬습니다.
- 조치를 취한 후 설치에 성공했습니다.


- `HeliumBundleFactory.java` 409번째 줄의 코드를 아래와 같이 수정해봤습니다.

```java
if (result != null && result.errors.length > 0) {
  FileUtils.deleteQuietly(heliumBundle);
  throw new IOException(result.errors[0]);
}
```

### HeliumBundleFactory 버전 올리기

- 아래는 설치 결과값입니다. `\n`은 실제 줄바꿈으로 대체했습니다.

```shell
"Running 'yarn run bundle --registry=https://registry.npmjs.org/' in
./local-repo/helium-bundle/bundles/sogou-map-vis-linkid
yarn run v1.22.0
$ webpack --display-error-details --json --registry=https://registry.npmjs.org/
{
  "errors": [],
  "warnings": [],
  "version": "1.15.0"
  ....
```

- 옵션이 `--json`으로 끝나지 않습니다.
- 그래서 `String.endsWith()`를 `String.contains()`로 변경했습니다.
  - 그러면 json이 잘 읽힙니다.

```java
if (!webpackRunDetected) {
  String trimed = line.trim();
  if (trimed.contains("webpack") && trimed.endsWith("--json")) {
    webpackRunDetected = true;
  }
  continue;
}
```

- 변경 후 코드

```java
if (!webpackRunDetected) {
  String trimed = line.trim();
  if (trimed.contains("webpack") && trimed.contains("--json")) {
    webpackRunDetected = true;
  }
  continue;
}
```

### 그런데 테스트에서 에러 발생

- Timeout 에러가 발생하는데 원인을 찾아봐야할 것 같습니다.
- 기존 master 형상에서는 에러가 발생하지 않고 있거든요.

```shell
Error:  Errors: 
Error:    ZeppelinIT.testAngularRunParagraph:337->AbstractZeppelinIT.clickAndWait:196->AbstractZeppelinIT.pollingWait:161 
        » Timeout Expected condition failed: waiting for org.apache.zeppelin.AbstractZeppelinIT$$Lambda$1046/0x00000001007c3440@59d0f14b 
        (tried for 30 second(s) with 1000 milliseconds interval)
[INFO] 
Error:  Tests run: 39, Failures: 0, Errors: 1, Skipped: 4
[INFO] 
[INFO] 
[INFO] --- maven-failsafe-plugin:3.2.2:verify (default) @ zeppelin-integration ---
[INFO] ------------------------------------------------------------------------
[INFO] BUILD FAILURE
[INFO] ------------------------------------------------------------------------
[INFO] Total time:  24:01 min
[INFO] Finished at: 2024-08-09T12:44:45Z
[INFO] ------------------------------------------------------------------------
```
