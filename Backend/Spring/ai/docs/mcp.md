# Model Context Protocol

## 기능

### Spring AI와 통합

- Spring AI의 도구 실행 프레임워크와 통합되는 툴 콜백(tool callback) 구성을 자동으로 설정할 수 있으며, 이를 통해 MCP 도구들을 AI 상호작용에 활용할 수 있다.
- 이 통합은 기본적으로 활성화되어 있으며, 다음 속성을 false로 설정하여 비활성화할 수 있다.

```yaml
spring:
  ai:
    mcp:
      client:
        toolcallback:
          enabled: false
```

# 참고자료

- [Spring AI 공식문서](https://docs.spring.io/spring-ai/reference/api/mcp/mcp-overview.html)
