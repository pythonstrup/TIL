# Decorator

- 데코레이터는 **클래스/메서드를 "재료"로 받아서, 가공한 결과로 바꿔치기하는 함수**다.
- `@deco`는 "컴파일러가 대신 그 함수를 호출하고 대상(target)을 자동으로 넣어주는" 문법 설탕(syntactic sugar)이다.

```ts
function deco(target: any) {
  console.log(target.name);
}

@deco
class Foo {}

// 위 @deco 는 컴파일러가 자동으로 아래를 실행한 것과 같다
deco(Foo); // target = Foo 자동 주입
```

- 핵심 확인: 인스턴스를 만들지 않아도 로그가 찍힌다. **데코레이터는 "클래스 정의 시점"에 실행**된다.

## 괄호 `()`의 유무 = 커링 여부

- **괄호 없음** → 함수 자체가 데코레이터. 컴파일러가 곧바로 `deco(target)` 호출.
- **괄호 있음** → "데코레이터 팩토리". 내가 먼저 호출해서 데코레이터를 만들고, 컴파일러가 거기에 target을 넣는다. = **커링**.

```ts
function Injectable(options: any) {   // (1) 내가 괄호로 먼저 호출
  return function (target: any) {     // (2) 이게 진짜 데코레이터
    console.log(target.name, options);
  };
}

@Injectable({ scope: 'request' })
class Foo {}

// 컴파일러가 자동으로:
Injectable({ scope: 'request' })(Foo);
//        └─ 내가 호출 ─┘└ 컴파일러가 호출(target 자동) ┘
```

- 그래서 NestJS의 `@Get('/users')`, `@Inject('TOKEN')`, `@Injectable()`은 **전부 커링 구조**다.
  - `Get('/users')`를 먼저 호출 → 데코레이터 반환 → 컴파일러가 target 주입.

## 데코레이터에는 두 종류가 있다 (이게 혼란의 근원)

| | 레거시(실험적) 데코레이터 | 표준(Stage 3) 데코레이터 |
|---|---|---|
| 활성화 | `"experimentalDecorators": true` | TS 5.0+ 기본 (플래그 없으면 이쪽) |
| 기반 | 옛 TC39 제안 (stage 2) | 확정된 TC39 stage 3 (JS 표준) |
| 시그니처 | `(target, key, descriptor)` | `(value, context)` |
| 동작 방식 | descriptor를 **수정(mutate)** | 새 값을 **반환 → 원본 교체** |
| 파라미터 데코레이터 | ✅ 지원 | ❌ 미지원 |
| 타입 메타데이터 자동 방출 | ✅ (`emitDecoratorMetadata`) | ❌ |
| 쓰는 곳 | **NestJS, TypeORM, Angular, class-validator** | 신규 라이브러리 일부 |

- 둘은 **함수 모양 자체가 달라 호환되지 않는다.**
- TS 5.0부터 `experimentalDecorators` 플래그가 없으면 표준 방식으로 컴파일 → NestJS 코드와 **충돌**.

## 레거시 데코레이터 (NestJS가 쓰는 방식)

- 컴파일러가 `target, 메서드이름, descriptor`를 넣어주고, descriptor를 직접 주물러서 동작을 바꾼다.

```ts
function Log(target: any, key: string, desc: PropertyDescriptor) {
  const original = desc.value;
  desc.value = function (...args: any[]) {
    console.log(`→ ${key}`);
    const result = original.apply(this, args);
    console.log(`← ${key} =`, result);
    return result;
  };
  return desc;
}

class Calc {
  @Log
  add(a: number, b: number) { return a + b; }
}
```

- 로깅, 실행시간 측정, 트랜잭션 래핑, 캐싱, 재시도 → 전부 이 "메서드 감싸기" 패턴(AOP).
- `original.apply(this, args)`를 빼면 원본 메서드가 호출되지 않는다.

## 표준 Stage 3 데코레이터의 `(value, context)`

- 컴파일러가 **원본(value)을 그냥 건네주고**, 내가 **반환한 값으로 원본을 갈아끼운다.** descriptor를 안 만짐 → 더 함수형.

