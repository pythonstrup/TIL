# Git Submodule — gitlink 원리와 버전 관리

서브모듈은 "다른 git 저장소를 내 저장소 안의 한 폴더로 끼워 넣는" 기능이다.
핵심은 부모 저장소가 서브모듈의 **파일 내용을 저장하지 않고, 가리키는 커밋 SHA(포인터)만 저장**한다는 것이다.

## gitlink — 어디에, 어떻게 저장되나

git의 트리 객체는 각 항목을 `<mode> <type> <sha> <name>` 형태로 저장한다.

```
160000 commit 6cfaee2f...   libs       ← gitlink (서브모듈)
100644 blob   b958ee0a...   libs.version
040000 tree   ...           일반 폴더라면 이렇게
```

mode 값이 항목의 종류를 결정한다.

| mode     | 의미                         |
| -------- | ---------------------------- |
| `100644` | 일반 파일 (blob)             |
| `100755` | 실행 파일                    |
| `040000` | 디렉토리 (tree)              |
| `160000` | **gitlink — 서브모듈 포인터** |

즉 부모 레포는 서브모듈 폴더의 파일을 하나도 저장하지 않고, 트리에 `160000 commit <SHA>` 항목 하나만 박아 둔다. 의미는 한 줄이다.

> "이 서브모듈은 커밋 `<SHA>`에 있어야 한다"

이 SHA 하나가 **버전 핀(pin)이자 진실의 원천**이다. 물리적으로는 부모의 다른 객체처럼 `.git/objects`의 트리 객체 안에 커밋과 함께 저장된다.

- `git add <submodule>` 을 하면 → git이 그 시점 서브모듈의 HEAD SHA를 읽어 인덱스에 `160000` 항목으로 써넣는다.
- 그래서 서브모듈을 업데이트한 뒤 `git add <submodule>` 을 해야 gitlink가 새 SHA로 갱신된다. (방향: **워킹트리 HEAD → gitlink**)

## 원리 — 정보가 사는 네 곳

서브모듈 정보는 4군데에 나뉘어 살고, 각자 역할이 다르다. 이게 동작 원리의 핵심이다.

| 위치                                    | 커밋됨?     | 역할                                                |
| --------------------------------------- | ----------- | --------------------------------------------------- |
| `.gitmodules`                           | O (추적됨)  | path→url **레시피**. 어떤 서브모듈이 어디 있는지    |
| 트리/인덱스의 gitlink (`160000`)        | O (추적됨)  | **버전** (어느 커밋에 고정되어 있는지)              |
| `.git/config` 의 `submodule.<>.url`     | X (로컬)    | **활성화 여부**. 여기 등록돼야 "초기화된" 서브모듈  |
| `.git/modules/<>/`                      | X (로컬)    | 서브모듈의 실제 `.git` 저장소(objects/refs)가 사는 곳 |

`.gitmodules`(공유 레시피)와 `.git/config`(내 로컬 활성화)가 **분리**되어 있다는 점이 포인트다.
클론만 했다고 서브모듈 폴더가 자동으로 채워지지 않는 이유이며, 그래서 초기화(`--init`)가 필요하다.

## git submodule update --init --recursive 가 하는 일

세 부분이 합쳐진 명령이다.

1. **`--init`**: `.git/config`에 아직 없는 서브모듈에 대해, `.gitmodules`의 url을 `.git/config`로 복사하고 클론(초기화)한다. 이 옵션이 없으면 미초기화 서브모듈은 그냥 건너뛴다.
2. **`update`**: `.git/config`에 등록된 각 서브모듈에 대해 —
   - 부모 인덱스에서 gitlink SHA를 읽고
   - 서브모듈 안에서 그 SHA로 `git checkout` (→ **detached HEAD**)
   - 그 SHA가 로컬에 없을 때만 remote에서 fetch
3. **`--recursive`**: 위 과정을 각 서브모듈 내부에서 또 반복한다. (서브모듈이 자기 서브모듈을 가질 수 있으므로 끝까지 내려간다)

결과: 모든 서브모듈 워킹트리가 부모가 핀한 SHA로 정확히 checkout되고, 설정 안 된 건 클론까지 해서 트리 전체가 일관된 상태가 된다.

## 멘탈 모델

> 부모는 **내용이 아니라 포인터(SHA)** 만 저장한다.
> `git submodule update`는 그 **포인터를 해석(resolve)** 하는 행위 — "기록된 SHA대로 워킹트리를 맞춰라".

`git submodule update`는 **단방향**(`gitlink → 워킹트리`)이다. 이 명령은 절대 gitlink를 옮기지 않는다.
gitlink를 옮기는 건 오직 `git add <submodule>`(`워킹트리 → gitlink`)뿐이다.

그래서 의도에 따라 명령이 갈린다.

| 하고 싶은 것                        | 명령                                                       |
| ----------------------------------- | ---------------------------------------------------------- |
| 서브모듈을 새 버전으로 올리기 (bump) | `cd sub && git pull` → 부모에서 `git add sub && git commit` |
| 남이 올린 버전에 내 것 맞추기 (consume) | `git pull && git submodule update --init --recursive`      |

- `git submodule update`는 `--remote` 없이는 절대 최신을 받아오지 않는다. (gitlink가 가리키는 과거 SHA로 되돌릴 뿐)
- 클론 시 한 번에 받으려면 `git clone --recurse-submodules <url>`.

## 정상 서브모듈의 디스크 레이아웃

제대로 초기화된 서브모듈은 워킹트리의 `.git`이 **디렉토리가 아니라 파일**이며, 실제 저장소는 `.git/modules/` 아래에 산다.

```
shared/.git        →  "gitdir: ../.git/modules/shared"   (파일 포인터)
.git/modules/shared/  ←  실제 objects/refs
```

만약 서브모듈 폴더 안의 `.git`이 **통째로 디렉토리**라면, 그것은 `git submodule update --init`으로 초기화된 게 아니라 평범한 `git clone`으로 폴더에 박아 넣은 **독립 저장소(embedded repo)** 다.
`.git/config`에 url이 등록돼 있으면 git이 "등록은 됐다"고 인식하긴 하지만, 레이아웃은 비표준이라 `git submodule update`가 의도와 다르게 동작할 수 있다.

## 한 폴더에 두 시스템을 두지 말 것

서브모듈 버전을 별도 해시 파일(예: `libs.version`)과 동기화 스크립트로도 관리하면서 동시에 git submodule로도 등록하면, 같은 폴더에 **버전 핀이 두 개** 생긴다.

- gitlink와 해시 파일은 자동으로 동기화되지 않는다.
- 한쪽만 올리면 다른 방식을 쓰는 동료는 옛 버전을 받게 되어 **드리프트(drift)** 가 발생한다.

→ 버전 고정 방식은 **하나로 통일**해야 한다. 표준 도구·CI 지원과 원자성(gitlink가 커밋과 함께 묶여 드리프트 불가) 때문에 **네이티브 submodule 방식 권장**.
편의 스크립트를 유지하려면 별도 해시 파일을 두지 말고 스크립트가 gitlink에서 해시를 읽게 한다.

```bash
# 별도 버전 파일 대신 gitlink에서 직접 해시 추출
LIBS_GIT_HASH=$(git ls-tree HEAD libs | awk '{print $3}')
```
