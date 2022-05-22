#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 600        // 노드의 갯수
#define M 9999     // 최대 값

FILE* fp;
int data[N][N];

typedef struct Node {
	char numbername[N][20]; // 역 고유번호
	int number[N]; // 역 순서에 따른 번호
	char name[N][20]; // 역 이름
	int distance[N]; // 정점까지의 거리
	int via[N]; // 이전 정점을 가리키는 포인터
	int startnumber; // 시작 정점(지하철 인덱스 할당)
	int destnumber;  // 도착 정점(지하철 인덱스 할당)
} Node;

int namecheck(char insert[], Node* Subway);
int find_index_byname(char name[], Node* Subway);
int find_index_bynumbername(char numbername[], Node* Subway);
void input(Node* Subway);
void find_start_and_dest(Node* Subway, char start[], char dest[]);


// 입력된 역이름을 확인하는 함수
int namecheck(char insert[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		// printf("%s\n", Subway->name[i]);
		if (!strcmp(insert, Subway->name[i]))
			return 1;
	}
	return 0;
}

// 역이름으로 인덱스를 찾는 함수
int find_index_byname(char name[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		if (strcmp(name, Subway->name[i]) == 0)
			return i;
	}
	return -1; // 찾는 값이 없을 때
}

// 고유번호로 인덱스를 찾는 함수
int find_index_bynumbername(char numbername[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		if (strcmp(numbername, Subway->numbername[i]) == 0)
			return i;
	}
	return -1; // 찾는 값이 없을 때
}

// 역이름을 넣고, 정점 간의 거리정보를 입력하는 함수
void input(Node* Subway) {
	int weight, num1, num2, w = 0;
	int i = 0, s, e, temp = 0;

	// 데이터 초기화 - 노드의 개수만큼 반복함
	for (s = 0; s < N; s++) {
		for (e = 0; e < N; e++) {
			if (s == e) // 같은 정점이라면 이동시간이 0이다.
				data[s][e] = 0;
			else // 최대수로 초기화함
				data[s][e] = M;
		}
	}

	char* filename = "data/역이름.txt";
	fp = fopen(filename, "r");
	if (!fp) {
		printf("Missing input file: %s\n", filename);
		return -1;
	}

	int count = 0;
	char line[1024];
	while (!feof(fp)) {
		fgets(line, 1024, fp);
		sscanf(line, "%[^,], %s", Subway->numbername[count], Subway->name[count]);
		Subway->number[count] = count;
		/*printf("Number: %s, ", Subway->numbername[count]); // 테스트
		printf("Name: %s\n", Subway->name[count]);*/
		count++;
	}
	fclose(fp);

	// 역이름 외의 모든 파일의 이름을 저장한 배열
	char* filename_list[19] = { "data/1지선.csv", "data/1호선.csv", "data/2지선.csv", "data/2호선.csv", "data/3호선.csv", "data/4호선.csv", "data/5지선.csv",
						"data/5호선.csv", "data/6호선.csv", "data/7호선.csv", "data/8호선.csv", "data/9호선.csv", "data/경의선.csv", "data/경춘선.csv",
						"data/공항철도.csv", "data/분당선.csv", "data/인천1선.csv", "data/중앙선.csv", "data/환승정보.csv" };

	for (s = 0; s < 19; s++) {
		fp = fopen(filename_list[s], "r");
		if (!fp) {
			printf("Missing input file: %s\n", filename_list[s]);
			return -1;
		}
		
		i = 1; // 인덱스에 따라 고유번호를 처리해 data를 채워나갈 것임
		char seq[N][100]; // 해당 노선의 모든 고유번호를 저장하는 배열
		fgets(line, 1024, fp); // 첫 번째 라인은 고유번호들의 모임을 seq에 저장하기 위해 따로 빼둠
		count = 0;
		char* p = strtok(line, ",");
		while (p != NULL) {
			strcpy(seq[count], p); // strcpy로 하지 않고 그냥 대입하면 얕은 복사가 되어 오류가 발생할 수 있다.
			p = strtok(NULL, ",");
			count++;
		}
		
		while (!feof(fp)) {
			fgets(line, 1024, fp);
			char* p = strtok(line, ",");
			int start_number = find_index_bynumbername(p, Subway); // 고유번호의 인덱스를 찾아 저장해둠
			int end_number;

			count = 1;
			while(1) { // strtok을 이용해 차례로 정보를 해석
				p = strtok(NULL, ",");
				if (p == NULL) break;
				weight = atoi(p); // 거리정보를 int형으로 변환

				if (weight < M) { // 9999보다 작으면 서로 연결되어 있는 정점이다.
					end_number = find_index_bynumbername(seq[count], Subway); // 고유번호 행렬을 이용해 정점의 인덱스를 찾아준다.
					// printf("%s: %d\n", seq[count], end_number); // 정상적으로 인덱스가 찾아지는지 확인
					data[start_number][end_number] = weight; // 양쪽으로 연결되어 있으므로 반대의 경우도 똑같이 넣어준다.
					data[end_number][start_number] = weight;
				}
				count++;
			}
		}

		fclose(fp);
	}
}

// 출발역과 도착역에 따른 고유번호 찾기
void find_start_and_dest(Node* Subway, char start[], char dest[]) {
	Subway->startnumber = find_index_byname(start, Subway);
	Subway->destnumber = find_index_byname(dest, Subway);
}

void main() {
	
	Node subway, * Subway;
	Subway = &subway;

	input(Subway, 0, 0);

	int way;
	char start[100], dest[100];
	
	 do {
		printf("출발역을 입력해주세요: ");
		scanf_s("%s", &start, 100);

		printf("도착역을 입력해주세요: ");
		scanf_s("%s", &dest, 100);

		if (namecheck(start, Subway) && namecheck(dest, Subway))
			break;
		else
			printf("다시 입력하시오\n");

		// printf("%d %d\n", namecheck(start, Subway), namecheck(end, Subway));

	}while (1);
	
	find_start_and_dest(Subway, start, dest);
	/*printf("%s: %d\n", start, Subway->startnumber); // 인덱스값 테스트
	printf("%s: %d", dest, Subway->destnumber);*/

	printf("방식? 1. 최단경로 2. 최소환승\n");
	scanf_s("%d", &way);
	switch (way)
	{
	case 1:
		printf("최단경로\n");
		break;
	case 2:
		printf("최소환승\n");
		break;
	default:
		printf("잘못된 값을 입력했습니다. 종료합니다.");
		break;
	}
}