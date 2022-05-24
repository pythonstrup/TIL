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
	char name[N][20]; // 역 이름
	char nosun[N][20]; // 노선 이름
	int number[N]; // 역 순서에 따른 번호
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
void transfer(Node* Subway, int option);
void shortfind(Node* Subway, int option);
void print(Node subway, char start[], char dest[], int option);


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

	// 역이름, 환승정보 외의 모든 파일의 이름을 저장한 배열
	char* filename_list[18] = { "data/1지선.csv", "data/1호선.csv", "data/2지선.csv", "data/2호선.csv", "data/3호선.csv", "data/4호선.csv", "data/5지선.csv",
						"data/5호선.csv", "data/6호선.csv", "data/7호선.csv", "data/8호선.csv", "data/9호선.csv", "data/경의선.csv", "data/경춘선.csv",
						"data/공항철도.csv", "data/분당선.csv", "data/인천1선.csv", "data/중앙선.csv" };

	// 노선의 이름을 저장한 배열
	char* nosun[18] = { "1지선", "1호선", "2지선", "2호선", "3호선", "4호선", "5지선", "5호선", "6호선",
					"7호선", "8호선", "9호선", "경의선", "경춘선", "공항철도", "분당선", "인천1호선", "중앙선" };

	for (s = 0; s < 18; s++) {
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
			strcpy(Subway->nosun[start_number], nosun[s]); // 인덱스를 이용해 노선 정보 넣어주기
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

	// 환승정보 담기 - 해당하는 숫자 이하의 랜덤숫자를 담는다.
	srand(time(NULL)); // 난수 초기화
	filename = "data/환승정보.csv"; // 파일 이름
	fp = fopen(filename, "r"); // 파일 읽기모드로 열기
	if (!fp) { // 파일이 정상적으로 존재하지 않을 때
		printf("Missing input file: %s\n", filename);
		return -1;
	}

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
		while (1) { // strtok을 이용해 차례로 정보를 해석
			p = strtok(NULL, ",");
			if (p == NULL) break;
			weight = atoi(p); // 거리정보를 int형으로 변환

			if (weight < M) { // 9999보다 작으면 서로 연결되어 있는 정점이다.
				int random = (rand() % weight) + 1; // 환승정보 랜덤난수 만들기 (1이상으로)
				end_number = find_index_bynumbername(seq[count], Subway); // 고유번호 행렬을 이용해 정점의 인덱스를 찾아준다.
				// printf("%s: %d\n", seq[count], end_number); // 정상적으로 인덱스가 찾아지는지 확인
				data[start_number][end_number] = random; // 양쪽으로 연결되어 있으므로 반대의 경우도 똑같이 넣어준다.
				data[end_number][start_number] = random;
			}
			count++;
		}
	}

	fclose(fp);
}

// 출발역과 도착역에 따른 고유번호 찾기
void find_start_and_dest(Node* Subway, char start[], char dest[]) {
	Subway->startnumber = find_index_byname(start, Subway);
	Subway->destnumber = find_index_byname(dest, Subway);
}

// 옵션에 따라 최단거리 혹은 최소환승을 구하는 함수
// 최소환승일 때는 weight값에 1000을 더해서 환승을 되도록 안하게 만든다.
// 최단거리일 때는 정상적으로 진행
void shortfind(Node* Subway, int option) {		//구조체안에 배열값을 변경하기위해
	int i = 0, j, k, min;						//구조체포인터 사용(Call by reference)
	int v[N];

	//최소 환승일경우 데이터 초기화
	if (option == 2)
	{
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				// 환승장이 있는 역은 호선마다 이름이 같더라도 고유번호가 다르기 때문에 이름으로 비교해준다.
				if (strcmp(Subway->name[i + 1], Subway->name[j + 1]) == 0) {
					data[i][j] += 1000;			//환승역일경우 가중치 1000을 더해서 최소환승을 함
				}

				if (i == j) {
					data[i][j] = 0;
				}
			}
		}
	}

	// 시작지점으로 부터 각 지점까지의 최단거리 구하기
	for (j = 0; j < N; j++)
	{
		v[j] = 0;
		Subway->distance[j] = M; // distance의 모든 수를 9999로 초기화 한다.
	}

	//2.시작 지점부터 시작 하도록 지정
	Subway->distance[Subway->startnumber - 1] = 0;

	//3.정점의 수만큼 돈다.
	for (i = 0; i < N; i++)
	{
		//4. 최단거리인 정점을 찾는다.
		min = M; // min을 9999로 초기화 한다.
		for (j = 0; j < N; j++)
		{
			if (v[j] == 0 && Subway->distance[j] < min) {
				k = j;
				min = Subway->distance[j];
			}
		}
		v[k] = 1;  // 최단거리가 확인된 정점
		if (min == M)break;  // error 그래프가 연결되어있지 않음
		// k를 경유해서 j에 이르는 경로가 최단거리이면 갱신
		for (j = 0; j < N; j++)
		{
			if (Subway->distance[j] > Subway->distance[k] + data[k][j])
			{
				Subway->distance[j] = Subway->distance[k] + data[k][j];
				Subway->via[j] = k;
			}
		}
	}
}

