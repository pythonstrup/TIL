# 파이썬 속성 강좌

### 출처

- "Data Science from scratch" - 저자 Joel Grus

### 1. 기본기 다지기

<hr/>

파이썬에는 설계 원칙에 대한 일종의 교리인 'The Zen of Python'이 있다.
여기서 가장 중요한 것은 "어떤 일에든 명확한-바람직하고 유일한 방법이 존재한다"라는 말이다.
명확한 방식으로 쓰여진 코드를 일반적으로 '파이썬스럽다(Pythonic)'이라고 한다. 이제부터 파이썬에 대해 알아보자.
<br/><br/>
"아름다움이 추함보다 좋다. 명시가 암시보다 좋다. 단순함이 복잡함보다 좋다."

<br/>

### 2. 파이썬 설치

<hr/>

- python.org 또는 아나콘다(Anaconda) 배포판

<br/>

### 3. 가상환경

<hr/>

matplotlib 라이브러리 사용하여 데이터 시각화를 해야하는데,
matplotlib은 파이썬에서 기본적으로 제공하는 라이브러리가 아니기 때문에 직접 설치해야한다.
독립적인 파이썬 및 라이브러리 환경을 제공하는 가상 환경(virtual environment)은 버전 충돌 문제를 해결해줄 수 있다.
아나콘다에서 가상환경을 생성해보자. 가상환경이 활성화되어 있는 동안 설치한 모든 라이브러리는 dsfs 가상 환경에만 설치된다.

```shell
# dsfs라는 파이썬 3.6 가상 환경 생성
conda create -n dsfs python=3.9

# 가상 환경 활성화
source activate dsfs

# 가상 환경 비활성화
source deactivate
```

여기에 더해 파이썬 셸인 IPython을 설치하면 좋을 것이다.

```shell
python -m pip install ipython
```

<br/>

### 4. 들여쓰기

<hr/>

다른 언어와는 다르게 파이썬에서는 들여쓰기를 통해 단락을 구분한다.
들여쓰기를 잘못하면 오류가 발생할 수 있기 때문에 주의해야 한다.

### 5. 모듈

<hr/>

모듈을 사용할 때는 import를 사용해 불러온다.
matplotlib 라이브러리로 데이터를 시각화할 때는 다음과 같은 별칭을 관습적으로 사용한다.

```python
import matplotlib.pyplot as plt

plt.plot(...)
```

<br/>

특정기능만 필요하다면 전체 모듈을 부르지 않고 해당 기능만 명시해서 불러올 수도 있다.

```python
from collections import defaultdict, Counter
lookup = defaultdict(int)
my_counter = Counter()
```

가장 좋지 않은 방법은 모듈의 기능을 통째로 불러와서 기존의 변수를 덮어쓰는 것이다.

```python
match = 10
from re import *   # re 라이브러리에 match라는 함수가 있기 때문에 변수를 덮어쓴다.
print(match)       # <function match at ~~~~~~>
```

### 6. 함수

<hr/>

일반함수

```python
def double(num):
    return num * 2
```

람다함수

```python
double = lambda x: x*2  # 좋은 방법은 아니다.
```

<br/>

### 7. 문자열

<hr/>

작은 따옴표 혹은 큰 따옴표로 묶어 나타낸다.
특수문자를 인코딩할 때는 \(역슬래쉬)를 붙여준다.
문자열을 합치는 방법은 아래와 같다. ('+' 사용하기 or .format 사용하기)

```python
first_name = "Joel"
last_name = "Grus"
full_name1 = first_name + " " + last_name
full_name1 = "{0} {1}".format(first_name, last_name)
```

<br/>

### 8. 예외처리

<hr/>

예외처리를 위해 사용하는 것은 try & except 문이다. 기본적인 형태는 아래와 같다.

```python
try:
    예외 발생 가능 코드
except <Exception Type>:
    예외 발생 시 대응하는 코드
else:
    예외가 발생하지 않을 때 동작하는 코드 - 별로 좋은 코드는 아님.
finally:
    예외 발생여부와 상관없이 수행되는 코드
```

<br/>

- Built-in Exception의 종류

|  Exception 이름   |                    설명                     |
| :---------------: | :-----------------------------------------: |
|     Exception     |             전체 Exception 포함             |
|    IndexError     |        List의 Index 범위를 넘어갈 때        |
|     NameError     |       존재하지 않는 변수를 호출할 대        |
| ZeroDivisionError |            0으로 숫자를 나눌 때             |
|    ValueError     | 변환할 수 없는 문자/숫자를 변환하려고 할 때 |
| FileNotFoundError |       존재하지 않는 파일을 호출할 때        |

아래는 0으로 나눴을 때의 예외처리를 보여주는 예시이다.

```python
for i in range(4):
    try:
        print(10 // i)
    except ZeroDivisionError:
        print("cannot divide by zero")
```

결과

```
>> cannot divide by zero
>> 10
>> 5
>> 3
```

<br/>

에러를 강제로 발생시키고 싶다면 아래와 같이 적어주면 된다.

```python
raise <ExceptionType> (예외정보)
```

```python
value = "1234English"
for digit in value:
    if digit not in "0123456789":
        raise ValueError("숫자값을 입력하지 않았습니다.")

print("정수로 변환된 숫자:", int(value))
```

<br/>

### 9. 리스트

<hr/>

파이썬의 가장 기본적인 데이터 구조이다. 리스트는 순서가 있는 자료의 집합이다.
슬라이싱을 통해 원하는 자료를 분리하여 가져오는 것도 가능하다. (문자열도 슬라이싱이 가능하다!!)

#### 리스트 안에서 항목의 존재 여부 확인

```python
1 in [1, 2, 3]   #True
0 in [1, 2, 3]   #False
```

#### 리스트에 다른 리스트의 내용 추가하기

