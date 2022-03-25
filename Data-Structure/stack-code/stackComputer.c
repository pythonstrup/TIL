#define _CRT_SECURE_NO_WARNINGS // scanf 오류 방지
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STACK_SIZE 100

// 피연산자와 연산자를 문자열 형태로 받을 것
typedef char element;
#define MAX_STACK_SIZE 100  // 스택 최대 사이즈

// 스택 형태
typedef struct {
	element data[MAX_STACK_SIZE];
	int top;
} StackType;

// 스택 초기화 함수
void init_stack(StackType* s) {
	s->top = -1;
}

// 공백 상태 검출 함수
int is_empty(StackType* s) {
	return (s->top == -1);
}

// 포화 상태 검출 함수
int is_full(StackType* s) {
	return (s->top == (MAX_STACK_SIZE - 1));
}

// 삽입 함수
void push(StackType* s, element item) {
	if (is_full(s)) {
		fprintf(stderr, "스택 포화 에러\n");
		return;
	}
	else s->data[++(s->top)] = item;
}

// 삭제 함수
element pop(StackType* s) {
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[(s->top)--];
}

// 피크 함수
element peek(StackType* s) {
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[s->top];
}

// 괄호 검사
int check_matching(const char* in)
{
	StackType s;
	char ch, open_ch;
	int i, n = strlen(in);  	// n= 문자열의 길이
	init_stack(&s);			// 스택의 초기화

	for (i = 0; i < n; i++) {
		ch = in[i];		// ch = 다음 문자
		switch (ch) {
		case '(':   case '[':    case '{':
			push(&s, ch);
			break;
		case ')':   case ']':    case '}':
			if (is_empty(&s))  return 0;
			else {
				open_ch = pop(&s);
				if ((open_ch == '(' && ch != ')') ||
					(open_ch == '[' && ch != ']') ||
					(open_ch == '{' && ch != '}')) {
					return 0;
				}
				break;
			}
		}
	}
	if (!is_empty(&s)) return 0; // 스택에 남아있으면 오류
	return 1;
}

// 연산자의 우선순위를 반환하는 함수
int prec(char op)
{
	switch (op) {
	case '(': case ')': return 0;
	case '+': case '-': return 1;
	case '*': case '/': return 2;
	}
	return -1;
}

// 중위표기식을 후위표기식으로 바꾸는 함수
void infix_to_postfix(char exp[], char postfix[])
{
	int i = 0, idx = 0;
	char ch, top_op;
	int len = strlen(exp);
	StackType s;
	init_stack(&s);		// 스택 초기화 

	for (i = 0; i < len; i++) {
		ch = exp[i];
		switch (ch) {
		case '+': case '-': case '*': case '/': // 연산자

			// 우선순위를 반환해주는 prec(char op)함수를 사용
			// 스택에 있는 연산자의 우선순위가 더 크거나 같으면 출력
			while (!is_empty(&s) && (prec(ch) <= prec(peek(&s)))) 
				postfix[idx++] = pop(&s);

			push(&s, ch);
			break;
		case '(':	// 왼쪽 괄호
			push(&s, ch);
			break;
		case ')':	// 오른쪽 괄호
			top_op = pop(&s);
			// 왼쪽 괄호를 만날때까지 출력
			while (top_op != '(') {
				postfix[idx++] = top_op;
				top_op = pop(&s);
			}
			break;
		default:		// 피연산자
			postfix[idx++] = ch;
			break;
		}
	}
	while (!is_empty(&s))	// 스택에 저장된 연산자들 출력
		postfix[idx++] = pop(&s);

}

// 후위 표기 수식 계산 함수
int eval(char exp[])
{
	int op1, op2, value, i = 0;
	int len = strlen(exp);
	char ch;
	StackType s;

	init_stack(&s);
	for (i = 0; i < len; i++) {
		ch = exp[i];
		if (ch != '+' && ch != '-' && ch != '*' && ch != '/') {
			value = ch - '0';	// 입력이 피연산자이면 아스키코드를 이용해 정수로 바꿔준다.
			push(&s, value);
		}
		else {	//연산자이면 피연산자를 스택에서 제거
			op2 = pop(&s);
			op1 = pop(&s);
			switch (ch) { //연산을 수행하고 스택에 저장 
			case '+': push(&s, op1 + op2); break;
			case '-': push(&s, op1 - op2); break;
			case '*': push(&s, op1 * op2); break;
			case '/': push(&s, op1 / op2); break;
			}
		}
	}
	return pop(&s);
}

int main(void) {
	StackType com; // 후위표기식 계산 스택
	char postfix[MAX_STACK_SIZE] = { '\0' }; // 후위 표기식 문자열
	int result; // 연산 결과를 저장하는 변수

	scanf("%s", com.data);

	// 괄호 체크
	if (check_matching(com.data)) {
		printf("괄호검사성공\n");
		
		infix_to_postfix(com.data, postfix);
		printf("후위표기식: %s\n", postfix);

		result = eval(postfix);
		printf("%d\n", result);
	}
	else {
		printf("괄호검사실패\n");
	}
}