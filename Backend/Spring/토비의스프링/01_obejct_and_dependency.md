# Chapter01. 오브젝트와 의존관계

- 스프링을 이해하려면 먼저 오브젝트에 깊은 관심을 가져야 한다.
- 애플리케이션에서 오브젝트가 생성되고 다른 오브젝트와 관계를 맺고, 사용되고, 소멸하기까지의 전 과정을 진지하게 생각해볼 필요가 있다.
- 더 나아가서 오브젝트는 어떻게 설계돼야 하는지, 어떤 단위로 만들어지며 어떤 과정을 통해 자신의 존재를 드러내고 등장해야 하는지에 대해서도 살펴봐야 한다.
- 결국 오브젝트 설계와 구현에 관한 여러 가지 응용 기술과 지식이 요구된다.

## 1.1 초난감 DAO

### 1.1.1 User

> #### DAO
> - `Data Access Object`는 DB를 사용해 데이터를 조회하거나 조작하는 기능을 전담하도록 만든 오브젝트를 말한다.

> #### JavaBean
> - JavaBean은 원래 비주얼 툴에서 조작 가능한 컴포넌트를 말한다.
> - 이제 JavaBean이라고 말하면 비주얼 컴포넌트라기보다는 다음 두 가지 관계에 따라 만들어진 오브젝트를 가리킨다. (간단하게 Bean이라고 부르기도 한다.)
> 1. `디폴트 생성자`: 자바빈은 파라미터가 없는 디폴트 생성자를 갖고 있어야 한다. 툴이나 프레임워크에서 리플렉션을 이용해 오브젝트를 생성하기 때문에 필요하다.
> 2. `프로퍼티`: 자바빈이 노출하는 이름을 가진 속성을 프로퍼티라고 한다. 프로퍼티는 set으로 시작하는 수정자 메소드와 get으로 시작하는 접근자 메소드를 이용해 수정 또는 조회할 수 있다.

### 1.1.2 UserDao

- JDBC를 이용하는 작업의 일반적인 순서는 다음과 같다.
1. DB 연결을 위한 `Connection`을 가져온다.
2. SQL을 담은 `Statement(또는 PreparedStatement)`를 만든다.
3. 만들어진 `Statement`를 실행한다.
4. 조회의 경우 SQL 쿼리의 실행 결과를 `ResultSet`으로 받아서 정보를 저장할 오브젝트에 옮겨준다. (여기서는 User)
5. 작업 중에 생성된 `Connection`, `Statement`, `ResultSet` 같은 리소스는 작업을 마친 후 반드시 닫아준다.
6. JDBC API가 만들어내느 `예외 Exception`을 잡아서 직접 처리하거나, 메소드에 `throws`를 선언해서 예외가 발생하면 메소드 밖으로 던지게 한다.

```java
public class UserDao {

  public void add(User user) throws ClassNotFoundException, SQLException {
    Class.forName("com.mysql.cj.jdbc.Driver");
    Connection c = DriverManager.getConnection(
        "jdbc:mysql://localhost:3307/spring", "root", "qwer1234");

    PreparedStatement ps = c.prepareStatement(
        "insert into users(id, name, password) values(?,?,?)");
    ps.setString(1, user.getId());
    ps.setString(2, user.getName());
    ps.setString(3, user.getPassword());

    ps.executeUpdate();

    ps.close();
    c.close();
  }

  public User get(String id) throws ClassNotFoundException, SQLException {
    Class.forName("com.mysql.cj.jdbc.Driver");
    Connection c = DriverManager.getConnection(
        "jdbc:mysql://localhost:3307/spring", "root", "qwer1234");

    PreparedStatement ps = c.prepareStatement(
        "select * from users where id = ?");
    ps.setString(1, id);

    ResultSet rs = ps.executeQuery();
    rs.next();
    User user = new User();
    user.setId(rs.getString("id"));
    user.setName(rs.getString("name"));
    user.setPassword(rs.getString("password"));

    rs.close();
    ps.close();
    c.close();

    return user;
  }
}
```

- 초난감 코드의 조건을 두루 갖춘 DAO 코드
  - 왜 이 코드에 문제가 많다고 하는 것일까?
  - 잘 동작하는 코드를 굳이 수정하고 개선해야 하는 이유는 뭘까?
  - 그렇게 DAO 코드를 개선했을 때의 장점은 무엇일까?
  - 그런 장점들이 당장에, 또는 미래에 주는 유익은 무엇인가?
  - 또, 객체지향 설계의 원칙과는 무슨 상관이 있을까?
  - 이 DAO를 개선하는 경우와 그대로 사용하는 경우, 스프링을 사용하는 개발에서 무슨 차이가 있을까?
- 스프링을 공부한다는 건 바로 이런 문제 제기와 의문에 대한 답을 찾아나가는 과정이다.
  - 스프링은 기계적인 답변이나 성급한 결론을 주지 않는다.

## 1.2 DAO의 분리

### 1.2.1 관심사의 분리

- 어떻게 변경이 일어날 때 필요한 작업을 최소화하고, 그 변경이 다른 곳에 문제를 일으키지 않게 할 수 있을까?
  - `분리와 확장`을 고려한 설계

#### 분리 

- `관심사의 분리 Separation of Concerns`
- 변화는 대체로 집중된 한 가지 관심(ex. DB를 오라클에서 MySQL로 수정)에 대해 일어나지만 그에 따른 작업은 한 곳에 집중되지 않는 경우가 많다.
  - 단지 DB 접속용 암호를 변경하려고 DAO 클래스 수백 개를 모두 수정?
  - 트랜잭션이 기술이 바뀌어 비즈니스 로직이 담긴 코드의 구조를 모두 변경해야 한다면?
- 변화가 한 번에 한 가지 관심에 집중돼서 일어난다면, 우리가 준비해야 할 일은 한 가지 관심이 한 군데에 집중되게 하는 것이다.
  - 즉 관심이 같은 것끼리는 모으고, 관심이 다른 것은 따로 떨어져 있게 하는 것이다.

### 1.2.2 커넥션 만들기 추출

#### UserDao 관심사

1. DB와 연결을 위한 커넥션 가져오기
   - 어떤 DB? 어떤 드라이버? 어떤 로그인 정보? 그 커넥션을 생성하는 방법은 또 어떤 것인지?
2. 사용자 등록을 위해 DB에 보낼 SQL 문장을 담을 `Statement`를 만들고 실행하는 것.
   - 바인딩과 실행.
3. 작업이 끝난 리소스를 시스템에 반

- 현재 관심사가 방만하게 중복되어 있고, 여기저기 흩어져 있어서 다른 관심의 대상과 얽혀 있으면, 변경이 일어날 때 엄청난 고통을 일으키는 원인이 된다.

#### 중복 코드 메소드 추출

```java
public class UserDao {
  
  //...
  
  private static Connection getConnection() throws ClassNotFoundException, SQLException {
    Class.forName("com.mysql.cj.jdbc.Driver");
    Connection c = DriverManager.getConnection(
        "jdbc:mysql://localhost:3307/spring", "root", "qwer1234");
    return c;
  }
}
```

#### 변경에 대한 검증: 리팩토링과 테스트

> #### 리팩토링
> - 기존의 코드를 외부의 동작방식에는 변화 없이 내부 구조를 변경해서 재구성하는 작업 또는 기술.

- 위의 기법은 리팩토링 중에서도 `메소드 추출 extract method`이다.

# 참고자료

- 토비의 스프링, 이일민 저