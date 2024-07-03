#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 최대 size 지정
#define MAX_SIZE 200

// enum형태로 element의 속성을 attribute로 나타냄.
// LP는 Left parentheses
// RP는 Right parentheses를 의미함.
enum {NUM, ADD, SUB, MUL, DIV, LP, RP};

// **** 0. 스택 설정 ****

typedef struct element {
	char value[MAX_SIZE]; // 문자열을 담을 배열
	int attribute; // 문자열의 속성(enum을 저장할 것임)
} element;

typedef struct StackNode { 
	element data; // 문자열과 속성을 담을 element
	struct StackNode* link; // 스택자기참조
} StackNode; // 스택노드


/*
	함수명: error
	인자: char*
	리턴형: 없음(void)
	설명: 오류를 발생시키는 함수
*/
void error(char* message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}

// **** 1. 우선순위 ****

/*
	함수명: prec
	인자: int
	리턴형: int
	설명: element의 담긴 속성에 따라 우선순위를 리턴
		  우선 순위가 높을수록 높은 숫자 리턴
*/
int prec(int attribute)
{
	switch (attribute) {
	case LP: case RP: return 4; // 괄호
	case MUL: case DIV: return 3; // 곱하기 나눗셈
	case ADD: case SUB: return 2; // 더하기 빼기
	case NUM: return 1; // 피연산자
	default: return -1;
	}
}


// **** 2. 스택 기본 함수 가지 ****
// pop, push, peek, isEmpty, isFull

/*
	함수명: is_empty
	인자: StackNode
	리턴형: int
	설명: 공백 상태 검출 함수
*/
int is_empty(StackNode* top) {
	return (top == NULL); // 스택의 꼭대기가 NULL이면 1을 리턴
}

/*
	함수명: is_full
	인자: StackNode
	리턴형: int
	설명: 포화 상태 검출 함수
		  힙의 공간이 허용하는 한 노드를 계속 만들 수 있음.
*/
int is_full(StackNode* top) {
	return 0; // 계속해서 노드를 만들 수 있기 때문에 0을 반환
}

/*
	함수명: push
	인자: StackNode**, element
	리턴형: void
	설명: 스택 삽입 함수
*/
void push(StackNode** top, element item) {
	// 스택이 가득찼을 때 에러 발생
	if (is_full(*top)) {
		error("스택이 가득찼습니다.");
	}

	// 삽입할 스택을 임시로 만듬
	StackNode *temp = (StackNode*)malloc(sizeof(StackNode));
	// item의 내용을 문자열로 초기화시켜줌(오류방지)
	item.value[MAX_SIZE - 1] = '\0';

	// item의 값과 속성을 복사붙여넣기
	strcpy(temp->data.value, item.value);  // 값
	temp->data.attribute = item.attribute; // 속성

	// 생성한 노드 연결
	temp->link = *top;
	*top = temp;
}


/*
	함수명: pop
	인자: StackNode**, element*
	리턴형: int
	설명: 스택 삭제 함수
*/
// 표기식 변환을 위해 element를 반환하는 것이 아닌 int형 반환
// 값을 꺼냄과 동시에 우선순위나 조건에 맞는지 비교하기위해 그렇게 설계함
// 대신 item 매개변수를 통해 값을 가져옴
int pop(StackNode** top, element *item) {

	// 스택이 만약 비었다면
	if (is_empty(*top)) {
		// 0을 반환해 스택이 비었음을 표시
		return 0;
	}

	StackNode* temp = *top;
	// 만약 item이 NULL이라면?
	// item에 저장이 불가능하므로 값만 삭제해준다.
	if (item == NULL) {
		*top = temp->link;
		free(temp);
	}
	else {
		// item에 값을 저장해주고 임시노드에 저장된 노드를 삭제처리
		strcpy(item->value, temp->data.value);	// 값 저장
		item->attribute = temp->data.attribute;	// 속성 저장
		*top = temp->link;	// 스택의 꼭대기를 다음 노드로 옮겨줌
		free(temp);	// 메모리 해제
	}

	// 프로그램이 정상적으로 실행되면 1을 반환
	return 1;
}


