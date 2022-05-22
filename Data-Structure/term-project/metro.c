#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 600        // ����� ����
#define M 9999     // �ִ� ��

FILE* fp;
int data[N][N];

typedef struct Node {
	char numbername[N][20]; // �� ������ȣ
	int number[N]; // �� ������ ���� ��ȣ
	char name[N][20]; // �� �̸�
	int distance[N]; // ���������� �Ÿ�
	int via[N]; // ���� ������ ����Ű�� ������
	int startnumber; // ���� ����(����ö �ε��� �Ҵ�)
	int destnumber;  // ���� ����(����ö �ε��� �Ҵ�)
} Node;

int namecheck(char insert[], Node* Subway);
int find_index_byname(char name[], Node* Subway);
int find_index_bynumbername(char numbername[], Node* Subway);
void input(Node* Subway);
void find_start_and_dest(Node* Subway, char start[], char dest[]);


// �Էµ� ���̸��� Ȯ���ϴ� �Լ�
int namecheck(char insert[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		// printf("%s\n", Subway->name[i]);
		if (!strcmp(insert, Subway->name[i]))
			return 1;
	}
	return 0;
}

// ���̸����� �ε����� ã�� �Լ�
int find_index_byname(char name[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		if (strcmp(name, Subway->name[i]) == 0)
			return i;
	}
	return -1; // ã�� ���� ���� ��
}

// ������ȣ�� �ε����� ã�� �Լ�
int find_index_bynumbername(char numbername[], Node* Subway) {
	int i;
	for (i = 0; i < N; i++) {
		if (strcmp(numbername, Subway->numbername[i]) == 0)
			return i;
	}
	return -1; // ã�� ���� ���� ��
}

// ���̸��� �ְ�, ���� ���� �Ÿ������� �Է��ϴ� �Լ�
void input(Node* Subway) {
	int weight, num1, num2, w = 0;
	int i = 0, s, e, temp = 0;

	// ������ �ʱ�ȭ - ����� ������ŭ �ݺ���
	for (s = 0; s < N; s++) {
		for (e = 0; e < N; e++) {
			if (s == e) // ���� �����̶�� �̵��ð��� 0�̴�.
				data[s][e] = 0;
			else // �ִ���� �ʱ�ȭ��
				data[s][e] = M;
		}
	}

	char* filename = "data/���̸�.txt";
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
		/*printf("Number: %s, ", Subway->numbername[count]); // �׽�Ʈ
		printf("Name: %s\n", Subway->name[count]);*/
		count++;
	}
	fclose(fp);

	// ���̸� ���� ��� ������ �̸��� ������ �迭
	char* filename_list[19] = { "data/1����.csv", "data/1ȣ��.csv", "data/2����.csv", "data/2ȣ��.csv", "data/3ȣ��.csv", "data/4ȣ��.csv", "data/5����.csv",
						"data/5ȣ��.csv", "data/6ȣ��.csv", "data/7ȣ��.csv", "data/8ȣ��.csv", "data/9ȣ��.csv", "data/���Ǽ�.csv", "data/���ἱ.csv",
						"data/����ö��.csv", "data/�д缱.csv", "data/��õ1��.csv", "data/�߾Ӽ�.csv", "data/ȯ������.csv" };

	for (s = 0; s < 19; s++) {
		fp = fopen(filename_list[s], "r");
		if (!fp) {
			printf("Missing input file: %s\n", filename_list[s]);
			return -1;
		}
		
		i = 1; // �ε����� ���� ������ȣ�� ó���� data�� ä������ ����
		char seq[N][100]; // �ش� �뼱�� ��� ������ȣ�� �����ϴ� �迭
		fgets(line, 1024, fp); // ù ��° ������ ������ȣ���� ������ seq�� �����ϱ� ���� ���� ����
		count = 0;
		char* p = strtok(line, ",");
		while (p != NULL) {
			strcpy(seq[count], p); // strcpy�� ���� �ʰ� �׳� �����ϸ� ���� ���簡 �Ǿ� ������ �߻��� �� �ִ�.
			p = strtok(NULL, ",");
			count++;
		}
		
		while (!feof(fp)) {
			fgets(line, 1024, fp);
			char* p = strtok(line, ",");
			int start_number = find_index_bynumbername(p, Subway); // ������ȣ�� �ε����� ã�� �����ص�
			int end_number;

			count = 1;
			while(1) { // strtok�� �̿��� ���ʷ� ������ �ؼ�
				p = strtok(NULL, ",");
				if (p == NULL) break;
				weight = atoi(p); // �Ÿ������� int������ ��ȯ

				if (weight < M) { // 9999���� ������ ���� ����Ǿ� �ִ� �����̴�.
					end_number = find_index_bynumbername(seq[count], Subway); // ������ȣ ����� �̿��� ������ �ε����� ã���ش�.
					// printf("%s: %d\n", seq[count], end_number); // ���������� �ε����� ã�������� Ȯ��
					data[start_number][end_number] = weight; // �������� ����Ǿ� �����Ƿ� �ݴ��� ��쵵 �Ȱ��� �־��ش�.
					data[end_number][start_number] = weight;
				}
				count++;
			}
		}

		fclose(fp);
	}
}

// ��߿��� �������� ���� ������ȣ ã��
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
		printf("��߿��� �Է����ּ���: ");
		scanf_s("%s", &start, 100);

		printf("�������� �Է����ּ���: ");
		scanf_s("%s", &dest, 100);

		if (namecheck(start, Subway) && namecheck(dest, Subway))
			break;
		else
			printf("�ٽ� �Է��Ͻÿ�\n");

		// printf("%d %d\n", namecheck(start, Subway), namecheck(end, Subway));

	}while (1);
	
	find_start_and_dest(Subway, start, dest);
	/*printf("%s: %d\n", start, Subway->startnumber); // �ε����� �׽�Ʈ
	printf("%s: %d", dest, Subway->destnumber);*/

	printf("���? 1. �ִܰ�� 2. �ּ�ȯ��\n");
	scanf_s("%d", &way);
	switch (way)
	{
	case 1:
		printf("�ִܰ��\n");
		break;
	case 2:
		printf("�ּ�ȯ��\n");
		break;
	default:
		printf("�߸��� ���� �Է��߽��ϴ�. �����մϴ�.");
		break;
	}
}