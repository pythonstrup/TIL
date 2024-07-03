#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 600        // 노드의 갯수
#define M 9999     // 최대 값

FILE* fp; // 파일을 오픈할 때 사용할 전역변수

typedef struct Node { // 집약적인 정보를 담아둘 구조체
	char numbername[N][20]; // 역 고유번호
	char name[N][20]; // 역 이름
	char nosun[N][20]; // 노선 이름
	int distance[N]; // 정점까지의 거리
	int via[N]; // 이전 정점을 가리키는 포인터
	int startnumber; // 시작 정점(지하철 인덱스 할당)
	int destnumber;  // 도착 정점(지하철 인덱스 할당)
} Node;

typedef struct GraphType { // 메인부에서 동적할당할 것임.
	int n; // 총 역의 개수
	int **data; // 인접행렬_함수를 통해 동적할당을 나중에 진행
} GraphType;

// 함수 선언부 - 자세한 설명은 뒤쪽에 있음
void init_graph(GraphType* g);
int namecheck(char insert[], Node* Subway);
int find_index_byname(char name[], Node* Subway);
int find_index_bynumbername(char numbername[], Node* Subway);
void input_name(Node* Subway, GraphType* g);
void input_distance(Node* Subway, GraphType* g);
void input_transfer(Node* Subway, GraphType* g);
void find_start_and_dest(Node* Subway, char start[], char dest[]);
int choose(int distance[], int n, int found[]);
void shortfind(Node* Subway, GraphType* g, int option);
void print(Node subway, GraphType* g, char start[], char dest[], int option);

/*
	함수명: init_graph
	인자: GraphType
	리턴형: void
	설명: Graph에 담긴 총 정점의 수를 담은 n을 초기화함.
*/
void init_graph(GraphType* g) {
	g->n = 1; // 총 역의 개수 - index 0를 더미값으로 설정
}

/*
	함수명: init_adjacent_matrix
	인자: GraphType
	리턴형: void
	설명: Graph에 담긴 총 정점의 수를 담은 n을 이용해 인접행렬을 초기화함.
*/
void init_adjacent_matrix(GraphType* g) {
	int s, e;

	// 2차원 배열을 동적할당한다.
	g->data = (int**)malloc(sizeof(int*) * g->n);
	for (int i = 0; i < g->n; i++)
		g->data[i] = (int*)malloc(sizeof(int) * g->n);

	// 데이터 초기화 - 노드의 개수만큼 반복함
	for (s = 0; s < g->n; s++) {
		for (e = 0; e < g->n; e++) {
			if (s == e) // 같은 정점이라면 이동시간이 0이다.
				g->data[s][e] = 0;
			else // 최대수로 초기화함
				g->data[s][e] = M;
		}
	}
}

/*
	함수명: namecheck
	인자: char[], Node
	리턴형: int
	설명: 입력된 역이름이 존재하는지 확인하는 함수
		  만약 역이름이 존재하지 않는다면 -1을 리턴한다.
*/
int namecheck(char insert[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		// printf("%s\n", Subway->name[i]);
		if (!strcmp(insert, Subway->name[i]))
			return 1; // 이름을 찾으면 참을 반환
	}
	return 0;
}

/*
	함수명: find_index_byname
	인자: char[], Node
	리턴형: int
	설명: 역이름으로 인덱스를 찾는 함수
		  만약 역이름이 존재하지 않는다면 -1을 리턴한다.
*/
int find_index_byname(char name[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		if (strcmp(name, Subway->name[i]) == 0)
			return i; // 이름을 찾으면 그 인덱스를 반환
	}
	return -1; // 찾는 값이 없을 때
}

/*
	함수명: find_index_bynumbername
	인자: char[], Node
	리턴형: int
	설명: 역의 고유번호로 인덱스를 찾는 함수
		  만약 해당 고유번호가 존재하지 않는다면 -1을 리턴한다.
*/
int find_index_bynumbername(char numbername[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		if (strcmp(numbername, Subway->numbername[i]) == 0)
			return i; // 고유번호를 찾으면 그 인덱스를 반환
	}
	return -1; // 찾는 값이 없을 때
}


