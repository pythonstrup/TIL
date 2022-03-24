#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100
typedef int element;

typedef struct {
	element data[MAX_SIZE];
	int top;
} Stack;

void init(Stack *s) {
	s->top = -1;
}

int is_empty(Stack* s) {
	return (s->top == -1);
}

int is_full(Stack* s) {
	return (s->top == (MAX_SIZE - 1));
}

void push(Stack* s, element item) {
	if (is_full(s)) {
		fprintf(stderr, "스택 포화 에러\n");
		return;
	}
	else s->data[++(s->top)] = item;
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
	return 0;
}