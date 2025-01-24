# P6Spy

- P6Spy는 데이터베이스 활동을 intercept하여 로깅을 할 수 있는 툴이다.

## Installation

- 먼저 의존성을 추가해준다.

```kotlin
implementation("p6spy:p6spy:3.9.1")
```

- class path(resources 디렉토리)에 spy.properties 파일을 만들고 JDBC driver를 설정해준다.

```properties
driverlist=com.mysql.jdbc.Driver
```

- `application.yaml`이나 `application.properties`, 혹은 Configuration에서 Datasource를 설정해줄 때, URL을 약간 다르게 설정해줘야 한다.
  - driver class: `com.p6spy.engine.spy.P6SpyDriver`
  - url: `jdbc:p6spy:mysql://<hostname>:<port>/<database>`
- 아래는 application.yaml의 예시

```yaml
spring:

  datasource:
    url: jdbc:p6spy:mysql://${DATABASE_HOST}/${DATABASE_NAME}
    driver-class-name: com.p6spy.engine.spy.P6SpyDriver
```

## Spring Boot 지원 (라이브러리)

- `spring-boot-data-source-decorator`라는 라이브러리에서 spring boot starter를 지원한다. (공식문서에 언급)
- [spring-boot-data-source-decorator](https://github.com/gavlyukovskiy/spring-boot-data-source-decorator)

### 의존성 추가

- 아래 형태로 추가할 수 있다고 한다.

```kotlin
implementation("com.github.gavlyukovskiy:p6spy-spring-boot-starter:${version}")
```

### 데코레이터

- 모든 데코레이터를 동시에 사용할 수 있으며, 이 경우 데코레이터의 적용 순서는 다음과 같다.

`P6DataSource` -> `ProxyDataSource` -> `FlexyPoolDataSource` -> `DataSource`



### Connection 커스텀 이벤트 추가

- JDBC 연결과 관련된 이벤트가 발생했을 때 이를 가로채서 원하는 동작을 수행할 수 있다.

```java
@Bean
public JdbcEventListener myListener() {
    return new JdbcEventListener() {
        @Override
        public void onAfterGetConnection(ConnectionInformation connectionInformation, SQLException e) {
            System.out.println("got connection");
        }

        @Override
        public void onAfterConnectionClose(ConnectionInformation connectionInformation, SQLException e) {
            System.out.println("connection closed");
        }
    };
}
```

- 이것은 `RuntimeListenerSupportFactory`를 P6Spy 모듈 리스트에 추가함으로써 이뤄진다. 
  - 따라서 이 속성을 재정의하면 해당 팩토리가 등록되지 않으므로 리스너가 적용되지 않는다는 사실에 유의해야 한다.
- 몇 가지 파라미터는 `application.properties` 파일에서 설정할 수 있다.

> #### 참고
> 아래 설정은 사용 가능한 모든 파라미터와 기본값을 포함한 예제이며, 반드시 명시적으로 설정할 필요는 없다.

```properties
# Register P6LogFactory to log JDBC events
decorator.datasource.p6spy.enable-logging=true
# Use com.p6spy.engine.spy.appender.MultiLineFormat instead of com.p6spy.engine.spy.appender.SingleLineFormat
decorator.datasource.p6spy.multiline=true
# Use logging for default listeners [slf4j, sysout, file, custom]
decorator.datasource.p6spy.logging=slf4j
# Log file to use (only with logging=file)
decorator.datasource.p6spy.log-file=spy.log
# Class file to use (only with logging=custom). The class must implement com.p6spy.engine.spy.appender.FormattedLogger
decorator.datasource.p6spy.custom-appender-class=my.custom.LoggerClass
# Custom log format, if specified com.p6spy.engine.spy.appender.CustomLineFormat will be used with this log format
decorator.datasource.p6spy.log-format=
# Use regex pattern to filter log messages. If specified only matched messages will be logged.
decorator.datasource.p6spy.log-filter.pattern=
```


# 참고자료

- [P6Spy 공식문서](https://p6spy.readthedocs.io/en/latest/index.html)
- [spring-boot-data-source-decorator](https://github.com/gavlyukovskiy/spring-boot-data-source-decorator)
- [Intercept SQL Logging with P6Spy](https://www.baeldung.com/java-p6spy-intercept-sql-logging)
