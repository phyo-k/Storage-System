#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHIFT -3 //���Ͱ� �� á�� ����� �ű�� Ÿ��
#define BASIC -2 //���Ͱ� �� �� á������� �⺻ Ÿ��
#define RESET -1 //�ʱ�ȭ��
#define SECTOR 32 //1���� ����� 32�� ���ͷ� ����
#define BLOCK 64 //�� 64���
#define MAX 512 //���� 512����Ʈ�� ����
#define SIZE 32 //ũ�� ������ ��

enum boolean { FALSE, TRUE, BUFFER = 1 };

typedef struct flash {
	char byte[MAX / SIZE];
} flash;

typedef struct table {
	int lbn;
	int pbn;
} table;

void init(const int inclination, int* const megabyte);
void flash_read(const int lsn, int* const megabyte);
void flash_erase(const int lbn, int* const megabyte);
void flash_write(const int lsn, const char* string, int* const megabyte);
void ftl_read(FILE* fp, const int lsn, int* const megabyte);
void ftl_write(FILE* fp, const int lsn, const char* string, int* const megabyte);
void print_table(const int inclination, int* const megabyte);
void title();
void function();