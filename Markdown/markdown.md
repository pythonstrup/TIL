# 마크다운 작성하는 법

### 출처

- https://gist.github.com/ihoneymon/652be052a0727ad59601
- https://velog.io/@yuuuye/velog-%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4MarkDown-%EC%9E%91%EC%84%B1%EB%B2%95
- https://thisblogbusy.tistory.com/entry/%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4Markdown-%EC%9D%B4%EB%9E%80

<hr/>

## 1. 마크다운의 정의

<hr/>

### 1-1. 마크다운이란?

<hr/>

마크다운이란 2004년 존그루버가 만든 텍스트 기반의 마크업 언어이다. 쉽게 읽고 쓸 수 있으며 HTML로 변환도 가능해 간편하다. 단 모든 HTML을 지원하지는 않는다. 작성법은 특수기호와 문자를 이용하며, 문법이 간단한 편이라 웹에서도 빠르게 컨텐츠를 작성할 수 있고 읽기도 좋다. 마크다운이 많이 쓰이기 시작한 때는 GitHub가 생겨난 후 부터이다. 깃허브의 원격저장소에 관한 정보를 기록하고 알려주는 README는 마크다운 언어로 쓰인다. 이를 기점으로 온라인 게시물에도 마크다운 언어를 많이 사용하기 시작했다.
<br/><br/>

## 2. 마크다운 작성법

<hr/>

### 2-1. 타이틀 작성

<hr/>

HTML과 비슷하게 h1 ~ h6 지원

```
h1
======
h2
------

# h1
## h2
### h3
#### h4
##### h5
###### h6
```

# h1

## h2

<br/>

# h1

## h2

### h3

#### h4

##### h5

###### h6

<br/><br/>

### 2-2. 인용블럭(BlockQuote)

<hr/>
HTML의 blockquote 태그와 같다. 인용구문은 <span style="background-color: #f6f8fa">&nbsp;> </span> 기호를 사용하는데, 아래와 같이 중첩이 가능하다.

```
> 인용구문1
>   > 인용구문2
>   >   > 인용구문3
```

> 인용구문1
>
> > 인용구문2
> >
> > > 인용구문3

<br/><br/>

### 2-3. 목록

<hr/>

#### 2-3-1. 순서가 있는 목록

<span style="background-color: #f6f8fa">&nbsp;숫자 </span>와 <span style="background-color: #f6f8fa">&nbsp;. </span>을 이용해 목록을 작성한다.

```
1. 1번
2. 2번
3. 3번
```

1. 1번
2. 2번
3. 3번

#### 2-3-2. 순서가 없는 목록

<span style="background-color: #f6f8fa">&nbsp;\* </span>, <span style="background-color: #f6f8fa">&nbsp;+ </span>, <span style="background-color: #f6f8fa">&nbsp;- </span>를 지원한다.

```
- 목록1
  - 목록 2
    - 목록 3

* 목록 3

+ 목록 4
```

- 목록1
  - 목록 2
    - 목록 3

* 목록 3

- 목록 4

<br/><br/>

### 2-4. 폰트 스타일

<hr/>
글자를 굵게 만들거나 기울여쓰기, 취소라인 그리기, 글자 꾸미기 등이 가능하다.

- 기본적인 강조 표현

```
__bold__
**bold**
_italic_
*italic*
~~cancleLine~~
```

**bold** <br/>
**bold** <br/>
_italic_ <br/>
_italic_ <br/>
~~cancleLine~~

<br/>

- 글자에 색 넣기

```
<span style="color:blue">글자파란색</span>
<span style="background-color: #f6f8fa">회색형광펜</span>
```

<span style="color:blue">글자파란색</span> <br/>
<span style="background-color: #f6f8fa">회색형광펜</span>

<br/><br/>

### 2-5. 문단 스타일

<hr/>
수평선이나 줄바꿈, 띄어쓰기 등을 통해 문단을 구분하는 기준을 표기할 수 있다.

```
<hr/>  # 수평선
<br/>  # 줄바꿈
&nbsp; # 띄어쓰기(여러번 가능)
```

<br/><br/>

### 2-6. 코드

<hr/>

#### 2-6-1. 코드 그대로 보여주기

<hr/>
백틱(`)을 사용하면 코드를 그대로 보여줄 수 있다.

```
`<hr/>`
```

`<hr/>`

<br/>

#### 2-6-2. 코드블럭

<hr/>
코드 블럭을 사용할 수 있는 2가지 방식이 있다.

- `<pre><code></code></pre>` 사용하기

```
<pre>
<code>
코드작성!!!
</code>
</pre>
```

<pre>
<code>
코드작성!!!
</code>
</pre>

<br/>

- 백틱(`) 사용하기

````

```
코드작성!!!
```

````

