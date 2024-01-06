# 스프링 배치 도메인

## Job

- 배치 계층 구조에서 가장 상위에 있는 개념, Job을 구성하기 위한 최상위 인터페이스
- 하나의 배치 작업 자체를 의미한다.
- Job Configuration을 통해 생성되는 객체 단위, 배치 작업을 어떻게 구성하고 실행할 것인지 전체적으로 설정하고 명세해 놓은 객체
- 스프링 배치에서 기본 구현체를 제공하며 여러 Step을 포함하고 있는 컨테이너이다. (무조건 한 개 이상의 Step으로 구성해야함)

### 기본 구현체

- SimpleJob
  - 순차적으로 Step을 실행, 표준 기능
  - Step 객체를 실행시켜서 작업을 진행함
- FlowJob
  - 특정한 조건과 흐름에 따라 Step을 구성하여 실행시키는 Job
  - Flow 객체를 실행시켜서 작업을 진행함

<img src="img/job/job1.png">

## JobInstance

- Job이 실행되는 시점에 생성하는 Job의 논리적 실행 단위 객체, 고유하게 식별 가능한 작업 실행
- Job의 설정과 구성은 동일하지만 Job이 실행되는 시점에 처리하는 내용은 다르기 때문에 Job의 실행을 구분해야함. (오늘 처리하는 내용과 내일 처리할 내용이 다르듯)
  - 주기에 따라 매번 실행되는 Job을 JobInstance로 구분한다.

### JobInstance 생성 및 실행

- 처음 시작하는 Job + JobParameter 일 경우 새로운 JobInstance를 생성
- 이전과 동일한 Job + JobParameter로 실행할 경우 이미 존재하는 JobInstance를 리턴 => 다시 수행할 필요가 없다고 판단해 실행 실패한다.
- 
```
Caused by: org.springframework.batch.core.repository.JobInstanceAlreadyCompleteException: 
          A job instance already exists and is complete for parameters={name=user1}.  
          If you want to run this job again, change the parameters.
```

- 내부적으로 JobName과 JobKey(JobParameter의 해시값)를 가지고 JobInstance 객체를 얻음

### 테이블 매핑

- `BATCH_JOB_INSTANCE` 테이블과 매핑
- JOB_NAME(job)과 JOB_KEY(JobParameter 해시값)과 동일한 데이터는 중복해서 저장할 수 없다.

### 플로우

- 일별 정산의 예시

<img src="img/job/jobInstance1.png">

- JobName과 JobParameters의 해시값을 통해 JobInstance의 고유성을 보장

<img src="img/job/jobInstance2.png">

## JobParameter

- Job을 실행할 때 함께 포함되어 사용되는 파라미터를 가진 도메인 객체
- 하나의 Job에 존재할 수 있는 여러 개의 JobInstance를 구분하기 위해 사용
- JobParameters와 JobInstance는 1대 1 관계다.

### 생성 및 바인딩

- 애플리케이션 실행 시 주입
  - `java -jar batch.jar 'name=user2' 'seq(long)=5L' 'date(date)=2023/12/09' 'age(double)=27.1'`
- 코드로 생성 (주로 `JobParameterBuilder`를 많이 사용)
  - `JobParameterBuilder`, `DefaultParametersConverter`
- SpEL 사용
  - `@Value("#{jobParameter[name]}")`, `@JobScope`, `@StepScope` 선언 필수

### 테이블 매핑

- `BATCH_JOB_EXECUTION_PARAM` 테이블과 매핑
- `JOB_EXECUTION` 테이블과 1:M 관계

### 플로우

<img src="img/job/jobParameters1.png">

## JobExecution

- JobInstance의 한 번의 시도를 의미하는 객체, Job 실행 중에 발생한 정보들을 저장하고 있는 객체
- 시작 시간, 종료 시간, 상태(시작, 완료, 실패), 종료 상태 등의 속성을 가짐