/*
	함수명: peek
	인자: StackNode*, element*
	리턴형: int
	설명: 스택 top 값 반환 함수
*/
// pop과 마찬가지로 표기식 변환을 위해 element를 반환하는 것이 아닌 int형 반환
// 대신 item 매개변수를 통해 값을 가져옴
int peek(StackNode* top, element *item) {
	if (is_empty(top))
		return 0; // 0을 반환해 스택이 비었음을 표시
	else { // item에 값을 저장하기
		strcpy(item->value, top->data.value); // 값 저장
		item->attribute = top->data.attribute; // 속성 저장
	}
	// 프로그램이 정상적으로 실행되면 1을 반환
	return 1;
}

// **** 3. 링크드리스트 관련 메소드  ****

/*
	함수명: convert_to_stack
	인자: StackNode** ,char*
	리턴형: void
	설명: 문자열 -> 링크드리스트로 변환
*/
// 표현식에 문제가 있는지 검사해줌.
// 정확한 값 저장을 위해 스택의 이중포인터를 사용, 이중포인터를 사용하지 않으면 값저장이 안됨.
void convert_to_stack(StackNode** s, char* exp) {
	int i = -1; // while문에서 사용할 index값
	element item; // push할 때 사용할 element값 저장
	item.attribute = -1;  // item 속성 초기화
	int is_blank = 0; // 공백이 있는지 검사
	int is_point = 0; // 소수점 처리를 위해 점이 있는지 여부 저장
	char buf[MAX_SIZE]; // element의 값을 저장하기 위해 사용할 버퍼
	int j = 0; //  버퍼에 사용할 인덱스

	// exp 문자열이 끝날 때까지 반복
	while (exp[++i] != '\0') {
		// 공백 처리
		if (exp[i] == ' ' || exp[i] == '\t') {
			is_blank = 1; // 공백이 있기 때문에 1(true)값 저장
			continue; // while문을 처음부터 다시 시작
		}

		// 숫자 및 소수점 처리
		if ('0' <= exp[i] && exp[i] <= '9' || (exp[i] == '.' && (is_point = 1))) {
			// 숫자와 숫자 사이에 공백 표현이 있을 때의 예외처리
			if (is_blank == 1 && item.attribute == NUM) {
				// 커스텀 error 함수 사용
				error("숫자 사이에 공백이 있습니다 - 불가능한 표현식입니다\n");
			}
			buf[j++] = exp[i]; // 버퍼에 숫자 저장(소수점을 저장하기 위해 j를 증가시킴)
			item.attribute = NUM; // 숫자임을 판별하기 위해 NUM(enum)을 속성에 저장
		}
		// 숫자가 아니라면? -> 연산자 처리
		else {
			// 연산자 처리를 하는데 버퍼에 숫자가 이미 있다면 링크드리스트로 숫자를 푸시
			if (item.attribute == NUM && (exp[i] < '0' || '9' < exp[i])) {
				buf[j++] = '\0'; // 숫자를 입력이 끝났음을 표시하기 위해 \0을 버퍼에 넣음
				strcpy(item.value, buf); // item에 값을 저장
				push(s, item); // 값과 속성이 저장된 item을 스택에 저장함.
				is_point = 0; // 소수점 여부 0으로 바꿔줌
				j = 0; // 버퍼 인덱스를 초기화해줌
			}
			// 연산자일 때, attribute를 enum으로 처리해 식별이 쉽도록 해줌
			switch (exp[i]) { // switch문을 이용해 item에 속성을 저장해줌
			case '+': item.attribute = ADD; break; // 더하기
			case '-': item.attribute = SUB; break; // 빼기
			case '*': item.attribute = MUL; break; // 곱하기
			case '/': item.attribute = DIV; break; // 나누기 - 사칙연산 완료
			case '(': item.attribute = LP; break; // 왼쪽 괄호
			case ')': item.attribute = RP; break; // 오른쪽 괄호 - 괄호 완료
			default:
				// 커스텀 error 함수 사용
				// 만약 연산자나 숫자가 아닌 다른 문자가 입력되면 에러발생시킴
				error("잘못된 문자가 입력되었습니다.\n");
			}

			// 연산자는 숫자와 달리 무조건 한 자리이다.
			// 인덱스 0은 연산을 저장하고 인덱스 1은 문자열 처리를 위해 '\0'을 넣어준다.
			buf[0] = exp[i]; // 연산자 저장
			buf[1] = '\0'; // 문자열의 끝 의미
			strcpy(item.value, buf); // item에 저장
			push(s, item); // item을 스택에 푸시
		}
		is_blank = 0; // 공백 여부 초기화
	}

	// 마지막 입력값인 숫자를 처리해주기 위한 분기문
	if (item.attribute == NUM) {
		buf[j++] = '\0'; // 문자열 마무리
		strcpy(item.value, buf); // item에 저장
		push(s, item); // item을 스택에 푸시
	}
}