/*
	함수명: input_name
	인자: Node, GraphType
	리턴형: void
	설명: 역이름.csv를 해석해 모든 역을 Node에 입력해준다.
		  모든 정점의 수를 가지고 있는 GraphType의 n을 같이 증가시켜준다.
*/
void input_name(Node* Subway, GraphType* g) {
	int weight, w = 0; // 가중치 변수
	int i = 0, s, e;
	char* filename; // 파일 이름을 저장할 변수
	char line[1024]; // 라인을 읽어올 변수


	filename = "data/역이름.txt";
	fp = fopen(filename, "r"); // 파일 오픈
	if (!fp) { // 만약 파일 이름이 존재하지 않으면 알려줌
		printf("Missing input file: %s\n", filename);
		return;
	}

	while (!feof(fp)) { // 파일의 모든 라인을 다 읽을 때까지 반복
		fgets(line, 1024, fp); // 라인 읽어옴
		sscanf(line, "%[^,], %s", Subway->numbername[g->n], Subway->name[g->n]);
		//printf("Number: %s, ", Subway->numbername[g->n]); // 테스트
		//printf("Name: %s\n", Subway->name[g->n]);
		(g->n)++; // 총 정점의 개수 늘려주기
	}
	fclose(fp);
}

// *** 요구사항 1. 역 정보가 저장된 텍스트파일을 읽어 인접행렬을 생성하는 함수 구현 ***
/*
	함수명: input_distance
	인자: Node, GraphType
	리턴형: void
	설명: Subway에 담긴 역이름을 바탕으로 역 사이의 거리정보를 출력한다.
		  배열을 이용해 파일 이름을 저장하고 하나씩 꺼내 해석한다.
		  노선 정보도 배열로 저장해놓고, 같이 인덱스에 맞게 같이 입력해준다.
*/
void input_distance(Node* Subway, GraphType* g) {
	int weight, w = 0; // 가중치 변수
	int i = 0, s, e;
	int count = 0; // 인덱스 역할을 할 변수
	char line[1024]; // 라인을 읽어올 변수


	// 역이름, 환승정보 외의 모든 파일의 이름을 저장한 배열
	char* filename_list[18] = { "data/1호선.csv", "data/1호선.csv", "data/2지선.csv", "data/2호선.csv", "data/3호선.csv", "data/4호선.csv", "data/5지선.csv",
						"data/5호선.csv", "data/6호선.csv", "data/7호선.csv", "data/8호선.csv", "data/9호선.csv", "data/경의선.csv", "data/경춘선.csv",
						"data/공항철도.csv", "data/분당선.csv", "data/인천1선.csv", "data/중앙선.csv" };

	// 노선의 이름을 저장한 배열
	char* nosun[18] = { "1지선", "1호선", "2지선", "2호선", "3호선", "4호선", "5지선", "5호선", "6호선",
					"7호선", "8호선", "9호선", "경의선", "경춘선", "공항철도", "분당선", "인천1호선", "중앙선" };

	for (s = 0; s < 18; s++) { // 배열만큼 반복시킨다.
		fp = fopen(filename_list[s], "r"); // 파일 오픈
		if (!fp) { // 파일이 존재하지 않으면 알려줌
			printf("Missing input file: %s\n", filename_list[s]);
			return;
		}

		i = 1; // 인덱스에 따라 고유번호를 처리해 data를 채워나갈 것임
		char seq[N][100]; // 해당 노선의 모든 고유번호를 저장하는 배열
		fgets(line, 1024, fp); // 첫 번째 라인은 고유번호들의 모임을 seq에 저장하기 위해 따로 빼둠
		count = 0; // 인덱스 역할
		char* p = strtok(line, ","); // ","를 기준으로 문자열을 끊어 읽어온다.
		// seq배열을 만드는 작업 - seq배열에 저장된 고유번호를 읽어 가중치를 할당할 것이다.
		while (p != NULL) { // 만약 다 읽어오면 끝낸다.
			strcpy(seq[count], p); // strcpy로 하지 않고 그냥 대입하면 얕은 복사가 되어 오류가 발생할 수 있다.
			p = strtok(NULL, ","); // ","를 기준으로 문자열을 끊어 읽어온다.
			count++; // 인덱스 증가
		}

		while (!feof(fp)) {
			fgets(line, 1024, fp); // 라인 읽어오기
			char* p = strtok(line, ","); // ","를 기준으로 문자열을 끊어 읽어온다.
			int start_number = find_index_bynumbername(p, Subway); // 고유번호의 인덱스를 찾아 저장해둠
			strcpy(Subway->nosun[start_number], nosun[s]); // 인덱스를 이용해 노선 정보 넣어주기
			int end_number;

			count = 1; // 인덱스를 초기화한다.
			while (1) { // strtok을 이용해 차례로 정보를 해석
				p = strtok(NULL, ",");
				if (p == NULL) break; // 만약 라인을 다 읽었으면 루프 탈출
				weight = atoi(p); // 거리정보를 int형으로 변환
				if (weight < M) { // 9999보다 작으면 서로 연결되어 있는 정점이다.
					end_number = find_index_bynumbername(seq[count], Subway); // 고유번호 행렬을 이용해 정점의 인덱스를 찾아준다.
					if (end_number == -1) { //마지막 seq는  '\n'을 가지고 있다. 그러면 end_number가 -1이 되는데 이를 예외처리해주자.
						for (int i = 0; seq[count][i] != 0; i++) {
							if (seq[count][i] == '\n') { // seq에 있는 문자열에 한 부분이 '\n'을 포함하고 있다면
								seq[count][i] = 0; // 대신 NULL문자를 대입해준다.
								break;
							}
						}
						end_number = find_index_bynumbername(seq[count], Subway); // 다시 end_number를 찾아준다.
					}
					if (end_number != -1) { // 혹시 모를 오류처리를 위해 -1 이외의 것만 처리해준다.
						//printf("%d %d\n", start_number, end_number); // 테스트
						//printf("%s %s\n", Subway->name[start_number], seq[count]);
						//printf("%d\n", find_index_bynumbername(seq[count], Subway));
						g->data[start_number][end_number] = weight; // 양쪽으로 연결되어 있으므로 반대의 경우도 똑같이 넣어준다.
						g->data[end_number][start_number] = weight;
					}

				}
				count++; // 인덱스 증가
			}
		}

		fclose(fp); // 파일을 닫는다.
	}
}

