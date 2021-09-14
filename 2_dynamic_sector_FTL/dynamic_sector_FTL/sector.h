#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strcmp
#include <assert.h>
//#include <time.h>

#define SHIFT (-3) //���Ͱ� �� á�� ����� �ű�� Ÿ��
#define BASIC (-2) //���Ͱ� �� �� á������� �⺻ Ÿ��
#define RESET (-1) //�ʱ�ȭ��
#define FALSE (0)
#define TRUE (1)
#define SECTOR (32) //1���� ����� 32�� ���ͷ� ����
#define BLOCK (64) //32 * 64 * 512 = 1048576����Ʈ��. �� ����ϸ� 1MB�̴�.
#define MAX (512) //���� ũ��� 512 bytes�̴�.
#define SIZE (32) //������ 512 bytes�� �ϸ� ������ ũ�Ⱑ �ʹ� ũ�Ƿ�, ���Ƿ� ���δ�.
//  ���� �� ����� ��: 64��, ���� ����: 2048��, �� ���� �� ����Ʈ ũ��: 16����Ʈ
//  ���� ����(Spare Block)�� �ϳ��� ��
//  ���� ���ο��� ���� ������ ������ ���
//  ���� ���ο��� ���� ������ Ž���ϴٰ� �� �� ���

typedef struct flash {
	char byte[MAX / SIZE]; //�⺻ ����Ʈ ũ�� 16
} flash;

typedef struct table {
	int lsn; //�� ���� �ѹ�
	int psn; //���� ���� �ѹ�
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