/*
	함수명: print_list
	인자: StackNode*
	리턴형: int
	설명: 링크드리스트 출력
*/
// 반복문을 통해 순서대로 출력하면 스택순서대로 출력하기 때문에 
// 재귀함수를 통해 순서를 바꿔 출력해줄 것임
void print_list(StackNode *s) {
	if (s == NULL) {
		return; // 재귀 함수의 끝내기 위한 신호
	}
	print_list(s->link); // 스택을 거꾸로 보여주기 위해서 재귀를 타고 들어감.
	printf("%s ", s->data.value); // 연산자, 피연산자 출력
	return;
}

// **** 4. 표기식 변환 ****

/*
	함수명: infix_to_postfix
	인자: StackNode**, StackNode**
	리턴형: void
	설명: 중위표기식 -> 후위표기식
*/
void infix_to_postfix(StackNode** s, StackNode** postfix) {
	
	// 후위표기식 변환을 위한 정적 저장공간
	// 재귀함수를 사용할 것이기 때문에 정적공간이 필요함
	static StackNode* storage; // 우선순위에 따라 처리하기 위한 연산자 스택
	static int end = 0; // 재귀함수가 끝났음을 보여줄 수 있는 변수
	element item; // push를 위한 item 저장공간

	// 만약 스택이 비었다면 함수를 끝낸다.
	if (is_empty(*s)) {
		end--;
		return;
	}
	end++; // end에 1을 더해줌
	// 재귀 함수
	// 값을 순서대로 꺼내기 위해 재귀를 이용해 거꾸로 타고 들어감
	infix_to_postfix(&((*s)->link), postfix);

	// 값의 속성에 따른 푸시 연산
	switch ((*s)->data.attribute){
	case NUM:  // 숫자일 경우
		// 현재 재귀함수로 타고 들어왔기 때문에 스택 꼭대기의 data를 그대로 넣어주면 됨.
		// 피연산자를 만나면 연산자스택을 거치지 않고 postfix에 바로 넣어줌
		push(postfix, (*s)->data); 
		break;
	case ADD: case SUB: case MUL: case DIV: // 연산자일 경우
		// item의 우선순위가 같거나 높을 때 변화가 발생함
		// 만약 연산자 스택이 비어있지 않다면 반복문을 돌릴 것임.
		if (peek(storage, &item) && (prec(item.attribute) >= prec((*s)->data.attribute))) {
			// 임시공간에 넣어둔 피연산자와 연산자를 모두 꺼낼 때가지 반복
			// (단, 그 값이 왼쪽 괄호이면 반복문을 중단시킴)
			while (peek(storage, &item) && (item.attribute != LP)) { 
				pop(&storage, &item); // 연산자 스택의 연산을 삭제해서
				push(postfix, item);  // postfix에 넣어줌
			}
		}
		push(&storage, (*s)->data); // 현재 값을 연산자 스택에 넣어줌
		break;
	case LP:
		push(&storage, (*s)->data); // 왼쪽 괄호를 만나면 연산자스택에 넣어줌
		break;
	case RP: // 오른쪽 괄호를 만나면 연산자 스택의 값을 제거하기 위한 작업 시작
		// 단, 왼쪽 괄호를 만나거나 연산자 스택이 비면 반복문을 빠져나옴.
		while (peek(storage, &item) && (item.attribute != LP || (!pop(&storage, NULL)))) {
			pop(&storage, &item); // 연산자 스택의 값을 꺼내서
			push(postfix, item); // postfix 스택에 넣어줌
		}
		break;
	default: // 연산자, 피연산자가 아닌 값은 에러를 발생시킴.
		error("잘못된 값이 입력되었습니다.");
	}
	// 만약 end값이 0이 되면 더이상 push할 값이 없는 것이기 때문에
	// 정적공간에 남아있는 값들을 postfix에 넣어줌
	if (end < 1) {
		while (peek(storage, &item)) {
			pop(&storage, &item); // 임시보관되어있는 정보를 꺼내
			push(postfix, item); // postfix에 push함
		}
	}
	// 재귀들 돌면서 함수가 끝날 때마다 end를 빼주면
	// 재귀가 끝나고 마지막 함수에서 end값이 0이 되게 만들 수 있음
	end--;
}

