# Chapter 12 다형성

- 코드 재사용을 목적으로 상속을 사용하면 변경하기 어렵고 유연하지 못한 설계에 이를 확률이 높아진다.
  - 상속의 목적은 코드 재사용이 아니다.
  - 상속은 타입 계층을 구조화하기 위해 사용해야 한다.
- 상속의 관점에서 다형성이 구현되는 기술적인 메커니즘.
  - 다형성이 런타임에 메시지를 처리하기에 적합한 메소드를 동적으로 탐색하는 과정을 통해 구현되며, 상속이 이런 메소드를 찾기 위한 일종의 탐색 경로를 클래스 계층의 형태로 구현하기 위한 방법이라는 사실.


## 1. 다형성

- `다형성 polymorphism`이라는 단어는 그리스어에서 '많은'을 의미하는 'poly'와 '형태'를 의미하는 `morph`의 합성어.
  - '많은 형태를 가질 수 있는 능력'
  - 컴퓨터 과학에서는 다형성을 하나의 추상 인터페이스에 대해 코드를 작성하고 이 추상 인터페이스에 대해 서로 다른 구현을 연결할 수 있는 능력으로 정의한다.

<img src="img/polymorphism01.jpg">

- 객체지향 프로그래밍에서 사용되는 다형성은 `유니버설(Universal) 다형성 `과 `임시(Ad Hoc) 다형성`으로 분류할 수 있다.
- 유니버설 다형성은 다시 `매개변수(Parametric) 다형성`과 `포함(Inclusion) 다형성`으로 분류할 수 있다.
- 임시 다형성은 `오버로딩(Overloading) 다형성`과 `강제(Coercion) 다형성`으로 분류할 수 있다.

#### 오버로딩

- 아래와 같이 동일한 이름의 다양한 메소드가 존재하는 것을 의미한다.
  - 기억해야 하는 이름의 수를 극적으로 줄일 수 있다는 장점이 있다.

```java
public class Money {
  public Money plus(Money amount) {...}
  public Money plus(BigDecimal amount) {...}
  public Money plus(long amount) {...}
}
```

#### 강제 다형성

- 언어가 지원하는 자동적인 타입 변환이나 사용자가 직접 구현한 타입 변환을 이용해 동일한 연산자를 다양한 타입에 사용할 수 있는 방식을 가리킨다.
- 예를 들어 자바에서 이항 연산자인 '+'는 피연산자가 모두 정수일 경우에는 정수에 대한 덧셈 연산자로 동작하지만 하나는 정수형이고 다른 하나는 문자열인 경우 연결 연산자로 동작한다.
  - 정수형 피연산자는 문자열 타입으로 **강제 형변환**된다.

```java
10 + 10 // 덧셈 연산으로 동작. 결과값: 20
"10" + "10" // 연결 연산으로 동작. 결과값: "1010"
```

#### 매개변수 다형성

- `제너릭 프로그래밍 Generic Programming`과 관련이 깊다.
- 클래스의 인스턴스 변수나 메소드의 매개변수 타입을 임의의 타입으로 선언한 후 사용하는 시점에 구체적인 타입으로 지정하는 방식을 가리킨다.
- 자바의 `List`
  - 컬렉션에 보관할 요소의 타입의 임의의 타입 T로 지정
  - 실제 인스턴스를 생성하는 시점에 T를 구체적인 타입으로 지정할 수 있게 하고 있다.
  - 따라서 다양한 타입의 요소를 다루기 위해 동일한 오퍼레이션을 사용할 수 있다.

```java
public interface List<E> extends SequencedCollection<E> {...}
```

```java
List<String> string = new ArrayList<>();
List<Integer> integer = new ArrayList<>();
List<Member> members = new ArrayList<>();        
```

#### 포함 다형성

- 메시지가 동일하더라도 수신한 객체의 타입에 따라 실제로 수행되는 행동이 달라지는 능력을 의미.
- `서브타입(Sybtype) 다형성`이라고도 부른다.
- 객체지향 프로그래밍에서 가장 널리 알려진 형태의 다형성이기 때문에 특별한 언급 없이 다형성이라고 할 때는 포함 다형성을 의미하는 것이 일반적이다.
- `Movie` 클래스의 예시
  - `Movie` 클래스는 `discountPolicy`에게 `calculateDiscountAmount` 메시지를 전송하지만 실제로 실행되는 메서드는 메시지를 수신한 객체의 타입에 따라 달라진다.

