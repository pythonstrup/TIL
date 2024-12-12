# Chapter03. 템플릿

- 변화의 특성이 다른 부분을 구분해주고, 각각 다른 목적과 다른 이유에 의해 다른 시점에 독립적으로 변경될 수 있는 효율적인 구조를 만들어 주는 것이 OCP다.
- 템플릿이란 바뀌는 성질이 다른 코드 중에서 변경이 거의 일어나지 않으며 일정한 패턴으로 유지되는 특성을 가진 부분을 자유롭게 변경되는 성질을 가진 부분으로부터 독립시켜서 효과적으로 활용할 수 있도록 하는 방법이다.

## 1. 다시 보는 초난감 DAO

### 1-1. 예외처리 기능을 갖춘 DAO

- DB 커넥션이라는 제한적인 리소스 공유할 때 반드시 지켜야 하는 원칙! 바로 예외처리!
  - 예외가 발생하더라도 사용한 리소스를 반드시 반환하도록 만들어야 하기 때문!

#### JDBC 수정 기능의 예외처리 코드

- 만약 문제가 발생해서 `Connection`과 `PreparedStatement`의 `close()` 메소드가 실행되지 않아서 제대로 리소스가 반환되지 않는다면???
  - 이 문제가 계속해서 쌓이면 어느 순간에 커넥션 풀에 여유가 없어지고 리소스가 모자란다는 심각한 오류를 내며 서버가 중단될 수 있다.

> #### 리소스 반환과 close
> - `Connection`과 `PreparedStatement`에는 `close()`라는 메소드가 있다.
>   - 단순하게 생각하면 만들어진 걸 종료하는 것이라고 볼 수 있지만 보통 리소스를 반환한다는 의미로 이해하는 것이 좋다.
> - `Connection`과 `PreparedStatement`는 보통 `풀 Pool` 방식으로 운영된다.
>   - 미리 정해진 풀 안에 제한된 수의 리소스를 만들어두고 필요할 때 이를 할당하고, 반환하면 다시 풀에 넣는 방식으로 운영된다.
> - 요청이 매우 많을 경우 유리하며, 사용한 리소스는 빠르게 반환해야 한다. 그렇지 않으면 리소스가 고갈되고 결국 문제가 발생한다.

- 따라서 어떤 경우에도 리소스를 반환하도록 `try/catch/finally` 구문 사용을 권장한다. 

```java
public void deleteAll() throws ClassNotFoundException, SQLException {
    Connection c = null;
    PreparedStatement ps = null;
    try {
      c = dataSource.getConnection();
      ps = c.prepareStatement("delete from users");
      ps.executeUpdate();
    } catch (SQLException e) {
      throw e;
    } finally {
      if (ps != null) {
       try {
         ps.close();
       } catch (SQLException e) {}
      }
      if (c != null) {
       try {
         c.close();
       } catch (SQLException e) {}
      }
    }
}
```

#### JDBC 조회 기능의 예외처리

- 조회를 위한 JDBC 코드는 좀더 복잡해진다. `Connection`과 `PreparedStatement` 외에도 `ResultSet`이 추가되기 때문이다.

```java
public int getCount() throws ClassNotFoundException, SQLException {
  Connection c = null;
  PreparedStatement ps = null;
  ResultSet rs = null;

  try {
    c = dataSource.getConnection();
    ps = c.prepareStatement("select count(*) from users");
    rs = ps.executeQuery();
    rs.next();
    return rs.getInt(1);
  } catch (SQLException e) {
    throw e;
  } finally {
    if (rs != null) {
      try {
        rs.close();
      } catch (SQLException e) {
      }
    }
    if (ps != null) {
      try {
        ps.close();
      } catch (SQLException e) {
      }
    }
    if (c != null) {
      try {
        c.close();
      } catch (SQLException e) {
      }
    }
  }
}
```

## 2. 변하는 것과 변하지 않는 것

### 2-1. JDBC try/catch/finally 코드의 문제점

- 복잡한 `try/catch/finally` 블록이 2중으로 중첩까지 되어 나오는데다, 모든 메소드마다 반복된다.
  - 위 코드를 Copy&Paste 하다보면 실수가 나올 수밖에 없다.
