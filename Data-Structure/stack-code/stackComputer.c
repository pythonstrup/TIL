#define _CRT_SECURE_NO_WARNINGS // scanf ���� ����
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STACK_SIZE 100

// �ǿ����ڿ� �����ڸ� ���ڿ� ���·� ���� ��
typedef char element;
#define MAX_STACK_SIZE 100  // ���� �ִ� ������

// ���� ����
typedef struct {
	element data[MAX_STACK_SIZE];
	int top;
} StackType;

// ���� �ʱ�ȭ �Լ�
void init_stack(StackType* s) {
	s->top = -1;
}

// ���� ���� ���� �Լ�
int is_empty(StackType* s) {
	return (s->top == -1);
}

// ��ȭ ���� ���� �Լ�
int is_full(StackType* s) {
	return (s->top == (MAX_STACK_SIZE - 1));
}

// ���� �Լ�
void push(StackType* s, element item) {
	if (is_full(s)) {
		fprintf(stderr, "���� ��ȭ ����\n");
		return;
	}
	else s->data[++(s->top)] = item;
}

// ���� �Լ�
element pop(StackType* s) {
	if (is_empty(s)) {
		fprintf(stderr, "���� ���� ����\n");
		exit(1);
	}
	else return s->data[(s->top)--];
}

// ��ũ �Լ�
element peek(StackType* s) {
	if (is_empty(s)) {
		fprintf(stderr, "���� ���� ����\n");
		exit(1);
	}
	else return s->data[s->top];
}

// ��ȣ �˻�
int check_matching(const char* in)
{
	StackType s;
	char ch, open_ch;
	int i, n = strlen(in);  	// n= ���ڿ��� ����
	init_stack(&s);			// ������ �ʱ�ȭ

	for (i = 0; i < n; i++) {
		ch = in[i];		// ch = ���� ����
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
	if (!is_empty(&s)) return 0; // ���ÿ� ���������� ����
	return 1;
}

// �������� �켱������ ��ȯ�ϴ� �Լ�
int prec(char op)
{
	switch (op) {
	case '(': case ')': return 0;
	case '+': case '-': return 1;
	case '*': case '/': return 2;
	}
	return -1;
}

// ����ǥ����� ����ǥ������� �ٲٴ� �Լ�
void infix_to_postfix(char exp[], char postfix[])
{
	int i = 0, idx = 0;
	char ch, top_op;
	int len = strlen(exp);
	StackType s;
	init_stack(&s);		// ���� �ʱ�ȭ 

	for (i = 0; i < len; i++) {
		ch = exp[i];
		switch (ch) {
		case '+': case '-': case '*': case '/': // ������

			// �켱������ ��ȯ���ִ� prec(char op)�Լ��� ���
			// ���ÿ� �ִ� �������� �켱������ �� ũ�ų� ������ ���
			while (!is_empty(&s) && (prec(ch) <= prec(peek(&s)))) 
				postfix[idx++] = pop(&s);

			push(&s, ch);
			break;
		case '(':	// ���� ��ȣ
			push(&s, ch);
			break;
		case ')':	// ������ ��ȣ
			top_op = pop(&s);
			// ���� ��ȣ�� ���������� ���
			while (top_op != '(') {
				postfix[idx++] = top_op;
				top_op = pop(&s);
			}
			break;
		default:		// �ǿ�����
			postfix[idx++] = ch;
			break;
		}
	}
	while (!is_empty(&s))	// ���ÿ� ����� �����ڵ� ���
		postfix[idx++] = pop(&s);

}

// ���� ǥ�� ���� ��� �Լ�
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
			value = ch - '0';	// �Է��� �ǿ������̸� �ƽ�Ű�ڵ带 �̿��� ������ �ٲ��ش�.
			push(&s, value);
		}
		else {	//�������̸� �ǿ����ڸ� ���ÿ��� ����
			op2 = pop(&s);
			op1 = pop(&s);
			switch (ch) { //������ �����ϰ� ���ÿ� ���� 
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
	StackType com; // ����ǥ��� ��� ����
	char postfix[MAX_STACK_SIZE] = { '\0' }; // ���� ǥ��� ���ڿ�
	int result; // ���� ����� �����ϴ� ����

	scanf("%s", com.data);

	// ��ȣ üũ
	if (check_matching(com.data)) {
		printf("��ȣ�˻缺��\n");
		
		infix_to_postfix(com.data, postfix);
		printf("����ǥ���: %s\n", postfix);

		result = eval(postfix);
		printf("%d\n", result);
	}
	else {
		printf("��ȣ�˻����\n");
	}
}