# RestClient

## `content type`이 `application/x-www-form-urlencoded`일 때 처리

- 아래와 같이 `RestClient`를 구성하고 자바 Object로 요청을 보내면 에러가 발생한다.

```java
DefaultUriBuilderFactory uriBuilderFactory = new DefaultUriBuilderFactory(naverCommerceUrl);
RestClient.builder()
    .uriBuilderFactory(uriBuilderFactory)
    .defaultHeader(HttpHeaders.CONTENT_TYPE, "application/x-www-form-urlencoded;charset=utf-8")
    .build();
```

- `ContentType`을 `application/x-www-form-urlencoded;charset=utf-8`로 사용 시, content를 전송하려면  `MultiValueMap` 형태로 전해줘야 한다. 
- `HttpMessageConverter`가 `application/x-www-form-urlencoded;charset=utf-8` 타입을 만나면 `FormHttpMessageConverter`로 지정된다.
- `FormHttpMessageConverter`는 `MultiValueMap`을 받아야 변환 작업을 할 수 있다.
- [스프링 공식문서](https://docs.spring.io/spring-framework/reference/integration/rest-clients.html#rest-message-conversion)

> An HttpMessageConverter implementation that can read and write form data from the HTTP request and response. 
> By default, this converter reads and writes the application/x-www-form-urlencoded media type. 
> Form data is read from and written into a MultiValueMap<String, String>. 
> The converter can also write (but not read) multipart data read from a MultiValueMap<String, Object>. 
> By default, multipart/form-data is supported. 
> Additional multipart subtypes can be supported for writing form data. 
> Consult the javadoc for FormHttpMessageConverter for further details.

## % 인코딩 문제

### DefaultUriBuilderFactory 설명

**DefaultUriBuilderFactory** 구현은 내부적으로 **UriComponentsBuilder**를 사용하여 URI 템플릿을 확장하고 인코딩합니다. Factory로서 아래 인코딩 모드 중 하나를 기반으로 인코딩 접근 방식을 구성할 수 있는 단일 위치를 제공합니다.

- **TEMPLATE_AND_VALUES**: 이전 목록의 첫 번째 옵션에 해당하는 **UriComponentsBuilder#encode()**를 사용하여 URI 템플릿을 미리 인코딩하고 확장될 때 URI 변수를 엄격하게 인코딩합니다.
- **VALUES_ONLY**: URI 템플릿을 인코딩하지 않고 대신 템플릿으로 확장하기 전에 **UriUtils#encodeUriVariables**를 통해 URI 변수에 엄격한 인코딩을 적용합니다.
- **URI_COMPONENT**: 이전 목록의 두 번째 옵션에 해당하는**UriComponents#encode()**를 사용하여 URI 변수가 확장된 후 URI 구성 요소 값을 인코딩합니다.
- **NONE**: 인코딩이 적용되지 않습니다.

### 코드 수정

`DefaultUriBuilderFactory`의 `EncodingMode`를 `None으`로 설정해주면 `%` 인코딩 문제가 해결된다.

- 수정 전

```java
private RestClient createRestClient(String baseUrl) {
  return RestClient.builder()
      .baseUrl(baseUrl)
      .defaultHeader(HttpHeaders.CONTENT_TYPE, MediaType.APPLICATION_JSON_VALUE)
      .build();
}
```

- 수정 후

```java
private RestClient createRestClient(String baseUrl) {
  DefaultUriBuilderFactory uriBuilderFactory = new DefaultUriBuilderFactory(baseUrl);
  uriBuilderFactory.setEncodingMode(EncodingMode.NONE);
  return RestClient.builder()
      .uriBuilderFactory(uriBuilderFactory)
      .defaultHeader(HttpHeaders.CONTENT_TYPE, MediaType.APPLICATION_JSON_VALUE)
      .build();
}
```

### EncodingMode.NONE을 사용해서 문제될 건 없을까?

- `RequestParam이나` `PathVariable에` 특수문자나 한글이 왔을 때 인코딩 문제가 생길 수 있다.
- 그럼 어떻게???
    - `RequestParam`으로 한글을 전달하고 싶다면 변수로 전달하기 전에 인코딩해서 전달하면 된다!!