```java
public class Movie {
  private DiscountPolicy discountPolicy;
  
  public Money calculateMoveiFee(Screening screening) {
    return fee.minus(discountPolicy.calculateDiscountAmount(screening));
  }
}
```

- 포함 다형성을 구현하는 가장 일반적인 방법은 상속을 사용하는 것이다.
- 포함 다형성을 서브타입 다형성이라고 부른다는 사실에서 예상할 수 있겠지만 포함 다형성을 위한 전제조건은 자식 클래스가 부모 클래스의 서브타입이어야 한다는 것이다.
  - 그리고 상속의 진정한 목적은 코드 재사용이 아니라 다형성을 위한 서브타입 계층을 구축하는 것이다.
- 상속의 일차적인 목적은 코드 재사용이 아니라 서브타입의 구현이다.
- 포함 다형성을 위해 상속을 사용하는 가장 큰 이유는 상속이 클래스들을 계층으로 쌓아 올린 후 상황에 따라 적절한 메소드를 선택할 수 있는 메커니즘을 제공하기 때문이다.
  - 객체가 메시지를 수신하면 객체지향 시스템은 메시지를 처리할 적절한 메서드를 상속 계층 안에서 탐색한다.
  - 실행할 메서드를 선택하는 기준은 어떤 메시지를 수신했는지에 따라, 어떤 클래스의 인스턴스인지에 따라, 상속 계층이 어떻게 구성돼 있는지에 따라 달라진다.

<br/>

## 2. 상속의 양면성

- 객체지향 패러다임의 근간을 이루는 아이디어. 데이터와 행동을 객체라고 불리는 하나의 실행 단위 안으로 통합하는 것이다.
  - 데이터와 행동. 두 가지 관점을 함께 고려해야 한다.
- 상속 역시 예외는 아니다.
  - 부모 클래스에서 정의한 모든 데이터를 자식 클래스의 인스턴스에 자동으로 포함시킬 수 있다. 이것이 데이터 관점의 상속.
  - 데이터뿐만 아니라 부모 클래스에서 정의한 일부 메소드 역시 자동으로 자식 클래스에 포함시킬 수 있다. 이것이 행동 관점의 상속.
- 단순히 데이터와 행동의 관점에서만 바라보면 상속이란 부모 클래스에서 정의한 데이터와 행동을 자식 클래스에서 자동적으로 공유할 수 있는 재사용 메커니즘으로 보일 것이다.
  - 하지만 이 관점은 상속을 오해한 것이다.
- 상속의 목적은 코드 재사용이 아니다.
  - 상속은 프로그램을 구성하는 개념들을 기반으로 다형성을 가능하게 하는 타입 계층을 구축하기 위한 것이다.
  - 타입 계층에 대한 고민 없이 코드를 재사용하기 위해 상속을 사용하면 이해하기 어렵고 유지보수하기 버거운 코드가 만들어질 확률이 높다.
  - 문제를 피할 수 있는 유일한 방법은 상속이 무엇이고 언제 사용해야 하는지를 이해하는 것뿐이다.
- 상속의 메커니즘을 이해하기 위한 몇 가지 개념
  - 업캐스팅
  - 동적 메소드 탐색
  - 동적 바인딩
  - self 참조
  - super 참조

### 상속을 사용한 강의 평가

#### Lecture 클래스 살펴보기

- 프로그램은 아래와 같이 전체 수강생들의 성적 통계를 출력한다.

```
Pass:3 Fail:2, A:1 B:1 C:1 D:0 F:2
```

- 출력은 두 부분으로 구성
  - `Pass:3 Fail:2`: 이수한 학생과 낙제한 학생의 수
  - `A:1 B:1 C:1 D:0 F:2`: 학생들의 분포 현황
- `Lecture` 클래스의 구현

```java
public class Lecture {
  private String title;
  private int pass;
  private List<Integer> scores = new ArrayList<>();

  public Lecture(final String title, final int pass, final List<Integer> scores) {
    this.title = title;
    this.pass = pass;
    this.scores = scores;
  }

  public double average() {
    return scores.stream()
        .mapToInt(Integer::intValue)
        .average().orElse(0);
  }

  public List<Integer> getScores() {
    return Collections.unmodifiableList(scores);
  }

  public String evaluate() {
    return String.format("Pass:%d Fail:%d", passCount(), failCount());
  }

  private long passCount() {
    return scores.stream().filter(score -> score >= pass).count();
  }

  private long failCount() {
    return scores.size() - passCount();
  }
}
```

