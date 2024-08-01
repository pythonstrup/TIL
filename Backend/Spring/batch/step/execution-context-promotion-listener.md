# Step에서 생성한 정보를 다음 Step으로 전달하는 방법

- `ExecutionContext`의 종류는 크게 2가지다. `Step`과 `Job` 
- `Step`의 `ExecutionContext`는 하나의 `Step`에만 종속된다. `Step`이 끝나면 같이 사라진다는 얘기다.
- `Job`의 `ExecutionContext` 또한 `Job`과 수명을 함께 한다.
- 현재 진행하는 `Step`이 다음 진행할 `Step`에게 전달하고 싶다면 어떻게 하면 될까? `Job ExecutionContext`에 값을 저장하면 된다.
  - `Job ExecutionContext`는 모든 `Step`이 실행될 때까지 살아있으니깐!
- 이 아이디어를 가능하게 만드는 것이 `ExecutionContextPromotionListener`라는 객체다.

## ExecutionContextPromotionListener

- Spring Batch가 제공하는 객체다.
- Step이 완료되는 시점에 `StepExecutionContext` 중 `ExecutionContextPromotionListener` 설정한 키 값에 대해 `Job ExecutionContext`로 승격(promotion)시켜주는 구현체다. 

```java
@Bean
public ExecutionContextPromotionListener promotionListener() {
  ExecutionContextPromotionListener listener = new ExecutionContextPromotionListener();
  listener.setKeys(new String[] {"member"});
  return listener;
}
```

## 코드 예시

### 1. 먼저 Step ExecutionContext에 필요한 값을 저장하자.

- chunk-oriented로 `Step`을 구성하고 `itemReader`와 `itemWriter`를 활용해 저장할 수도 있지만 `Tasklet`을 사용해 간단하게 구성해보겠다.
- `@BeforeStep`을 통해 `Tasklet`이 실행되기 전, `Step Execution`을 가져온다.
- Tasklet이 실행될 때, 필요한 값을 구성해 `Step Execution`에 저장하도록 하자.

```java
@Bean("memberTasklet")
public Tasklet memberTasklet() {
  return new Tasklet() {

    private StepExecution stepExecution;

    @Override
    public RepeatStatus execute(
        StepContribution contribution, ChunkContext chunkContext) throws Exception {
      Member member = memberService.find();
      ExecutionContext stepContext = this.stepExecution.getExecutionContext();
      stepContext.put("member", member);
      return RepeatStatus.FINISHED;
    }

    @BeforeStep
    public void saveStepExecution(StepExecution stepExecution) {
      this.stepExecution = stepExecution;
    }
  };
}
```

- `Step`을 등록할 때 주의할 점! 
- `@BeforeStep`이나 `@AfterStep` 어노테이션을 사용하려면 `Step`의 `listener()` 메소드를 사용해 리스너로 등록해줘야 한다! 

```java
@Bean("memberStep")
public Step memberStep(
    final JobRepository jobRepository, final PlatformTransactionManager transactionManager) {
  return new StepBuilder("memberStep", jobRepository)
      .tasklet(memberTasklet(), transactionManager)
      .listener(memberTasklet())
      .listener(promotionListener())
      .build();
}
```

### 2. ExecutionContextPromotionListener의 역할

- `ExecutionContextPromotionListener`는 다음 Step으로 전달하고 싶은 키 값을 저장해 넘겨주면 된다.
- Step의 listener로 `ExecutionContextPromotionListener`를 등록해주면 된다.
- 이제 Step이 종료되면 자동으로 `Job Execution`에 구성한 값이 저장된다.

```java
@Bean
public ExecutionContextPromotionListener promotionListener() {
  ExecutionContextPromotionListener listener = new ExecutionContextPromotionListener();
  listener.setKeys(new String[] {"member"});
  return listener;
}
```

### 3. 값 꺼내서 사용하기

- 이제 다음 `Step`의 ItemWriter에서 `JobExecution`을 사용해 컨텍스트 값을 가져올 수 있다.

```java
@Bean("orderWriter")
public ItemWriter<Order> orderWriter() {

  return new ItemWriter<Order>() {
    private Member member;

    @Override
    public void write(final Chunk<? extends Order> chunk) throws Exception {
      orderService.registerOrder(this.member, chunk);
    }

    @BeforeStep
    public void retrieveInterStepData(StepExecution stepExecution) {
      final JobExecution jobExecution = stepExecution.getJobExecution();
      final ExecutionContext jobContext = jobExecution.getExecutionContext();
      this.member = (Member) jobContext.get("member");
    }
  };
}
```


# 참고자료

- [Spring Batch 공식문서](https://docs.spring.io/spring-batch/reference/common-patterns.html#passingDataToFutureSteps)
