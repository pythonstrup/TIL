# Spring에서 RabbitMQ 사용하기

## Spring Dependency

- 아래 코드를 build.gradle에 추가해준다. (RabbitMQ 라이브러리)
- 여기서 AMQP는 Advanced Message Queuing Protocol의 약자이다. 

```groovy
implementation 'org.springframework.boot:spring-boot-starter-amqp'
```

## Spring Configuration

- RabbitMQ의 요소들을 @Bean을 통해 등록해 애플리케이션에서 사용가능하도록 한다.
- Queue, Exchange, RabbitTemplate 등을 설정해준다.
- 멀티 모듈을 사용할 경우 아래와 같이 추상클래스로 템플릿화하여 사용하면 코드 중복을 줄일 수 있으니 참고.

```java
@Configuration
public class RabbitmqConfig {

  private final String exchangeName;

  private final String queueName;

  private final String routerKey;

  public RabbitmqExchangeQueueConfigTemplate() {
    this.exchangeName = "my.exchange";
    this.queueName = "my.queue";
    this.routerKey = "my.key";
  }

  @Bean
  public Queue queue() {
    return new Queue(queueName);
  }

  @Bean
  public TopicExchange exchange() {
    return new TopicExchange(exchangeName);
  }

  @Bean
  public Binding binding(Queue queue, TopicExchange exchange) {
    return BindingBuilder.bind(queue).to(exchange).with(routerKey);
  }

  @Bean
  RabbitTemplate rabbitTemplate(
      ConnectionFactory connectionFactory, MessageConverter messageConverter) {
    RabbitTemplate rabbitTemplate = new RabbitTemplate(connectionFactory);
    rabbitTemplate.setMessageConverter(messageConverter);
    return rabbitTemplate;
  }

  @Bean
  MessageConverter messageConverter() {
    return new Jackson2JsonMessageConverter();
  }
}

```

## Provider

- Exchange에 이벤트를 전달해야 Queue로 전파된다. 
- Exchange에 정보를 넣기 위해 RabbitTemplate를 사용한다.
- 아래와 같이 서비스단에서 Exchange 이름과 라우팅키, 전달하려는 메시지를 전달할 수 있다.

```java
@Service
public class MyService {

  private final String exchangeName;
  private final String routerKey;
  
  public PublicPrescriptionPharmacyService() {
    this.exchangeName = "my.exchange";
    this.routerKey = "my.key";
  }
  
  public void send() {
    ...
    ...
    ...
    
    rabbitTemplate.convertAndSend(exchangeName, routerKey, message);
  }
} 
```

## Consumer

- `@RabbitListener` 어노테이션을 통해 queue를 지정하고 이벤트를 받아올 수 있다.
- Config에서 작성한 Queue 이름과 일치해야 하고 Provider에서 발송 라우터키와 Config에 작성된 라우터키가 동일해야 이벤트 리스너가 정상작동한다. (Config의 내용에 따라 Exchange나 기타 설정이 달라질 수 있다는 점 유의) 

```java
@Slf4j
@Component
public class RabbitmqConsumer {

  @RabbitListener(queues = "my.queue")
  public void consumePublicPrescription(Object message) {
    log.info("result={}", message);
  }
}
```