- 테스트를 통해 DAO마다 예외상황에서 리소스를 반납하는지 체크한다면? => 좋은 생각이긴 한데 막상 적용하기는 쉽지 않다.
  - 예외상황을 처리하는 코드는 테스트하기가 매우 어렵고 모든 DAO 메소드에 대해 이런 테스트를 일일이 한다는 건 매우 번거롭다.
  - 게다가 강제로 예외상황을 만들려면 테스트를 위해 특별히 개발한 `Connection`과 `PreparedStatement` 구현 클래스가 필요하다. => 구현? 쉽지 않다.
  - 또한, 테스트 코드의 양이 엄청나게 늘어난다.

### 2-2. 분리와 재사용을 위한 디자인 패턴 적용

- 먼저 변하는 부분과 변하지 않는 부분을 분리해보자.
  - 변하지 않는 부분: `Connection`과 `PreparedStatement` (+ `ResultSet`)을 가져오는 코드와 실행하는 부분
  - 변하는 부분: `PreparedStatement`를 만드는 부분 (+ `ResultSet`으로 조회하는 부분)

#### 메소드 추출

- 먼저 생각해볼 수 있는 방법은 변하는 부분을 메소드로 빼는 것이다.
  - 당장 적용해봐선 별 이득이 없어보인다.

#### 템플릿 메소드 패턴의 적용

- 변하지 않는 부분은 슈퍼클래스에 두고 변하는 부분은 추상 메소드로 정의해둬서 서브클래스에서 오버라이드하여 새롭게 정의해 쓰도록 하는 방법이다.
- 아래는 `UserDao`를 Abstract Class로 만들고 `UserDaoDeleteAll` 클래스를 구현한 예시이다.

```java
public class UserDaoDeleteAll extends UserDao {
  
  protected PreparedStatement makeStatement(final Connection c) throws SQLException {
    return c.prepareStatement("delete from users");
  }
}
```

- OCP를 그럭저럭 지키는 구조를 만들어 낼 수는 있을 것 같다. 하지만 템플릿 메소드 패턴으로의 접근은 제한이 많다.
  - 가장 큰 문제는 DAO 로직마다 상속을 통해 새로운 클래스를 만들어야 한다는 점이다.

```mermaid
classDiagram
  UserDao <|-- UserDaoAdd
  UserDao <|-- UserDaoDeleteAll
  UserDao <|-- UserDaoGet
  UserDao <|-- UserDaoGetCount
  class UserDao {
      <<abstract>>
      makeStatement()*
  }
  class UserDaoAdd {
      makeStatement()
  }
  class UserDaoDeleteAll {
      makeStatement()
  }
  class UserDaoGet {
      makeStatement()
  }
  class UserDaoGetCount {
      makeStatement()
  }
```

- 또 확장구조가 이미 클래스를 설계하는 시점에서 고정되어 버린다는 점이다.
  - 이미 클래스 레벨에서 컴파일 시점에 이미 그 관계가 결정되어 있따.
  - 따라서 그 관계에 대한 유연성이 떨어져 버린다.

#### 전략 패턴의 적용

- 전략 패턴은 OCP를 잘 지키는 구조이면서도 템플릿 메소드 패턴보다 유연하고 확장성이 뛰어나다.
- `deleteAll()`의 컨텍스트를 정리해보면 다음과 같다.
  - DB 커넥션 가져오기
  - `PreparedStatement`를 만들어줄 외부 기능 호출하기
  - 전달받은 `PreparedStatement` 실행하기
  - 예외가 발생하면 이를 다시 메소드 밖으로 던지기
  - 모든 경우에 만들어진 `PreparedStatement`와 `Connection`을 적절히 닫아주기
- 두 번째 작업인 "`PreparedStatement`를 만들어줄 외부 기능 호출하기"가 전략 패턴에서 말하는 전략이라고 볼 수 있다.

```java
public interface StatementStrategy {
  PreparedStatement makePreparedStatement(Connection connection) throws SQLException;
}
```

- `deleteAll()` 예시

```java
public class DeleteAllStatement implements StatementStrategy {
  @Override
  public PreparedStatement makePreparedStatement(final Connection c) throws SQLException {
    return c.prepareStatement("delete from users");
  }
}
```

