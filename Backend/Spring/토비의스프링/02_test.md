# Chapter2. 테스트

- 스프링 개발자에게 가장 중요한 가치. => 객체지향과 테스트
  - 스프링의 핵심인 IoC와 DI는 오브젝트의 설계과 생성, 관계, 사용에 관한 기술.
  - 또한 복잡한 애플리케이션을 효과적으로 개발하기 위한 기술이다. 복잡한 애플리케이션을 개발하는 데 필요한 도구 하나는 객체지향 기술이다.
  - 그리고 스프링이 강조하고 가치를 두고 있는 것이 바로 **테스트**
- 애플리케이션은 계속 변하고 복잡해져 간다. 그 변화에 대응하는 방법은 아래의 2가지 방법이다.
1. 확장과 변화를 고려한 객체지향적 설계와 그것을 효과적으로 담아낼 수 있는 IoC/DI 같은 기술
2. 만들어진 코드를 확신할 수 있게 해주고, 변화에 유연하게 대처할 수 있는 자신감을 주는 테스트 기술

## 1. UserDaoTest 다시 보기

### 1-1. 테스트의 유용성

- 테스트란 결국 내가 예상하고 의도했던 대로 코드가 정확히 동작하는지를 확인해서, 만든 코드를 확신할 수 있게 해주는 작업이다.

### 1-2. UserDaoTest의 특징

- 이전에 작성한 예시를 정리해보면 아래와 같다.
  - 자바에서 가장 손쉽게 실행 가능한 `main()` 메소드를 사용
  - 테스트할 대상인 `UserDao`의 오브젝트를 가져와 메소드를 호출한다.
  - 테스트에 사용할 입력 값을 직접 코드에서 만들어준다.
  - 테스트의 결과를 콘솔에 출력한다.
  - 각 단계의 작업이 에러 없이 끝나면 콘솔에 성공 메시지로 출력해준다.

#### 웹을 통한 DAO 테스트 방법의 문제점

- DAO 뿐만 아니라 서비스 클래스, 컨트롤러와 뷰 등 모든 레이어의 기능을 다 만들고 나서야 테스트가 가능하다는 점이 가장 큰 문제다.
  - 하나의 테스트를 수행하는 데 참여하는 클래스와 코드가 너무 많기 때문에 문제가 어디에서 발생했는지 찾기 어렵다.

#### 작은 단위의 테스트

- 테스트하고자 하는 대상이 명확하다면 그 대상에만 집중해서 테스트하는 것이 바람직하다.
  - 한꺼번에 너무 많은 것을 몰아서 테스트하면 테스트 수행 과정도 복잡해지고, 오류가 발생했을 때 정확한 원인을 찾기가 힘들어진다.
  - 따라서 테스트는 가능하면 작은 단위로 쪼개서 집중해서 할 수 있어야 한다. (관심사의 분리는 여기에도 적용!)
- 이렇게 작은 단위의 코드에 대해 테스트를 수행한 것을 `단위 테스트 unit test`라 한다.
- 때로는 애플리케이션의 전 계층이 참여하는 하나의 프로세스 전체를 묶어서 테스트할 필요가 있다.
  - 각 단위 기능은 잘 동작하는데 묶어놓으면 안 되는 경우가 종종 발생하기 때문이다.
  - 하지만 단위 테스트 없이 긴 테스트만 한다면, 수많은 에러를 만나거나 에러는 안 나지만 제대로 기능이 동작하지 않는 경험을 하게 될 것이다. 문제의 원인도 찾기 매우 힘들다.

#### 자동수행 테스트 코든

- `UserDaoTest`의 한 가지 특징은 테스트할 데이터가 코드를 통해 제공되고, 테스트 작업 역시 코드를 통해 자동으로 실행한다는 점이다.
  - 테스트를 자주 수행해도 부담이 없다.

#### 지속적인 개선과 점진적인 개발을 위한 테스트

