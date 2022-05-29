#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 50

// 스택
typedef struct {
	int data[MAX_VERTICES];
	int top;
} StackType;

// 그래프
typedef struct GraphType {
	int n; // 정점의 개수
	int adj_mat[MAX_VERTICES][MAX_VERTICES];
} GraphType;

int visited[MAX_VERTICES];

// 스택 - 초기화 함수
void init_stack(StackType* s) {
	s->top = -1;
}

// 스택 - 공백 상태 검출 함수
int is_empty(StackType* s) {
	return (s->top == -1);
}
// 스택 - 포화 상태 검출 함수
int is_full(StackType* s) {
	return (s->top == (MAX_VERTICES - 1));
}

// 스택 - 삽입 함수
void push(StackType* s, int item) {
	if (is_full(s)) {
		fprintf(stderr, "스택 포화 에러\n");
		return;
	}
	else
		s->data[++(s->top)] = item;
}

// 스택 - 삭제 함수
int pop(StackType* s) {
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
	}
	else
		return s->data[(s->top)--];
}

// 스택 - 피크함수
int peek(StackType* s)
{
	if (is_empty(s)) {
		fprintf(stderr, "스택 공백 에러\n");
		exit(1);
	}
	else return s->data[s->top];
}

// 그래프 - 초기화
void init_graph(GraphType* g) {
	int r, c;
	g->n = 0;
	for (r = 0; r < MAX_VERTICES; r++)
		for (c = 0; c < MAX_VERTICES; c++)
			g->adj_mat[r][c] = 0;
}

// 그래프 - 정점 삽입
void insert_vertex(GraphType* g, int v) {
	if (((g->n) + 1) > MAX_VERTICES) {
		fprintf(stderr, "그래프: 정점의 개수 초과");
		return;
	}
	g->n++;
}

// 그래프 - 간선 삽입
void insert_edge(GraphType* g, int start, int end) {
	if (start >= g->n || end >= g->n) {
		fprintf(stderr, "그래프: 정점 번호 오류");
		return;
	}
	g->adj_mat[start][end] = 1;
	g->adj_mat[end][start] = 1;
}

// DFS
void dfs_iterate(GraphType* g, int v) {
	
	int w, vertex;

	// 스택을 생성
	StackType stack;
	init_stack(&stack);

	// 스택에 시작 정점을 넣는다.
	push(&stack, v);
	while (!is_empty(&stack)) {
		vertex = pop(&stack);
		if (!visited[vertex]) {
			visited[vertex] = TRUE;
			printf("정점 %d -> ", vertex);
			// 스택이기 때문에 거꾸로 넣어줘야함.
			for (w = (g->n) -1; w >= 0; w--) {
				// 인접정점 중 방문하지 않은 정점을 넣어줌
				if (g->adj_mat[vertex][w] && !visited[w])
					push(&stack, w);
			}
		}
	}
}

int main(void) {
	GraphType* g;
	g = (GraphType*)malloc(sizeof(GraphType));
	init_graph(g);
	for (int i = 0; i < 6; i++)
		insert_vertex(g, i);
	/*insert_edge(g, 0, 1);
	insert_edge(g, 0, 2);
	insert_edge(g, 0, 3);
	insert_edge(g, 1, 2);
	insert_edge(g, 2, 3);*/

	insert_edge(g, 0, 2);
	insert_edge(g, 0, 4);
	insert_edge(g, 1, 2);
	insert_edge(g, 1, 5);
	insert_edge(g, 2, 3);
	insert_edge(g, 2, 5);
	insert_edge(g, 3, 5);
	insert_edge(g, 4, 5);

	printf("깊이 우선 탐색\n");
	dfs_iterate(g, 0);
	printf("\n");
	free(g);
	return 0;
}