```python
x = [1, 2, 3]
x.extend([4, 5, 6]) # x는 [1, 2, 3, 4, 5, 6]이 된다.

# x의 내용을 바꾸고 싶지 않아 다른 변수를 선언할 때.
x = [1, 2, 3]
y = x + [4, 5, 6]
```

<br/>

### 10. 튜플

<hr/>

변경할 수 없는 리스트이다. 대괄호 대신 소괄호를 사용한다. 함수에서 여러 개의 값을 반환할 때 튜플을 사용하면 편리하다.

#### 리스트와 튜플은 다중할당 지원

```python
x, y = 1, 2
x, y = y, x
```

<br/>

### 11. 딕셔너리

<hr/>

기본적인 데이터 구조로 특정 값(value)과 연관된 키(key)를 연결해 값을 빠르게 검색할 수 있다.

#### 대괄호를 통해 키의 값 불러오기

```python
grades = {"Joel": 80, "Tim": 95}
joels_grade = grades["Joel"]  # 80이 저장됨.
```

딕셔너리에 존재하지 않는 키를 입력하면 KeyError가 발생하니 주의하자.
<br/>

#### in을 이용해 키의 존재 여부를 확인

```python
exist1 = "Joel" in grades   #True
exist2 = "Kate" in grades   #False
```

#### 대괄호를 사용해 키와 값을 수정하거나 넣어주기

```python
grades["Tim"] = 99   # 기존의 값을 수정
grades["Kate"] = 100   # Kate키를 가진 새로운 값 추가
```

#### 딕셔너리의 모든 값을 살펴보기

```python
grades_keys = grades.keys()  # 키에 대한 리스트
grades_values = grades.values()  # 값에 대한 리스트
grades_items = grades.items()  # (key, value) 튜플에 대한 리스트

"Kate" in grades_keys  # True. 그러나 리스트에서 사용하는 in은 속도가 느리다. O(n)
"Kate" in grades       # True. O(1) 훨씬 빠르다.
```

리스트보다 딕셔너리에서 in을 사용하는 것이 훨씬 빠르니 알아두자.

<br/>

### 11-1. defalutdict

<hr/>
defaultdict와 평범한 딕셔너리의 차이점은 만약 존재하지 않는 키가 주어진다면 defaultdict는 이 키와 인자에서 주어진 값으로 dict에 새로운 항목을 추가해 준다는 것이다.
defaultdict를 사용하기 위해서는 collections 모듈에서 defaultdict를 가져와야한다.

```python
from collections import defaultdict

word_counts = defaultdict(int)  #int()는  0을 생성
for word in document:
    word_counts[word] += 1
```

리스트, 딕션너리 혹은 직접 만든 함수를 인자에 넣어줄 수도 있다.

```python
dd_list = defaultdict(list)         # list()는 빈 리스트를 생성
dd_list[2].append(1)                # 이제 dd_list는 {2: [1]}을 포함

dd_dict = defaultdict(dict)         # dict()는 빈 딕셔너리를 생성
dd_dict["Joel"]["City"] = "Seattle" # {"Joel": {"City": "Seattle"}}

dd_pair = defaultdict(lambda: [0, 0])
dd_pair[2][1] = 1                   # dd_pair는 {2: [0,1]}을 포함
```

<br/>

### 12. Counter

Counter는 연속된 값을 defalutdict(int)와 유사한 객체로 변환해주면, 키와 값의 빈도를 연결시켜준다.

```python
from collections import Counter
c = Counter([0, 1, 2, 0])  # c는 {0: 2, 1: 1, 2: 1} 이 된다.
```

특정 문서에서 단어의 개수를 셀 때도 유용하다.

```python
# document는 단어의 리스트
word_counts = Counter(document)
```

Counter 객체에는 most_common 메소드가 있는데, 빈도에 관련된 함수이다.

```python
# 가장 자주 나오는 단어 10개와 이 단어들의 빈도수를 출력
for word, count in word_counts.most_common(10):
    print(word, count)
```

### 13. Set

<hr/>
집합(Set)은 파이썬의 데이터 구조 중 유일한 항목의 집합을 나타내는 구조다. 집합은 중괄호를 사용해서 정의한다.

```python
mySet = {2, 3, 5, 7}
```

{}는 비어있는 딕셔너리를 의미하기 때문에 set()을 사용해서 비어있는 set을 생성할 수도 있다.

```python
s = set()
s.add(1)
s.add(2)
s.add(2)    # 중복을 허용하지 않기 때문에 {1, 2} 그대로이다.
x = len(s)  # 출력값: 2
y = 2 in s  # True
z = 3 in s  # False
```

Set에는 장점이 있다. 첫째로 in이 굉장히 빨리 작동한다는 점이다.
두 번째로 중복된 원소를 제거해준다는 점이 있다. 그러나 Set보다 리스트이 더 많이 사용된다.

<br/>

### 14. 흐름 제어

<hr/>
대부분의 프로그래밍 언어처럼 if를 통해 코드의 분기를 만들어 제어할 수 있다.
또한 삼항연산자를 통해 if-then-else문을 한 줄로 표현하기도 한다.

```python
parity = "even" if x % 2 == 0 else "odd"
```

파이썬에서는 while보다는 for in문이 더 많이 사용되는 편이다. 복잡한 논리체계가 필요하다면 continue와 break를 사용할 수도 있다.

<br/>

### 15. True와 False

<hr/>
파이썬의 boolean 타입은 대부분의 다른 언어와는 다르게 대문자로 시작한다. 
또한 존재하지 않는 값을 null이 아닌 None으로 표기한다.

```python
x = None
assert x == None, "this is the not the Pythonic way to check for None"
assert x is None, "this is the the Pythonic way to check for None"
```

파이썬에서는 다른 값으로도 boolean 타입을 표현할 수 있는데 다음은 모두 거짓을 의미한다.

- False
- None
- [] (빈 리스트)
- {} (빈 딕셔너리)
- ""
- set()
- 0
- 0.0