- 처음 만든 초난감 DAO 코드를, 스프링을 이용한 깔끔하고 완성도 높은 객체지향적 코드로 발전시키는 과정의 일등 공신은 바로 테스트였다.
  - 기능에 문제가 없는지 확인해주는 테스트가 있기에 잘못된 동작을 바로 체크할 수 있었다.

### 1-3. UserDaoTest의 문제점

#### 수동 확인 작업의 번거로움

- 테스트 수행과 데이터의 준비를 모두 자동으로 진행하도록 만들어졌지만 여전히 사람의 눈으로 확인하는 과정이 필요하다.
  - `add()`를 통해 추가하고 `get()`으로 확인할 때 자동으로 확인해주지 않는다. => 휴먼 에러 가능성

#### 실행 작업의 번거로움

- 매번 실행하는 것은 번거로운 일이다.
- `main()` 메소드를 이용하는 방법보다는 좀 더 편리하고 체계적으로 테스트를 실행하고 그 결과를 확인하는 방법이 필요.

## 2. UserDaoTest 개선

- 문제 두 가지 개선

### 2-1. 테스트 검증의 자동화

- 테스트 실패
1. 테스트 진행 중 에러
2. 결과가 기대한 것과 다름

### 2-2. 테스트의 효율적인 수행과 결과 관리

- JUnit. 단위 테스트를 만들 때 유용한 툴

#### JUnit 테스트로 전환

- JUnit은 프레임워크다. 주도적으로 애플리케이션의 흐름을 제어한다. `main()` 메소드도 필요 없고 오브젝트를 만들어서 실행시키는 코드를 만들 필요도 없다.

#### 테스트 메소드 전환

- 토비의 스프링에서 사용하는 JUnit4에서는 @Test를 사용하려면 메소드를 `public`으로 선언해야 했다.
  - 현재 사용하는 JUnit5에서는 default 메소드로 둬도 테스트 실행이 가능하다.

```java
public class UserDaoTest  {

  @Test
  public void addAndGet() throws SQLException {
    ClassPathXmlApplicationContext context = new ClassPathXmlApplicationContext("applicationContext.xml");

    UserDao userDao = context.getBean("userDao", UserDao.class);
  }
}
```

#### 검증 코드 전환

- `assertThat`의 사용

```java
public class UserDaoTest  {

  @Test
  public void addAndGet() throws SQLException, ClassNotFoundException {
    ClassPathXmlApplicationContext context = new ClassPathXmlApplicationContext("applicationContext.xml");

    UserDao dao = context.getBean("userDao", UserDao.class);

    User user = new User();
    user.setId("bell");
    user.setName("홍길동");
    user.setPassword("hong1234");

    dao.add(user);

    User user2 = dao.get(user.getId());
    assertThat(user2.getName(), is(user.getName()));
    assertThat(user2.getPassword(), is(user.getPassword()));
  }
}
```

#### 테스트 실행

- JUnit4 러너가 없어서 직접 실행해줘야 하는 불편함이 있다.  
  - JUnit5는 그냥 실행된다. JUnit Platform이 테스트를 발견하고 실행하는 역할을 한다.
- 아래와 같이 JUnitCore 클래스의 main() 메소드를 통해 실행할 수 있다. (`@RunWith` 같은 어노테이션을 붙일 수도 있다.)

```java
public class Main {

  public static void main(String[] args) {
    JUnitCore.main("org.mobilohas.bell.ch1.user.dao.UserDaoTest");
  }
}
```

- 위 테스트를 실행하면 아래와 같은 메시지가 뜨면서 테스트가 성공할 것이다.

```shell
JUnit version 4.12

Time: 0.446

OK (1 test)
```

- 테스트가 실패하면 OK 대신 FAILURES!! 라는 내용을 출력한다.
  - 어떤 것이 실패했는지도 알려준다.

## 3. 개발자를 위한 테스팅 프레임워크 JUnit

