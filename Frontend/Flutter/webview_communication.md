# Flutter WebView Two Way Communication

- JavaScript로 작성된 웹 페이지와 데이터 통신 방법

## webview_flutter 패키지

- `webview_flutter` 패키지에서는 WebViewController의 `addJavaScriptChannel()` 를 통해 웹에서 플러터로 메세지를 보낼 수 있다.

```dart
@override
void initState() {
  super.initState();
  
  controller = WebViewController()
        // ... 중략
        ..addJavaScriptChannel('channelName', onMessageReceived: (JavaScriptMessage message) {
          
        });
}
```

- Flutter에서 위와 같이 channel을 등록하면 javascript를 통해 채널에 접근할 수 있다.

```javascript
ChannelName.postMessage('eventName');
```

- 하지만 자바스크립트에서 ChannelName이 등록되어 있지 않으므로 `//@ts-ignore`가 필요할 수 있다.
  - Vue/Nuxt에서는 플러그인으로 잘 등록해볼 수 있고
  - React/Next에서는 React Context API나 Custom Hook을 활용해볼 수 있을듯.

## React 기반 FlutterProvider 만들어보기?

- window interface는 알맞게 설정 필요

```typescript jsx
import React, { createContext, useContext, useEffect, useState } from "react";

// Flutter 통신을 위한 Context 생성
const FlutterContext = createContext<{
  sendMessage: (message: string) => void;
  lastMessage: string | null;
}>({
  sendMessage: () => {},
  lastMessage: null,
});

// Flutter Provider 구현
export const FlutterProvider = ({ children }: { children: React.ReactNode }) => {
  const [lastMessage, setLastMessage] = useState<string | null>(null);

  // Flutter에서 메시지 받을 함수
  useEffect(() => {
    window.flutterMessage = (message: string) => {
      console.log("Flutter에서 받은 메시지:", message);
      setLastMessage(message);
    };
  }, []);

  // Next.js → Flutter로 메시지 보내기
  const sendMessage = (message: string) => {
    if (window.flutterChannel) {
      window.flutterChannel.postMessage(message);
    }
  };

  return (
    <FlutterContext.Provider value={{ sendMessage, lastMessage }}>
      {children}
    </FlutterContext.Provider>
  );
};

// Context를 사용하기 위한 커스텀 훅
export const useFlutter = () => useContext(FlutterContext);
```

- app 전체가 FlutterProvider에 감싸져야 통신이 수월

```typescript jsx
import type { AppProps } from "next/app";
import { FlutterProvider } from "../contexts/FlutterContext";

function MyApp({ Component, pageProps }: AppProps) {
  return (
    <FlutterProvider>
      <Component {...pageProps} />
    </FlutterProvider>
  );
}

export default MyApp;
```

- 이제 컴포넌트에서 CustomHook을 사용해 Flutter와 통신이 가능해진다.

```typescript jsx
import { useFlutter } from "../contexts/FlutterContext";

export default function Home() {
  const { sendMessage, lastMessage } = useFlutter();

  return (
    <div>
      <h1>Next.js ↔ Flutter 통신</h1>
      <button onClick={() => sendMessage("Next.js에서 보낸 메시지!")}>
        Flutter로 메시지 보내기
      </button>
      {lastMessage && <p>Flutter에서 받은 메시지: {lastMessage}</p>}
    </div>
  );
}
```

## 알림 권한 요청 예시



# 참고자료

- [How to develop a Javascript Channel in Flutter between Dart and Javascript Code](https://medium.com/@madhanrkv10/how-to-develop-a-javascript-channel-in-flutter-between-dart-and-javascript-code-7d545bcd73f7)
- [🍀 Flutter WebView 도입기 (2) 웹 - 플러터통신](https://velog.io/@lee7198/Flutter-WebView-%EB%8F%84%EC%9E%85%EA%B8%B0-2-%EC%9B%B9-%ED%94%8C%EB%9F%AC%ED%84%B0%ED%86%B5%EC%8B%A0)
- [[Flutter] Flutter WebView Two Way Communication](https://velog.io/@parrottkim21/Flutter-Flutter-WebView-Two-Way-Communication)
