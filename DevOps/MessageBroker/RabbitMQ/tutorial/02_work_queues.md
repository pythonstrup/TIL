# RabbitMQ tutorial - Work Queues

- [링크](https://www.rabbitmq.com/tutorials/tutorial-two-spring-amqp)

- 이전 예시에서는 named queue로부터 메시지를 수신하는 프로그램을 작성했다. 이번엔 시간이 많이 소요되는 작업을 분배할 때 사용하는 `작업 대기열 Work Queue`를 생성할 것이다.
- `Work Queue`는 리소스 집약적인 작업을 즉시 수행하지 않고 완료될 때까지 기다리지 않는다.
  - 대신 나중에 작업을 수행하도록 예약한다. 작업을 메시지로 캡슐화하여 대기열로 보낸다.
  - 백그라운드에서 실행되는 작업자 프로세스가 작업을 꺼내면(pop) 작업을 실행한다.
  - 많은 `workers`를 실행하면 작업이 공유된다.

