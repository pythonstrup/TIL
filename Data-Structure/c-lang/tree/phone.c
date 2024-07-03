#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define MAX_WORD_SIZE 100
#define MAX_PHONE_SIZE 200

typedef struct { // 데이터 형식
	char name[MAX_WORD_SIZE];   // 이름
	char phone[MAX_PHONE_SIZE]; // 전화번호
} element;

typedef struct TreeNode {
	element key;
	struct TreeNode* left, * right;
} TreeNode;

// 이름(name)을 key로 사용한다
int compare(element e1, element e2) {
	// strcmp는 문자열의 ASCII값을 비교해 문자열의 대소를 알려준다.
	return strcmp(e1.name, e2.name);
}

// 이진 탐색 트리 출력 함수
void display(TreeNode* p) {
	if (p != NULL) {
		//printf("(");
		display(p->left);
		printf("%s: %s, ", p->key.name, p->key.phone);
		display(p->right);
		//printf(")");
	}
}

// 이진 탐색 트리 탐색 함수
TreeNode* search(TreeNode* root, element key) {
	TreeNode* p = root;

	while (p != NULL) {
		if (compare(key, p->key) == 0)
			return p;
		else if (compare(key, p->key) < 0)
			p = p->left;
		else if (compare(key, p->key) > 0)
			p = p->right;
	}
	return p; // 탐색에 실패하면 NULL 반환
}

TreeNode* new_node(element item) {
	TreeNode* temp = (TreeNode*)malloc(sizeof(TreeNode));
	temp->key = item;
	temp->left = temp->right = NULL;
	return temp;
}

TreeNode* insert_node(TreeNode* node, element key) {
	if (node == NULL) return new_node(key);

	if (compare(key, node->key) < 0)
		node->left = insert_node(node->left, key);
	else if (compare(key, node->key) > 0)
		node->right = insert_node(node->right, key);

	return node;
}

// 요구사항 min_value_node()가 아닌 max_value_node()로 구현
TreeNode* max_value_node(TreeNode* node) {
	TreeNode* current = node;

	// 맨 오른쪽 단말 노드를 찾아가면 가장 큰 갑을 얻을 수 있다.
	while (current->right != NULL)
		current = current->right;

	return current;
}

// max_value_node()를 사용함에 따라 약간의 변화가 있다.
// 삭제 함수
TreeNode* delete_node(TreeNode* root, element key) {
	if (root == NULL) return root;

	if (compare(key, root->key) < 0)
		root->left = delete_node(root->left, key);
	else if (compare(key, root->key) > 0)
		root->right = delete_node(root->right, key);
	else {
		if (root->left == NULL) {
			TreeNode* temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL) {
			TreeNode* temp = root->right;
			free(root);
			return temp;
		}
		
		// 세 번째 경우
		// 여기서 변화가 발생
		TreeNode* temp = max_value_node(root->left);
		root->key = temp->key;
		root->left = delete_node(root->left, temp->key);
	}
	return root;
}

void help() {
	printf("\n삽입(i), 탐색(s), 출력(p), 삭제(d), 종료(q): ");
}

int main(void) {
	char command;
	element e;
	TreeNode* root = NULL;
	TreeNode* tmp;

	do {
		help();
		command = getchar();
		getchar();

		switch (command) {
		case 'i':
			printf("이름: ");
			gets_s(e.name, MAX_WORD_SIZE);
			printf("전화번호: ");
			gets_s(e.phone, MAX_PHONE_SIZE);
			root = insert_node(root, e);
			break;
		case 'd':
			printf("이름: ");
			gets_s(e.name, MAX_WORD_SIZE);
			root = delete_node(root, e);
			break;
		case 'p':
			display(root);
			printf("\n");
			break;
		case 's':
			printf("검색 이름: ");
			gets_s(e.name, MAX_WORD_SIZE);
			tmp = search(root, e);
			if (tmp != NULL)
				printf("'%s'의 전화번호: %s\n", tmp->key.name, tmp->key.phone);
			break;
		}
	} while (command != 'q');
	return 0;
}