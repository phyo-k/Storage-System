#include "block.h"

table* mapping;

void init(const int inclination, int* const megabyte)
{
	int i = 0;
	flash data;
	FILE* fp = fopen("static_block_flash_memory.txt", "wb");

	if (fp == NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		return;
	}

	if (inclination <= 0)
	{
		printf("용량은 1을 입력해야 생성됩니다.\n");
		return;
	}

	//할당할 MB : 섹터 * 블록 * 사용자 의향
	*megabyte = SECTOR * BLOCK * inclination;

	//매핑 테이블 생성
	mapping = (table*)malloc(*megabyte * sizeof(table));

	//매핑 테이블 부여
	for (i = 0; i <= *megabyte - 1; i++)
	{
		if (mapping != NULL) {
			(mapping + i)->lbn = i;
			(mapping + i)->pbn = i % SECTOR;
		}
	}

	//전부 값을 00으로 초기화합니다.
	memset(data.byte, 0x00, sizeof(data.byte));

	//파일의 처음으로 간다.
	fseek(fp, 0, SEEK_SET);

	//데이터를 파일에 쓴다.
	for (i = 0; i < *megabyte; i++)
	{
		fwrite(&data, sizeof(data), 1, fp);
	}

	printf("%d MB flash memory\n", inclination);

	fclose(fp);
}

void flash_read(const int lbn, int* const megabyte)
{
	flash data;
	FILE* fp = fopen("static_block_flash_memory.txt", "rb");

	if (fp == NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		return;
	}

	if (lbn <= -1)
	{
		printf("명령 구문이 올바르지 않습니다.\n");
		return;
	}
	else if (lbn > *megabyte - 1)
	{
		printf("%d번 섹터가 없습니다.\n", lbn);
		return;
	}

	fseek(fp, lbn * sizeof(data), SEEK_SET);
	fread(data.byte, sizeof(data), 1, fp);
	if (data.byte[0] == 0x00)
	{
		printf("입력된 데이터가 없습니다.\n");
		return;
	}

	ftl_read(fp, lbn, megabyte);

	fclose(fp);
}

void flash_write(const int lbn, const char* string, int* const megabyte)
{
	FILE* fp = fopen("static_block_flash_memory.txt", "rb+");

	if (fp == NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		return;
	}

	if (lbn <= -1)
	{
		printf("명령이 올바르지 않습니다.\n");
		return;
	}
	
	else if (lbn > *megabyte - 1)
	{
		printf("%d번 섹터가 없습니다.\n", lbn);
		return;
	}

	ftl_write(fp, lbn, string, megabyte);
	
	fclose(fp);
}

void flash_erase(const int lbn, int* const megabyte)
{
	int i;
	int count = 0;
	flash data;
	flash empty;
	FILE* fp = fopen("static_block_flash_memory.txt", "rb+");

	if (lbn <= -1)
	{
		printf("명령 구문이 올바르지 않습니다.\n");
		return;
	}
	else if (lbn > (*megabyte - 1) / SECTOR)
	{
		printf("%d 블록이 없습니다.\n", lbn);
		return;
	}

	memset(empty.byte, 0x00, sizeof(data.byte));

	//처음에서 number만큼 블록을 이동한다.
	fseek(fp, SECTOR * lbn * sizeof(data), SEEK_SET);

	for (i = 0; i <= SECTOR; i++)
	{
		fwrite(empty.byte, sizeof(data), 1, fp);
		count++;
	}

	printf("block %d번이 지워졌습니다.\n", lbn);
	printf("지우기 연산은 % d회 일어났습니다.\n", count);

	fclose(fp);
}

void ftl_read(FILE* fp, const int lbn, int* const megabyte)
{
	int i;
	int cnt = 0;
	const int sector_offset = lbn % SECTOR;
	const int block = lbn / SECTOR;
	flash data;

	//구조체 초기화
	memset(data.byte, 0x00, sizeof(data.byte));

	//처음에서 lbn만큼 섹터를 이동한다.
	//fseek(fp, (mapping + lbn)->pbn * sizeof(data), SEEK_SET);
	fseek(fp, ((mapping + block)->pbn * SECTOR + sector_offset) * sizeof(data), SEEK_SET);

	//구조체를 읽어온다.
	fread(&data, sizeof(data), 1, fp);

	printf("블록은 %d번 block입니다.\n", block);
	printf("오프셋은 %d번 sector입니다.\n", sector_offset);
	/*printf("data : ");

	for (i = 0; i <= sizeof(data.byte) - 1; i++)
	{
		printf("%2c ", data.byte[i]);
	}*/

	for (int n = 0; n < *megabyte - 1; n++) {
		cnt++;
		if ((mapping + block)->pbn == n) {
			printf("%d번 읽었습니다.", cnt);
			break;
		}
	}

	printf("\n");
}

