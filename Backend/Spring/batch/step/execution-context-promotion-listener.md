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

# 다른 방법

## 1. JobExecution에 바로 저장해버리기

- 간단하고 실용적인 방법이다.
- 하지만 이 방식은 위에서 간단히 얘기했다시피 추천하지 않는 방식이다. 그 이유는 아래와 같다.

1. Step 실행 중 JobExecutionContext에 값을 저장하더라도 Step이 실패하는 경우 데이터가 유실될 수 있다.
2. 이 방법은 Step이 JobExecutionContext에 강결합하게 된다. 따라서 다른 Job에서 Step 구현체를 재사용하기 어려워진다.

### 코드 예시

- 간단하다. 
- 데이터를 저장할 Step에서 JobExecutionContext에 데이터를 저장하고 다음 스텝에서 JobExecutionContext에서 데이터를 가져와 사용하면 된다.

```java
@Bean
public Step memberStep() {
  return stepBuilderFactory.get("memberStep")
    .tasklet((contribution, chunkContext) -> {
      StepContext stepContext = chunkContext.getStepContext();
      JobExecution jobExecution = stepContext.getStepExecution().getJobExecution();
      Member member = memberService.getMember();
      jobExecution.getExecutionContext().put("key", member);  
      return RepeatStatus.FINISHED;
    })
    .build();
}

@Bean
public Step orderStep() {
  return stepBuilderFactory.get("orderStep")
      .tasklet((contribution, chunkContext) -> {
        StepContext stepContext = chunkContext.getStepContext();
        JobExecution jobExecution = stepContext.getStepExecution().getJobExecution();
        Member member = (Member) jobExecution.getExecutionContext().getString("dataKey");
        
        ...
        
        return RepeatStatus.FINISHED;
      })
      .build();
}
```

## 2. Singleton Bean 사용하기

- Step 간에 데이터를 공유할 때 ExecutionContext를 사용하면 발생하는 치명적인 문제가 하나있다. 
- 바로 ExecutionContext에 저장할 객체는 직렬화된다는 것이다. 
- 이게 왜 치명적인 문제냐? ExecutionContext에 데이터를 저장하면서 json string 형태로 변환하는데 이 비용이 생각보다 크다고 한다.
- ExecutionContext에 담을 객체가 크지 않다면 별로 문제가 되지 않지만 크기가 너무 크다면 성능 이슈가 발생할 수 있다. 
- 이 때 사용할 수 있는 것이 스레드 세이프한 자료구조를 가진 Singleton Bean이다.

### 코드 예시

```java
@Component
public class SharedData<T> {

  private final Map<String, T> values = new ConcurrentHashMap<>();

  public void putData(String key, T data) {
    values.put(key, data);
  }

  public T getData (String key) {
    return values.get(key);
  }

  public int getSize () {
    return values.size();
  }
}
```


# 참고자료

- [Spring Batch 공식문서](https://docs.spring.io/spring-batch/reference/common-patterns.html#passingDataToFutureSteps)