/*
	함수명: infix_to_prefix
	인자: StackNode**, StackNode**
	리턴형: void
	설명: 중위표기식->전위표기식
*/

/*  [전위표기식의 알고리즘]
	1. 중위표기식을 꺼내 역순으로 뒤집어준다.
	2. 숫자는 다른 곳에 임시로 저장하고, 연산자만 연산자 스택에 넣어준다.
	3. 닫는 괄호 ')'는 그냥 스택에 넣어준다. (스택을 뒤집었기 때문에 닫는 괄호부터 나옴)
	4. 여는 괄호 '('를 만나면 모든 연산자를 pop해 스택에 넣어준다.
	5. 모든 과정이 끝나면 다시 역순으로 뒤집어준다.
*/
void infix_to_prefix(StackNode** s, StackNode** prefix) {
	
	// 재귀함수를 사용할 것이기 때문에 정적공간이 필요함
	int i = 0; // 인덱스
	static StackNode* storage; // 연산자 스택
	static StackNode* temp; // 식을 역순으로 뒤집어주기 위한 스택
	element item; // prefix에 값을 넣기 위한 item 저장소

	// 재귀의 끝에 도달했을 때 역순으로 뒤집어주는 과정
	if (is_empty(*s)) {
		while (peek(*prefix, &item)) { // prefix가 끝날 때까지 돌린다.
			pop(prefix, &item); // prefix의 값을 꺼내
			push(&temp, item); // temp에 임시 저장한다.
		}
		while (peek(storage, &item)) {
			pop(&storage, &item); // 연산자 스택의 값을 꺼내
			push(&temp, item);  // temp에 저장한다.
		}
		// **중요** 역순으로 뒤집어줌
		while (peek(temp, &item)) {
			pop(&temp, &item); // temp의 값을 꺼내
			push(prefix, item); // prefix에 넣어주는데 이 과정을 통해 역순으로 뒤집힌다.
		}

		return; // 함수 종료
	}

	// 값의 속성에 따라 피연산자와 연산자를 나눔
	switch ((*s)->data.attribute){
		// 전위표기식은 피연산자는 뒤로 들어가기 때문에 아래의 과정을 거친다.
		// (피연산자가 스택의 밑바닥에 깔려야함.)
		// 1. pre에서 temp로 다뺀다
		// 2. 연산자 넣는다
		// 3. pre를 다시 temp에서 넣는다
	case NUM: // 숫자일 때
		while (peek(*prefix, &item)) {
			pop(prefix, &item); // prefix에서 값을 꺼내
			push(&temp, item); // temp에 넣어준다.
		}
		push(prefix, (*s)->data); // 텅빈 prefix에 지금 숫자를 넣어준다.

		while (peek(temp, &item)) { // temp가 빌 때까지 반복함
			pop(&temp, &item); // temp의 값을 꺼내
			push(prefix, item); // prefix에 넣어줌
		}
		break;

	case ADD: case SUB: case MUL: case DIV: // 연산자일 때
		while (peek(*prefix, &item)) { // preifx가 빌 때까지 돌림
			// 현재 연산자 값이 아래쪽에 깔려야하기 때문에 prefix의 내용을 잠시만 temp에 옮겨줌
			pop(prefix, &item); // prefix에서 값을 제거해
			push(&temp, item);  // temp로 옮겨줌
		}
		// 후위표기식으로 바꿀 때와 약간 다름
		// storage에 내용물이 있고, item의 우선순위가 스택의 우선순위보다 높으며, item의 ')'이 아니라면
		while (peek(storage, &item) && (prec(item.attribute) > prec((*s)->data.attribute) && (item.attribute != RP))) {
			pop(&storage, &item); // 연산자 스택에 저장된 내용을 제거해
			push(prefix, item);   // prefix 스택에 넣어줌
		}
		push(&storage, (*s)->data); // 연산자 스택에 현재 값을 넣어줌
		// temp가 비어있지 않고, 오른쪽 괄호를 만나기 전까지 반복문을 돌려줌
		while (peek(temp, &item) && (item.attribute != RP)) {
			pop(&temp, &item);  // temp의 값을 꺼내
			push(prefix, item); // 다시 prefix로 원상복구 시켜줌
		}
		break;
	case RP: // 오른쪽 괄호일 때
		push(&storage, (*s)->data); // 연산자 스택에 넣어줌
		break;
	case LP: // 왼쪽 괄호일 때
		while (peek(*prefix, &item)) { // prefix를 빌 때까지 돌림
			pop(prefix, &item); // prefix의 값을 꺼내
			push(&temp, item);  // 잠시 temp로 옮겨줌
		}
		// 연산자 스택이 빌 때까지 반복문을 돌려줌
		// 만약 해당 요소가 오른쪽 괄호이면 반복문을 끝내고
		while (peek(storage, &item) && (item.attribute != RP || (!(pop(&storage, NULL))))) {
			pop(&storage, &item); // 연산자 스택의 내용을 꺼내
			push(&temp, item); // 잠시 temp로 옮겨 놓는다.
		}
		// **중요** 역순으로 뒤집어주는 연산
		while (peek(temp, &item)) { // temp가 빌 때 까지 반복문을 돌린다.
			pop(&temp, &item); // temp의 값을 꺼내
			push(prefix, item); // prefix에 저장한다.
		}
		break;
	default: // 이상한 값이 발견되면 오류코드 발생
		error("잘못된 값이 입력되었습니다.");
	}

	// 재귀함수로 위 과정 반복
	infix_to_prefix(&((*s)->link), prefix);
}