- 스프링의 핵심 기능 중 하나인 스프링 테스트 모듈도 JUnit을 이용한다.
  - 스프링의 기능을 익히기 위해서라도 JUnit을 사용할 줄 알아야 한다.

### 3-1. JUnit 테스트 실행 방법

- IDE를 이용해 JUnit 테스트 지원 도구를 사용하면 실행할 수 있다.

#### IDE

- 인텔리제에서는 Run을 실행하면 자체 JUnit Platform Launcher를 사용해 테스트를 실행한다.

#### 빌드 툴

- ANT, Maven, Gradle 등의 빌드 툴과 스크립트를 사용하고 있다면 test 명령을 통해 JUnit 테스트를 실행할 수 있다.

### 3-2. 테스트 결과의 일관성

- 아직 아쉬운 점이 있다. 테스트를 실행하기 전에 DB의 USER 테이블 데이터를 모두 삭제해줘야 한다는 점이다.
- 반복적으로 실행했을 때 테스트가 실패하기도 하고 성공하기도 한다면 이는 좋은 테스트라고 할 수 없다.
  - 코드에 변경사항이 없다면 테스트는 항상 동일한 결과를 내야 한다.

#### 동일한 결과를 보장하는 테스트

- 단위 테스트는 항상 일관성 있는 결과가 보장돼야 한다는 점을 잊어선 안 된다.
- DB에 남아 있는 데이터와 같은 외부 환경에 영향을 받지 말아야 하는 것은 물론이고, 테스트를 실행하는 순서를 바궈도 동일한 결과가 보장되도록 만들어야 한다.

### 3-3. 포괄적인 테스트

- 테스트를 안 만드는 것도 위험한 일이지만 성의 없이 테스트를 만드는 바람에 문제가 있는 코드인데도 테스트가 성공하게 만드는 건 더 위험하다.
  - 특히 한 가지 결과만 검증하고 마는 것은 상당히 위험하다.
- 개발자가 테스트를 직접 만들 때 자주 하는 실수가 하나 있다. 바로 성공하는 테스트만 골라서 만드는 것이다.
  - 조금만 신경써도 코드에서 발생할 수 있는 다양한 상황과 입력 값을 고려하는 포괄적인 테스트틀 만들 수 있다.
  - 스프링의 창시자 로드 존슨은 "항상 네거티브 테스트를 먼저 만들라"는 조언을 했다.
- 테스트를 작성할 때 부정적인 케이스를 먼저 만드는 습관을 들이는 게 좋다.
  - 예외적인 상황을 빠뜨리지 않는 꼼꼼한 개발이 가능하다!

### 3-4. 테스트가 이끄는 개발

#### 기능설계를 위한 테스트

- 테스트에는 만들고 싶은 기능에 대한 `조건`과 `행위`, `결과`에 대한 내용이 잘 표현되어 있다.
  - 이렇게 보면 테스트 코드는 마치 잘 작성된 하나의 기능정의서처럼 보인다.
  - 그래서 보통 기능설계, 구현, 테스트라는 일반적인 개발 흐름의 기능설계에 해당하는 부분을 테스트 코드가 일부분 담당하고 있다고 볼 수도 다.
  - 이런 식으로 추가하고 싶은 기능을 일반 언어가 아니라 테스트 코드로 표현해서, 마치 코드로 된 설계문서처럼 만들어놓은 것이라고 생각해보자.

#### 테스트 주도 개발 TDD

- "실패한 테스트를 성공시키기 위한 목적이 아닌 코드는 만들지 않는다"는 것이 TDD의 기본 원칙이다.
  - 이 원칙을 따랐다면 만들어진 모든 코드는 빠짐없이 테스트로 검증된 것이라고 볼 수 있다.
- 테스트는 자신감을, 한편으로는 마음의 여유를 주는 방법이다.
  - 테스트를 작성하고 이를 성공시키는 코드를 만드는 작업의 주기를 가능한 한 짥게 가져가도록 권장한다.
