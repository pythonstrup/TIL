# Tasklet 구현체만으로는 @BeforeStep을 사용할 수 없다.

- 아래와 같이 Tasklet에서 `@BeforeStep`이나 `@AfterStep`을 그냥 사용하려고 하면 아무 실행도 하지 못한다.

```java
@Configuration
public class MemberJobConfig {

  @Bean("memberJob")
  public Job memberJob(
      JobRepository jobRepository, PlatformTransactionManager transactionManager) {
    return new JobBuilder("memberJob", jobRepository)
        .start(memberStep(jobRepository, transactionManager))
        .build();
  }

  @Bean("memberStep")
  public Step memberStep(
      JobRepository jobRepository, PlatformTransactionManager transactionManager) {
    return new StepBuilder("memberStep", jobRepository)
        .tasklet(sendingItemScheduleAlarmTasklet(), transactionManager)
        .build();
  }

  @Bean("memberTasklet")
  public Tasklet memberTasklet() {
    return new Tasklet() {
      @Override
      public RepeatStatus execute(
          StepContribution contribution, ChunkContext chunkContext) throws Exception {/** 로직 */}

      @BeforeStep
      public void beforeStep(StepExecution stepExecution) {/** 실행되지 않는다. */}

      @AfterStep
      public ExitStatus afterStep() {/** 실행되지 않는다. */}
    };
  }
}
```

<br/>

# Tasklet에서 Step 이벤트를 실행하고 싶다면?

## 1. StepExecutionListener를 구현해준다.

- 대신 `StepExecutionListener`를 구현해줘야 한다.
- StepExecutionListener을 구현하고 `beforeStep`과 `afterStep`을 오버라이드를 해서 작성하면 된다.

```java
@Component
public class MemberTasklet implements Tasklet, StepExecutionListener {

  @Override
  public RepeatStatus execute(final StepContribution contribution, final ChunkContext chunkContext)
      throws Exception {
    return RepeatStatus.FINISHED;
  }

  @Override
  public void beforeStep(final StepExecution stepExecution) {
    // 실행
  }

  @Override
  public ExitStatus afterStep() {
    // 실행
  }
}
```

- 등록만 해줘도 Spring Batch 내부적으로 Tasklet으로 등록한 `StepExecutionListener` 구현체의 `beforeStep()`, `afterStep()` 메소드를 실행해준다.

```java
@Bean("memberStep")
public Step memberStep(
    final JobRepository jobRepository, final PlatformTransactionManager transactionManager) {
  return new StepBuilder("myStep", jobRepository)
      .tasklet(memberTasklet, transactionManager)
      .build();
}
```

- 단, `StepExecutionListener`을 구현해도 `@BeforeStep`, `@AfterStep`이 트리거되지 않는다는 사실은 주의하자.

```java
@Component
public class MemberTasklet implements Tasklet, StepExecutionListener {

  @Override
  public RepeatStatus execute(final StepContribution contribution, final ChunkContext chunkContext)
      throws Exception {/** 로직 */}

  @BeforeStep
  public void findMember(final StepExecution stepExecution) {/** 실행되지 않는다! */}

  @AfterStep
  public ExitStatus writeMember() {/** 실행되지 않는다! */}
}
```

<br/>

## 2. Tasklet을 StepListener로 등록하는 방법

- 아래와 같이 StepBuilder의 listener() 메소드를 사용해 Tasklet를 리스너로 등록해준다.

```java
@Bean("memberStep")
public Step memberStep(
    JobRepository jobRepository, PlatformTransactionManager transactionManager) {
  return new StepBuilder("memberStep", jobRepository)
      .tasklet(memberTasklet(), transactionManager)
      .listener(memberTasklet())
      .build();
}

@Bean("memberTasklet")
public Tasklet memberTasklet() {
  return new Tasklet() {
    @Override
    public RepeatStatus execute(
      StepContribution contribution, ChunkContext chunkContext) throws Exception {/** 로직 */}

    @BeforeStep
    public void beforeStep(StepExecution stepExecution) {/** 실행 */}

    @AfterStep
    public ExitStatus afterStep() {/** 실행 */}
  };
}
```

### 어떻게 동작하길래 트리거가 동작하는 걸까?

- StepBuilder에서 listener 메소드를 호출하면 `AbstractTaskletStepBuilder`에서 `super.listener(listener)`를 호출해 리스너를 추가해준다.

```java
public B listener(Object listener) {
	super.listener(listener);

	Set<Method> chunkListenerMethods = new HashSet<>();
	chunkListenerMethods.addAll(ReflectionUtils.findMethod(listener.getClass(), BeforeChunk.class));
	chunkListenerMethods.addAll(ReflectionUtils.findMethod(listener.getClass(), AfterChunk.class));
	chunkListenerMethods.addAll(ReflectionUtils.findMethod(listener.getClass(), AfterChunkError.class));

	if (chunkListenerMethods.size() > 0) {
		StepListenerFactoryBean factory = new StepListenerFactoryBean();
		factory.setDelegate(listener);
		this.listener((ChunkListener) factory.getObject());
	}

	return self();
}
```

- invokers에는 현재 리스너를 실행시키고 있는 컨텍스트에 대한 정보와 내가 `@BeforeStep`나 `@AfterStep`을 달았던 메소드 이름이 담긴다.
- 나중에 StepListener의 `beforeStep`이나 `afterStep`이 실행될 때, 프록시를 통해 `@BeforeStep`나 `@AfterStep`가 달려있는 메소드를 실행해준다.