// *** 요구사항 2.  환승할 경우 환승정보 파일에서 제공한 최대 환승 소요시간 안에서 난수 값을 생성하는 함수구현 ***
/*
	함수명: input_transfer
	인자: Node, GraphType
	리턴형: void
	설명: Subway에 담긴 고유번호를 바탕으로 같은 역이름의 환승정보를 입력한다.
		  요구사항에 맞게 난수를 생성해 환승거리를 입력해준다.
*/
void input_transfer(Node* Subway, GraphType* g) {
	int weight, w = 0; // 가중치
	int i = 0, s, e;
	char* filename; // 파일이름 저장할 변수
	int count = 0; // 인덱스 역할
	char line[1024]; // 라인을 읽어올 변수

	// 환승정보 담기 - 해당하는 숫자 이하의 랜덤숫자를 담는다.
	srand(time(NULL)); // 난수 초기화
	filename = "data/환승정보.csv"; // 파일 이름
	fp = fopen(filename, "r"); // 파일 읽기모드로 열기
	if (!fp) { // 파일이 정상적으로 존재하지 않을 때
		printf("Missing input file: %s\n", filename);
		return;
	}

	char seq[N][100]; // 해당 노선의 모든 고유번호를 저장하는 배열
	fgets(line, 1024, fp); // 첫 번째 라인은 고유번호들의 모임을 seq에 저장하기 위해 따로 빼둠
	count = 0; // 인덱스 초기화
	char* p = strtok(line, ","); // ","를 기준으로 문자열을 끊어 읽어온다.
	// seq배열을 만드는 작업 - seq배열에 저장된 고유번호를 읽어 가중치를 할당한다.
	while (p != NULL) { // 해당 라인을 다 읽을 때까지 반복
		strcpy(seq[count], p); // strcpy로 하지 않고 그냥 대입하면 얕은 복사가 되어 오류가 발생할 수 있다.
		p = strtok(NULL, ",");
		count++; // 인덱스 증가
	}

	while (!feof(fp)) { // 모든 라인을 읽을 때까지 반복
		fgets(line, 1024, fp); // 라인단위로 읽어옴
		char* p = strtok(line, ",");
		int start_number = find_index_bynumbername(p, Subway); // 고유번호의 인덱스를 찾아 저장해둠
		int end_number;

		count = 1; // 인덱스 초기화
		while (1) { // strtok을 이용해 차례로 정보를 해석
			p = strtok(NULL, ",");
			if (p == NULL) break; // 한 라인의 라인의 내용을 모두 읽으면 루프를 탈출, 다음 라인을 읽는다.
			weight = atoi(p); // 거리정보를 int형으로 변환
			// weight가 0인 것은 서로 호선도 같은 역이므로 제외한다.
			if (weight < M && weight != 0) { // 9999보다 작으면 서로 연결되어 있는 정점이다.
				int random = (rand() % weight) + 1; // 환승정보 랜덤난수 만들기 (1이상으로)
				end_number = find_index_bynumbername(seq[count], Subway); // 고유번호 행렬을 이용해 정점의 인덱스를 찾아준다.
				
				if (end_number == -1) { //마지막 seq는  '\n'을 가지고 있다. 그러면 end_number가 -1이 되는데 이를 예외처리해주자.
					for (int i = 0; seq[count][i] != 0; i++) {
						if (seq[count][i] == '\n') { // seq에 있는 문자열에 한 부분이 '\n'을 포함하고 있다면
							seq[count][i] = 0; // 대신 NULL문자를 대입해준다.
							break;
						}
					}
					end_number = find_index_bynumbername(seq[count], Subway); // 다시 end_number를 찾아준다.
				}
	
				if (end_number != -1) { // 혹시 모를 오류처리를 위해 -1 이외의 것만 처리해준다.
					//printf("%d %d\n", start_number, end_number); // 테스트
					//printf("%s %s\n", Subway->name[start_number], seq[count]);
					//printf("%d\n", find_index_bynumbername(seq[count], Subway));
					g->data[start_number][end_number] = random; // 양쪽으로 연결되어 있으므로 반대의 경우도 똑같이 넣어준다.
					g->data[end_number][start_number] = random;
				}
			}
			count++; // 인덱스 증가
		}
	}

	fclose(fp); // 파일 닫기
}

