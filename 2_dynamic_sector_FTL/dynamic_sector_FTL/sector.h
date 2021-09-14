#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strcmp
#include <assert.h>
//#include <time.h>

#define SHIFT (-3) //섹터가 꽉 찼을 경우의 옮기는 타입
#define BASIC (-2) //섹터가 꽉 안 찼을경우의 기본 타입
#define RESET (-1) //초기화값
#define FALSE (0)
#define TRUE (1)
#define SECTOR (32) //1개의 블록은 32개 섹터로 구성
#define BLOCK (64) //32 * 64 * 512 = 1048576바이트다. 즉 계산하면 1MB이다.
#define MAX (512) //섹터 크기는 512 bytes이다.
#define SIZE (32) //하지만 512 bytes로 하면 파일의 크기가 너무 크므로, 임의로 줄인다.
//  따라서 총 블록의 수: 64개, 섹터 개수: 2048개, 한 섹터 당 바이트 크기: 16바이트
//  여유 공간(Spare Block)은 하나로 둠
//  정적 매핑에서 여유 공간은 마지막 블록
//  동적 매핑에서 여유 공간은 탐색하다가 빈 한 블록

typedef struct flash {
	char byte[MAX / SIZE]; //기본 바이트 크기 16
} flash;

typedef struct table {
	int lsn; //논리 섹터 넘버
	int psn; //물리 섹터 넘버
} table;

void init(const int inclination, int* const megabyte);
void flash_read(const int lsn, int* const megabyte);
void flash_write(const int lsn, const char* string, int* const megabyte);
void flash_erase(const int lbn, int* const megabyte);
void ftl_read(FILE* fp, const int lsn, int* const megabyte);
void ftl_write(FILE* fp, const int lsn, const char* string, int* const megabyte);
void print_table(const int inclination, int* const megabyte);
void title();
void function();