```java
public void deleteAll() throws SQLException {
  ...
  try {
    c = dataSource.getConnection();
  
    StatementStrategy strategy = new DeleteAllStatement();
    ps = strategy.makePreparedStatement(c);
  
    ps.executeUpdate();
  } catch (SQLException e) {
  ...
}
```

- 하지만 이렇게 컨텍스트 안에 구체적인 전략 클래스인 `DeleteAllStatement`를 사용하도록 고정되어 있다면 뭔가 이상하다.
  - 컨텍스트가 `StatementStrategy` 인터페이스뿐 아니라 특정 구현 클래스인 `DeleteAllStatement`를 직접 알고 있다는 건, 전략패턴에도 OCP에도 잘 들어맞는다고 볼 수 없기 때문.

#### DI 적용을 위한 클라이언트/컨텍스트 분리

- 전략 패턴에 따르면 `Context`가 어떤 전략을 사용하게 할 것인가는 `Context`를 사용하는 앞단의 `Client`가 결정하는 게 일반적이다.
  - `Client`가 구체적인 전략의 하나를 선택하고 오브젝트로 만들어서 `Context`에 전달하는 것이다.

```mermaid
classDiagram
  Client --> Context: 전략 제공
  Context --> Strategy: 컨텍스트 정보 제공
  Strategy <|.. ConcreteStrategyA
  Strategy <|.. ConcreteStrategyB
  Client --> ConcreteStrategyA: 전략 선택, 생성
  class Context {
      contextMethod()
  }
  class Strategy {
      <<interface>>
      algorithmMethod()*
  }
  class ConcreteStrategyA {
      algorithmMethod()
  }
  class ConcreteStrategyB {
      algorithmMethod()
  }
```

- 중요한 것은 이 컨텍스트에 해당하는 JDBC `try/catch/finally` 코드를 클라이언트 코드인 `StatementStrategy`를 만드는 부분에서 독립시켜야 한다는 점이다.

```java
StatementStrategy strategy = new DeleteAllStatement();
```

- 전략 인터페이스인 `StatementStrategy`를 컨텍스트 메소드 파라미터로 지정할 필요가 있다.

```java
public void jdbcContextWithStatementStrategy(StatementStrategy strategy) throws SQLException {
  Connection c = null;
  PreparedStatement ps = null;
  try {
    c = dataSource.getConnection();

    ps = strategy.makePreparedStatement(c);

    ps.executeUpdate();
  } catch (SQLException e) {
    throw e;
  } finally {
    if (ps != null) {
      try {
        ps.close();
      } catch (SQLException e) {
      }
    }
    if (c != null) {
      try {
        c.close();
      } catch (SQLException e) {
      }
    }
  }
}
```

> #### 마이크로 DI
> - 의존관계 주입은 다양한 형태로 적용할 수 있다.
>   - DI의 가장 중요한 개념은 제 3자의 도움을 통해 두 오브젝트 사이에 유연한 관계가 설정되도록 만든다는 것이다.
> - 일반적으로 DI는 의존관계에 있는 두 개의 오브젝트와 이 관계를 다이내믹하게 설정해주는 `오브젝트 팩토리(DI Container)`, 그리고 이를 사용하는 `클라이언트`라는 4개의 오브젝트 사이에서 일어난다.
> - 하지만 때로는 원시적인 전략 패턴 구조를 따라 클라이언트가 오브젝트 팩토리의 책임을 함께 지고 있을 수도 있다.
>   - 또는 클라이언와 전랴이 결합될 수도 있다.
>   - 심지어 클라이언트와 DI 관계에 있는 두 개의 오브젝트가 모두 하나의 클래스 안에 담길 수도 있다.
> - 이런 경우에는 DI가 매우 작은 단위의 코드와 메소드 사이에서 일어나기도 한다.
>   - 얼핏 보면 DI 같아 보이지 않지만, 세밀하게 관찰해보면 작은 단위지만 엄연히 DI가 이뤄지고 있음을 알 수 있따.
>   - 이렇게 DI의 장점을 단순화해서 IoC 컨테이너의 도움 없이 코드 내에서 적용한 경우를 마이크로 DI라고 한다.
>   - 또는 코드에 의한 DI라는 의미로 수동 DI라고 부를 수도 있다.