/*
	함수명: find_start_and_dest
	인자: Node, char[], dest[]
	리턴형: void
	설명: 출발역과 도착역의 이름을 통해 인덱스를 찾는다.
		여기서 구한 값을 기준으로 다익스트라 알고리즘을 적용한다.
*/
void find_start_and_dest(Node* Subway, char start[], char dest[]) {
	Subway->startnumber = find_index_byname(start, Subway); // start에 해당하는 이름의 인덱스 찾기
	Subway->destnumber = find_index_byname(dest, Subway); // dest에 해당하는 이름의 인덱스 찾기
}

/*
	함수명: choose
	인자: int[], int, int[]
	리턴형: int
	설명: 최단경로를 구하는 알고리즘
		교재와 내용 동일, 방문하지 않은 값 중 최소값을 찾아 반환해준다.
*/
int choose(int distance[], int n, int found[]) {
	int i, min, minpos;
	min = M; // min을 9999로 초기화하기
	minpos = -1; // 최소값의 인덱스를 -1로 초기화
	for (i = 0; i < n; i++) // 노드의 개수만큼 반복
		if (distance[i] < min && !found[i]) {
			min = distance[i]; // 거리 할당
			minpos = i; // 인덱스 할당
		}
	return minpos; // 인덱스 반환
}

// *** 요구사항 3. 다익스트라 알고리즘을 이용하여 최단경로 탐색함수 구현 ***
// *** 요구사항 4. 사용자로부터 최단경로와 최소환승의 옵션을 입력받아 각각의 조건에 맞게 수행 ***

