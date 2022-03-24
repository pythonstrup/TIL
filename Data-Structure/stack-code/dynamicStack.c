#include <stdio.h>
#include <stdlib.h>

typedef int element;

typedef struct {
	element *data;
	int capacity;  // 현재 스택 크기
	int top;
} Stack;

void init(Stack* s) {
	s->top = -1;
	s->capacity = 1;
	s->data = (element*)malloc(s->capacity * sizeof(element));
}

int is_empty(Stack* s) {
	return (s->top == -1);
}

int is_full(Stack* s) {
	return (s->top == (s->capacity - 1));
}

void push(Stack* s, element item) {
	if (is_full(s)) {
		s->capacity *= 2;
		s->data = (element*)realloc(s->data, s->capacity * sizeof(element));
	}
	s->data[++(s->top)] = item;
}

element pop(Stack* s) {
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[(s->top)--];
}

element peek(Stack* s) {
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[s->top];
}

int main(void) {
	Stack s;
	init(&s);

	push(&s, 1);
	push(&s, 2);
	push(&s, 3);
	printf("%d\n", pop(&s));
	printf("%d\n", pop(&s));
	printf("%d\n", pop(&s));
	free(s.data);

	return 0;
}