## 3. JDBC 전략 패턴의 최적화

### 3-1. 전략 클래스의 추가 정보

- `add()`는 `deleteAll()`과 달리 `User`를 전달받아야 한다.
  - `StatementStrategy`의 구현체인 `AddStatement`의 생성자에 `User`를 전달받는 방식 사용.

```java
public void add(User user) throws SQLException {
  StatementStrategy st = new AddStatement(user);
  jdbcContextWithStatementStrategy(st);
}
```

### 3-2. 전략과 클라이언트의 동거

- 두 가지 불만
1. DAO 메소드마다 새로운 `StatementStrategy` 구현 클래스를 만들어야 한다는 점.
2. DAO 메소드에서 `StatementStrategy`에 전달할 `User`와 같은 부가적인 정보가 있는 경우, 이를 위해 오브젝트를 전달받는 생성자와 이를 저장해둘 인스턴스 변수를 번거롭게 만들어야 한다는 점.

#### 로컬 클래스

- `StatementStrategy` 전략 클래스를 매번 독립된 파일로 만들지 말고 `UserDao` 메소드 안에 내부 클래스로 정의

> #### 중첩 클래스의 종류
> - 다른 클래스 내부에 정의되는 클래스를 `중첩 클래스 nested class`라고 한다. 중첩 클래스는 두 가지로 분류할 수 있다. 
>   - 독립적으로 오브젝트로 만들어질 수 있는 `static class`
>   - 자신이 정의된 클래스의 오브젝트 안에서만 만들어질 수 있는 `inner class`
> - `inner class`는 다시 `범위 scope`에 따라 세 가지로 구분된다.
>   - `member inner class`: 오브젝트 레벨에서 정의
>   - `local class`: 메소드 레벨에서 정의
>   - `anonymous inner class`: 이름을 갖지 않는 클래스. 범위는 선언된 위치에 따라서 다르다.

#### 익명 내부 클래스

> #### anonymous inner class
> `new 인터페이스이름() {클래스 본문}`

## 4. 컨텍스트와 DI

### 4-1. JdbcContext 분리

#### 클래스 분리

```java
public class UserDao {

  private JdbcContext jdbcContext;

  public void setJdbcContext(final JdbcContext jdbcContext) {
    this.jdbcContext = jdbcContext;
  }

  public void add(User user) throws SQLException {
    jdbcContext.workWithStatementStrategy(c -> {
      PreparedStatement ps = c.prepareStatement(
          "insert into users(id, name, password) values(?,?,?)");
      ps.setString(1, user.getId());
      ps.setString(2, user.getName());
      ps.setString(3, user.getPassword());
      return ps;
    });
  }
  
  // ...
}
```

#### 빈 의존관계 변경

```mermaid
classDiagram 
    direction LR
  UserDao ..> JdbcContext
  JdbcContext ..> Datasource
  Datasource <|.. SimpleDiverDs
  
  class UserDao {
      add()
      deleteAll()
  }
  class JdbcContext {
      contextMethod()
  }
  class Datasource {
      <<interface>>
      getConnection()*
  }
  class SimpleDiverDs {
    getConnection()
  }
```

- 스프링의 빈 설정은 클래스 레벨이 아니라 런타임 시에 만들어지는 오브젝트 레벨의 의존관계에 따라 정의된다.
- 빈으로 정의되는 오브젝트 사이의 관계를 그려보면 아래와 같다.
  - 기존에는 `userDao` 빈이 `dataSource` 빈을 직접 의존했지만 이제는 `jdbcContext` 빈이 그 사이에 끼게 된다.

```mermaid
classDiagram
  UserDao ..> JdbcContext
  JdbcContext ..> Datasource-SimpleDiverDs

  class UserDao {
    jdbcContext
  }
  class JdbcContext {
      dataSource
  }
  class Datasource-SimpleDiverDs {
      driverClass
      url
      username
      password
  }
```