/*
	함수명: shortfind
	인자: Node, GraphType, int
	리턴형: void
	설명: 1.최단경로 2.최소환승 옵션값을 받아 다른 결과물을 출력해준다.
		인접행렬에 따른 다익스트라 알고리즘을 이용해 경로를 구한다.
		최소환승일 때는 weight값에 1000을 더해서 환승을 되도록 안하게 만든다.
		중요! - 만약 출발역이 환승역이라면 가중치를 0으로 만들어서 환승이 추가되지 않도록 한다!
*/
void shortfind(Node* Subway, GraphType* g, int option) {		//구조체안에 배열값을 변경하기위해
	int i = 0, j, k = 0, min;	//구조체포인터 사용(Call by reference)
	int v[N]; // 방문여부 판독

	//최소 환승일경우 데이터 초기화
	if (option == 2)
	{
		for (i = 1; i < g->n; i++) { // graph의 총 정점의 개수를 담은 n으로 for문을 돌림
			for (j = 1; j < g->n; j++) {
				// 환승장이 있는 역은 호선마다 이름이 같더라도 고유번호가 다르기 때문에 이름으로 비교해준다.
				if (strcmp(Subway->name[i], Subway->name[j]) == 0) {
					if (j == Subway->startnumber || i == Subway->startnumber) { // 만약 출발점이 환승장이라면 거리를 0으로 둔다.
						g->data[i][j] = 0;
					}
					else
						g->data[i][j] += 1000;			//환승역일경우 가중치 1000을 더해서 최소환승을 함
					//printf("%s %d %s %d\n", Subway->name[i + 1], i + 1, Subway->name[j + 1], j + 1);
					//printf("%d %d\n", data[i][j], data[i+1][j+1]); // 테스트
				}

				if (i == j) {
					g->data[i][j] = 0; // 호선도 아예 똑같다면 0으로 초기화해준다.
				}
			}
		}
	}
	else { // 만약 그냥 옵션 1이라면
		for (i = 1; i < g->n; i++) {
			for (j = 1; j < g->n; j++) {
				if (strcmp(Subway->name[i], Subway->name[j]) == 0) { // 이름이 같아야함
					if (j == Subway->startnumber || i == Subway->startnumber) { // 출발점이 환승장일 때 거리를 0으로 둔다.
						g->data[i][j] = 0;
					}
				}
			}
		}
	}


	// 시작지점으로 부터 각 지점까지의 최단거리 구하기
	for (j = 1; j < g->n; j++)
	{
		v[j] = 0; // 방문여부를 0으로 초기화해준다.
		Subway->distance[j] = g->data[Subway->startnumber][j]; // distance의 모든 수를 9999로 초기화 한다.
		Subway->via[j] = -1; // 경로정보를 초기화한다.
	}

	//2.시작 지점부터 시작 하도록 지정
	v[Subway->startnumber] = 1; // 시작 노드를 방문처리 한다.
	Subway->distance[Subway->startnumber] = 0; // 시작노드를 0으로 둬 출발점으로 설정

	// Subway와 Graph사이의 인덱스 관계(Subway+1 = Graph)
	//3.정점의 수만큼 돈다.
	for (i = 1; i < g->n; i++)
	{
		//4. 최단거리인 정점을 찾는다.
		k = choose(Subway->distance, g->n, v); // 최소값의 인덱스를 찾아오기
		if (k == -1) break; // 만약 -1이 발견되면 반복문을 종료시킨다.
		v[k] = 1;
		// k를 경유해서 j에 이르는 경로가 최단거리이면 갱신
		for (j = 1; j < g->n; j++)
		{
			if (v[j] == 0) {
				if (Subway->distance[j] > Subway->distance[k] + g->data[k][j])
				{	// 최단경로를 갱신해준다.
					Subway->distance[j] = Subway->distance[k] + g->data[k][j];
					Subway->via[j] = k; // 경로를 따로 저장해준다.
					// printf("% d ", Subway->distance[j]);
				}
			}
		}
	}
}