나머지 거의 모든 것이 참(True)을 의미한다. 이를 통해 리스트, 문자열, 딕셔너리 등이 비어 있는지 쉽게 확인할 수 있다.
하지만 이로 인해 예상치 못한 오류가 발생하기도 한다.

```python
s = some_function_that_returns_a_string()
if s:
    first_char = s[0]
else:
    first_char = ""
```

위 코드는 아래와 같이 간략하게 표현할 수 있다.

```python
first_char = s and s[0]
```

파이썬에는 리스트의 모든 항목이 참이라면 True를 반환해주는 all 함수와 적어도 하나의 항목이 참이라면 True를 반환해주는 함수 any가 있다.

```python
all([True, 1, {3}])     # True
all([True, 1, {}])      # False
any([True, 1, {}])      # True
any([])                 # False
```

<br/>

### 16. 정렬

<hr/>

파이썬 리스트를 정렬하고 싶다면 sort()메소드를 사용하면 된다.
만약 이미 만든 리스트를 망치고 싶지 않다면 sorted 함수를 이용해 새로운 리스트를 반환할 수도 있다.

```python
x = [4, 1, 3, 2]
y = sorted(x)         # y = [1, 2, 3, 4], x는 그대로!
x.sort()              # x = [1, 2, 3, 4]
```

기본적으로 위 메소드들은 리스트의 각 항목을 일일이 비교해서 오름차순으로 정렬해준다.
만약 내림차순으로 정렬하고 싶다면 인자에 reverse=True를 추가해주면 된다.
그리고 key를 이용해서 비교하는 기준을 바꿀 수 있다.

```python
# 절대값의 내림차순으로 리스트 정렬
x = sorted([-4, 1, -2, 3], key=abs, reverse=True)  # [-4, 3, -2, 1]

#빈도의 내림차순으로 단어와 빈도를 정렬
wc = sorted(word_counts.items(),
            key=lambda word_and_count: word_and_count[1],
            reverse=True)
```

<br/>

### 17. 리스트 컴프리헨션

<hr/>

기존의 리스트에서 특정 항목을 선택하거나 변환시킨 결과를 새로운 리스트에 저장해야 하는 경우가 자주 발생한다.
가장 파이썬스럽게 처리하는 방법은 리스트 컴프리헨션(list comprehension)이다.

```python
even_numbers = [x for x in range(5) if x % 2 == 0]  # [0, 2, 4]
squares = [x * x for x in range(5)]                 # [0, 1, 4, 9, 16]
even_squares = [x for x in even_numbers]            # [0, 4, 16]
```

딕셔너리나 집합으로 변환시킬 수도 있다.

```python
square_dict = {x: x*x for x in range(5)}  # {0: 0, 1:1, 2:4, 3:9, 4:16}
square_set = {x*x for x in [1, -1]}       # {1}
```

리스트에서 불필요한 값은 \_(밑줄)로 표시한다.

```python
zeros = [0 for _ in range(5)]   # [0, 0, 0, 0, 0]
```

리스트 컴프리헨션은 여러 for을 포함할 수 있다.

```python
pairs = [(x, y)
         for x in range(10)
         for y in range(10)] # [(0,0), (0,1), ..., (9,9)] 총 100개
```

뒤에 나오는 for문은 앞에 나온 결과를 반복한다.

```python
# x < y인 경우만 넣기
increasing_pairs = [(x, y)
                     for x in range(10)
                     for y in range(x+1, 10)]
```

<br/>

### 18. 자동 테스트와 assert

<hr/>

코드가 제대로 작성되었는지 확인하려면 테스트를 진행해야한다.
다양한 테스팅 프레임워크가 존재하지만 그 중 assert에 대해 알아보자.
assert는 조건이 충족되지 않는다면 AssertinoError를 반환한다.

```python
assert 1 + 1 == 2
assert 1 + 1 == 2, "1 + 1 should equal 2 but didn't"
```

위의 두 번째 예시는 조건이 충족되지 않았을 때, 출력하고 싶은 문구를 추가하는 방법이다.
<br/><br/>
이번엔 직접 작성한 함수를 테스팅해보자.

```python
def smallest_item(xs):
    return min(xs)

assert smallest_item([10, 20, 5, 40]) == 5
assert smallest_item([10, 20, 5, 40]) == -1
```

아래와 같이 함수의 인자를 검증할 수도 있지만 많이 사용하지는 않는다.

```python
def smallest_item(xs):
    assert xs, "empty list has no smallest item"
    return min(xs)
```

<br/>

### 19. 객체지향 프로그래밍

<hr/>

파이썬도 다른 언어처럼 클래스를 이용한 객체 지향 프로그래밍이 가능하다. 코드를 깔끔하고 간단하게 작성할 수 있도록 도와줄 것이다. 객체 지향 언어란 실제 세상을 객체로 모델링하는 것을 말하는데 이것을 가능하게 만드는 3대 요소가 있다. Inheritance(상속), Polymorphism(다형성), Visibility(가시성)이 그것이다. 아래의 예를 통해 차례로 살펴보자.

<br/>

모임에 몇 명이 참여했는지 확인하는 CountingClicker 클래스를 만든다고 해보자.
이 클래스에는 참석자 수를 의미하는 count, count를 증가시키는 click 메소드, count를 반환해주는 read 메소드,
count를 0으로 재설정해주는 reset 메소드 등이 있다.

```python
class CountingClicker:
    def __init__(self, count = 0):
        self.count = count

    # __repr__는 클래스 인스턴스를 문자열 형태로 반환해주는 dunder 메소드
    def __repr__(self):
        return f"CountingClicker(count={self.count})"

    def click(self, num_times=1):
        self.count += num_times

    def read(self):
        return self.count

    def reset(self):
        self.count = 0

clicker = CountingClicker()
# 클리커가 0으로 시작하는 지 검사
assert clicker.read() == 0, "clicker should start with count 0"

# 클리커의 click() 메소드가 제대로 실행됐는지 검증
clicker.click()
clicker.click()
assert clicker.read() == 2, "after two clicks, clicker should have count 2"

# 클리커의 reset() 메소드가 실행됐는지 검증
clicker.reset()
assert clicker.read() == 0, "after reset, clicker should be back to 0"
```