- 개발자의 머릿속에서는 계속 시뮬레이션이 돌아간다. 사실상 테스트가 미리 만들어지고 개발 중에 계속 실행된다고 볼 수 있지 않을까?
  - 문제가 발생한다면 코드를 수정할 것이다. 테스트가 실패했으니 테스트가 성공하도록 코드를 수정하는 것과 다를 바 없다.
  - 문제는 머릿속에서 진행되는 테스트는 제약이 심하고, 오류가 많고, 나중에 다시 반복하기가 힘들다는 점이다.
  - 차라리 테스트를 작성하면 이것이 TDD가 된다.
- 개발한 코드의 오류는 빨리 발견할수록 좋다. 쉽게 대응이 가능하기 때문이다.
- 혹시 테스트를 자주 실행하면 개발이 지연되지는 않을까 염려할지도 모르겠다.
  - 하지만 테스트는 애플리케이션 코드보다 상대적으로 작성하기 쉽다.
  - 각 테스트가 독립적이기 때문에, 코드의 양에 비해 작성하는 시간이 얼마 걸리지 않는다.
  - 게다가 테스트 덕분에 오류를 빨리 잡아낼 수 있어서 전체적인 개발 속도는 오히려 빨라진다.
  - 테스트 코드가 없으면 언제가는 웹 화면을 통한 테스트라도 하게 될 것이다.

### 3-5. 테스트 코드 개선

- 테스트를 실행할 때마다 반복되는 준비 작업을 별도의 메소드에 넣게 해주고, 이를 매번 테스트 메소드를 실행하기 전에 먼저 실행할 수 있다.

#### @Before

```java
public class UserDaoTest  {

  private UserDao dao;

  @Before
  public void setUp() throws Exception {
    ClassPathXmlApplicationContext context = new ClassPathXmlApplicationContext("applicationContext.xml");
    this.dao = context.getBean("userDao", UserDao.class);
  }
}
```

- JUnit4가 테스트를 수행하는 방식은 아래와 같다. (`JUnit4`의 `@Before`는 `JUnit5`의 `@BeforeEach`처럼 각 메소드가 실행되기 전에 실행된다. `JUnit5`에는 `@BeforeAll`이라는 개념이 추가되었다.)

1. `@Test`가 붙은 `public void`형의 파라미터 없는 테스트 메소드를 모두 찾는다.
2. 테스트 클래스의 오브젝트를 하나 만든다.
3.  `@Before`가 붙은 메소드가 있으면 실행한다.
4. `@Test`가 붙은 메소드를 하나 호출하고 테스트 결과를 저장해둔다.
5. `@After`가 붙은 메소드가 있으면 실행한다.
6. 나머지 테스트에 대해 2~5번 반복
7. 모든 테스트의 결과를 종합해서 돌려준다.

- 그런데 왜 테스트 메소드를 만들 때마다 새로운 오브젝트를 만드는 것일까?
  - JUnit 개발자는 각 테스트가 서로 영향을 주지 않고 독립적으로 실행됨을 확실히 보장해주기 위해 매번 새로운 오브젝트를 만들게 했다.
  - 어차피 다음 테스트 메소드가 실행될 때는 새로운 오브젝트가 만들어져서 다 초기화될 것이다.
- 하지만 공통적으로 사용되는 코드가 있다면 `@Before`를 사용하기보다는 분리하는 것이 나을 수 있다.

#### 픽스처

- 테스트를 수행하는 데 필요한 정보나 오브젝트를 `픽스처 Fixture`라고 한다.
- 개인적으로 픽스처 사용을 자제하자는 주의 => DAMP에 어긋난다.

## 4. 스프링 테스트 적용

- 애플리케이션 컨텍스트 생성 방식.
  - 현재 구조로는 `@Before`가 메소드 개수만큼 반복되기 때문에 애플리케이션 컨텍스트도 세 번 만들어진다.
  - 메소드 수가 늘어나는만큼 애플리케이션 컨텍스트를 생성? => 나중에는 엄청나게 많은 시간이 필요해질 것이다.