/*
	함수명: print
	인자: Node, GraphType, char[], char[], char[]
	리턴형: void
	설명: 다익스트라 알고리즘으로 구한 경로를 이용해 출력해주는 함수
		최소환승일 때  weight값에 1000이 더해져있기 때문에 이를 빼주고 출력해준다.
*/
void print(Node subway, GraphType* g, char start[], char dest[], int option) {
	int path[N], path_cnt = 0; // 경로와 관련된 변수
	int i = 0, k, temp = 0; // temp를 의미없는 인덱스에 배치
	int count = 0; // 총 정거장의 개수를 리턴해줄 count
	int transfer = 0; // 총 환승장의 개수
	int transfer_time = 0; // 환승소요 시간

	//이동경로 저장
	k = subway.destnumber; // k에 먼저 도착지의 인덱스를 저장한다.
	while (1)
	{
		// printf("%d ", k);
		if (k == -1) break; // 시작 인덱스에 도착하면 끝낸다.
		path[path_cnt++] = k;  //path[]에 이동 경로 저장
		//printf("%d %d %s: %d\n", subway.startnumber, subway.destnumber, subway.name[k], subway.distance[k]); // 경로 인덱스 확인 - 정상적
		k = subway.via[k]; // k에 이전 경로지를 알려준다.
	}


	//이동 경로 출력
	printf("\n출발\n");
	while (strcmp(start, subway.name[path[path_cnt - 1]]) == 0) {	//출발역이 환승역일경우 예외처리
		//printf("%s %s \n", start, subway.name[path[path_cnt - 1]]); // 테스트
		path_cnt--; // 만약 환승역일 시 불필요한 path가 출력될 수 있으므로 빼준다.
		subway.distance[subway.destnumber] -= g->data[path[path_cnt]][path[path_cnt - 1]]; // 거리에 환승시간이 포함되어있기 때문에 빼준다.
	}

	count = path_cnt + 1; // 총 정거장 수 (환승역 포함)
	path[path_cnt] = subway.startnumber; // 마지막 요소에 start인덱스를 넣어줌
	for (i = path_cnt; i >= 1; i--)
	{
		//printf("%s %s\n", subway.name[temp], subway.name[path[i] + 1]);
		if (strcmp(subway.name[temp], subway.name[path[i]]) == 0) {		//환승역 두번출력 방지
			//printf("%s-%s: %d\n", subway.name[path[i]], subway.name[path[i + 1]], g->data[path[i]][path[i + 1]]); //테스트
			//printf("%d-%d\n", path[i], path[i + 1]);
			//printf("%s-%s: %d\n", subway.name[path[i]], subway.name[path[i - 1]], g->data[path[i]][path[i - 1]]); //테스트
			//printf("%d-%d\n", path[i], path[i -1]);
			transfer += 1; // 환승을 카운트해준다.
			if (option == 2) // 만약 최소환승이라면
			{
				subway.distance[subway.destnumber] -= 1000; // 현재 거리에 환승역마다 1000이 더해져 있는 상태이므로 1000씩 다시 빼주면 된다.
				printf("─> < 환승: 소요시간 %d 분 > %s\n", g->data[path[i]][path[i + 1]] - 1000, subway.name[path[i]]); // 환승 출력
				transfer_time += g->data[path[i]][path[i + 1]] - 1000; // 총 환승 소요 시간 구하기
			}
			else { // 최단경로일 경우
				printf("─> < 환승: 소요시간 %d 분 > %s\n", g->data[path[i]][path[i + 1]], subway.name[path[i]]); // 환승 출력
				transfer_time += g->data[path[i]][path[i + 1]]; // 총 환승 소요 시간 구하기
			}
			continue; // 환승을 출력하고 다음 역으로 넘긴다.
		}
		if (!(strcmp(dest, subway.name[path[i]])))	//도착역이 환승역일경우 예외처리
		{
			// printf("%d %d\n", data[path[i]][path[i + 1]], data[path[i]][path[i - 1]]); //테스트
			if (option == 2 && !(strcmp(dest, subway.name[path[i]])))	//최소환승이며 도착역이 환승역일경우
			{
				subway.distance[subway.destnumber] -= 1000; // 최단경로에 가중치 1000이 더해져있기때문에 빼준다.
				g->data[path[i]][path[i - 1]] -= 1000;
			}
			transfer++; // 환승역 추가
			count -= i - 1; // 도착역에 왔기 때문에 돌지 않고 도착역이 왔다면 -0이 될 것이고 뺑뺑 돌았다면 제대로된 값이 할당될 것이다.
			subway.destnumber = path[i]; // 도착역을 다시 한 번 확인시켜준다.
			subway.distance[subway.destnumber] -= g->data[path[i]][path[i + 1]]; // 거리에 환승시간이 포함되어있기 때문에 빼준다.
			break; // 알고리즘의 오류로 인해 경로를 뺑뺑돌고 있을 수 있기 때문에 break로 끝내고 나옴.
		}
		printf("─> <%s> %s\n", subway.nosun[path[i]], subway.name[path[i]]); // 정거장 정보 출력
		temp = path[i]; // 환승역 예외처리를 위해 다음 정거장으로 이동하기 전에 현재 정거장을 저장해둠
		//printf("%s ", subway.name[path[i-1]]);
	}

	printf("─> <%s> %s <도착>\n", subway.nosun[path[i]], subway.name[path[i]]);// 마지막역 출력

	count -= transfer; // 지나온 역수에 환승역을 빼준다.
	printf(" 정거장 수 : %d 개\n", count); // 지나온 정거장 수 출력
	// 소요시간 출력
	printf(" 소요 시간 : %d분 ( %d분 + 환승 소요 시간: %d분)\n",
		subway.distance[subway.destnumber], subway.distance[subway.destnumber] - transfer_time, transfer_time);
}