다른 언어와 마찬가지로 부모 클래스의 기능을 상속받는 서브클래스(subclass)를 사용할 수 있다.
메소드를 오버라이딩하는 것도 가능하다.

```python
# CountingClicker 상속받기
class NoResetClicker(CountingClicker):
    # reset 오버라이딩해서 기능 없애기.
    def reset(self):
        pass
```

파이썬에는 '\_\_', Name Mangling(일명, 맹글링)이 있는데, 정보은닉의 효과를 만들 수 있다.

```python
class MyClass:
    def __init__(self, name):
        self.__name = name
```

위와 같이 변수 앞에 맹글링을 넣어 유사한 효과를 볼 수 있다. 아래와 같이 맹글링이 들어간 변수에 직접 접근하게 되면 AttributeError가 발생하게 된다.

```python
my = MyClass("JH")
my.__name

>> AttributeError
```

대신 아래와 같이 메소드를 통해 반환하면 에러 없이 값을 가져올 수 있다.

```python
class MyClass:
    def __init__(self, name):
        self.__name = name

    def getName():
        return self.__name

my = MyClass("JH")
print(my.getName())
```

그 외에도 특수한 예약 함수나 변수, 함수명에도 사용하는데 그 예로는 `__main__` , `__add__` , `__str__` , `__eq__` 등이 있다. 이 중 `__str__` 을 사용하면 자바의 toString()과 같은 효과를 볼 수 있다.

```python
class MyClass:
    def __init__(self, name):
        self.__name = name # 정보은닉

    def __str__(self):
        return "My name is %s" %(self.__name)

my = MyClass("JH")
print(my)

>> "My name is JH"
```

<br/>

### 20. 이터레이터와 제너레이터

<hr/>

리스트는 순서나 인덱스(index)만 알고 있으면 쉽게 특정 항목을 가져올 수 있다는 장점이 있다.
그런데 만약 10억 개의 항목을 가지는 리스트를 생성해본다고 해보자.
장점이 약점으로 바뀌는 순간이다. 컴퓨터의 메모리가 부족해질 수 있다.
앞부분의 몇몇 값만 필요한데도 10억 개의 항목을 갖는 리스트 전체를 생성하는 것은 매우 비효율적이다.<br/>
제너레이터(generator)는 주로 for문을 이용해 반복할 수 있으며, 제너레이터의 각 항목은 필요한 순간에 그때그때 생성되기 때문에 메모리 효율성을 높일 수 있다.
제너레이터를 만드는 한 가지 방법은 yield를 사용하는 것이다.

```python
def generate_range(n):
    i = 0
    while i < n:
        yield i   # yield가 호출될 때마다 제너레이터에 해당 값을 생성
        i += 1

for i in generate_range(10):
    print(f"i: {i}")
```

반복문은 yield로 반환되는 값이 없을 때까지 반환된 값을 차례로 하나씩 사용한다.
이는 무한한 수열도 메모리의 제약을 받지 않고 구현할 수 있다는 것을 의미한다.<br/><br/>

또 다른 방법은 괄호 안에 for문을 추가하는 방법으로 제너레이터를 만드는 것이다.
물론 for문이나 next를 통해서 반복문이 시작되기 전까지는 제너레이터가 생성되지 않는다.
이를 사용해 정교한 데이터 처리 파이프라인을 만들 수 있다.

```python
# 제너레이터 선언하기
even_below_20 = (i for i in generate_range(20) if i % 2 == 0)

# 실제로 반복문이 실행되기 전까지는 제너레이터가 생성되지 않는다.
data = natural_numbers()
evens = (x for x in data)
even_squares = (x**2 for x in evens)
even_squares_ending_in_six = (x for x in even_squares if x % 10 == 6)
```

<br/>
종종 리스트나 제너레이터에서 항목을 하나씩 확인할 때, 항목의 순서를 반환할 수 있는데 
enumerate 함수를 사용하면 (인덱스, 항목) 형태로 값을 반환할 수 있다.

```python
names = ["Alice", "Bob", "Charlie", "Debbie"]

for i, name in enumerate(names):
    print(f"name {i} is {name}")
```

<br/>

### 21. 난수 생성

<hr/>

난수(random number)는 파이썬의 random 모듈을 이용해 생성할 수 있다.

```python
import random
random.seed(10)  # 매번 동일한 결과를 반환해주는 설정

four_uniform_randoms = [random.random() for _ in range(4)]
# [0.5714025946899135, 0.4288890546751146, 0.5780913011344704, 0.20609823213950174]
# 항상 똑같은 난수 배열이 반환된다.
```

random.seed()를 통해 매번 고정된 난수를 생성할 수 있다.

```python
random.seed(10)
print(random.random())
random.seed(10)
print(random.random())
```

random.randrange() 메소드를 사용하면 범위에 해당하는 구간 안의 난수를 생성할 수 있다.

```python
random.randrange(10)    # 0~9 난수 생성
random.randrange(3, 6)  # 3~5 난수 생성
```

random.shuffle() 메소드는 리스트의 항목을 임의의 순서로 섞어준다.

```python
myarr = [i for i in range(10)]
random.shuffle(myarr)
print(myarr)  # [9, 8, 4, 2, 5, 3, 1, 0, 7, 6]
# 사람마다 순서는 다 다를 것이다.
```

random.choice() 메소드는 리스트에서 랜덤으로 하나의 항목을 선택해준다.

```python
# 랜덤으로 myarr 리스트에 있는 숫자를 선택
nubmer = random.choice(myarr)
```