- 설정도 아래와 같이 변경된다.

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://www.springframework.org/schema/beans
                           http://www.springframework.org/schema/beans/spring-beans-3.0.xsd">

  <bean id="dataSource" class="org.springframework.jdbc.datasource.SimpleDriverDataSource">
    <property name="driverClass" value="com.mysql.cj.jdbc.Driver"/>
    <property name="url" value="jdbc:mysql://localhost:3307/testdb" />
    <property name="username" value="root"/>
    <property name="password" value="qwer1234"/>
  </bean>

  <!-- 추가된 부분 -->
  <bean id="userDao" class="org.mobilohas.bell.ch3.user.dao.UserDao">
    <property name="jdbcContext" ref="jdbcContext"/>
  </bean>

  <bean id="jdbcContext" class="org.mobilohas.bell.ch3.user.dao.JdbcContext">
    <property name="dataSource" ref="dataSource" />
  </bean>
</beans>
```

### 4-2. JdbcContext의 특별한 DI

- `UserDao`는 인터페이스 거치지 않고 바로 `JdbcContext` 클래스를 사용하고 있다.
  - `UserDao`와 `JdbcContext`는 클래스 레벨에서 의존관계가 결정된다.
  - 비록 런타임 시에 DI 방식으로 외부에서 오브젝트를 주입해주는 방식을 사용하긴 했지만, 의존 오브젝트의 구현 클래스를 변경할 수는 없다.

#### 스프링 빈으로 DI

- `의존관계 주입 DI`이라는 개념을 충실히 따르자면, 인터페이스를 사이에 둬서 클래스 레벨에서 의존관계가 고정되지 않게 하고, 런타임 시에 의존할 오브젝트와의 관계를 동적으로 주입해주는 게 맞다.
  - 따라서 인터페이스를 사용하지 않았다면 엄밀히 말해 온전한 DI라고 볼 수는 없다.
  - 그러나 DI를 넓은 개념에서 본다면, 객체의 생성과 관계 설정에 대한 제어권한을 오브젝트에서 제거하고 외부로 위임했다는 IoC라는 개념을 포괄한다.
  - 그런 의미에서 `JdbcContext`를 스프링을 이용해 `UserDao` 객체에서 사용하게 주입했다는 건 DI의 기본을 따르고 있다고 볼 수 있다.
- 인터페이스를 사용해서 클래스를 자유롭게 변경할 수 있게 하지는 않았지만, `JdbcContext`를 `UserDao`와 DI 구조로 만들어야 할 이유
1. `JdbcContext`가 스프링 컨테이너의 싱글톤 레지스트리에서 관리되는 싱글톤 빈이 되기 때문
2. `JdbcContext`가 DI를 통해 다른 빈에 의존하고 있기 때문 (`DataSource`)
- 여기서 중요한 것은 인터페이스의 사용 여부다. 왜 인터페이스를 사용하지 않았을까?
  - 인터페이스 X => 객체 간에 강하게 결합
  - 하지만 `JdbcContext`는 `DataSource`와는 달리 테스트에서도 다른 구현으로 대체해서 사용할 이유가 없다.

#### 코드를 이용하는 수동 DI

- `JdbcContext`를 스프링의 빈으로 등록해서 `UserDao`에 DI 하는 대신 사용할 수 있는 방법이 있다. `UserDao` 내부에서 직접 DI를 적용하는 방법이다.
  - 대신 싱글톤으로 만드는 것을 포기해야 한다. Dao마다 `JdbcContext`를 가지게 해야할 것이다.
- 남은 문제는 스프링의 빈인 `DataSource`를 주입받아야 한다는 것이다.
  - `JdbcContext`는 빈으로 등록되지 않아 DI를 받을 수 없기 때문에 `UserDao`를 통해 받는 수 밖에 없다.  

```mermaid
classDiagram
  UserDao ..> Datasource-SimpleDiverDs: 스프링을 통한 DI
  UserDao --> JdbcContext: 생성 및 DataSource 주입
  JdbcContext --> Datasource-SimpleDiverDs: UserDao 코드에 의한 DI

  class UserDao {
    dataSource
    jdbcContext
  }
  class JdbcContext {
    dataSource
  }
  class Datasource-SimpleDiverDs {
    driverClass
    url
    username
    password
  }
