# Publish/Subscribe

이번 튜토리얼에서는 하나의 메시지를 여러 소비자(컨슈머)에게 동시에 전달하는 `Publish/Subscribe` 또는 'Fanout' 패턴

## 발행 & 구독

- 이전 장의 작업 큐.
  - 기본 가정은 각 작업이 정확히 하나의 `워커 worker`에게만 전달된다는 것.
- `Fanout` 패턴을 구현하여 여러 소비자에 메시지를 전달할 수 있다.
  - 이 패턴은 `Publish/Subscribe`으로 널리 알려져 있다.
  - 발행된 모든 메시지는 모든 `수신자 receiver`에게 브로드캐스트된다.

## Exchanges

- RabbitMQ 메시징 모델의 핵심 아이디어는 `생산자 Producer`가 메시지를 큐에 직접 보내지 않는다는 것이다.
  - 대신 `Exchange`에만 보낼 수 있다.
- `Exchange`는 생산자로부터 메시지를 받고, 그 메시지를 큐로 밀어 넣는다.
  - 따라서, 수신한 메시지를 어떻게 처리해야 할지 정확히 알아야 한다.
  - 특정 큐에 추가하거나, 여러 큐에 한 번에 추가할 수도 있다.
- 이러한 규칙은 `ExchangeType`에 의해 정의된다.
  - `direct`, `topic`, `headers`, `fanout`이 있다.
- `Fanout Exchange`는 간단하다.
  - 수신하는 모든 메시지를 자신이 알고 있는 모든 큐에 브로드캐스트한다.

> #### Exchange 목록 보기
> - `rabbitmqctl`을 통해 알 수 있다.
> ```bash
> sudo rabbitmqctl list_exchanges
> ```
> - 이 목록에는 몇몇 `amq.*` exchange와 기본 exchange가 있을 것이다.
> - 이들은 기본적으로 생성되지만, 지금 당장 사용할 필요는 거의 없다.

> #### nameless exchange
> - 이전 파트에서 exchange가 없었는데 큐로 메시지를 보낼 수 있었던 이유는, 기본 exchange가 있기 때문이다.
> - 각 큐의 이름은 바인딩 키로 사용하여 기본 exchange에 자동으로 바인딩된다.

## 임시 큐 Temporary queues

- 모든 메시지를 수신하고 싶고, 오래된 메시지가 아닌 현재 흐르고 있는 메시지에만 관심이 있는 상황이라고 해보자. 이를 위해선 2가지가 필요하다.

1. RabbitMQ에 연결할 때마다 새롭고 비어있는 큐가 필요하다. 이를 위해 임의의 이름으로 큐를 만들거나, 더 좋은 방법은 서버가 우리를 위해 임의의 큐 이름을 선택하도록 하는 것이다.
2. 소비자의 연결이 끊어지만 큐는 자동으로 삭제되어야 한다. 

- Spring AMQP 클라이언트에서는 이를 수행하기 위해, `AnonymousQueue`가 정의되어 있다.
  - 이는 임의로 생성된 이름(ex-`spring.gen-1Rx9HOqvTAaHeeZrQWu8Pg`)과 함께 `비영속적 non-durable`, `배타적 exclusive`, `자동 삭제 auto-delete` 큐를 만든다.