random.sample() 메소드를 사용하면 리스트에서 중복이 허용되지 않는 임의의 표본 리스트를 만들 수 있다.
만약 중복이 허용되는 임의의 표본리스트를 만들고 싶다면 random.choice를 여러 번 호출해 사용한다.

```python
# 로또 번호 뽑기 예시
lottery_numbers = range(60)
winning_numbers = random.sample(lottery_numbers, 6)
```

<br/>

### 22. 정규표현식

정규표현식(regular expressions, regex)를 사용하면 문자열을 찾을 수 있다.

```python
import re

re_examples = [
    not re.match("a", "cat"),
    re.search("a", "cat"),
    not re.search("c", "dog"),
    3 == len(re.split("[ab]", "carbs")),

    "R-D-" == re.sub("[0-9]", "-", "R2D2")
]

assert all(re_examples), "all the regex examples should be True"
```

re.match 메소드는 문자열의 시작이 정규표현식과 같은지 비교하고,
re.search메소드는 문자열 전체에서 정규표현식과 같은 부분이 있는지 찾는다.<br/>
정규문서: [파이썬 정규표현식](https://docs.python.org/3/library/re.html)

<hr/>

<br/>

### 23. 함수형 도구

<hr/>

partial, map, reduce, filter등을 메소드를 데이터 사이언스에 활용할 수 있지만
for문과 리스트 컴프리헨션으로 충분히 대체할 수 있다.

<br/>

### 24. zip과 인자 언패킹

<hr/>

두 개 이상의 리스트를 서로 묶어주고 싶다면 zip을 사용하면 된다. zip은 여러 개의
리스트를 서로 상응하는 항목의 튜플로 구성된 리스트로 변환해준다.

```python
list1 = ['a', 'b', 'c']
list2 = [1, 2, 3]

mylist = [pair for pair in zip(list1, list2)]

letters, numbers = zip(*mylist)
```

zip 메소드에서 "\*"을 사용하면 인자 언패킹을 할 수 있다. 자주 사용하지는 않는다.

<br/>

### 25. args와 kwargs

<hr/>

특정 함수 f를 입력하면 f의 결과를 두 배로 만드는 함수를 반환해주는 고차 함수를 만든다고 해보자.

```python
def double(f):
    def g(x):
        return 2 * f(x)

    # 새로운 함수를 반환
    return g

# 인자가 1개라면 정상 실행된다.
def f1(x):
    return x + 1

g = double(f1)
assert g(3) == 8, "(3+1) * 2 should equal 8"
assert g(-1) == 0, "(-1+1) * 2 should equal 0"

# 인자가 2개라면 문제가 발생한다.
def f2(x, y):
    return x + y

g2 = double(f2)
try:
    g(1, 2)
except TypeError:
    print("as defined, g only takes one argument")
```

인자를 2개 받을 때의 문제를 해결하기 위해서는 임의의 수의 인자를 받는 함수를 만들어줘야 한다.
인자 언패킹을 사용하면 임의의 수의 인자를 받는 함수를 만들 수 있다.

```python
def myfunc(*args, **kwargs):
    print("unnamed:", args)
    print("keyword args:", kwargs)

myfunc(1, 2, key="word", key2="word2")
```

args는 이름이 없는 인자로 구성된 튜플이고, kwargs는 이름이 주어진 인자로 구성된 딕셔너리다.
반대로, 정해진 인자가 있는 함수를 호출할 때도 리스트나 딕셔너리로 인자를 전달할 수 있다.

```python
def other_way_func(x, y, z):
    return x + y + z

x_y_list = [1, 2]
z_dict = {"z": 3}
assert other_way_func(*x_y_list, **z_dict) == 6, "1 + 2 + 3 should be 6"
```

이제 마지막으로, 처음 예시로 보여줬던 잘못된 함수를 맞게 고쳐보겠다.

```python
def f2(x, y):
    return x + y

def double_correct(f):
    def g(*args, **kwargs):
        return 2 * f(*args, **kwargs)
    return g

g = double_correct(f2)
assert g(1, 2) == 6, "double should work now"
```

<br/>

### 26. 타입 어노테이션

<hr/>

파이썬은 동적 타입 언어다. 이는 변수를 올바르게만 사용한다면 변수의 타입은 신경쓰지 않아도 된다는 말이다.

```python
def add(a, b):
    return a + b

assert add(10, 5) == 15, "+ is valid for numbers"
assert add([1,2], [3]) == [1,2,3], "+ is valid for lists"
assert add("hi ", "there") == "hi there", "+ is valid for strings"
```

반면 정적 타입의 언어(C, C++, 자바 등등)은 모든 함수나 객체의 타입을 명시해야하는데,
파이썬에서도 타입을 명시할 수 있는 기능이 있다. 하지만 이렇게 명시된 타입은 실제로 아무런 기능도 하지 않는다.
타입이 int로 명시된 함수여도 문자열을 더할 수도 있고 int형과 문자열을 더했다가 TypeError가 발생할 수도 있다.
하지만 타입을 명시하면 좋은 이유는 4개나 있다.
<br/>

- 문서를 작성할 때 중요하다

이론적이거나 수학적인 개념을 설명할 때 큰 도움이 된다. 아래의 코드를 살펴보자.
두 번째의 함수가 활용할 때 도움이 될만한 정보를 제공한다.

```python
def dot_product(x, y): ...

# Vector라는 것을 사전에 정의했다고 가정.
def dot_product(x: Vector, y: Vector): ...
```

<br/>

- 에러검사에 도움이 된다

mypy 등의 도구를 통해 타입 체크를 할 수 있다. 예를 들어 mypy로 add("hi ", "there")가 포함되어 있는 파일을 검사한다고 해보자.
그러면 아래와 같은 에러를 출력할 것이다.

```python
error: Argument 1 to "add" has incompatible type "str"; expected "int"
```

마치 assert로 테스트하는 것처럼 코드 안의 오류를 사전에 잡아낼 수 있다.
<br/>

- 깔끔하고 이해하기 쉬운 함수를 만들 수 있다.

```python
from typing import Union

def secretly_ugly_function(value, operation): ...

def ugly_function(value: int,
                  operation: Union[str, int, float, bool]) -> int: ...
```

위 함수는 operation 인자는 str, int, float, bool 객체를 받을 수 있다.
함수가 굉장히 복잡할 가능성이 크지만 타입이 명시되어 있으면, 훨씬 명확하게 코드를 파악할 수 있다.
<br/>

- 에디터의 자동 완성 기능을 사용할 수 있다.

자동 완성 기능을 사용할 수 있고, 타입 에러 또한 사전에 잡아낼 수 있다.
또한, 코드를 더 빠르게 작성할 수 있게 도와준다.

<br/>

#### 26-1. 타입 어노테이션 작성법

<hr/>
int, bool, float 같은 기본적인 객체는 타입을 바로 명시해주면 된다. 리스트의 경우에는 어떻게 타입을 명시하는게 좋을까?

```python
def total(xs: list) -> float:
    return sum(total)

from typing import List
def total(xs: List[float]) -> float:
    return sum(total)
```

첫 번째 방법도 틀린 것은 아니지만, 두 번째 방식이 구체적으로 리스트의 타입을 명시할 수 있기 때문에 좋은 방법이다.
<br/><br/>
int형이나 str형 변수는 선언할 때 타입이 명확해지기 때문에 따로 어노테이션을 붙일 필요가 없지만,
list를 사용하거나 None을 사용하면 타입형이 명확하지 않기 때문에 이럴 때 힌트를 줄 수 있다.

```python
from typing import Optional
from typing import List

values: List[int] = []
best_so_far: Optional[float] = None  # float이나 None으로 타입 명시
```

```python
from typing import Dict, Iterable, Tuple

counts: Dict[str, int] = {'data': 1, 'science': 2}

if lazy:
    evens: Iterable[int] = (x for x in range(10) if x % 2 ==0)
else:
    evens = [0, 2, 4, 6, 8]

triple: Tuple[int, float, int] = (10, 2.3, 5)
```

<br/>

파이썬의 일급 함수(first-class function)에 대해서도 타입을 명시할 수 있다.

```python
from typing import Callable

# repeater 함수가 문자열과 int를 인자로 받고
# 문자열을 반환해준다는 것을 명시
def twice(repeater: Callable[[str, int], str], s: str) -> str:
    return repeater(s, 2)

def comma_repeater(s: str, n: int) -> str:
    n_copies = [s for _ in range(n)]
    return ', '.join(n_copies)

assert twice(comma_repeater, "type hints") == "type hints, type hints"
```

명시된 타입 자체도 파이썬 객체이기 때문에 변수로 선언할 수 있다.

```python
Number = int
Numbers = List[Number]

def total(xs: Numbers) -> Number:
    return sum(xs)
```

<br/>

### 27. 파일 다루기

<hr/>

기본적인 파일 종류로는 text파일과 binary 파일이 있다. 컴퓨터는 text파일을 처리하기 위해 binary 파일로 변환시킨다.

|                                                                          Binary 파일                                                                          |                                                                   Text 파일                                                                   |
| :-----------------------------------------------------------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------------------------------------------------------: |
| 컴퓨터만 이해할 수 있는 형태인<br/>이진형식으로 된 파일이다. <br/><br/>일반적으로 메모장을 열면<br/>내용이 깨져 보인다.<br/><br/> ex) 엑셀파일, 워드파일 등등 | 인간도 이해할 수 있는 형태인<br/>문자열 형식으로 저장된 파일<br/><br/>메모장으로 열면 내용확인 가능<br/><br/>ex) 메모장, HTML파일, 파이썬코드 |

