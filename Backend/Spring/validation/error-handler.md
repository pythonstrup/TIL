# Spring Validation의 에러 처리

## 정확한 예외 처리의 중요성

- 만약 각 상황에 맞는 정확한 예외 처리를 하지 않으면 어떤 일이 발생할까? 
- 500 코드가 남발되어 모든 문제가 서부 내부의 문제처럼 보일 수 있다.
- 결과적으로 프론트엔드에서도 API를 통한 개발이 어려워질 수 있다.
  - Swagger와 같은 API 문서에 필수값을 지정하더라도 개발자는 사람이기 때문에 실수할 수 있다. 
  - 만약 필수값을 빼먹었을 때, 그냥 500 에러가 발생한다면 프론트엔드 개발자는 이 에러가 왜 발생하는지 서버 개발자에게 물어보는 수밖에 없다.
- 하지만 Bad Request에 대한 정확한 예외 코드와 메시지를 받을 수 있다면 실수할 여지가 줄어들게 되는 효과가 있다.

```
400: "필수 필드 'username'이 누락되었습니다."
401: "유효한 인증 토큰이 필요합니다."
404: "요청한 리소스를 찾을 수 없습니다."
```

## 스프링 6.1의 변화

- 스프링의 버전을 업그레이드했더니 ControllerTest에서 에러가 발생하기 시작한다.
- Spring Validation에 대한 예외 테스트들에게 4xx가 아닌 5xx를 뱉고 있었다. 이유가 뭘까?
- 그 이유는 이번에 새롭게 추가된 `HandlerMethodValidationException` 예외 때문이다.

# 코드 예시

## 코드 세팅

```java
public class ApiErrorResponse {

  private HttpStatus status;
  private String message;
  private List<String> errors;

  private ApiErrorResponse(HttpStatus status, String message, List<String> errors) {
    this.status = status;
    this.message = message;
    this.errors = errors;
  }

  public static ApiErrorResponse of(HttpStatus status, String message, List<String> errors) {
    return new ApiErrorResponse(status, message, errors);
  }

  public static ApiErrorResponse of(HttpStatus status, String message, String error) {
    return new ApiErrorResponse(status, message, Arrays.asList(error));
  }
}
```

```java
@Slf4j
@RestControllerAdvice
public class GlobalExceptionHandler {
  
  /* Custom ExceptionHandler 추가 예정 */

  /* 기본 ExceptionHandler - 500 Internal Server Error */
  @ExceptionHandler({Exception.class})
  public ResponseEntity<Object> handleInternalServerError(final RuntimeException e) {
    log.error("Uncontrolled Exception ", e);
    String defaultMessage = "알 수 없는 오류가 발생했어요. 고객센터로 문의해주세요.";
    ApiErrorBody body = ApiErrorBody.of(HttpStatus.INTERNAL_SERVER_ERROR, defaultMessage, e.getMessage());
    return new ResponseEntity<>(body, new HttpHeaders(), HttpStatus.INTERNAL_SERVER_ERROR);
  }
}
```

## MethodArgumentNotValidException

- `@Valid` 또는 `@Validated`가 메서드의 요청 본문(RequestBody)을 검증할 때 발생하는 예외이다.
- 주로 `@RequestBody`에 매핑된 DTO 객체의 유효성 검증에서 발생한다.
- Spring의 Bean Validation API(JSR 380)를 기반으로 동작하며, `jakarta.validation`(구 `javax.validation`)의 표준 어노테이션(`@NotNull`, `@Size` 등)을 사용한다.

```java
@PostMapping("/users")
public ResponseEntity<UserResponse> createUser(@Valid @RequestBody UserRequest userRequest) {
    UserResponse response = userService.createUser(userRequest);
    return ResponseEntity.ok(response);
}
```

```java
public record UserRequest(
   @NotBlank String username,
   @NotBlank String email
) {}
```

- 만약 위의 예시에서 `username`이나 `email` 필드의 요청 값을 빈 값으로 보냈다면 `MethodArgumentNotValidException`가 발생할 것이다.
- 해당 예외를 처리하지 않으면 기본 에러 핸들러에 의해 500 에러로 처리되기 때문에 400 Bad Request 예외 처리를 위해 아래와 같이 핸들러를 추가할 수 있다.

