#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHIFT -3 //섹터가 꽉 찼을 경우의 옮기는 타입
#define BASIC -2 //섹터가 꽉 안 찼을경우의 기본 타입
#define RESET -1 //초기화값
#define SECTOR 32 //1개의 블록은 32개 섹터로 구성
#define BLOCK 64 //총 64블록
#define MAX 512 //보통 512바이트로 구성
#define SIZE 32 //크기 조정용 값

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