### JobInstance와의 관계 (Execution이 여러 번 실행될 수도, 한 번만 실행될 수도, 아예 실행되지 않을 수도 있다.)

- JobExecution은 `FAILED` 또는 `COMPLETED` 등의 Job의 실행 결과 상태를 가지고 있다.
- JobExecution 상태가 `COMPLETED`면 JobInstance 실행이 완료된 것으로 간주해 재실행하지 않는다.
- JobExecution 상태가 `FAILED`면 JobInstance 실행이 완료되지 않은 것으로 간주해 재실행한다.
  - JobParameter가 동일하더라도 다시 실행한다는 말이다.
- 상태가 `COMPLETED`가 될 때까지 여러 번의 시도를 할 수 있다.

### 테이블 매핑

- BATCH_JOB_EXECUTION 테이블과 매핑
- JobInstance와 JobExecution은 1:M 관계로 JobInstance에 대한 성공/실패 내역을 가지고 있다.

### 플로우

<img src="img/job/jobExecution1.png">
<img src="img/job/jobExecution2.png">
<img src="img/job/jobExecution3.png">

<br/>

## Step

- Batch Job을 구성하는 독립적인 하나의 단계다. 실제 배치 처리를 정의하고 컨트롤하는 데 필요한 모든 정보를 가지고 있는 도메인 객체다.
- 단순한 단일 Task와 입력, 처리 및 출력과 관련된 복잡한 비즈니스 로직을 포함하는 모든 설정을 담고 있다.
- 배치 작업을 어떻게 구성하고 실행할 것인지 Job의 세부 작업을 Task 기반으로 설정하고 명세해놓은 객체다.
- Job은 하나 이상의 Step으로 구성된다.

### 구현체

- `TaskletStep`
  - 가장 기본이 되는 클래스
  - Tasklet 타입의 구현체들을 제어
- `PartitionStep`
  - 멀티 스레드 방식으로 Step을 여러 개로 분리해서 실행
- `JobStep`
  - Step 내에서 Job을 실행하도록 함
- `FlowStep`
  - Step 내에서 Flow를 실행하도록 함

<img src="img/step/step1.png">

<img src="img/step/step2.png">

### API 설정에 따른 각 Step의 생성

- Tasklet: 직접 생성한 Tasklet 실행

```java
public Step taskletStep() {
  return this.stepBuilderFactory.get("step")
        .tasklet(myTasklet())
        .build();
}
```

- TaskletStep: ChunkOrientedTasklet을 실행

```java
public Step taskletStep() {
  return this.stepBuilderFactory.get("step")
        .<Member, Member>chunk(100)
        .reader(reader())
        .writer(writer())
        .build();
}
```

- JobStep: Step에서 Job을 실행

```java
public Step jobStep() {
  return this.stepBuilderFactory.get("step")
        .job(job())
        .launcher(jobLauncher)
        .parametersExtractor(JobParametersExtractor())
        .build();
}
```

- FlowStep: Step에서 Flow를 실행

```java
public Step flowStep() {
  return this.stepBuilderFactory.get("step")
        .flow(myFlow())
        .build();
}
```

<br/>

## StepExecution

<br/>

## StepContribution

<br/>

## ExecutionContext


<br/>

## JobRepository / JobLauncher

<br/>

## 참고자료

- [인프런 스프링 배치 강의: 정수원 님](https://www.inflearn.com/course/%EC%8A%A4%ED%94%84%EB%A7%81-%EB%B0%B0%EC%B9%98/dashboard)

- [Spring Batch 이해하고 사용하기](https://khj93.tistory.com/entry/Spring-Batch%EB%9E%80-%EC%9D%B4%ED%95%B4%ED%95%98%EA%B3%A0-%EC%82%AC%EC%9A%A9%ED%95%98%EA%B8%B0)

- [Spring Batch 개념 잡기](https://azderica.github.io/01-spring-batch/)

- [Spring Batch 가이드: 향로님 자료](https://jojoldu.tistory.com/324)
