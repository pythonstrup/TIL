#include <stdio.h>
#include <stdlib.h>

#define MAX_LIST_SIZE 100 	// 리스트의 최대크기

typedef int element;			
typedef struct {
	element array[MAX_LIST_SIZE];	  
	int size;		  
} ArrayListType;

void error(char* message){
	fprintf(stderr, "%s\n", message);
	exit(1);
}

// 리스트 초기화 함수
void init(ArrayListType* L) {
	L->size = 0;
}

int is_empty(ArrayListType* L) {
	return L->size == 0;
}

int is_full(ArrayListType* L) {
	return L->size == MAX_LIST_SIZE;
}

element get_entry(ArrayListType* L, int pos) {
	if (pos < 0 || pos >= L->size)
		error("위치 오류");
	return L->array[pos];
}

void print_list(ArrayListType* L) {
	int i;
	for (i = 0; i < L->size; i++)
		printf("%d->", L->array[i]);
	printf("\n");
}

void insert_last(ArrayListType* L, element item) {
	if (L->size >= MAX_LIST_SIZE) {
		error("리스트 오버플로우");
	}
	L->array[L->size++] = item;
}

void insert(ArrayListType* L, int pos, element item) {
	if (!is_full(L) && (pos >= 0) && (pos <= L->size)) {
		for (int i = (L->size - 1); i >= pos; i--)
			L->array[i + 1] = L->array[i];
		L->array[pos] = item;
		L->size++;
	}
}

element delete(ArrayListType* L, int pos) {
	element item;

	if (pos < 0 || pos >= L->size)
		error("위치 오류");
	item = L->array[pos];
	for (int i = pos; i < (L->size - 1); i++)
		L->array[i] = L->array[i + 1];
	L->size--;
	return item;
}

int main(void) {
	ArrayListType list;

	init(&list);
	insert(&list, 0, 1);	print_list(&list);	// 0번째 위치에 1 추가
	insert(&list, 0, 2);	print_list(&list);	// 0번째 위치에 2 추가
	insert(&list, 0, 3);	print_list(&list);	// 0번째 위치에 3 추가
	insert_last(&list, 4);	print_list(&list);	// 맨 끝에 4 추가
	delete(&list, 0);		print_list(&list);	// 0번째 항목(3) 삭제
	return 0;
}