<br/><br/>

#### Python File I/O

파이선은 파일을 처리하기 위해 `open` 키워드와 `close` 키워드를 사용한다.

```python
f = open("<파일이름>", "접근 모드")
f.close()
```

<br/>

| 접근모드 |                     설명                      |
| :------: | :-------------------------------------------: |
|   `r`    |       읽기모드, 이미 있는 파일을 읽음.        |
|   `w`    |        쓰기모드, 새로운 파일을 생성함.        |
|   `a`    | 추가모드, 파일의 마지막에 새로운 내용을 추가. |

<br/>

- 파이썬 File Read

```python
f = open("myfile.txt", "r" )
contents = f.read()
print(contents)
f.close()
```

<br/>

with를 이용해 파일을 사용할 수 있다.

```python
with open("myfile.txt", "r") as myfile:
contents = myfile.read()
print(type(contents), contents)
```

<br/>

한 줄씩 읽어 List로 변환하는 것도 가능하다.

```python
with open("myfile.txt", "r") as myfile:
content_list = myfile.readlines() #파일 전체를 list로 반환
print(type(content_list)) #Type 확인
print(content_list) #리스트 값 출력
```

<br/>

아래는 파일의 통계 정보를 산출하는 코드이다. 글자의 개수, 단어의 개수, 줄의 개수를 출력해준다.

```python
with open("myfile.txt", "r") as myfile:
    contents = my_file.read()
    word_list = contents.split(" ")
    #빈칸 기준으로 단어를 분리 리스트
    line_list = contents.split("\n")
    #한줄 씩 분리하여 리스트
print("Total Number of Characters :", len(contents))
print("Total Number of Words:", len(word_list))
print("Total Number of Lines :", len(line_list))
```

<br/>

- 파이썬 File Write

`w` 모드는 파일을 새로 작성하는 모드이다. encoding 옵션을 통해 "uft-8" 등의 인코딩 방식을 정할 수 있다. `file.write()` 메소드를 통해 파일에 기록할 수 있다.

```python
f = open("myfile.txt", 'w', encoding="utf8")
for i in range(1, 11):
    data = "%d번째 줄입니다.\n" % i
    f.write(data)
f.close()
```

