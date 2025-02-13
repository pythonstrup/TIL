# 서버 컴포넌트에서 Zustand 사용하기

- 전역 상태 관리는 zustand. 쿠키는 'next/headers'의 쿠키 혹은 세션을 사용.
- zustand를 사용해서 몇몇 id 값들을 전역 상태 관리 그 값을 set할 때 쿠키나 세션에 담음
- 서버 컴포넌트에서는 zustand에 직접 접근할 수 없기 때문에 'next/headers'를 사용해 쿠키나 세션에서 가져와 해당 정보를 꺼내서 사용

# 참고자료

- [Using React Context in NextJS Server Components](https://dev.to/codingbrowny/using-context-providers-in-nextjs-server-components-2gk4)
- [Zustand with Next.js (app router) - Server and Client Components](https://medium.com/@mak-dev/zustand-with-next-js-14-server-components-da9c191b73df)
- [[Next.js14] 다크모드 구현하기 : Next.js와 zustand로 깜빡임 문제 해결하기](https://velog.io/@meowoof/%EB%8B%A4%ED%81%AC%EB%AA%A8%EB%93%9C-%EA%B5%AC%ED%98%84%ED%95%98%EA%B8%B0-Next.js%EC%99%80-zustand%EB%A1%9C-%EA%B9%9C%EB%B9%A1%EC%9E%84-%EB%AC%B8%EC%A0%9C-%ED%95%B4%EA%B2%B0%ED%95%98%EA%B8%B0)
- [[React] Zustand 라이브러리: 상태(state) 관리 라이브러리](https://tensdiary.tistory.com/entry/React-Zustand-%EB%9D%BC%EC%9D%B4%EB%B8%8C%EB%9F%AC%EB%A6%AC-React-%EC%83%81%ED%83%9Cstate-%EA%B4%80%EB%A6%AC-%EB%9D%BC%EC%9D%B4%EB%B8%8C%EB%9F%AC%EB%A6%AC)
- [서버컴포넌트에서 zustand 사용하기?](https://seopport-u.tistory.com/93)
- [React Query와 Zustand를 사용한 상태관리 (ft. Custom hook)](https://medium.com/@jihyun-j/react-query-vs-swr-9470f685c1fe)
- [[Next.js] 서버 & 클라이언트 컴포넌트에서 토큰 관리하기](https://imdaxsz.tistory.com/66)