- 애플리케이션 컨텍스트는 초기화되고 나면 내부의 상태가 바뀌는 일은 거의 없다.
  - 빈은 싱글톤으로 만들었기 때문에 상태를 갖지 않는다.
  - 따라서 한 번만 만들고 여러 테스트가 공유해서 사용해도 된다.
- 문제는 애플리케이션 컨텍스트를 오브젝트 레벨에 저장해두면 곤란하다는 점인데.
  - static 변수에 저장해두고 사용? 불편하다. 
- 스프링이 직접 제공하는 애플리케이션 컨텍스트 테스트 지원 기능을 사용하는 것이 더 편리하다.

### 4-1. 테스트를 위한 애플리케이션 컨텍스트 관리

- 스프링은 테스트 컨텍스트 프레임워크를 제공한다.
- 테스트 컨텍스트의 지원을 받으면 간단한 어노테이션 설정만으로 테스트에서 필요로 하는 애플리케이션 컨텍스트를 만들어서 모든 테스트가 공유하게 할 수 있다.

#### 스프링 테스트 컨텍스트 프레임워크 적용

```java
@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration(locations = "/applicationContext.xml")
public class UserDaoTest {

  @Autowired
  private ApplicationContext context;

  private UserDao dao;

  @Before
  public void setUp() {
    this.dao = context.getBean("userDao", UserDao.class);
  }
  
  // ...
}
```

- `@RunWith`는 JUnit 프레임워크의 테스트 실행 방법을 확장할 때 사용하는 어노테이션이다.
  - `SpringJUnit4ClassRunner`라는 JUnit용 테스트 컨텍스트 프레임워크 확장 클래스를 지정해주면 JUnit이 테스트를 징행하는 중에 테스트가 사용할 애플리케이션 컨텍스트를 만들고 관리하는 작업을 진행해준다.

#### 테스트 메소드의 컨텍스트 공유

- 아래와 같이 콘솔을 추가해보자.

```java
@Before
public void setUp() {
  this.dao = context.getBean("userDao", UserDao.class);
  System.out.println(context);
  System.out.println(dao);
}
```

```shell
org.springframework.context.support.GenericApplicationContext@7c7b252e: startup date [Fri Dec 06 17:45:20 KST 2024]; root of context hierarchy
org.mobilohas.bell.ch1.user.dao.UserDaoTest@6c2ed0cd
org.springframework.context.support.GenericApplicationContext@7c7b252e: startup date [Fri Dec 06 17:45:20 KST 2024]; root of context hierarchy
org.mobilohas.bell.ch1.user.dao.UserDaoTest@5a62b2a4
```

- 오브젝트 값을 잘 살펴보면, context는 두 번 모두 동일하다.
- 반면 `userDaoTest`는 매번 주소 값이 다르다.
  - JUnit은 테스트 메소드를 실행할 때마다 새로운 테스트 오브젝트를 만들기 때문이다.
  - 따라서 첫 테스트에서 가장 오랜 시간이 걸린다.

#### 테스트 클래스의 컨텍스트 공유

- 여러 개의 테스트 클래스가 모두 같은 설정 파일을 가진 애플리케이션 컨텍스트를 사용한다면, 스트링은 테스트 클래스 사이에서도 애플리케이션 컨텍스트를 공유하게 해준다.
- 테스트의 성능이 대폭 향상된다.

#### @Autowired

- DI에 사용하는 어노테이션
- `@Autowired`를 사용하면 굳이 `ApplicationContext` 인스턴스 변수를 통해 `getBean()`으로 빈을 가져올 필요가 없다.
- 빈이 2개 이상 설정되어 있다면 문제가 발생할 수 있다.
  - 타입으로 가져올 빈 하나를 선택할 수 없는 경우에는 변수의 이름과 같은 이름의 빈이 있는지 확인한다.
  - 변수 이름으로도 빈을 찾을 수 없는 경우에는 예외가 발생한다.

