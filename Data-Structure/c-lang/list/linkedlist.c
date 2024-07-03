#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char name[100];
} element;

typedef struct {
	element data;
	struct ListNode* link;
} ListNode;

// 오류 처리
void error(char* message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}

// 값을 넣는 함수
ListNode* insert_first(ListNode* head, element value) {
	ListNode* p = (ListNode*)malloc(sizeof(ListNode));
	p->data = value;
	p->link = head;
	head = p;
	return head;
}

// 값을 탐색하는 함수
ListNode* search(ListNode* head, element word) {
	ListNode* p = head;

	while (p != NULL) {
		if (strcmp(p->data.name, word.name) == 0)
			return p;
		p = p->link;
	}
	return NULL;  // 탐색 실패
}

// 삭제할 노드를 기준으로 두 개의 리스트를 하나로 합치는 함수
ListNode* concat_list(ListNode* head1, ListNode* head2, ListNode* removed) {
	if (head1 == NULL) return head2;
	else if (head2 == NULL) return head1;
	else {
		ListNode* p;
		p = head1;
		while (p->link != removed)
			p = p->link;
		p->link = head2;
		return p;
	}
}

ListNode* delete(ListNode* head, element word) {

	if (head == NULL) { // 연결리스트가 비어있을 때 예외처리
		fprintf(stderr, "리스트가 비어있음\n");
		exit(1);
	}
	
	// 단어 찾기
	ListNode* removed;
	removed = search(head, word);
	if (removed == NULL) { // 정보 찾기에 실패했을 때 예외처리
		fprintf(stderr, "찾는 정보가 없음\n");
		exit(1);
	}

	// 삭제할 단어의 다음 부분을 연결해주기 위해 노드 저장
	ListNode* head2;
	head2 = removed->link;
	
	// 노드 연결하고 연결점 위치 반환받기
	ListNode* pre;
	pre = concat_list(head, head2, removed);

	// 삭제하기
	free(removed);
	return pre;
}

// head2를 반환해 리스트를 2개로 나눔.
ListNode* split_list(ListNode* head, ListNode* pre) {

	// 삭제했던 노드의 뒤쪽 부분을 head2에 저장
	ListNode* head2;
	head2 = pre->link;

	// 첫 번째 리스트 마지막 부분 NULL처리
	ListNode* p;
	p = head;
	while (p->link != pre->link)
		p = p->link;
	p->link = NULL;
	
	// 두 번째 리스트 반환
	return head2;
}

void print_list(ListNode* head) {
	printf("head->");
	for (ListNode* p = head; p != NULL; p = p->link) {
		printf("%s->", p->data.name);
	}
	printf("NULL \n");
}

int main(void) {
	ListNode* head = NULL;
	element data;

	// 값 입력
	strcpy(data.name, "PAECH");
	head = insert_first(head, data);

	strcpy(data.name, "MELON");
	head = insert_first(head, data);

	strcpy(data.name, "MANGO");
	head = insert_first(head, data);

	strcpy(data.name, "LEMON");
	head = insert_first(head, data);
	
	strcpy(data.name, "BANANA");
	head = insert_first(head, data);

	strcpy(data.name, "KIWI");
	head = insert_first(head, data);

	strcpy(data.name, "APPLE");
	head = insert_first(head, data);
	print_list(head);

	// 단어 삭제 및 단어를 삭제한 부분을 반환받음
	strcpy(data.name, "LEMON");
	printf("삭제 단어: %s\n", data.name);
	ListNode* pre = delete(head, data);

	// 삭제한 부분 기준으로 리스트 쪼개기
	ListNode* head2;
	head2 = split_list(head, pre);
	printf("list1: ");
	print_list(head);
	printf("list2: ");
	print_list(head2);
}