```ts
// value = 원본 메서드 함수, context = { kind, name, addInitializer, ... }
function Log(original: any, context: ClassMethodDecoratorContext) {
  return function (this: any, ...args: any[]) {  // 이 반환 함수가 원본을 대체
    console.log(`call ${String(context.name)}`);
    return original.apply(this, args);
  };
}

class Calc {
  @Log
  add(a: number, b: number) { return a + b; }
}

// 컴파일러가 자동으로 하는 일(개념):
// Calc.prototype.add = Log(Calc.prototype.add, { kind: 'method', name: 'add', ... });
```

- 대상마다 `value`가 다르다:
  - 클래스 → `value = 클래스 자신`, 반환 = 새 클래스
  - 메서드 → `value = 메서드 함수`, 반환 = 새 함수
  - accessor → `value = { get, set }`
  - **필드 → `value = undefined`**, 반환 = 초기화 함수 `(초기값) => 새값` (특이점)

### 핵심 차이 한 줄

| | 컴파일러가 넣는 것 | 내가 하는 것 |
|---|---|---|
| 레거시 | `(target, key, descriptor)` | descriptor를 **수정** |
| stage 3 | `(value, context)` | 새 값을 **반환 → 교체** |

## reflect-metadata: "선언은 데코레이터, 동작은 런타임"

- NestJS의 진짜 핵심 패턴. **데코레이터는 메타데이터만 심고, 런타임이 그걸 읽어 동작을 조립**한다.

```ts
import 'reflect-metadata';

const PARAM_META = Symbol('params');

// NestJS createParamDecorator의 발상을 직접 구현
function createParamDecorator(factory: (ctx: any) => any) {
  return (): ParameterDecorator => (target, key, index) => {
    const factories = Reflect.getMetadata(PARAM_META, target, key!) ?? [];
    factories[index] = factory;                 // "N번째 인자는 이 추출기로"
    Reflect.defineMetadata(PARAM_META, factories, target, key!);
  };
}

const CurrentUser = createParamDecorator((ctx) => ctx.request.user);
const Body        = createParamDecorator((ctx) => ctx.request.body);

// "위빙" 담당: 호출 시점에 메타데이터를 읽어 인자를 조립
function Handler(target: any, key: string, desc: PropertyDescriptor) {
  const original = desc.value;
  desc.value = function (ctx: any) {
    const factories = Reflect.getMetadata(PARAM_META, target, key) ?? [];
    const args = factories.map((f: any) => (f ? f(ctx) : undefined));
    return original.apply(this, args);          // 진짜 인자로 펼쳐서 호출
  };
  return desc;
}

class UserController {
  @Handler
  findMe(@CurrentUser() user: any, @Body() body: any) {
    return { user, body };
  }
}
```

- 호출은 `findMe(ctx)` 한 덩어리인데 선언은 `(user, body)`로 보이는 마법의 정체:
  - 데코레이터(`@CurrentUser()`)는 **"몇 번째 인자에 무슨 추출기"라는 메타데이터만** 저장
  - 실제 값 주입은 **`@Handler`(=런타임)가 호출 시점에** 수행
- NestJS는 `@Handler` 역할을 **라우터/실행 컨텍스트**가 대신 해주는 것뿐이다.

### 타입 기반 DI: `design:paramtypes`

- `emitDecoratorMetadata: true`가 생성자 파라미터의 **타입 정보를 런타임 메타데이터로 심어준다.** NestJS DI의 심장.

```ts
function Injectable(): ClassDecorator { return () => {}; }

@Injectable()
class Database { query() { return 'data'; } }

@Injectable()
class UserService {
  constructor(private db: Database) {} // 타입만 적었는데
}

function resolve<T>(cls: new (...a: any[]) => T): T {
  const deps: any[] = Reflect.getMetadata('design:paramtypes', cls) ?? [];
  return new cls(...deps.map((d) => resolve(d)));
}

resolve(UserService); // Database를 알아서 만들어 주입
```

