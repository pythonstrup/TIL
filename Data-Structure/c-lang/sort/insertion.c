#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 10

int list[MAX_SIZE];
int n;

// 삽입정렬
void insertion_sort(int list[], int n)
{
	int i, j, key;
	for (i = 1; i<n; i++) {
		key = list[i];
		for (j = i - 1; j >= 0 && list[j]>key; j--)
			list[j + 1] = list[j]; /* 레코드의 오른쪽 이동 */
		list[j + 1] = key;
	}
}

int main(void)
{
	int i;
	n = MAX_SIZE;
	srand(time(NULL));
	for (i = 0; i < n; i++)      	// 난수 생성 및 출력 
		list[i] = rand() % 100; // 난수 발생 범위 0~99

	insertion_sort(list, n); // 삽입정렬 호출 
	for (i = 0; i < n; i++)
		printf("%d ", list[i]);
	printf("\n");
	return 0;
}