#### 상속을 사용해 Lecture 클래스 재사용하기

- `Lecture` 클래스는 새로운 기능을 구현하는 데 필요한 대부분의 데이터와 메소드를 포함하고 있다.
  - 따라서 `Lecture` 클래스를 상속받으면 새로운 기능을 쉽고 빠르게 추가할 수 있을 것이다.

```java
public class GradeLecture extends Lecture {
  private List<Grade> grades;

  public GradeLecture(final String title, final int pass, final List<Integer> scores,
      final List<Grade> grades) {
    super(title, pass, scores);
    this.grades = grades;
  }

  @Override
  public String evaluate() {
    return super.evaluate() + ", " + gradesStatistics();
  }

  private String gradesStatistics() {
    return grades.stream()
        .map(this::format)
        .collect(Collectors.joining(" "));
  }

  private String format(Grade grade) {
    return String.format("%s:%d", grade.getName(), gradeCount(grade));
  }

  private long gradeCount(final Grade grade) {
    return getScores().stream()
        .filter(grade::include)
        .count();
  }
}
```

- 여기서 주목할 부분은 `GradeLecture`와 `Lecture`에 구현된 두 `evaluate` 메소드의 시그니처가 완전히 동일하다는 것이다.
  - 부모 클래스와 자식 클래스에 동일한 시그니처를 가진 메소드가 존재할 경우 자식 클래스의 메소드 우선순위가 더 높다.
  - 여기서 우선순위가 더 높다는 것은 메시지를 수신했을 때 부모 클래스의 메소드가 아닌 자식 클래스의 메소드가 실행된다는 것을 의미한다.
- 결과적으로 동일한 시그니처를 가진 자식 클래스의 메소드가 부모 클래스의 메소드를 가리게 된다.
  - 이처럼 자식 클래스 안에 상속받은 메소드와 동일한 시그너처의 메서드를 재정해서 부모 클래스의 구현을 새로운 구현으로 대체하는 것을 `메소드 오버라이딩`이라고 부른다.
- 자식 클래스에 부모 클래스에 없던 새로운 메소드를 추가하는 것도 가능하다.

```java
public class GradeLecture extends Lecture {
  public double average(String gradeName) {
    return grades.stream()
            .filter(each -> each.isName(gradeName))
            .findFirst()
            .map(this::gradeAverage)
            .orElse(0d);
  }

  private double gradeAverage(final Grade grade) {
    return getScores().stream()
            .filter(grade::include)
            .mapToInt(Integer::intValue)
            .average()
            .orElse(0);
  }
}
```

- evaluate 메소드와 달리 `GradeLecture`의 `average` 메소드는 부모 클래스인 `Lecture`에 정의된 `average` 메소드와 이름은 같지만 시그니처는 다르다.
  - 두 메소드의 시그니처가 다르기 때문에 `GradeLecture`의 `average`메소드는 `Lecture`의 `average` 메소드를 대체하지 않으며 결과적으로 두 메소드는 사이좋게 공존할 수 있다.
  - 다시 말해서 클라이언트는 두 메소드 모두를 호출할 수 있다는 것이다.
  - 이처럼 부모 클래스에서 정의한 메소드와 이름은 동일하지만 시그니처는 다른 메소드를 자식 클래스에 추가하는 것을 `메소드 오버로딩 method overloading`이라고 부른다.

### 데이터 관점의 상속

- 다음과 같이 `Lecture` 인스턴스를 생성했다.

```java
Lecture lecture = new Lecture("객체지향 프로그래밍", 70, Arrays.asList(81, 95, 75, 50, 45));
```

- `Lecture` 인스턴스를 생성하면 시스템은 인스턴스 변수 `title`, `pass`, `scores`를 저장할 수 있는 메모리 공간을 할당하고 생성자의 매개변수를 이용해 값을 설정한 후 생성된 인스턴스의 주소를 `lecture`라는 이름의 변수에 대입한다.

<img src="img/polymorphism02.jpg">

- 이번에는 `GradeLecture`를 생성해보자.