- `Reflect.getMetadata('design:paramtypes', cls)`가 `[Database]`를 돌려준다.
- **표준(stage 3) 데코레이터에는 이 자동 타입 방출이 없다** → NestJS가 표준으로 못 가는 결정적 이유.

## NestJS가 레거시 데코레이터에 묶이는 이유

1. **`emitDecoratorMetadata` 의존** — 생성자 타입 기반 DI. 표준에는 자동 타입 메타데이터가 없음.
2. **파라미터 데코레이터 의존** — `@Param`, `@Body`, `@Query`, `@Inject`, `@CurrentUser` 전부 파라미터 데코레이터인데, 표준은 파라미터를 **문법적으로** 지원하지 않음.
3. **순환 의존 + DI 안정성** — CJS(`require`, 동기·동적) 위에서 메타데이터 등록이 안정적.

→ 그래서 회사 tsconfig는 `experimentalDecorators: true` + `emitDecoratorMetadata: true` + `module: commonjs` 한 세트.
→ `import` 문법으로 작성해도 컴파일 후엔 `require`로 바뀐다(작성=표준 표기, 실행=안정적 CJS).

## 표준(Stage 3)에서 `@CurrentUser` 기능을 흉내내려면

- 파라미터 데코레이터가 없으니 **"파라미터에 주입"을 포기**하고 우회한다.

```ts
// 패턴 A: 매핑을 메서드 데코레이터 인자로 끌어올림 (순서로 연결)
type Extractor = (ctx: any) => any;
function Inject(extractors: Extractor[]) {
  return function (original: any, _ctx: ClassMethodDecoratorContext) {
    return function (this: any, ctx: any) {
      return original.apply(this, extractors.map((fn) => fn(ctx)));
    };
  };
}
const CurrentUser: Extractor = (ctx) => ctx.request.user;

class Ctrl {
  @Inject([CurrentUser])     // 파라미터 대신 여기에 선언
  findMe(user: any) { return user; }
}
```

```ts
// 패턴 B: 주입 자체를 버리고 AsyncLocalStorage + 함수 호출로 (현대적)
import { AsyncLocalStorage } from 'node:async_hooks';
const als = new AsyncLocalStorage<{ user: any }>();
const currentUser = () => als.getStore()?.user;

class Ctrl {
  findMe() { return currentUser(); } // 데코레이터 없이 그냥 호출
}
```

## 스스로 학습하는 법 (사다리)

> 글로 읽으면 안 잡힌다. **반드시 직접 타이핑해서 실행**할 것. 복붙 금지.

0. `@deco`와 `deco(Foo)`가 같음을 증명 (인스턴스 안 만들어도 로그 찍힘 확인)
1. 데코레이터 없이 손으로 `deco(Foo)` 호출 — `@`는 문법 설탕
2. 메서드 감싸기 `@Log` 직접 작성 (`apply` 빼보기)
3. 인자 받는 데코레이터 `@Log('DEBUG')` — 함수를 반환하는 함수(커링)
4. reflect-metadata로 "메타데이터만 심기" → 별도 함수가 읽어서 동작
5. 미니 `createParamDecorator` 재현

### 스스로 답해볼 질문

1. 데코레이터는 **언제** 실행되나? (인스턴스 생성 시 X, 클래스 정의 시 O)
2. `@deco`와 `deco(Target)`은 정말 같은가?
3. 메서드 데코레이터가 `return`한 값은 어디로 가나?
4. `@Log('x')`에서 `('x')`는 누가 받나?
5. reflect-metadata는 왜 필요한가? 없으면 뭘 못 하나?

## 한 줄 요약

- **`@` = 컴파일러가 그 함수를 호출하며 대상을 자동 주입.** 괄호가 있으면 커링(팩토리).
- **레거시**: `(target, key, descriptor)` 받아 descriptor 수정. 파라미터 데코레이터·타입 메타데이터 O → NestJS가 의존.
- **stage 3**: `(value, context)` 받아 새 값 반환·교체. 파라미터·자동 타입 메타데이터 X → NestJS가 못 옮김.
- NestJS의 마법 = "데코레이터로 메타데이터 심고, 런타임이 읽어 동작 조립".