// **** 5. 괄호검사 및 식검사 ***

/*
	함수명: check_matching
	인자: StackNode**, const char
	리턴형: int
	설명: 괄호검사 함수
*/
int check_matching(StackNode** chk, const char* in) {
	element item; // chk값에 푸시해주기 위한 item
	char ch, open_ch; // 괄호값을 받아 비교해줄 변수
	int i, n = strlen(in); // 인덱스와 문자열의 길이
	char buf[MAX_SIZE]; // element에 붙여넣기하기 위한 버퍼

	for (i = 0; i < n; i++) { // 문자열의 길이 만큼 반복문이 돌아감
		ch = in[i]; // 현재 값을 ch 에 저장
		switch (ch) { // 현재 값을 switch문으로 돌려줌
		// 왼쪽 괄호는 스택에 넣어줌
		case '(':   case '[':    case '{':
			buf[0] = in[i]; // 왼쪽 괄호를 저장
			buf[1] = '\0';  // 문자열 끝마치기
			strcpy(item.value, buf); // strcpy를 사용하지 않으면 에러 발생함.
			push(chk, item); // item을 체크 스택에 넣어줌
			break;
		case ')':   case ']':    case '}':
			// 왼쪽 괄호가 없는데 오른쪽 괄호만 있으면 0 반환
			if (is_empty(*chk))  return 0;
			else { // 괄호의 짝이 맞는지 검사
				pop(chk, &item); // 괄호에서 왼쪽 괄호를 꺼냄
				// open_ch값은 char이기 때문에 문자열의 요소만 가지고 와야함
				open_ch = item.value[0]; 
				// 괄호의 짝이 맞는지 검사
				if ((open_ch == '(' && ch != ')') ||
					(open_ch == '[' && ch != ']') ||
					(open_ch == '{' && ch != '}')) {
					return 0; // 만약 괄호 짝이 맞지 않으면 0 반환
				}
				break;
			}
		}
	}
	// 스택에 남아있으면 왼쪽 괄호가 더 많아 짝이 안 맞는 것이다.
	if (!is_empty(*chk)) { 
		return 0; // 짝이 맞지 않기 때문에 0 반환
	} 
	return 1; // 문제가 없으면 1 반환
}

