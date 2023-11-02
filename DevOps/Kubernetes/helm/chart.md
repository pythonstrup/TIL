# 차트 작성

## Template

- `templates/` 디렉토리는 다음과 같이 구조화되어야 한다.
  - YAML을 만드는 템플릿 확장자가 `.yaml`을 사용해야 한다.
  - 형식이 정해지지 않은 컨텐츠를 만드는 템플릿 파일에는 `.tpl` 확장자를 사용할 수 있다.
  - 리소스 정의는 자체 템플릿 파일 내에 있어야 한다.
  - 템플릿 파일 이름은 대시 표기법을 사용하고! 이름 내에서 리소스 종류를 반영해야한다. 예를 들어 `doctor-deployment.yaml`, `doctor-service.yaml`


### 템플릿 형식

- 템플릿 지시문에서 여는 중괄호 뒤와 닫는 중괄호 앞에는 공백이 있어야 한다.
- 대신 공백은 최소화! 필요할 때 한 번만 넣자!

```yaml
{{ .foo }}
{{ print "foo" }}
{{- print "bar" -}}
```

### 템플릿의 화이트스페이스

- 가장 좋은 경우는 빈줄이 없는 템플릿이다.

```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: example
  labels:
    first: first
    second: second
```

### 주석

- YAML 주석

```yaml
# comment
```

- 템플릿 주석
  - 정의된 템플릿 설명과 같이 템플릿의 기능을 문서화할 때는, 템플릿 주석을 사용해야 한다.

```yaml
{{- /*
comment
*/}}
```

<br/>

## Tip

### 쌍따옴표

- 문자열에는 따옴표를 쓰고, 정수형에는 쓰지 않는다.
- 아래와 같은 문자열 데이터를 사용할 때는 Value에서 쌍따옴표로 묶는 것이 안전!

```yaml
name: {{ .Values.name | quote }}
```

- 정수형의 경우, 쿠버네티스에서 파싱 에러가 많이 발생할 수 있다! 쌍따옴표 사용 X

```yaml
name: {{ .Values.Port }}
```

### include 함수

- 다른 템플릿으로부터 템플릿을 가져오고 그 템플릿의 출력 값에 연산을 수행할 수 있게 한다.
- 아래의 경우, `myYamlFile`이라는 템플릿이 포함되어 있다. `$value`에 값을 전달하고 그 출력 값을 indent 함수에 전달한다.

```yaml
{{ include "myYamlFile" $value | indent 2 }}
```

### required 함수

- required 함수는 개발자가 템플릿이 랜더링될 때 필수로 입력되어야 하는 값(항목)을 선언한다.
- 필수값이 비어있다면 required 뒤에 적혀있는 에러메시지를 반환한다.

```yaml
{{ required "Error!" .Values.name }}
```

### tpl 함수

- `tpl` 함수를 이용하여 템플릿 내에 정의 된 템플릿 형식의 문자열의 렌더링 값을 구할 수 있다.
- 이 함수는 차트에 템플릿 문자열을 변수로 전달하거나 외부 설정 파일들을 랜더링 할때 유용하다.

```yaml
# values
template: "{{ .Values.name }}"
name: "Park"

# template
{{ tpl .Values.template . }}
```

- 위의 values로 template를 실행하면 결과값이 아래와 같다.

```
Park
```

<br/>


# 참고자료

- [차트 개발 팀과 비법](https://helm.sh/ko/docs/howto/charts_tips_and_tricks/)
- [템플릿](https://helm.sh/ko/docs/chart_best_practices/templates/)