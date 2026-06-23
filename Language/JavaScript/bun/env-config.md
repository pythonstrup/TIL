# Bun의 환경변수 로딩과 설정 (`.env` 자동 로드)

NestJS에서 설정은 보통 `@nestjs/config`로 한다. `ConfigModule.forRoot()`를 올리고 `ConfigService`를 주입받아 `config.get('KEY')`로 꺼낸다. 그런데 Bun 런타임으로 가면 이 패키지가 **불필요해질 수 있다.** 왜인지 정리한다.

전제: Bun 런타임 배경은 [javascript-runtime.md](../javascript-runtime.md), 서버는 [bun-server.md](./bun-server.md) 참고.

## 핵심: `process.env` 와 `.env 파일` 은 다른 것

설정 논의가 꼬이는 건 이 둘을 섞기 때문이다.

| 개념 | 누가 채우나 | 로더 필요? |
|---|---|---|
| **실제 환경변수** (shell `export`, `docker -e`, k8s env, ECS) | OS/런타임이 **항상** `process.env` 에 넣음 | ❌ |
| **`.env` 파일** | 누군가 *읽어서* `process.env` 에 주입해야 함 | ✅ (dotenv 등) |

`process.env`는 프로세스가 시작될 때 OS가 넘겨준 환경변수 맵이다. `export FOO=bar && node app.js` 하면 `process.env.FOO`는 그냥 있다 — 아무 라이브러리 없이. 운영 환경(k8s/ECS/Docker)은 env를 이렇게 주입하므로 **운영에서는 설정 로더가 애초에 필요 없다.**

로더(dotenv / `@nestjs/config`)가 필요한 건 오직 **`.env` _파일_** 케이스다. 파일은 자동으로 `process.env`에 들어가지 않으니, 누군가 파일을 읽어 심어야 한다. 그게 dotenv가 하는 전부다.

## Bun 은 `.env` 를 자동 로드한다

Node는 `.env`를 자동으로 읽지 않는다(그래서 dotenv/`@nestjs/config`가 필요했다). **Bun은 런타임이 시작될 때 `.env` 파일을 자동으로 읽어 `process.env`에 넣는다.**

```bash
bun run app.ts   # .env 가 있으면 자동으로 process.env 에 로드됨 (별도 패키지 불필요)
```

로드 순서(대략): `.env` → `.env.{NODE_ENV}` → `.env.local` (단 `NODE_ENV=test` 에선 `.env.local` 제외). 즉 Node에서 dotenv가 하던 일을 **Bun이 런타임 차원에서 공짜로** 해준다.

## 그래서 `@nestjs/config` 의 역할이 사라진다

`@nestjs/config`가 하는 일은 사실상 셋이다.

1. `.env` 파일 로드 (dotenv 래핑)
2. `ConfigService` 주입 제공
3. 검증 / 네임스페이스(`registerAs`) / 변수확장(`${VAR}`)

Bun + 자체 설정 서비스로 가면:

- **1번** → Bun 자동 로드가 대체
- **2번** → 직접 만든 설정 서비스가 대체
- **3번** → Zod로 직접 검증

세 역할이 모두 대체되면 `@nestjs/config`는 할 일이 없다 → 의존성째로 제거 가능.

## 패턴: Zod 로 검증하는 설정 서비스

`process.env`를 부팅 시 한 번 Zod로 파싱해, 타입 안전하고 fail-fast한 설정 객체로 만든다.

```ts
// env.schema.ts
import { z } from 'zod';

export const envSchema = z.object({
  PORT: z.coerce.number().int().positive().default(4002),
  REDIS_HOST: z.string().min(1),                 // 누락 시 부팅 실패(fail-fast)
  REDIS_PORT: z.coerce.number().default(6379),
  REDIS_DB: z.coerce.number().int().min(0).default(5),
});
export type Env = z.infer<typeof envSchema>;

// app-config.service.ts
@Injectable()
export class AppConfigService {
  private readonly env: Env;
  constructor() {
    this.env = envSchema.parse(process.env); // 경계 검증 + 타입 확정
  }
  get redis() {
    return { host: this.env.REDIS_HOST, port: this.env.REDIS_PORT, db: this.env.REDIS_DB };
  }
}
```

`config.get('REDIS_HOST')`(`string | undefined`, 약타입)가 `config.redis.host`(`string` 확정)로 바뀐다. 그리고 잘못되거나 누락된 env는 `ConfigService`처럼 런타임에 슬그머니 `undefined`가 되는 게 아니라 **부팅 시점에 `ZodError`로 즉시 터진다.** 이게 "경계에서 검증, fail-fast" 원칙과 맞는다.

## 주의: 이건 "Bun 런타임 한정" 이다

이 구조는 **Bun의 `.env` 자동 로드에 의존**한다.

- Bun 실행 + `.env` 파일 → 자동 로드 ✅
- 운영(env 주입, 파일 없음) → `process.env`에 이미 있음 ✅
- **Node 실행 → `.env` 안 읽힘** ❌ (이땐 dotenv/`@nestjs/config` 필요)

런타임을 Bun으로 고정한 프로젝트에서만 안전하게 `@nestjs/config`를 뺄 수 있다. Node 호환을 열어둬야 하면 로더는 남겨야 한다.

## 한 줄 요약

> `@nestjs/config`/dotenv는 **`.env` 파일을 `process.env`로 옮기는 도구**일 뿐이다. Bun은 그걸 런타임이 자동으로 하고, 실제 환경변수는 어차피 `process.env`에 항상 있으므로 — **Bun + Zod 설정 서비스면 `@nestjs/config` 없이 충분하다.**

관련: [dependency-management.md](./dependency-management.md) (DI 컨테이너 없이 의존성 관리) · [bun-server.md](./bun-server.md) · [javascript-runtime.md](../javascript-runtime.md)
