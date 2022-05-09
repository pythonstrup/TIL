# LaTex 문법으로 마크다운 작성하기

## 출처

- https://velog.io/@d2h10s/LaTex-Markdown-%EC%88%98%EC%8B%9D-%EC%9E%91%EC%84%B1%EB%B2%95

- https://devbull.xyz/simple-md-latex/

- https://typemin.tistory.com/8

- https://jjycjnmath.tistory.com/117

- http://tomoyo.ivyro.net/123/wiki.php/TeX_%EB%B0%8F_LaTeX_%EC%88%98%EC%8B%9D_%EB%AC%B8%EB%B2%95

<hr/>

## 0. 주의사항

- 깃허브에서 `md` 확장자로 LaTex 문법을 적어 올리면 문법을 인식하지 못한다.
- 해결방법: [Latex rendering in README.md on Github](https://localcoder.org/latex-rendering-in-readme-md-on-github)
- 사용하기 편리한 해결 방법을 정리하자면 아래와 같다.

#### 해결방법1: Jupyter Notebook 마크다운에 정리한다.

#### 해결방법2: 인라인 라텍스를 사용한다.
<pre>
<code>
```math
\frac{\sigma}{\sqrt{n}}
```
</code>
</pre>

#### 해결방법3: Readme2Tex를 사용한다.

#### 해결방법4: 온라인 에디터를 사용한다.

<br/>

개인적으로 jupyter notebook에 정리하는 것을 추천한다.

<br/>

<hr/>

## 1. LaTex

<hr/>

### 1-1. 필요성

"모두를 위한 딥러닝 시즌2"를 듣고 마크다운으로 정리하게 되면서, 수식을 정리할 방법을 찾기 시작했다. 내가 생각한 방법은 아래와 같다.

1. 손으로 적어 사진을 찍고 마크다운에 사진을 넣는다.
2. 똑같진 않더라도 키보드에 있는 것으로 해결한다.
3. 마크다운 문법을 찾아서 예쁘게 정리해본다.

<br/>

첫 번째 방법이 제일 쉽고 시간도 적게 걸리겠다고 생각했지만, 심하게 악필인 나한테 맞는 방법은 아니라고 생각했다. 그렇게 마크다운으로 정리하는 방법을 찾기 시작했고 LaTex 문법을 마크다운에 적용하면 수식을 쉽게 적을 수 있다는 것을 알게 됐다.

<br/>

딥러닝에 관련된 포스트를 해놓은 블로그를 돌아다니다 보면, 손으로 직접 적으신 분들도 많지만 확실히 LaTex를 이용해 적는 것이 훨씬 깔끔해보이고 좋았다. 수식표현을 적을 필요가 없다면 LaTex 문법을 배울 필요가 없겠지만, 논문을 쓰거나 딥러닝, 데이터 사이언스 등을 배우고 정리가 필요한 사람이라면 유용하게 사용할 수 있을 것이다.

<br>

### 1-2. LaTex 문법이란?

<hr/>

LaTex는 문서 작성 도구의 일종으로, 논문이나 출판물 등의 특수 형식 문서를 작성하는 데 쓰이는 시스템이다. 자연과학이나 인문과학중 수식, 그래프, 다이어그램을 많이 그리는 학자들에게 유용한 문서 저작도구이다.

<br/>

LaTex는 Tex를 좀 더 쉽게 쓸 수 있도록 만든 것인데, 여기서 Tex는 유명한 컴퓨터과학자인 도널드 커누스가 개발한 것으로 프로그래밍에 대한 책을 쓰려다가 적당한 조판 시스템이 없어서 직접 만들었다고 한다.

<br/>

LaTex는 컴퓨터공학 계열뿐만 아니라 다양한 자연과학, 공학, 사회과학 계열에서도 많이 사용되고 있다. 페이지 넘버와 각주, 미주, 목차, 레퍼런스, 페이지 레이아웃, 테이블, 그림 삽입 등 출판물 등급에 필요한 필수요소들을 모두 구현할 수 있디 때문에 학계의 사랑을 받고 있다고 한다.

<br/>

2022년 현재 가장 많이 쓰이고 있는 LaTex 버전은 LaTex2이다. LaTex3가 약 30년 동안(2022년 기준) 개발 중이지만 잘 쓰이지 않는다고 한다. 그 이유 중 하나는 LaTex2와 문법이 상당히 다르다는 점이다.

<br/><br/>

<hr/>

## 2. 수식 작성법

<hr/>

### 2-1. 정렬

<hr/>

#### 기본적인 방법

`$`로 양쪽을 감싸면 그 안에 수식을 작성할 수 있다. 기본적으로 왼쪽 정렬이다. 문장 사이사이에 수식을 넣는 것도 가능하다.

```
$ x + 1 = y $
New $a =$ Old $a - 10$
```

$ x + 1 = y $ <br/>
New $a =$ Old $a - 10$

<br/>

<hr/>

#### 가운데 정렬

`$$`로 감싸면 가운데 정렬이 된다.

```
$$ x + 1 = y $$
```

<br>

$$ x + 1 = y $$

<br/>

<hr/>

#### 특정 문자를 기준으로 정렬

`aligned` 심볼을 사용해 특정 문자를 기준으로 정렬할 수 있다. `&`가 적혀있는 문자를 기준으로 정렬된다.

```
$$
\begin{aligned}
f(x)&=3x+2\\
g(x)&=5x^2 - 3x + 10
\end{aligned}
$$
```

$$
\begin{aligned}
f(x)&=3x+2\\
g(x)&=5x^2 - 3x + 10
\end{aligned}
$$

<br/>

<hr/>

### 2-2. 줄바꿈과 띄어쓰기

<hr/>

#### 줄바꿈

`\\` 와 같이 역슬래쉬를 두번 입력하면 줄바꿈이 된다. 만약 그냥 엔터만 한다면 줄바꿈이 되지 않는다.

- 줄바꿈이 되는 것

```
$$
x + 1 = y\\
x + z = y
$$
```

$$
x + 1 = y\\
x + z = y
$$

- 줄바꿈이 안되는 것

```
$$
x + 1 = y
x + z = y
$$
```

$$
x + 1 = y
x + z = y
$$

<br/>

<hr/>

#### 띄어쓰기

띄어쓰기도 줄바꿈과 마찬가지로 따로 값을 입력해줘야한다.

- `\,` 는 띄어쓰기 1번
- `\;` 는 띄어쓰기 2번
- `\quad` 는 띄어쓰기 4번

```
$$
New a = Old a - 10\\
New\,a\,=\,Old\,a\,-\,10\\
New\; a = Old a - 10\\
New\quad a = Old a - 10\\
$$
```

$$
New a = Old a - 10\\
New\,a\,=\,Old\,a\,-\,10\\
New\; a = Old a - 10\\
New\quad a = Old a - 10\\
$$

<br/>

### 2-3. 수학표기

<hr/>

#### 곱셈

`\times` 로 적어준다.

```
$$ x \times y = z $$
```

$$ x \times y = z $$

<br/>

<hr/>

#### 첨자

보통 윗첨자는 제곱표기를 할 때, 아랫첨자는 수열의 순서표기 등을 할 때 사용된다. 윗첨자는 `^`로 표기하고 아랫첨자는 `_`로 표기한다. 둘을 같이 쓰는 것도 가능하다.

```
$$ a_n+1 = a_n + a_n-1  $$
$$ f(x) = x_1^2 + x_1 + 10  $$
```

$$ a_n+1 = a_n + a_n-1 $$
$$ f(x) = x_1^2 + x_1 + 10 $$

<br/>

<hr/>

#### 분수

`\frac{분자}{문모}` 형태로 적어주면 된다.

```
$$ \frac{x_1^2 + x_1}{x_2 + 10} $$
```

$$ \frac{x_1^2 + x_1}{x_2 + 10} $$

<br/>

<hr/>

#### 절대값

보통 `|`와 `\vert`을 사용하지만, 분수와 같이 큰 수식에 사용하기에는 적합하지 않기 때문에 `\left\vert`, `\right\vert` 로 감싸서 적어줄 수 있다.

```
$$ \vert x + 10 \vert $$
$$ \vert \frac{x_1^2 + x_1}{x_2 + 10} \vert $$
$$ \left\vert \frac{x_1^2 + x_1}{x_2 + 10} \right\vert $$
```

$$ \vert x + 10 \vert $$
$$ \vert \frac{x_1^2 + x_1}{x_2 + 10} \vert $$
$$\left\vert \frac{x_1^2 + x_1}{x_2 + 10} \right\vert $$

<br/>

<hr/>

#### Log 표기

`\log_2{}`, `\log_e{}` `\log{}` 와 같은 형태로 적을 수 있다.

```
$$ \log_2{n^2} $$
$$ \log{100} $$
```

$$ \log_2{n^2} $$
$$ \log{100} $$

<br/>

<hr/>

#### 삼각함수

`\sin()` `\cos()` `\tan()` 와 같이 써줄 수 있다.

```
$$ sin x $$
$$ cos(5x+2) $$
$$ tan 2x $$
```

$$ sin x $$
$$ cos(5x+2) $$
$$ tan 2x $$

<br/>

<hr/>

#### 극한

`\lim_{극한의밑}{함수}` 으로 적는다.

- `\rarr` 는 오른쪽 화살표이다.
- `\larr` 는 왼쪽 화살표이다.
- `\infin` 은 $\infin$ 이다.

```
$$ \infin \rarr \larr$$
```

$$ \infin \rarr \larr$$

```
$$ \lim_{x\rarr\infin}{x^3} $$
```

$$ \lim_{x\rarr\infin}{x^3} $$

만약 정상적으로 표기되지 않는다면 `\displaystyle`를 앞에 붙여주자.

```
$$ \displaystyle\lim_{x\rarr\infin}{x^3} $$
```

$$ \displaystyle\lim_{x\rarr\infin}{x^3} $$

<br/>

주피터 노트북에서 \infin과 \rarr, \larr가 적히지 않는 오류가 발생한다.
- 무한대는 `\infty`로 적어주자.
- 오른쪽 화살표는 `\rightarrow` 
- 왼쪽 화살표는 `\leftarrow`
- 극한의 밑을 `{x \to \infty}` 로 적어도 된다.

```
$$ \lim_{x \rightarrow \infty}{x+10} $$
$$ \lim_{x \to \infty}{x^3} $$
```
$$ \lim_{x \rightarrow \infty}{x+10} $$
$$ \lim_{x \to \infty}{x^3} $$

<br/>

<hr/>

#### 시그마

`\sum^{시그마의위}_{시그마의밑}{함수}` 로 적어준다.

```
$$ \sum^{10}_{k=1}{k} = 55 $$
```

$$ \sum^{10}_{k=1}{k} = 55 $$

극한과 마찬가지로 만약 정상적으로 표기되지 않는다면 `\displaystyle`를 앞에 붙여주자.

```
$$ \displaystyle\sum^{10}_{k=1}{k} = 55 $$
```

$$ \displaystyle\sum^{10}_{k=1}{k} = 55 $$

<br/>

<hr/>

#### 행렬

`matrix` 심볼을 사용하는데 `\begin{matrix} ... \end{matrix}`와 같이 적는다.
- `{matrix}` 테두리 없음
- `{pmatrix}` 둥근 테두리
- `{bmatrix}` 네모난 테두리
- `{Bmatrix}` 중괄호 테두리
- `{vmatrix}` 행렬식 테두리
- `{Vmatrix}` 노름 테두리

```
$$\begin{matrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{matrix}$$
$$\begin{pmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{pmatrix}$$
$$\begin{bmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{bmatrix}$$
$$\begin{Bmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{Bmatrix}$$
$$\begin{vmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{vmatrix}$$
$$\begin{Vmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{Vmatrix}$$
```

$$\begin{matrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{matrix}$$
$$\begin{pmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{pmatrix}$$
$$\begin{bmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{bmatrix}$$
$$\begin{Bmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{Bmatrix}$$
$$\begin{vmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{vmatrix}$$
$$\begin{Vmatrix}a_{11}&a_{12}\\a_{21}&a_{22}\\ \end{Vmatrix}$$

<br/>

<hr/>

#### 벡터

`\vec{}`과 `\overrightarrow{}`로 쓸 수 있다.

```
$$ \vec{v} $$
$$ \overrightarrow{v} $$
```

$$ \vec{v} $$
$$ \overrightarrow{v} $$

<br/>

<hr/>

#### 조각함수

`cases` 심볼을 사용한다. <br/>
`\begin{cases}...\end{cases}` 와 같이 적을 수 있다.

```
$$  
f(x) = 
\begin{cases}
-x^2, if x < 0\\
x^2, if x \geq 0
\end{cases}
$$
```

$$  
f(x) = 
\begin{cases}
-x^2, if x < 0\\
x^2, if x \geq 0
\end{cases}
$$

<br/>

<hr/>

#### 부등호 기호

`<`와 `>`는 키보드를 이용하면 되지만 그외의 부등호 기호는 LaTex 문법을 사용해야한다.

- `\leq` $\leq$
- '\leqq' $\leqq$ 
- `\geq` $\geq$
- `\geqq` $\geqq$
- `\nless` $\nless$
- `\ngtr` $\ngtr$
- `\nleq` $\nleq$
- `\ngeq` $\ngeq$

<br/>

<hr/>

#### 관계 기호

부등호와 마찬가지로 `=`는 쉽게 쓸 수 있지만 그 외의 것을 적으려면 LaTex 문법을 사용하자.

- `\equiv` $\equiv$ 동치기호
- `\neq` $\neq$
- `\fallingdotseq` $\fallingdotseq$
- `\risingdotseq` $\risingdotseq$
- `\sim` $\sim$

<br/>

<hr/>

#### 집합 기호

- `\in` $\in$
- `\notin` $\notin$
- `\subset` $\subset$
- `\supset` $\supset$
- `\subseteq` $\subseteq$
- `\supseteq` $\supseteq$
- `\cap` $\cap$ 곱집합
- `\cup` $\cup$ 합집합

<br/>

<hr/>

#### 기타 기호

- `\alpha` $\alpha$
- `\beta` $\beta$
- `\theta` $\theta$
- `\lambda` $\lambda$
- `\sigma` $\sigma$
- `\mu` $\mu$
- `\pi` $\pi$
- `\phi` $\phi$
- `\nabla` $\nabla$
- `\lceil x \rceil` $\lceil x \rceil$ 천장함수
- `\lfloor x \rfloo` $\lfloor x \rfloor$ 바닥함수

<br/>

<hr/>

#### 폰트

`\mathbb{}` 를 사용하면 칠판 볼드체를 사용할 수 있다. 수의 집합에서 자주 쓰인다.

```
$$ \mathbb{RQZ} $$
```

$$ \mathbb{RQZ} $$


<br/>

<hr/>