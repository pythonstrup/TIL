# Zod Schemas 정의

## Objects

- 아래와 같은 문법은 이제 지양하는 것이 좋으며

```typescript
const UserLooseSchema = z.object({
    name: z.string(),
}).loose();
const UserStripSchema = z.object({
    name: z.string(),
}).strip();
const UserStrictSchema = z.object({
    name: z.string(),
}).strict();
```

- 아래와 같이 사용하는 것이 더 좋은 문법이다.

```typescript
const UserLooseSchema = z.looseObject({
    name: z.string(),
});
const UserStripSchema = z.object({
    name: z.string(),
});
const UserStrictSchema = z.strictObject({
    name: z.string(),
});
```
