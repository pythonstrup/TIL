# RabbitMQ tutorial - "Hello World!"

- [RabbitMQ Tutorials](https://www.rabbitmq.com/tutorials)

## Introduction

- RabbitMQ는 메세지 브로커다.
  - 예를 들어 편지를 주고 받는 상황이라면 RabbitMQ는 우편함, 우체국, 집배원의 역할을 한다.
- 다른 점은 종이 대신 데이터의 binary blobs 형태로 메시지를 수신, 저장, 전달한다는 점이다.

### 비유

- 우편함: `queue`, 대기열은 호스트의 메모리 및 디스크 제한에 의해서만 제한되며, 기본적으로 큰 메시지 버퍼다.
  - 다수의 `producers`가 하나의 `queue`에 메시지를 보낼 수 있다. 그리고 다수의 `consumers`가 하나의 `qeueu`로부터 데이터를 받을 수 있다.
  - 여기서 `consuming`은 `receiving`과 비슷한 의미다.
- 우체국: `exchange`
- 집배원: `binding`, `routing key`

## 시작하기 - Spring AMQP

- RabbitMQ는 다중 프로토콜 메시지 브로커다.
  - [Which protocols does RabbitMQ support?](https://www.rabbitmq.com/docs/protocols) 
  - 기본적으로 `AMQP 0-9-1` 프로토콜이 핵심이다.
  - 그 외에 `AMQP 1.0`, `RabbitMQ Streams`, `MQTT`, `STOMP`, `HTTP`, `WebSockets` 등을 지원한다.

### 간단한 프로젝트 설정

- named `Queue`를 만들어보자.

```java
@Configuration
public class AMQPConfig {
  
  @Bean
  public Queue hello() {
    return new Queue("hello");
  }
}
```

- Spring AMQP를 사용하면 별도의 설정 없이 바로 `RabbitTemplate`를 주입받아 사용할 수 있다.

```java
@Component
@RequiredArgsConstructor
public class MySender {
  
  private final RabbitTemplate template;
  
  private final Queue queue;
  
  @Scheduled(fixedDelay = 1000, initalDelay = 500)
  public void send() {
    String message = "Hello World!";
    this.template.convertAndSend(queue.getName(), message);
  }
}
```

- 수신하는 쪽은 `@RabbitListener`와 `@RabbitHandler`를 활용하여 메시지를 받는다.

```java
@RabbitListener(queues = "hello")
public class MyReceiver {
  
  @RabbitHandler
  public void receive(Sring in) {
    // ...
  }
}
```

