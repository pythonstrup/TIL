# 테스트 컨테이너 현명하게 사용하기

## 가장 빠른 방법

- DataSource Only 방법을 사용!

```yaml
spring:
  datasource:
    driver-class-name: org.testcontainers.jdbc.ContainerDatabaseDriver
    url: jdbc:tc:mysql:8://testDB
```

# 참고자료

- [Java TestContainer 재사용하기(feat. 테스트 실행시간 최적화)](https://jwkim96.tistory.com/275)