// 구한 최단거리를 출력해주는 함수
void print(Node subway, char start[], char dest[], int option) {
	int path[N], path_cnt = 0;
	int i = 0, k, temp = 599;
	int count = 0; // 
	int transfer_time = 0; // 환승소요 시간

	//이동경로 저장
	k = subway.destnumber - 1;
	while (1)
	{
		path[path_cnt++] = k;  //path[]에 이동 경로 저장
		// printf("%d\n", k); // 경로 인덱스 확인 - 정상적
		if (k == (subway.startnumber - 1))break; // 시작 인덱스에 도달하면 끝낸다.
		count++; // 총 정거장 수 계산
		k = subway.via[k];
	}

	//이동 경로 출력
	printf("\n출발\n");
	while (strcmp(start, subway.name[path[path_cnt - 1]+1]) == 0) {	//출발역이 환승역일경우 예외처리
		path_cnt--;
		if (option == 2 && !(strcmp(start, subway.name[path[path_cnt - 1]+1]))) //최소환승이며 출발역이 환승역일경우
		{
			subway.distance[subway.destnumber - 1] -= 1000; // 1000이 더해져있는 상태이기 때문에 1000을 빼준다.
		}
	}

	for (i = path_cnt; i >= 1; i--)
	{	
		printf("%d\n", subway.distance[i]);
		// printf("%s %s\n", subway.name[temp], subway.name[path[i] + 1]);
		if (strcmp(subway.name[temp], subway.name[path[i]]) == 0) {		//환승역 두번출력 방지
			printf("─> < 환승: 소요시간 %d 분 > %s\n", subway.distance[i - 1] - subway.distance[i], subway.name[path[i]]);
			if (option == 2)
			{
				subway.distance[subway.destnumber - 1] -= 1000;
			}
			transfer_time += subway.distance[i-1] - subway.distance[i];
			continue;
		}
		if (!(strcmp(dest, subway.name[path[i] + 1])))					//도착역이 환승역일경우 예외처리
		{
			if (option == 2 && !(strcmp(dest, subway.name[path[i] + 1])))	//최소환승이며 도착역이 환승역일경우
			{
				subway.distance[subway.destnumber - 1] -= 1000;
			}
			break;
		}
		printf("─> <%s> %s\n", subway.nosun[path[i]], subway.name[path[i]]);
		temp = path[i];
	}
	
	printf("─> <%s> %s\n", subway.nosun[path[i]], subway.name[path[i] + 1]);// 마지막역 출력

	printf(" 정거장 수 : %d 개\n", count);
	printf(" 소요 시간 : %d분 ( %d분 + 환승 소요 시간: %d분)\n", 
		subway.distance[subway.destnumber - 1], subway.distance[subway.destnumber - 1]- transfer_time, transfer_time);
	_getch(); // Enter누르면 종료됨
}

void main() {
	
	Node subway, * Subway;
	Subway = &subway;

	input(Subway, 0, 0);

	// 이름 확인
	/*for (int i = 0; i < N; i++) {
		printf("%s ", Subway->name[i]);
	}*/

	// 인접행렬 확인
	/*for (int i = 0; i < N; i++) {
		printf("%s %s ", Subway->name[i], Subway->numbername[i]);
		for (int j = 0; j < N; j++) {
			printf("%d ", data[i][j]);
		}
		printf("\n");
	}*/

	// 노선정보 확인
	/*for (int i = 0; i < N; i++) {
		printf("%s ", Subway->nosun[i]);
	}*/

	int way;
	char start[100], dest[100];
	
	 do {
		printf("출발역을 입력해주세요: ");
		scanf_s("%s", &start, 100);

		printf("도착역을 입력해주세요: ");
		scanf_s("%s", &dest, 100);

		if (strcmp(start, dest) == 0)
			printf("출발역과 도착역이 같습니다. 다시 입력하시오\n");
		else if (namecheck(start, Subway) && namecheck(dest, Subway))
			break;
		else
			printf("잘못된 역을 입력했습니다. 다시 입력하시오\n");

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
		shortfind(Subway, way); // 최단경로 찾기
		print(subway, start, dest, way);
		break;
	case 2:
		printf("최소환승\n");
		shortfind(Subway, way); // 최소환승 찾기
		print(subway, start, dest, way);
		break;
	default:
		printf("잘못된 값을 입력했습니다. 종료합니다.");
		break;
	}
}