/*
	함수명: main
	인자: void
	리턴형: int
	설명: 메인부, 위의 함수들을 사용해 출력하는 함수
*/
int main(void) {

	Node subway, * Subway; // 지하철 정보를 담을 노드
	GraphType* graph = (GraphType*)malloc(sizeof(GraphType)); // 정점의 인접행렬을 담은 구조체 동적할당
	Subway = &subway;

	init_graph(graph); // 그래프 초기화
	input_name(Subway, graph); // 역이름.csv 읽기
	init_adjacent_matrix(graph); // 역이름을 읽으며 얻은 정점을 수를 이용해 인접행렬 초기화
	input_distance(Subway, graph); // 호선 정보를 통해 그래프 가중치 할당
	input_transfer(Subway, graph); // 환승정보 가중치 할당
	//input(Subway, graph);
	printf("인접행렬 입력완료\n");

	// 이름 확인
	/*for (int i = 0; i < N; i++) {
		printf("%s ", Subway->name[i]);
	}*/

	// 인접행렬 확인
	/*for (int i = 0; i < N; i++) {
		printf("%s %s ", Subway->name[i], Subway->numbername[i]);
		for (int j = 0; j < N; j++) {
			printf("%d ", graph->data[i][j]);
		}
		printf("\n");
	}*/

	// 노선정보 확인
	/*for (int i = 0; i < N; i++) {
		printf("%s ", Subway->nosun[i]);
	}*/

	int way;
	char start[100], dest[100];

	// *** 요구사항 5. 기본적인 예외 처리 함수 구현 ***
	do {
		printf("출발역을 입력해주세요: ");
		scanf("%s", start);

		printf("도착역을 입력해주세요: ");
		scanf("%s", dest);

		if (strcmp(start, dest) == 0) // 도착역과 출발역 이름이 똑같을 때 다시 입력
			printf("출발역과 도착역이 같습니다. 다시 입력하시오\n");
		else if (namecheck(start, Subway) && namecheck(dest, Subway))
			break; // 정상입력 시 루프 탈출
		else // 존재하지 않는 역을 입력했을 때 다시 입력
			printf("잘못된 역을 입력했습니다. 다시 입력하시오\n");

		// printf("%d %d\n", namecheck(start, Subway), namecheck(end, Subway));

	} while (1);

	find_start_and_dest(Subway, start, dest); // 시작 인덱스와 종료 인덱스를 불러온다.
	/*printf("%s: %d\n", start, Subway->startnumber); // 인덱스값 테스트
	printf("%s: %d", dest, Subway->destnumber);*/

	// *** 요구사항 4. 사용자로부터 최단경로와 최소환승의 옵션을 입력받아 각각의 조건에 맞게 수행 ***
	printf("방식? 1. 최단경로 2. 최소환승\n");
	scanf("%d", &way); // 방식값을 입력받음
	switch (way) // 스위치문을 이용해 분기
	{
	case 1: // 최단경로를 선택했을 때
		printf("최단경로\n");
		shortfind(Subway, graph, way); // 최단경로 찾기
		print(subway, graph, start, dest, way);
		break;
	case 2: // 최소환승일 경우
		printf("최소환승\n");
		shortfind(Subway, graph, way); // 최소환승 찾기
		print(subway, graph, start, dest, way);
		break;
	default: // 잘못된 수가 입력되면 그냥 종료시킨다.
		printf("잘못된 값을 입력했습니다. 종료합니다.");
		break;
	}

	free(graph); // 동적할당된 그래프 해제

	return 0;
}