/*
	함수명: check_exp
	인자: StackNode**, StackNode**
	리턴형: int
	설명: 식이 올바른지 검사하는 함수
*/
// 피연산자->연산자->피연산자->연산자->...->피연산자 순으로 식이 구성됨
// 이 특징과 스택을 이용해 문제를 해결할 수 있음
int check_exp(StackNode** s, StackNode** chk) {
	StackNode* temp = *s; // 스택을 가리킬 임시 스택을 만듬
	element item; // 값을 스택에 저장하기 위한 element 변수

	// 링크드리스트 스택을 이용해 식이 올바른지 검사
	for (temp; temp != NULL; temp = temp->link) {
		switch (temp->data.attribute) {
		case NUM: // 피연산자라면?
			strcpy(item.value, temp->data.value); // 값을 저장해
			push(chk, item); // 스택에 push 연산함
			break;
		case ADD: case SUB: case MUL: case DIV: // 연산자라면?
			// 오류의 예
			// 1. 맨 뒤에 연산자가 나오면 오류발생
			// 2. 연속으로 연산자가 나오면 오류 발생
			if (is_empty(*chk)) { // 스택에 피연산자 없을 때 오류의 전형임.
				return 0; // 오류임을 알리기 위해 0을 반환
			}
			pop(chk, NULL); // 피연산자 제거 해줌
			break;
		case LP: case RP: // 괄호는 상관 없음
			break;
		default:
			error("잘못된 값이 입력됨"); // 잘못된 값이 입력됐을 때 예외처리
		}
	}

	// 3. 식의 시작이 연산자라면 오류를 발생시킴.
	if (is_empty(*chk)) {
		return 0; // 0 반환
	}
	else {
		return 1; // 식이 올바르면 1을 반환
	}
}


// **** 6. 후위표기식 계산 ****