위와 같이 백틱(`)을 3번 겹쳐 사용하면 코드블럭을 만들어 사용할 수 있다.

```
코드작성!!!
```

<br/>

- 코드 문법 강조하기

아래와 같이 백틱 첫번째 줄에 언어명을 적어주면 코드 문법강조가 가능하다.

````

```python
a = 10
b = 20
print(a+b)
```

````

```python
a = 10
b = 20
print(a+b)
```

<br/><br/>

### 2-7. 링크

<hr/>
링크의 종류로는 참조링크, 외부링크, 자동연결 등이 있다.

- 참조링크

```
**참조링크**
[link keyword][id]

[id]: URL 주소 "Optional Title"

**예시**
블로그 링크: [myvelop][blogLink]

blogLink: https://myvelop.tistory.com/ "Go Blog"
```

블로그 링크: [myvelop][bloglink]

[bloglink]: https://myvelop.tistory.com/ "Go Blog"

여기서 "Optional Title"은 링크에 마우스를 올렸을 때 나오는 설명을 덧붙일 수 있는 기능이다.

<br/>

- 외부링크

```
**외부링크**
[Title](link)

**예시**
블로그링크: [myvelop](https://myvelop.tistory.com/)
```

블로그링크: [myvelop](https://myvelop.tistory.com/)

<br/>

- 자동연결

URL 주소를 그대로 가져와 링크를 만든다.

```
블로그링크: <https://myvelop.tistory.com/>
```

블로그링크: <https://myvelop.tistory.com/>

<br/><br/>

### 2-8. 이미지

<hr/>

- 링크 걸기

링크나 경로를 통해 이미지를 가져와야한다. 이미지의 URL링크나 파일에 저장되어 있는 이미지를 가져와 이미지를 표시할 수 있다.

```
**이미지 링크**
![이미지설명](path/img.jpg)
![이미지설명](path/img.jpg "Optional title")

**예시**
![그림](https://cdn.pixabay.com/photo/2022/02/26/08/23/people-7035403_960_720.jpg)
```

![그림](https://cdn.pixabay.com/photo/2022/02/26/08/23/people-7035403_960_720.jpg)

<br/>

- 이미지 크기 조절하기

마크다운에서 따로 지원하는 크기 조절 문법이 없기 때문에 HTML의 문법인 `<img src="" width="" height=""></img>` 를 이용해야한다.

```
**크기조절**
<img src="path/img.jpg" width="" height="" title="" alt=""></img>

**예시**
<img src="https://cdn.pixabay.com/photo/2022/02/26/08/23/people-7035403_960_720.jpg" width="300px" height="20%" title="그림" alt="그림"></img>
```

<img src="https://cdn.pixabay.com/photo/2022/02/26/08/23/people-7035403_960_720.jpg" width="300px" height="20%" title="그림" alt="그림"></img>

<br/><br/>

### 2-9. 이모지

<hr/>
이모지 링크 : https://kr.piliapp.com/twitter-symbols/ <br/>
위의 이모지를 복사 붙여넣기 하면 마크다운에서도 이모지를 사용할 수 있다. <br/>

😀😃🧐👄

<br/><br/>

### 2-10. 테이블 만들기

<hr/>

참고 자료:
https://inasie.github.io/it%EC%9D%BC%EB%B0%98/%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4-%ED%91%9C-%EB%A7%8C%EB%93%A4%EA%B8%B0/
<br/>

| 으로 열을 구분하며, 폰트스타일이 적용 가능하다.
하이픈(-)과 콜론(:)을 이용하면 정렬도 가능하다. 콜론이 왼쪽에만 있으면 왼쪽 정렬, 양쪽에 있으면 가운데 정렬, 오른쪽에만 있으면 오른쪽 정렬이다.

```
| 제목 | 작가 | 출판일 |
|:-----|:----:|----:|
| 제목1 | 작가1 | 출판일1 |
| 제목2 | 작가2 | 출판일2 |
```

<br/>

| 제목  | 작가  |  출판일 |
| :---- | :---: | ------: |
| 제목1 | 작가1 | 출판일1 |
| 제목2 | 작가2 | 출판일2 |

<br/><br/>

# 마무리

vscode에서 마크다운 preview 사용 단축키 : Ctrl + Shift + v
<br/>

10분 마크다운 배우기: [마크다운 배우기](https://www.markdowntutorial.com/kr/lesson/1/)

<br/>

티스토리에서 마크다운 사용하기: [티스토리 마크다운](https://redthing.tistory.com/entry/%ED%8B%B0%EC%8A%A4%ED%86%A0%EB%A6%AC-%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4-%EB%AC%B8%EB%B2%95%EC%9C%BC%EB%A1%9C-%ED%8F%AC%EC%8A%A4%ED%8C%85%ED%95%98%EB%8A%94-%EB%B0%A9%EB%B2%95)