void ftl_write(FILE* fp, const int lbn, const char* string, int* const megabyte)
{
	int i, j;
	int type = BASIC;
	int count = 0, count1 = 0;
	int check = 0, check1 = 0; //옮겨쓴 횟수 저장
	int next_block_lbn = 0; //옮겨 쓸 블록
	const int sector_offset = lbn % SECTOR;
	const int block = lbn / SECTOR;
	const int limit = (int)(lbn / SECTOR) + 1; //섹터가 다른 블록으로 넘어까지 않도록 제한 설정
	int indicator = (mapping + block)->pbn * SECTOR + sector_offset; //파일포인터 지시자
	flash data;

	fseek(fp, indicator * sizeof(data), SEEK_SET);
	fread(data.byte, sizeof(data), 1, fp);

	//같은 섹터에 쓰면, 다른 블록으로 복사 작업
	if (data.byte[0] != 0x00)
	{
		//복사용 빈 블록 검사
		for (i = *megabyte; i >= 0; i--)
		{
			count = 0;

			//빈 섹터 찾기
			for (j = 0; j <= SECTOR - 1; j++)
			{
				fseek(fp, (i * SECTOR + j) * sizeof(data), SEEK_SET);
				fread(data.byte, sizeof(data), 1, fp);

				if (data.byte[0] == 0x00)
				{
					count++;
					continue;
				}
				else
				{
					break;
				}
			}

			//빈 블록을 찾으면, 이 블록을 버퍼로 설정
			if (count == SECTOR)
			{
				next_block_lbn = i * SECTOR;

				break;
			}
		}

		count = 0;

		for (indicator = (limit - 1) * SECTOR; indicator <= limit * SECTOR - 1; indicator++)
		{
			count++;
			//기존 섹터 읽기
			fseek(fp, ((mapping + block)->pbn + indicator) * sizeof(data), SEEK_SET);
			fread(data.byte, sizeof(data), 1, fp);

			//빈 섹터면 건너뜀
			if (data.byte[0] == 0x00)
			{
				continue;
			}

			//임시 섹터에 쓰기
			//check++;
			count++;
			//mapping + next_block_lan)->lsn
			fseek(fp, next_block_lbn * sizeof(data), SEEK_SET); //다음 블록으로 순서대로 이동
			fwrite(data.byte, sizeof(data), 1, fp); //다음 블록으로 복사
			(mapping + indicator)->pbn = (mapping + next_block_lbn)->lbn;//
			next_block_lbn++;

			//최신 데이터 일경우
			if (indicator == sector_offset)
			{
				fseek(fp, next_block_lbn * sizeof(data), SEEK_SET);
				fwrite(string, strlen(string), 1, fp);
				next_block_lbn++;
				count++;
				continue;
			}

			if (next_block_lbn >= (limit + 1) * SECTOR)
			{
				break;
			}
		}

		fseek(fp, next_block_lbn * sizeof(data), SEEK_SET);
		fwrite(string, strlen(string), 1, fp);

		//기존 데이터 지우기
		flash_erase(block, megabyte);

		indicator = 0;

		//임시 데이터를 기존 데이터 복사
		for (next_block_lbn = i * SECTOR; next_block_lbn <= (i + 1) * SECTOR; next_block_lbn++)
		{
			fseek(fp, next_block_lbn * sizeof(data), SEEK_SET);
			fread(data.byte, sizeof(data), 1, fp);

			if (data.byte[0] == 0x00)
			{
				continue;
			}

			count++;
			fseek(fp, (limit - 1 + indicator) * sizeof(data), SEEK_SET); //다음 블록으로 순서대로 이동
			if (sector_offset == indicator)
			{
				fwrite(string, strlen(string), 1, fp);
			}
			fwrite(data.byte, sizeof(data), 1, fp); //다음 블록으로 복사
			indicator++;
		}

		//임시 데이터 지우기
		flash_erase(i, megabyte);
	}
	else
	{
		//실제로 쓴거 쓰기
		fseek(fp, indicator * sizeof(data), SEEK_SET);
		fwrite(string, strlen(string), 1, fp);
		count1++;
		(mapping + block)->pbn = block;
	}

	flash_read(lbn, megabyte);
	printf("쓰기 연산이 %d회 일어났습니다.\n", count+count1);
}

void print_table(const int inclination, int* const megabyte)
{
	int i;

	if (inclination > *megabyte * BLOCK)
	{
		return;
	}

	printf("| %4d번 블록 매핑 테이블                        |\n", inclination);
	printf("| %10s %10s || %10s %10s |\n", "lbn", "pbn", "lbn", "pbn");

	for (i = inclination * SECTOR; i <= (inclination + 1) * SECTOR - 1-16; i++)
	{
		printf("| %10d %10d || %10d %10d |\n", mapping[i].lbn, mapping[i].pbn, mapping[i + 16].lbn, mapping[i + 16].pbn);
	}
}