```java
GradeLecture gradeLecture = new GradeLecture(
        "객체지향 프로그래밍",
        70,
        Arrays.asList(81, 95, 75, 50, 45),
        Arrays.asList(
            new Grade("A", 100, 95),
            new Grade("B", 94, 80),
            new Grade("C", 79, 70),
            new Grade("D", 69, 50),
            new Grade("F", 49, 0)));
```

- 메모리 상에 생성된 `GradeLecture`의 인스턴스는 아래와 같이 표현할 수 있다.
  - 상속을 인스턴스 관점에서 바라볼 때는 개념적으로 자식 클래스의 인스턴스 안에 부모 클래스의 인스턴스가 포함되는 것으로 생각하는 것이 유용하다.
  - 인스턴스를 참조하는 `lecture`는 `GradeLecture`의 인스턴스를 가리키기 때문에 특별한 방법을 사용하지 않으면 `GradeLecture` 안에 포함된 `Lecture` 인스턴스에 직접 접근할 수 없다.

<img src="img/polymorphism03.jpg">

- 아래와 같이 자식 클래스의 인스턴스에서 부모 클래스의 인스턴스로 접근 가능한 링크가 존재한다고 생각하면 된다.

<img src="img/polymorphism04.jpg">

### 행동 관점의 상속

- 데이터 관점의 상속이 자식 클래스의 인스턴스 안에 부모 클래스의 인스턴스를 포함하는 개념이라면 행동 관점의 상속은 부모 클래스가 정의한 일부 메소드를 자식 클래스로 포함시키는 것을 의미한다.
- 공통적으로 부모 클래스의 모든 퍼블릭 메소드는 자식 클래스의 퍼블릭 인터페이스에 포함된다.
  - 따라서 외부의 객체가 부모 클래스의 인스턴스에게 전송할 수 있는 모든 메시지는 자식 클래스의 인스턴스에게도 전송할 수 있다.
  - 실제로 클래스의 코드를 합치거나 복사하는 작업이 수행되는 것은 아니다. 런타임에 시스템이 자식 클래스에 정의되지 않은 메소드가 있을 경우 이 메소드를 부모 클래스 안에서 탐색한다.
- 행동 관점에서 상속과 다형성의 기본적인 개념을 이해하기 위해서는 상속 관계로 연결된 클래스 사이의 메소드 탐색 과정을 이해하는 것이 가장 중요하다.
- 객체의 경우 서로 다른 상태를 저장할 수 있도록 각 인스턴스별로 독립적인 메모리를 할당받아야 한다.
  - 하지만 메소드의 경우에는 동일한 클래스의 인스턴스끼리 공유가 가능하기 때문에 클래스는 한 번만 메모리에 로드하고 각 인스턴스별로 클래스를 가리키는 포인터를 갖게 하는 것이 경제적이다.
- 아래의 그림은 두 개의 `Lecture` 인스턴스를 생성한 후의 메모리 상태를 개념적으로 표현한 것이다.
  - 인스턴스는 두 개가 생성됐지만 클래스는 단 하나만 메모리에 로드됐다는 사실에 주목하라.
  - 각 객체는 클래스인 `Lecture`의 위치를 가리키는 `class`라는 이름의 포인터를 가지며 이 포인터를 이용해 자신의 클래스 정보에 접근할 수 있다.
  - `Lecture` 클래스가 자신의 부모 클래스인 `Object`의 위치를 가리키는 `parent`라는 이름의 포인터를 가진다는 사실에도 주목하라
  - 이 포인터를 사용하면 클래스의 상속 계층을 따라 부모 클래스의 정의로 이동하는 것이 가능하다.
- 자식 클래스를 보고 메소드가 존재하지 않으면 클래스의 parent 포인터를 따라 부모 클래스를 차례대로 훑어가면서 적절한 메소드가 존재하는지를 검색한다.

<img src="img/polymorphism05.jpg">

- 마지막으로 `GradeLecutre` 클래스의 인스턴스를 생성했을 때의 메모리 구조를 살펴보자.
  - 아래 그림에서 볼 수 있는 것처럼 `GradeLecture`의 인스턴스는 `Lecture`의 인스턴스 내부에 포함된다.
  - `GradeLecture` 인스턴스의 class 포인터를 따라가면 `GradeLecture` 클래스에 이르고 parent 포인터를 따라가면 `Lecture` 그리고 `Object`까지 이어진다.

<img src="img/polymorphism06.jpg">

<br/>

# 참고자료

- 오브젝트, 조영호 지음