/*
	함수명: eval_postfix
	인자: StackNode**, float*
	리턴형: void
	설명: 후위표기식 계산 함수
*/
// void형 함수지만, 계산결과값은 result에 저장함.
void eval_postfix(StackNode** postfix, float* result) {
	static StackNode* storage; // 피연산자 스택
	element item; // push연산을 위한 element 변수
	char operand1[MAX_SIZE]; // 피연산자1 
	char operand2[MAX_SIZE]; // 피연산자2

	// 스택순서대로 postfix를 제거하면 후위표기식대로 계산할 수 없기 때문에
	// 재귀를 이용해 맨 아래에 깔려있는 요소부터 차례대로 계산한다.
	if (peek(*postfix, &item)) {
		eval_postfix(&((*postfix)->link), result);
	}
	
	pop(postfix, &item); // 값을 꺼내 item에 저장
	switch (item.attribute) {
	case NUM: // 숫자일 시 스택에 넣어둠.
		push(&storage, item); // 피연산자 스택에 삽입
		break;
	case ADD: // 더하기
		// 피연산자 2개를 스택에서 꺼내 연산하고 다시 스택에 넣어줌
		pop(&storage, &item); strcpy(operand1, item.value);
		pop(&storage, &item); strcpy(operand2, item.value);
		// sprintf함수를 통해 숫자로 연산하고 숫자를 다시 문자열로 바꿔줌
		sprintf(item.value, "%f", atof(operand1) + atof(operand2));
		push(&storage, item); // 결과를 피연산자 스택에 삽입
		break;
	case SUB: // 빼기
		// 피연산자 2개를 스택에서 꺼내 연산하고 다시 스택에 넣어줌
		pop(&storage, &item); strcpy(operand1, item.value);
		pop(&storage, &item); strcpy(operand2, item.value);
		// sprintf함수를 통해 숫자로 연산하고 숫자를 다시 문자열로 바꿔줌
		sprintf(item.value, "%f", atof(operand2) - atof(operand1));
		push(&storage, item); // 결과를 피연산자 스택에 삽입
		break;
	case MUL:
		// 피연산자 2개를 스택에서 꺼내 연산하고 다시 스택에 넣어줌
		pop(&storage, &item); strcpy(operand1, item.value);
		pop(&storage, &item); strcpy(operand2, item.value);
		// sprintf함수를 통해 숫자로 연산하고 숫자를 다시 문자열로 바꿔줌
		sprintf(item.value, "%f", atof(operand1) * atof(operand2));
		push(&storage, item); // 결과를 피연산자 스택에 삽입
		break; 
	case DIV:
		// 피연산자 2개를 스택에서 꺼내 연산하고 다시 스택에 넣어줌
		pop(&storage, &item); strcpy(operand1, item.value);
		pop(&storage, &item); strcpy(operand2, item.value);

		// 0으로 나누려할 때 예외처리
		// 인덱스 1값이 '\0'이여야만 소수가 아님
		if (operand1[0] == '0' && operand1[1] == '\0') {
			error("0으로 나눌 수 없습니다.");
		}
		// sprintf함수를 통해 숫자로 연산하고 숫자를 다시 문자열로 바꿔줌
		sprintf(item.value, "%f", atof(operand2) / atof(operand1));
		push(&storage, item); // 결과를 피연산자 스택에 삽입
		break;
	}
	
	peek(storage, &item); // item에 결과를 저장해줌
	// 마지막에 함수를 빠져나올 때 계산이 완료된 값을 result에 저장하고 함수 끝남.
	*result = atof(item.value);
}

// **** 7. 메인 함수 ****

/*
	함수명: main
	인자: void
	리턴형: int
	설명: main 함수
*/
int main(void) {
	StackNode* expression = NULL; // 표기식 스택
	StackNode* postfix = NULL; // 후위표현식 스택
	StackNode* prefix = NULL;  // 전위표현식 스택
	StackNode* checkStack = NULL; // 괄호 및 식 검사 스택
	char exp[MAX_SIZE]; // 중위표기식을 입력받을 배열
	float result; // 후위표기식 계산 결과를 저장할 변수

	// 원하는 식의 값 입력받기
	printf("값을 입력하시오: ");
	scanf("%s", exp);

	// 괄호 검사를 통과해야지만 다음 단계로 넘어갈 수 있음
	if (check_matching(&checkStack, exp)) {
		printf("괄호검사성공\n");
		// 링크드리스트스택으로 만들어줌
		convert_to_stack(&expression, exp);
		/*printf("원래식: "); print_list(expression);
		printf("\n")*/;

		// 식 검사를 통과해야 다음 단계로 넘어갈 수 있음
		if (check_exp(&expression, &checkStack)) {
			printf("올바른 식입니다.\n");

			infix_to_prefix(&expression, &prefix); // 전위표기식으로 변환
			printf("전위식: "); print_list(prefix); printf("\n"); // 출력

			infix_to_postfix(&expression, &postfix); // 후위표기식으로 변환
			printf("후위식: ");  print_list(postfix); printf("\n"); // 출력

			eval_postfix(&postfix, &result); // 후위표기식을 계산
			printf("연산결과: %lf\n", result); // 계산결과 출력
		}
		else { // 식 검사 실패시, 에러 발생 및 프로그램 종료
			printf("오류 발생 - 잘못된 식입니다.\n");
		}
	}
	else {// 괄호 검사 실패시, 에러 발생 및 프로그램 종료
		printf("오류발생 - 괄호검사실패\n");
	}
	
}