`a` 는 파일을 새로 생성하는 것이 아니라, 추가하는 모드이다.

```python
with open("myfile.txt", 'a', encoding="utf8") as f:
for i in range(11, 21):
    data = "%d번째 줄입니다.\n" % i
f.write(data)
```

<br/>

#### 파이썬 directory 다루기

os 모듈을 이용해 디렉토리를 생성할 수 있다.

```python
import os
# log라는 폴더를 생성함
try:
    os.mkdir("log")
except FileExistsError as e:
    print("Already created")

# log라는 폴더가 있는지 확인
print(os.path.exists("log"))
```

<br/>

shutil 모듈을 이용하면 소스파일을 복사할 수 있음.

```python
import os
import shutil

source = "myfile.txt"
# os.path.join을 통해 경로를 만듬.
dest = os.path.join("log", "text.txt")

# log 디렉토리에 있는 source파일을 dest경로로 복사붙여넣기
shutil.copy(source, dest)
```

<br/>

최근에는 **pathlib 모듈**을 사용해 path를 객체로 다루는 편이다.

```python
import pathlib

# Current Working Directory - 현재 작업 경로
cwd = pathlib.Path.cwd()
# 부모폴더까지만 반환
cwd.parent
# 조부모폴더까지만 반환
cwd.parent.parent

# 경로를 리스트로 반환
list(cwd.parent.parents)

# 일치하는 모든 파일을 산출
list(cwd.glob("*"))
```

<br/>

> 로그파일 만들기 예제

```python
import os

if not os.path.isdir("log"):
    os.mkdir("log")
if not os.path.exists("log/count_log.txt"):
    f = open("log/count_log.txt", "w", encoding="utf-8")
    f.write("기록이 시작됩니다.\n")
    f.close()

with open("log/count_log.txt", "a", encoding="utf-8") as f:
    import random, datetime
    for i in range(1, 11):
        stamp = str(datetime.datetime.now())
        value = random.random() * 1_000_000
        log_line = stamp + "\t" + str(value) + "값이 생성되었습니다." + "\n"
        f.write(log_line)
```

<br/>

#### 파이썬 객체를 영속화

Pickle은 텍스트 데이터가 아닌 파이썬 객체 자체를 파일로 저장할 때 사용하는 모듈이다. 리스트, 클래스 등의 파이썬 객체를 pickle 확장자 파일에 바이너리 형태로 저장한다. 이 과정을 영속화라고 한다.

```python
import pickle

f = open("list.pickle", "wb")
test = [1, 2, 3, 4, 5]
# test객체를 바이너리 파일로 저장
pickle.dump(test, f)
f.close()
```

```python
f = open("list.pickle", "rb") # read binary
test_pickle = pickle.load(f)
print(test_pickle)
f.close()
```

<br/><br/>

### 28. Logging

<hr/>

**로그**란 프로그램이 실행되는 동안 일어나는 일들(유저의 접근, 프로그램의 예외처리, 특정 함수의 사용 등)을 기록으로 남긴 결과물을 말한다. Console 화면에 출력하거나 파일에 기록해놓는 방식, DB에 기록하는 방식으로 로그를 남길 수 있다.<br/>
이렇게 기록된 로그를 분석해 의미있는 결과를 만들 수도 있고, 버그를 처리하는데 큰 도움이 될 수 있다. 로그를 남기는 시점이 중요한데, 유저를 분석하고 싶다면 실행시점에 로그를 남겨야한다. 반면 에러를 사전에 잡아내는 것이 목적이라면 개발시점에 로그를 남기는 것이 좋다.

<br/><br/>

- 파이썬의 기본 Log 관리 모듈

로그를 남기고 싶다면 `logging` 모듈을 사용하면 된다. 또한, 로그에는 레벨이 있는데 그 순서는 다음과 같다. `Debug > Info > Warning > Error > Critical`

```python
import logging

# Log Level
# 아래로 갈수록 심각한 상황
logging.debug("디버그")
logging.info("정보를 알려줌")
logging.warning("주의시킴")
logging.error("에러 발생을 알려줌")
logging.critical("프로그램이 완전히 종료되었을 때")
```

<br/>

|  Level   |                                               설명                                               |                          예시                          |
| :------: | :----------------------------------------------------------------------------------------------: | :----------------------------------------------------: |
|  debug   |                       개발 시 처리 기록을 <br/>남겨야하는 로그 정보를 남김                       |      함수로 어떤 것을 호출하는지<br/>변수의 변경       |
|   info   |                                처리가 진행되는 동안의 정보를 알림                                | 서버시작됨<br/>서버종료됨<br/>사용자가 프로그램에 접속 |
| warning  | 사용자가 잘못 입력한 정보나 처리는 가능하나<br/> 원래 개발시 의도치 않는 정보가 들어왔을 때 알림 |       int형 입력이 필요한데<br/>str 입력을 할 때       |
|  error   |              잘못된 처리로 인해 에러가 났으나<br/> 프로그램은 동작할 수 있음을 알림              |                  외부서비스 연결 불가                  |
| critical |           잘못된 처리로 데이터 손실이나 <br/>더 이상 프로그램이 동작할 수 없음을 알림            |  잘못된 접근으로 파일삭제<br/>사용자에 의한 강제 종료  |

<br/>

```python
import logging

# Logger 선언
logger = logging.getLogger("main")

# Logger의 output 방법 선언
stream_hander = logging.StreamHandler()

# Logger의 output 등록
logger.addHandler(stream_hander)

# Logger에 Level 부여 - 디폴트는 warning 레벨이다.
# DEBUG 레벌부터 출력시키기(모든 레벨 출력됨)
# logger.setLevel(logging.DEBUG)  # 예전 방식
logger.basicConfig(level=logging.DEBUG)

# INFO 레벌부터 출력시키기(DEBUG를 제외한 모든 레벨 출력됨)
logger.basicConfig(level=logging.INFO)
```