```java
@Slf4j
@RestControllerAdvice
public class GlobalExceptionHandler {
  
  @ExceptionHandler(MethodArgumentNotValidException.class)
  public ResponseEntity<Object> handleValidationException(
      final MethodArgumentNotValidException e, WebRequest request) {
    List<String> errors =
        Stream.concat(
                e.getBindingResult().getFieldErrors().stream()
                    .map(error -> error.getField() + ": " + error.getDefaultMessage()),
                e.getBindingResult().getGlobalErrors().stream()
                    .map(error -> error.getObjectName() + ": " + error.getDefaultMessage()))
            .collect(Collectors.toList());

    ApiErrorBody body = ApiErrorBody.of(HttpStatus.BAD_REQUEST, e.getLocalizedMessage(), errors);
    return new ResponseEntity<>(body, new HttpHeaders(), HttpStatus.BAD_REQUEST);
  }
}
```

## ConstraintViolationException

- `@PathVarible`과 `@RequestParam`에 대한 검증을 진행하면 어떤 에러가 발생할까?
    - ConstraintViolationException 타입니다.
- 메소드 레벨에서 검증하는 경우(`@Valid`를 통해 따라서 객체 안에서 검증하진 않는 경우)에 대한 예외처리 시 발생하는 에러다.
- 만약 아래 예시에서 `userId` 값으로 0 이하의 값이 들어온다면 `ConstraintViolationException` 에러가 발생할 것이다.

```java
@PostMapping("/users/{userId}")
public ResponseEntity<UserResponse> getUser(@Positive @PathVariable Long userId) {
    UserResponse response = userService.getUser(userId);
    return ResponseEntity.ok(response);
}
```

- 마찬가지로 GlabalExceptionHanlder에 등록해주지 않으면 500으로 처리될 것이기 때문에 400 Bad Request 처리를 위해 아래와 같이 `ConstraintViolationException`를 선언해줘야 한다. 

```java
@Slf4j
@RestControllerAdvice
public class GlobalExceptionHandler {

  @ExceptionHandler({ConstraintViolationException.class})
  public ResponseEntity<Object> handleConstraintViolation(
      ConstraintViolationException e, WebRequest request) {
    List<String> errors =
        e.getConstraintViolations().stream()
            .map(
                violation ->
                    violation.getRootBeanClass().getName()
                        + " "
                        + violation.getPropertyPath()
                        + ": "
                        + violation.getMessage())
            .collect(Collectors.toList());

    ApiErrorBody body = ApiErrorBody.of(HttpStatus.BAD_REQUEST, e.getLocalizedMessage(), errors);
    return new ResponseEntity<>(body, new HttpHeaders(), HttpStatus.BAD_REQUEST);
  }
}
```

## HandlerMethodValidationException

- 하지만 스프링 6.1에서 새롭게 도입된 메소드 레벨의 검증 예외가 새롭게 나왔다. `HandlerMethodValidationException`이다.
- 아래와 같이 Handler를 만들어줘야 한다.

```java
@Slf4j
@RestControllerAdvice
public class GlobalExceptionHandler {

  @ExceptionHandler(HandlerMethodValidationException.class)
  public ResponseEntity<Object> handleMethodValidationException(HandlerMethodValidationException e, WebRequest request) {
    List<String> errors = e.getAllValidationResults().stream()
        .map(ParameterValidationResult::getResolvableErrors)
        .flatMap(List::stream)
        .map(MessageSourceResolvable::getDefaultMessage)
        .peek(log::error)
        .toList();
    ApiErrorBody body = ApiErrorBody.of(HttpStatus.BAD_REQUEST,
        e.getAllErrors().get(0).getDefaultMessage(), errors);
    return new ResponseEntity<>(body, new HttpHeaders(), HttpStatus.BAD_REQUEST);
  }
}
```

# 참고자료

- [Custom Error Message Handling for REST API](https://www.baeldung.com/global-error-handler-in-a-spring-rest-api)
- [Validation 예외 핸들러로 응답 포맷 수정하기](https://velog.io/@gongmeda/Validation-%EC%98%88%EC%99%B8-%ED%95%B8%EB%93%A4%EB%9F%AC%EB%A1%9C-%EC%9D%91%EB%8B%B5-%ED%8F%AC%EB%A7%B7-%EC%88%98%EC%A0%95%ED%95%98%EA%B8%B0)
