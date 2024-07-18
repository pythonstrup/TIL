# Apache Zeppelin 시작

- [Github Link](https://github.com/apache/zeppelin)

## Github Repo

<img src="img/zeppelin_start01.png">

- 깃허브 레포지토리에서 Fork를 떠보자.

<img src="img/zeppelin_start02.png">

- Fork 떠온 레포지토리를 clone해서 프로젝트를 받아본다.

<img src="img/zeppelin_start03.png">

## Java 1.8 설정

- [MacOS openJDK 1.8 설치하기](https://velog.io/@sj-lee33/MacOS-openJDK-1.8-%EC%84%A4%EC%B9%98%ED%95%98%EA%B8%B0)

## Java 11 설정

- Zeppelin에서 Java 1.8은 Deprecated될 예정이라 Java 11로 설정하는 것이 좋다고 멘토님께서 조언해주셨다.
- OpenJdk11 설치

```shell
$ brew tap adoptopenjdk/openjdk      # adoptopenjdk/openjdk 저장소 추가
$ brew install cask                  # cask 없으면 설치
$ brew install --cask adoptopenjdk11 # openJdk 11 설치
```

- vim을 사용해 zshrc 설정을 변경해준다.

```shell
$ vim ~/.zshrc
```

- JAVA_HOME 변수 설정

```shell
export JAVA_HOME=$(/usr/libexec/java_home -v 11)
export PATH=${PATH}:$JAVA_HOME/bin:
```

- 다 작성했다면 `wq`로 빠져나와서 아래 명령어로 수정 사항을 적용해주자.
- 그리고 Java 버전을 확인해보면 11로 설정된 것을 확인할 수 있다.

```shell
$ source ~/.zshrc
$ java --version
openjdk 11.0.11 2021-04-20
OpenJDK Runtime Environment AdoptOpenJDK-11.0.11+9 (build 11.0.11+9)
OpenJDK 64-Bit Server VM AdoptOpenJDK-11.0.11+9 (build 11.0.11+9, mixed mode)
```

## Build

- build 방법은 문서에 잘 설명되어 있다.
- [공식문서](https://zeppelin.apache.org/docs/latest/setup/basics/how_to_build.html)
- 프로젝트 디렉토리에 들어가 명령어로 빌드를 실행한다.

```shell
# Test Skip
./mvnw clean install -DskipTests

# debug를 보고 싶다면 아래와 같이 실행
./mvnw clean install -DskipTests -X

# Test와 함께 실행
./mvnw clean install
```

- 성공적으로 빌드가 완료되면 아래와 같은 결과과 뜬다.

<img src="img/build_complete01.png">
<img src="img/build_complete02.png">
<img src="img/build_complete03.png">

- 빌드가 완료되면 Zeppelin Deamon을 실행하면 된다.

```shell
./bin/zeppelin-daemon.sh start
```

<img src="img/build_complete04.png">

- 이제 `localhost:8080` 접속해보자. 제플린이 동작하는 것을 확인할 수 있다.

<img src="img/build_complete05.png">