```

## 5. 템플릿과 콜백

- 현재 코드 구성은 전략 패턴의 기본 구조에 익명 내부 클래스를 활용한 방식이다.
  - 이런 방식을 스프링에서는 `템플릿/콜백 패턴`이라고 부른다.
  - 전략 패턴의 컨텍스트를 템플릿이라 부르고, 익명 내부 클래스로 만들어지는 오브젝트를 콜백이라고 부른다.

> #### 템플릿 Template
> - 어떤 목적을 위해 미리 만들어둔 모양이 있는 틀을 가리킨다.
> - 템플릿 메소드 패턴은 고정된 틀의 로직을 가진 템플릿 메소드를 슈퍼클래스에 두고, 바뀌는 부분을 서브클래스의 메소드에 두는 구조로 이뤄진다.
>
> #### 콜백 Callback
> - 실행되는 것을 목적으로 다른 오브젝트의 메소드에 전달되는 오브젝트를 의미
> - 파라미터로 전달되지만 값을 참조하기 위한 것이 아니라 특정 로직을 담은 메소드를 실행ㅎ시키기 위해 사용

### 5-1. 템플릿/콜백의 동작원리

- 여기서 콜백은 템플릿 안에서 호출되는 것을 목적으로 만들어진 오브젝트를 말한다.

#### 템플릿/콜백의 특징

- 여러 개의 메소드를 가진 일반적인 인터페이스를 사용할 수 있는 전략 패턴의 전략과 달리 템플릿/콜백 패턴의 콜백은 보통 단일 메소드 인터페이스를 사용한다.
  - 하나의 템플릿에서 여러 가지 종류의 전략을 사용해야 한다면 하나 이상의 콜백 오브젝트를 사용할 수도 있다.
- 클라이언트가 템플릿 메소드를 호출하면서 콜백 오브젝트를 전달하는 것은 메소드 레벨에서 일어나는 DI다.
  - 콜백 오브젝트가 내부 클래스로서 자신을 생성한 클라이언트 메소드 내의 정보를 직접 참조한다는 것도 템플릿/콜백의 고유한 특징이다.
  - 클라이언트와 콜백이 강하게 결합된다는 면에서도 일반적으로 DI와 조금 다르다.

### 5-2. 편리한 콜백의 재활용

#### 콜백의 분리와 재활용

- 복잡한 익명 내부 클래스의 사용을 최소화할 수 있는 방법
- 변하는 것과 변하지 않는 것을 분리
  - 변하는 SQL 문장 
  - 변하지 않는 콜백 클래스 정의와 오브젝트

```java
// 변하는 부분
public void deleteAll() throws SQLException {
  executeSql("delete from users");
}
```

```java
// 변하지 않는 부분
private void executeSql(final String query) throws SQLException {
  jdbcContext.workWithStatementStrategy(c -> c.prepareStatement(query));
}
```

#### 콜백과 템플릿의 결합

- `executeSql()` 메소드는 `UserDao`만 사용하기는 아깝다.
  - 이렇게 재사용 가능한 콜백을 담고 있는 메소드라면 DAO가 공유할 수 있는 템플릿 클래스 안으로 옮겨도 된다.

```java
public class JdbcContext {
  // ...
  public void executeSql(final String query) throws SQLException {
    workWithStatementStrategy(c -> c.prepareStatement(query));
  }
}
```

- 일반적으로 성격이 다른 코드들은 가능한 한 분리하는 편이 낫지만, 이 경우는 반대다.
  - 하나의 목적을 위해 서로 긴밀하게 연관되어 동작하는 응집력 강한 코드들이기 때문에 한 군데 모여 있는 게 유리하다.
- 구체적인 구현과 내부의 전략 패턴, 코드에 의한 DI, 익명 내부 클래스 등의 기술은 최대한 감춰두고, 외부에는 꼭 필요한 기능을 제공하는 단순한 메소드만 노출해주는 것이다.

### 5-3. 템플릿/콜백의 응용

- 스프링의 많은 API나 기능을 살펴보면 템플릿/콜백 패턴을 적용한 경우를 많이 발견할 수 있다.

#### 테스트와 try/catch/finally


# 참고자료

- 토비의 스프링, 이일민 저