### 4-2. DI와 테스트

- 구현체가 아닌 인터페이스로 가져와야 하는 이유가 있다면 무엇일까?
1. 소프트웨어 개발에서 절대로 바뀌지 않는 것은 없기 때문이다.
  - 인터페이스를 사용하고 DI를 적용하는 작은 수고를 하지 않을 이유가 없다.
2. 클래스의 구현 방식은 바뀌지 않는다고 하더라도 인터페이스를 두고 DI를 적용하게 해두면 다른 차원의 서비스 기능을 도입할 수 있기 때문이다.
3. 테스트 때문이다.
  - 단지 효율적인 테스트를 손쉽게 만들기 위해서라도 DI를 적용해야 한다.

#### 테스트 코드에 의한 DI

- DI는 애플리케이션 컨텍스트 같은 스프링 컨테이너에서만 할 수 있는 작업은 아니다.
- 애플리케이션이 사용할 `applicationContext.xml`에 정의된 `DataSource` 빈은 서버의 DB 풀 서비스와 연결해서 운영용 DB 커넥션을 돌려주도록 만들어져 있다고 해보자.
  - 테스트할 때 이 설정을 사용하면 대참사다. 만약 `deleteAll()`과 같이 위험한 함수가 실행되어 버린다면?
- 테스트용 DB에 연결해주는 `DataSource`를 테스트 내에서 직접 만들 수 있다.
  - `DataSource` 구현 클래스는 스프링이 제공하는 가장 빠른 `DataSource`인 `SingleConnectionDataSource`를 사용하면 된다.
  - 커넥션을 하나만 만들어두고 계속 사용하기 때문에 매우 빠르다.
  - 다중 사용자 환경에서는 사용할 수 없겠지만 순차적으로 진행되는 테스트에서라면 문제없다.

```java
// ...
@DirtiesContext
public class UserDaoTest {

  @Autowired
  private UserDao dao;

  @Before
  public void setUp() throws Exception {
    DataSource dataSource = new SingleConnectionDataSource(
        "jdbc:mysql://localhost:3307/testdb", "root", "qwer1234", true);
    dao.setDataSource(dataSource);
  }

  // ...
}
```

> #### @DirtiesContext
> - 테스트 메소드에서 애플리케이션 컨텍스트의 구성이나 상태를 변경한다는 것을 테스트 컨텍스트 프레임워크에 알려준다.

- 위 방법의 장점은 XML 설정파일을 수정하지 않고도 테스트 코드를 통해 오브젝트 관계를 재구성할 수 있다는 것이다.
  - 하지만 매우 주의해서 사용해야 한다.
  - 이미 `applicationContext.xml` 파일의 설정정보를 따라 구성한 오브젝트를 가져와 의존관계를 강제로 변경했기 때문이다.
  - 나머지 모든 테스트를 수행하는 동안 변경된 애플리케이션 컨텍스트를 사용하게 될지도 모른다.
  - 그래서 `@DirtiesContext`를 사용하는 것이다. 이 어노테이션이 붙은 테스트 클래스에는 애플리케이션 컨텍스트 공유를 허용하지 않는다. 대신 메소드마다 애플리케이션 컨텍스트를 새로 만든다.
  - 하지만 매번 새로 생성한다는 점에서 성능에 문제가 생길 수 있다.

#### 테스트를 위한 별도의 DI 설정

- 테스트 전용 설정파일을 따로 만드는 방법이 있다.
- 설정파일을 하나 더 작성하고 테스트에 맞게 수정해주는 수고만으로 테스트에 적합한 오브젝트 의존관계를 만들어 사용할 수 있게 된다. (DI 덕분)

```java
@ContextConfiguration(locations = "/test-applicationContext.xml")
public class UserDaoTest  {
  // ...
}
```

#### 컨테이너 없는 DI 테스트