```java
@Override
public Object getObject() {
	...

	Set<Class<?>> listenerInterfaces = new HashSet<>();

	Map<String, Set<MethodInvoker>> invokerMap = new HashMap<>();
	boolean synthetic = false;
	for (Entry<String, String> entry : metaDataMap.entrySet()) {
		...
    
		if (metaData.getAnnotation() != null) {
			invoker = getMethodInvokerByAnnotation(metaData.getAnnotation(), delegate, metaData.getParamTypes());
			if (invoker != null) {
				invokers.add(invoker);
				synthetic = true;
			}
		}

		if (!invokers.isEmpty()) {
			invokerMap.put(metaData.getMethodName(), invokers);
			listenerInterfaces.add(metaData.getListenerInterface());
		}

	}

	...
	proxyFactory.setInterfaces(listenerInterfaces.toArray(a));
	proxyFactory.addAdvisor(new DefaultPointcutAdvisor(new MethodInvokerMethodInterceptor(invokerMap, ordered)));
	return proxyFactory.getProxy();

}
```

- `StepBuilderHelper`는 Reflection을 통해 리스너로 등록의 객체의 메소드를 탐색해 `@BeforeStep`나 `@AfterStep` 어노테이션을 사용한 적이 있다면 Set 자료 구조에 담는다.
- 이제 `StepListenerFactoryBean`로 전달된 객체를 감싸고 StepExecutionListener 타입으로 Listener에 추가한다.

```java
public B listener(Object listener) {
	Set<Method> stepExecutionListenerMethods = new HashSet<>();
	stepExecutionListenerMethods.addAll(ReflectionUtils.findMethod(listener.getClass(), BeforeStep.class));
	stepExecutionListenerMethods.addAll(ReflectionUtils.findMethod(listener.getClass(), AfterStep.class));

	if (stepExecutionListenerMethods.size() > 0) {
		StepListenerFactoryBean factory = new StepListenerFactoryBean();
		factory.setDelegate(listener);
		properties.addStepExecutionListener((StepExecutionListener) factory.getObject());
	}

	return self();
}
```

- `AbstractStep`에서 리스너를 등록해준다.

```java
public class CompositeStepExecutionListener implements StepExecutionListener {

	private final OrderedComposite<StepExecutionListener> list = new OrderedComposite<>();

	public void register(StepExecutionListener stepExecutionListener) {
		list.add(stepExecutionListener);
	}

    protected StepExecutionListener getCompositeListener() {
      return stepExecutionListener;
    }
	
	...	
}
```

- Step이 실행되면 `AbstractStep`에서 `CompositeStepExecutionListener`을 가져와 `beforeStep`과 `afterStep`을 실행한다.

```java
public abstract class AbstractStep implements Step, InitializingBean, BeanNameAware {

	@Override
	public final void execute(StepExecution stepExecution)
			throws JobInterruptedException, UnexpectedJobExecutionException {
		...
		
		getCompositeListener().beforeStep(stepExecution);
		
		...
		
		exitStatus = exitStatus.and(getCompositeListener().afterStep(stepExecution));
		
		...
	}
}
```

- `CompositeStepExecutionListener`는 `Step`에 등록된 모든 `StepExecutionListener`의 `beforeStep`와 `afterStep`을 실행하게 된다.

```java
public class CompositeStepExecutionListener implements StepExecutionListener {

	...

  @Nullable
  @Override
  public ExitStatus afterStep(StepExecution stepExecution) {
    for (Iterator<StepExecutionListener> iterator = list.reverse(); iterator.hasNext();) {
      StepExecutionListener listener = iterator.next();
      ExitStatus close = listener.afterStep(stepExecution);
      stepExecution.setExitStatus(stepExecution.getExitStatus().and(close));
    }
    return stepExecution.getExitStatus();
  }

  @Override
  public void beforeStep(StepExecution stepExecution) {
    for (Iterator<StepExecutionListener> iterator = list.iterator(); iterator.hasNext();) {
        StepExecutionListener listener = iterator.next();
        listener.beforeStep(stepExecution);
    }
  }

}
```

<br/>

## 그렇다면 ItemReader, ItemProcessor, ItemWriter는 왜 따로 리스너로 등록하지 않아도 @BeforeStep나 @AfterStep을 사용할 수 있을까?

```java
@Bean("memberStep")
public Step memberStep(
    JobRepository jobRepository, PlatformTransactionManager transactionManager) {
  return new StepBuilder("memberStep", jobRepository)
      .<Member, Member>chunk(chunkSize, transactionManager)
      .reader(memberReader())
      .writer(memberWriter())
      .build();
}
```

### SimpleStepBuilder

- `registerAsStreamsAndListeners()` 메소드를 사용해 리스너로 자동 등록하기 때문이다.

```java
@Override
public TaskletStep build() {

	registerStepListenerAsItemListener();
	registerAsStreamsAndListeners(reader, processor, writer);
	return super.build();
}
```

### AbstractStep

- 위에서 설명했던 과정과 동일하게 beforeStep과 afterStep이 실행된다.

```java
@Override
public final void execute(StepExecution stepExecution) {
	...
	
	getCompositeListener().beforeStep(stepExecution);
	
	...
	
	exitStatus = exitStatus.and(getCompositeListener().afterStep(stepExecution));
	
	...
}
```

