# Google Apps Script

## 구글 폼 생성 및 스프레드시트 연결

- 구글 폼을 생성

<img src="img/connect_google_form_with_apps_script01.png">

- 응답 탭에서 `Sheets에 연결`을 클릭

<img src="img/connect_google_form_with_apps_script02.png">

- 만들어준 스프레드시트가 없다면 새로운 스프레드시트를 만들자.

<img src="img/connect_google_form_with_apps_script03.png">

- 스프레드시트가 완성되었다.

<img src="img/connect_google_form_with_apps_script04.png">

- 스프레드시트의 `확장 프로그램` > `Apps Sciprt`를 선택한다.

<img src="img/connect_google_form_with_apps_script05.png">

- 이제 해당 페이지에서 스크립트 작업을 하면 된다!!

<img src="img/connect_google_form_with_apps_script06.png">

## 트리거 설정

- 실행할 함수를 선택하고, 이벤트 유형을 `양식 제출 시`로 설정

<img src="img/apps_script_trigger01.png">

## 제출 트리거 이벤트 활용

```javascript
function onFormSubmit(e) {
    Logger.log("📩 새로운 폼 응답이 제출됨!");

    // 모든 응답을 배열로 가져오기
    var responses = e.values;
    Logger.log("응답 데이터: " + JSON.stringify(responses));

    // 특정 질문의 응답 가져오기
    var timestamp = responses[0]; // 폼 제출 시간
    var firstQuestionResponse = responses[1]; // 첫 번째 질문 응답
    var secondQuestionResponse = responses[2]; // 두 번째 질문 응답

    Logger.log("제출 시간: " + timestamp);
    Logger.log("첫 번째 질문 응답: " + firstQuestionResponse);
    Logger.log("두 번째 질문 응답: " + secondQuestionResponse);
}
```

- 아래와 같이 설문을 제출

<img src="img/apps_script_trigger02.png">

- 실행 결과를 확인

<img src="img/apps_script_trigger03.png">

## 환경변수 은닉

## 스크립트를 재사용하고 싶다면?

- 기존 스프레드시트 선택을 클릭하고, Apps Script가 적용된 스프레드시트와 연결해주면 재사용이 가능하다!
- 대신 질문을 사용하는 로직이 있다면 형식을 맞춰줘야 부작용이 발생하지 않는다.

<img src="img/app_script_reuse01.png">

# 참고자료

- [간단한 자동화로 Google Forms 응답 Slack에서 받아보기](https://www.babywhale.io/blog/google-forms-slack-integration)