```java
public class UserDaoTest2 {

  private UserDao dao;

  @Before
  public void setUp() {
    dao = new UserDao();
    DataSource dataSource = new SingleConnectionDataSource(
        "jdbc:mysql://localhost:3307/testdb", "root", "qwer1234", true);
    dao.setDataSource(dataSource);
  }
  
  // ...
}
```

- 테스트를 위한 `DataSource`를 직접 만드는 번거로움은 있지만 애플리케이션 컨텍스트를 아예 사용하지 않으니 코드는 더 단순해지고 이해하기 편해진다.
  - 테스트 시간도 줄어든다!
- `UserDao`가 스프링의 API에 의존하지 않고 자신의 관심에만 집중해서 깔끔하게 만들어진 코드이기 때문에 가능한 일이다.
  - 가볍고 깔끔한 테스트를 만들 수 있는 이유도 DI를 적용했기 때문이다.
  - DI는 객체지향 프로그래밍 스타일이다. 따라서 DI를 위해 컨테이너가 반드시 필요한 것은 아니다.
  - DI 컨테이너나 프레임워크는 DI를 편하게 적용하도록 도움을 줄 뿐, 컨테이너가 DI를 가능하게 해주는 것은 아니다.

> #### 침투적 기술과 비침투적 기술
> - `침투적 invasive` 기술은 기술을 적용했을 때 애플리케이션 코드에 기술 관련 API가 등장하거나, 특정 인터페이스나 클래스를 사용하도록 강제하는 기술을 말한다.
>   - 애플리케이션 코드가 해당 기술에 종속되는 결과를 가져온다.
> - 반면 `비침투적 noninvasive` 기술은 애플리케이션 로직을 담은 코드에 아무런 영향을 주지 않고 적용이 가능하다.
>   - 기술에 종속적이지 않은 순수한 코드를 유지할 수 있게 해준다.

## 5. 학습 테스트로 배우는 스프링

- 자신이 만들지 않은 프레임워크나 다른 개발팀에서 만들어서 제공한 라이브러리 등에 대해서도 테스트를 작성해야 한다.
  - 이런 테스트를 `학습 테스트 learning test`라고 한다.
- 학습 테스트의 목적은 자신이 사용할 API나 프레임워크의 기능을 테스트로 보면서 사용 방법을 익히려는 것이다.
  - 기능 검증 목적 X
  - 기술 이해도 검증 O

### 5-1. 학습 테스트의 장점

- 다양한 조건에 따른 기능을 손쉽게 확인해볼 수 있다.
- 학습 테스트 코드를 개발 중에 참고할 수 있다.
- 프레임워크나 제품을 업그레이드할 때 호환성 검증을 도와준다.
- 테스트 작성에 대한 훈련이 된다.

### 5-2. 학습 테스트 예제

#### JUnit 테스트 오브젝트 테스트

- 메소드마다 새로운 오브젝트를 생성하는지에 대한 테스트

```java
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.hamcrest.CoreMatchers.sameInstance;
import static org.junit.Assert.assertThat;

import org.junit.Test;

public class JUnitTest {

  static JUnitTest testObject;

  @Test
  public void test1() {
    assertThat(this, is(not(sameInstance(testObject))));
    testObject = this;
  }

  @Test
  public void test2() {
    assertThat(this, is(not(sameInstance(testObject))));
    testObject = this;
  }
}
``` 

- 위 테스트의 찜찜한 점은 직전 테스트에서 만들어진 테스트 오브젝트와만 비교한다는 점이다.
- 아래와 같이 개선해볼 수 있다.

```java
import static org.hamcrest.CoreMatchers.hasItem;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.hamcrest.CoreMatchers.sameInstance;
import static org.junit.Assert.assertThat;

import java.util.HashSet;
import java.util.Set;
import org.junit.Test;

public class JUnitTest2 {

  static Set<JUnitTest2> testObjects = new HashSet<>();

  @Test
  public void test1() {
    assertThat(testObjects, not(hasItem(this)));
    testObjects.add(this);
  }

  @Test
  public void test2() {
    assertThat(testObjects, not(hasItem(this)));
    testObjects.add(this);
  }

  @Test
  public void test3() {
    assertThat(testObjects, not(hasItem(this)));
    testObjects.add(this);
  }
}
```

