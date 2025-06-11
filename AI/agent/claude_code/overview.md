# Claude Code 마스터하기

> 프로그래밍의 역사는 끈임없는 `추상화`와 `도구 발전`의 역사

## Claude Code의 핵심 철학?

1. 유연성: 워크 플로우 강제 X, 다양한 접근 방식 모두 가능
2. 투명성: 모든 작업 과정이 공개
3. 협업: AI는 도구가 아닌 동료
4. 맥락 이해: 전체 프로젝트 맥락 파악

### 강점

- 프로젝트의 전체 컨텍스트를 파악할 수 있다는 점.
- 기능도 강력 -> 30분 만에 실시간 채팅 앱을 만들었다고 함.

----

## 기본 설정 최적화

### 전역 파일 설정

- `~/.claude/config.json`

```json
{
  "api_key": "sk-ant-...",              // API 인증 키
  "default_model": "claude-3-opus-20240229", // 기본 사용 모델
  "theme": "dark",                       // 인터페이스 테마 (dark/light)
  "editor": "vscode",                    // 선호 에디터
  "auto_commit": false,                  // 자동 커밋 여부
  "language": "ko",                      // 기본 언어 설정
  "permissions": {
    "file_write": true,                  // 파일 쓰기 권한
    "file_read": true,                   // 파일 읽기 권한
    "command_execution": true            // 명령 실행 권한
  }
}
```

### IDE?

- 나 같은 경우 IntelliJ의 Claude Code 플러그인을 설치

### 성능 최적화 팁

#### 1. 캐시 활성화

- 반복적인 요청에 대한 응답을 캐시하여 속도를 향상

```shell
# 응답 캐시 활성화
$ claude config set cache_enabled true

# 캐시 유지 시간 설정 (초 단위, 기본: 3600초 = 1시간)
$ claude config set cache_ttl 3600

# 캐시 크기 제한 설정 (MB 단위)
$ claude config set cache_max_size 100
```

- 동일한 코드를 반복 분석할 때 유용.
- 큰 프로젝트에서 점진적 작업 시 효과적
- 캐시 무효화 `claude cache clear`

#### 2. 컨텍스트 크기 조정

- Claude Code가 한 번에 처리할 수 있는 컨텍스트 크기를 조정

```shell
# 큰 프로젝트의 경우 컨텍스트 확대
$ claude config set max_context_length 100000

# 작은 프로젝트나 빠른 응답이 필요한 경우
$ claude config set max_context_length 50000

# 현재 설정 확인
$ claude config get max_context_length
```

- 설정 가이드라인: 
  - 소규모 프로젝트 (< 50개 파일): 50,000 
  - 중간 규모 프로젝트 (50-200개 파일): 100,000 
  - 대규모 프로젝트 (200개+ 파일): 200,000 

#### 3. 로컬 모델 사용

- 인터넷 연결 없이도 사용하는 방법

```shell
# 로컬 모드 활성화
$ claude config set local_mode true

# 로컬 모델 다운로드 (용량 주의: 수 GB)
$ claude download-model claude-instant

# 다운로드된 모델 확인
$ claude list-models --local

# 특정 모델로 실행
$ claude --model local:claude-instant "코드를 분석해줘"
```


# 참고자료

- [Claude Code 마스터하기](https://revfactory.github.io/claude-code-mastering/)
