# Git Subtree — 외부 레포를 내 레포 안으로 흡수하기

git subtree는 다른 git 저장소의 내용을 **내 저장소의 한 하위 디렉토리로 통째로 합쳐 넣는** 방법이다.
submodule이 "포인터(SHA)만 저장"하는 것과 정반대로, subtree는 **외부 코드의 실제 파일을 내 커밋 히스토리에 직접 포함**시킨다.

## submodule과의 근본 차이

| 항목                  | submodule                          | subtree                              |
| --------------------- | ---------------------------------- | ------------------------------------ |
| 부모에 저장되는 것    | gitlink (커밋 SHA 포인터, `160000`) | **실제 파일 전부** (일반 blob/tree)  |
| 클론하면              | 빈 폴더 (init/update 필요)         | 그냥 코드가 다 들어있음              |
| 별도 `.git`           | 있음 (`.git/modules/`)             | **없음** — 일반 파일과 동일          |
| 버전 고정 방식        | gitlink SHA                        | merge된 커밋 (히스토리에 녹아듦)     |
| 외부 코드 수정·반영   | 서브모듈에서 커밋 후 gitlink bump  | subtree push로 외부 레포에 되돌림    |
| 사용자 학습 비용      | detached HEAD 등 별도 개념 필요    | 거의 없음 (일반 git처럼 보임)        |

핵심: **submodule = 참조(reference), subtree = 복사+병합(merge).**
clone 받는 사람 입장에서 subtree는 "그냥 폴더"라 submodule처럼 `--init`을 까먹어서 빈 폴더가 되는 사고가 없다.

## 핵심 명령

### 외부 레포를 처음 가져오기 (add)

```bash
# libs 라는 하위 디렉토리로 외부 레포의 main 브랜치를 흡수
git subtree add --prefix=libs https://github.com/org/libs.git main --squash
```

- `--prefix=libs`: 흡수될 하위 디렉토리 경로
- `--squash`: 외부 레포의 전체 히스토리를 끌어오지 않고 **단일 커밋으로 압축**해서 합침 (대부분 이걸 권장. 안 그러면 외부 히스토리가 통째로 내 로그에 섞임)

### 외부 변경 가져오기 (pull)

```bash
git subtree pull --prefix=libs https://github.com/org/libs.git main --squash
```

### 내가 한 수정을 외부 레포로 되돌리기 (push)

```bash
git subtree push --prefix=libs https://github.com/org/libs.git main
```

`--prefix` 하위에서 한 커밋들만 골라내(split) 외부 레포에 반영한다.

### URL을 매번 치기 싫으면 remote 등록

```bash
git remote add libs-origin https://github.com/org/libs.git
git subtree pull --prefix=libs libs-origin main --squash
```

## 원리 — subtree merge 전략

subtree는 마법이 아니라 git의 **subtree merge 전략**을 감싼 편의 명령이다.

1. `add`/`pull` 시 외부 레포를 fetch한다.
2. 그 트리를 `--prefix` 경로 **아래로 이동시켜** 내 트리와 병합한다. (외부 레포의 루트 `src/foo.ts` → 내 레포 `libs/src/foo.ts`)
3. 결과적으로 외부 파일들이 **내 커밋의 일반 트리 항목(blob/tree)** 으로 들어온다. gitlink(`160000`) 같은 특수 항목이 아니다.

`push`(=subtree split)는 반대로, `--prefix` 경로의 변경만 추출해 외부 레포 기준의 커밋으로 재구성한 뒤 푸시한다.

## 장단점

### 장점

- **클론이 단순**: `git clone` 한 번이면 끝. init/update 불필요, "빈 폴더" 사고 없음
- 외부 코드가 일반 파일이라 **빌드·CI·IDE가 특별 설정 없이 동작**
- 외부 레포를 모르는 동료도 평소처럼 작업 가능 (학습 비용 낮음)
- 인터넷/접근 권한 없이도 이미 흡수된 코드는 그대로 빌드됨 (벤더링 효과)

### 단점

- 레포 용량이 커진다 (실제 파일 + 변경 히스토리가 다 들어옴)
- `--prefix`, `--squash` 옵션을 매번 정확히 줘야 해서 **명령이 길고 실수하기 쉬움**
- 외부로 되돌리는 push가 submodule보다 직관성이 떨어짐 (split 개념)
- "지금 이 폴더가 외부 어느 커밋 기준인지"가 gitlink처럼 한눈에 안 보임 (merge 커밋 메시지로 추적)

## 언제 subtree를 쓰나

- 소비자가 **외부 레포를 거의 수정하지 않고 가져다 쓰기만** 할 때 (벤더링)
- clone 받는 사람이 submodule 개념을 모르길 원할 때 (오픈소스 배포 등)
- CI/빌드에서 서브모듈 인증·init 단계를 없애고 싶을 때
- 외부 의존성을 **레포에 동결(freeze)** 시켜 재현성을 확보하고 싶을 때

반대로 lib을 **자주 양방향으로 수정**하거나, **여러 부모가 같은 lib을 각자 다른 버전으로** 공유해야 하면 submodule이나 패키지 레지스트리가 더 맞는다.

## 한 줄 요약

> submodule은 "남의 레포를 **가리킨다**", subtree는 "남의 레포를 **내 안에 복사해 녹인다**".
> 가져다 쓰기만 하고 클론을 단순하게 하고 싶으면 subtree, 독립 버전 관리와 명시적 핀이 필요하면 submodule.

---

관련: [[git-submodule]] (gitlink 원리, submodule vs 모노레포 비교)