#### 스프링 테스트 컨텍스트 테스트

- 같은 설정에 대한 애플리케이션 컨텍스트를 공유하는지에 대한 테스트

```java
import static org.hamcrest.CoreMatchers.hasItems;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.assertThat;

import java.util.HashSet;
import java.util.Set;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationContext;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration(locations = "/junit.xml")
public class JUnitTest3 {

  @Autowired
  ApplicationContext applicationContext;

  static Set<JUnitTest3> testObjects = new HashSet<>();
  static ApplicationContext contextObject = null;

  @Test
  public void test1() {
    assertThat(testObjects, not(hasItems(this)));
    testObjects.add(this);
    assertThat(contextObject == null || contextObject == this.applicationContext, is(true));
    contextObject = this.applicationContext;
  }

  @Test
  public void test2() {
    assertThat(testObjects, not(hasItems(this)));
    testObjects.add(this);
    assertThat(contextObject == null || contextObject == this.applicationContext, is(true));
    contextObject = this.applicationContext;
  }

  @Test
  public void test3() {
    assertThat(testObjects, not(hasItems(this)));
    testObjects.add(this);
    assertThat(contextObject == null || contextObject == this.applicationContext, is(true));
    contextObject = this.applicationContext;
  }
}
```

### 5-3. 버그 테스트

- 오류가 있을 때 그 오류를 가장 잘 드러내줄 수 있는 테스트
- 버그 테스트의 장점
1. 테스트의 완성도를 높여준다.
2. 버그의 내용을 명확하게 분석하게 해준다.
   - 동등분할이나 경계값 분석을 적용해볼 수도 있다.
3. 기술적인 문제를 해결하는 데 도움이 된다.

> #### 동등분할 equivalence partitioning
> - 같은 결과를 내는 값의 범위를 구분해서 각 대표 값으로 테스트를 하는 방법을 말한다.
> - 어떤 작업의 결과의 종류가 true, false 또는 예외 발생 세 가지라면 각 결과를 내는 입력 값이나 상황의 조합을 만들어 모든 경우에 대한 테스트를 해보는 것이 좋다.

> #### 경계값 분석 boundary value analysis
> - 에러는 동등분할 범위의 경계에서 주로 많이 발생한다는 특징을 이용해서 경계의 근처에 있는 값을 이용해 테스트하는 방법이다.
> - 보통 숫자의 입력 값인 경우 0이나 그 주변 값 또는 정수의 최대값, 최소값 등으로 테스트해보면 도움이 될 때가 많다.

# 새로 알게된 사실

- JUnit4 사용법
  - JUnit4는 JUnit5와 달리 Runner를 달아줘야 실행할 수 있다.
- 테스트 메소드를 실행할 때마다 새로운 테스트 오브젝트를 만든다.
  - JUnit5에서 `@BeforeAll`, `@AfterAll`이라는 어노테이션이 있길래, 인스턴스는 하나만 생성하는 줄 알았다.
- 첫 번째 테스트가 수행될 때 최초로 애플리케이션 컨텍스트가 만들어진다.
  - 이전에 테스트를 확인해보면 같은 컨텍스트를 가진 테스트끼리 애플리케이션 컨텍스트를 같이 사용할 수 있게 만들 수 있다. 
  - 그 때 최초로 실행되는 테스트 객체가 무엇인지 확인할 수 있어서, 최초에만 만드는 건가? 라고 생각하고 넘어 갔었다.

# 이해되지 않는 내용

# 모르는 내용

- JUnit5도 테스트 메소드를 실행할 때마다 새로운 테스트 오브젝트를 만들까?


# 참고자료

- 토비의 스프링, 이일민 저