<br/>

- configparser

프로그램 실행 설정을 file에 저장한다. Section, Key, Value 값의 형태로 설정된 설정 파일을 사용하는데 설정파일을 Dict Type으로 호출 후 사용한다.

> ex
>
> > example.cfg

```
[SectionOne]             => Section은 대괄호로 구분
Status: Single
Name: Derek
Value: Yes
Age: 30
Single: True

[SectionTwo]
FavoriteColor = Green

[SectionThree]
FamilyName: Johnson
```

> > 파이썬 코드

```python
import configparser

config = configparser.ConfigParser()
# 파일을 읽어옴
config.read('example.cfg')
print(config.sections())

print(config['SectionTwo'])
for key in config['SectionTwo']:
    value = config['SectionTwo'][key]
    print(key, ":", value)
```

> > 실행결과

```
>> ['SectionOne', 'SectionTwo', 'SectionThree']
>> <Section: SectionTwo>
>> favoritecolor : Green
```

<br/>

- argparse

Console 창에서 프로그램을 실행할 때 Setting 정보를 저장한다. 거의 모든 Console 기반 Python 프로그램을 기본으로 제공한다. 다른 특수 모듈도 많지만 일반적으로 argparse를 사용한다. Command-Line Option이라고도 불린다.

> arg_sum.py

```python
import argparse

parser = argparse.ArgumentParser(description="Sum two integers")

#                 #짧은이름  #긴 이름   #표시명   #Help설명    # Argument Type
parser.add_argument("-a", "--a_value", dest="A_value", help="A integers", type=int, required=True)
parser.add_argument("-b", "--b_value", dest="B_value", help="A integers", type=int, required=True)

args = parser.parse_args()
print(args)
print(args.a)
print(args.b)
print(args.a + args.b)
```

`python arg_sum.py -a 10 -b 10` 라는 명령어로 실행해보자.

> 결과

```
>> Namespace(a=10, b=10)
>> 10
>> 10
>> 20
```

참고자료: [argparse 구문 jupyter에서 사용하기](https://ddiri01.tistory.com/302)

<br/>

- Logging formmater

Log의 결과값의 format을 미리 지정하는 방법이 있는데, Formatter() 메소드를 사용하면 된다.

```python
formatter = logging.Formatter("%(asctime)s %(levelname)s %(process)d %(message)s")
```

위와 같이 설정해주면
`2022-03-27 13:53:04, 385 ERROR 4410 ERROR occurred` 와 같은 형식으로 로그가 기록된다.

<br/>

- Log config file

config 파일을 통해 로그의 포맷을 설정할 수 있다.

> logging.conf

```
[loggers]
keys=root

[handlers]
keys=consoleHandler

[formatters]
keys=simpleFormatter

[logger_root]
level=DEBUG
handlers=consoleHandler

[handler_consoleHandler]
class=StreamHandler
level=DEBUG
formatter=simpleFormatter
args=(sys.stdout,)

[formatter_simpleFormatter]
format=%(asctime)s - %(name)s - %(levelname)s - %(message)s
datefmt=%m/%d/%Y %I:%M:%S %p
```

> 파이썬으로 가져오기

```python
logging.config.fileConfig('logging.conf')
logger = logging.getLogger()
```

<br/><br/>

### 29. Python Data Handling

<hr/>

1. Comma Separate Value

CSV, 필드를 쉼표(,)로 구분한 텍스트 파일이다. 엑셀 양식의 데이터를 프로그램에 상관없이 쓰기 위한 데이터 형식이라고 보면 된다. 탭(TSV), 빈칸(SSV) 등으로 구분해서 만들기도 한다. 통칭하여 character-separated values (CSV)라고 부른다. 엑셀에서는 “다른 이름으로 저장하기” 기능으로 사용 가능하다.

<br/>

> CSV 읽기 예제

```python
line_counter = 0
data_header = []
customer_list = []

with open("csv/customers.csv") as customer_data:
    while True:
        data = customer_data.readline()
        if not data: break

        if line_counter == 0:
            data_header = data.split(",")
        else:
            customer_list.append((data.split(",")))
        line_counter += 1

print("Header: \t", data_header)
for i in range(0, 10):
    print("Data", i, ":\t\t", customer_list[i])
print(len(customer_list))
```

> CSV 쓰기 예제

```python
line_counter = 0
data_header = []
employee = []
customer_USA_only_list = []
customer = None

with open ("csv/customers.csv", "r") as customer_data:
    while True:
        data = customer_data.readline()
        if not data: break

        if line_counter==0:
            data_header = data.split(",")
        else:
            customer = data.split(",")
            if customer[10].upper() == "USA": #customer 데이터의 offset 10번째 값
                customer_USA_only_list.append(customer) #즉 country 필드가 “USA” 것만

        line_counter+=1 #sutomer_USA_only_list에 저장

print ("Header :\t", data_header)
for i in range(0,10):
    print ("Data :\t\t",customer_USA_only_list[i])
    print (len(customer_USA_only_list))
with open ("csv/customers_USA_only.csv", "w") as customer_USA_only_csv:
    for customer in customer_USA_only_list:
        customer_USA_only_csv.write(",".join(customer).strip('\n')+"\n")
```

- CSV 객체 활용

Text 파일형태로 데이터 처리시 문장 내에 있는 “,” 등에 대해 전처리 과정이 필요하다. 이때 사용하는 것이 CSV 모듈인데, 파이썬에서는 CSV 파일을 처리하기 위해 csv 객체를 제공한다.

<br/>
 
_주의사항: 한글로 되어있는 파일은 깨질 수 있기 때문에 따로 한글 처리가 필요_

```python
import csv
reader = csv.reader(f,
delimiter=',', quotechar='"',
quoting=csv.QUOTE_ALL)
```

<br/>
<br/>
