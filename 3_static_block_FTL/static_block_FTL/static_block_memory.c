#include "block.h"

table* mapping;

void init(const int inclination, int* const megabyte)
{
	int i = 0;
	flash data;
	FILE* fp = fopen("static_block_flash_memory.txt", "wb");

	if (fp == NULL)
	{
		printf("������ �� �� �����ϴ�.\n");
		return;
	}

	if (inclination <= 0)
	{
		printf("�뷮�� 1�� �Է��ؾ� �����˴ϴ�.\n");
		return;
	}

	//�Ҵ��� MB : ���� * ��� * ����� ����
	*megabyte = SECTOR * BLOCK * inclination;

	//���� ���̺� ����
	mapping = (table*)malloc(*megabyte * sizeof(table));

	//���� ���̺� �ο�
	for (i = 0; i <= *megabyte - 1; i++)
	{
		if (mapping != NULL) {
			(mapping + i)->lbn = i;
			(mapping + i)->pbn = i % SECTOR;
		}
	}

	//���� ���� 00���� �ʱ�ȭ�մϴ�.
	memset(data.byte, 0x00, sizeof(data.byte));

	//������ ó������ ����.
	fseek(fp, 0, SEEK_SET);

	//�����͸� ���Ͽ� ����.
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
		printf("������ �� �� �����ϴ�.\n");
		return;
	}

	if (lbn <= -1)
	{
		printf("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		return;
	}
	else if (lbn > *megabyte - 1)
	{
		printf("%d�� ���Ͱ� �����ϴ�.\n", lbn);
		return;
	}

	fseek(fp, lbn * sizeof(data), SEEK_SET);
	fread(data.byte, sizeof(data), 1, fp);
	if (data.byte[0] == 0x00)
	{
		printf("�Էµ� �����Ͱ� �����ϴ�.\n");
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
		printf("������ �� �� �����ϴ�.\n");
		return;
	}

	if (lbn <= -1)
	{
		printf("����� �ùٸ��� �ʽ��ϴ�.\n");
		return;
	}
	
	else if (lbn > *megabyte - 1)
	{
		printf("%d�� ���Ͱ� �����ϴ�.\n", lbn);
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
		printf("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		return;
	}
	else if (lbn > (*megabyte - 1) / SECTOR)
	{
		printf("%d ����� �����ϴ�.\n", lbn);
		return;
	}

	memset(empty.byte, 0x00, sizeof(data.byte));

	//ó������ number��ŭ ����� �̵��Ѵ�.
	fseek(fp, SECTOR * lbn * sizeof(data), SEEK_SET);

	for (i = 0; i <= SECTOR; i++)
	{
		fwrite(empty.byte, sizeof(data), 1, fp);
		count++;
	}

	printf("block %d���� ���������ϴ�.\n", lbn);
	printf("����� ������ % dȸ �Ͼ���ϴ�.\n", count);

	fclose(fp);
}

void ftl_read(FILE* fp, const int lbn, int* const megabyte)
{
	int i;
	int cnt = 0;
	const int sector_offset = lbn % SECTOR;
	const int block = lbn / SECTOR;
	flash data;

	//����ü �ʱ�ȭ
	memset(data.byte, 0x00, sizeof(data.byte));

	//ó������ lbn��ŭ ���͸� �̵��Ѵ�.
	//fseek(fp, (mapping + lbn)->pbn * sizeof(data), SEEK_SET);
	fseek(fp, ((mapping + block)->pbn * SECTOR + sector_offset) * sizeof(data), SEEK_SET);

	//����ü�� �о�´�.
	fread(&data, sizeof(data), 1, fp);

	printf("����� %d�� block�Դϴ�.\n", block);
	printf("�������� %d�� sector�Դϴ�.\n", sector_offset);
	/*printf("data : ");

	for (i = 0; i <= sizeof(data.byte) - 1; i++)
	{
		printf("%2c ", data.byte[i]);
	}*/

	for (int n = 0; n < *megabyte - 1; n++) {
		cnt++;
		if ((mapping + block)->pbn == n) {
			printf("%d�� �о����ϴ�.", cnt);
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
	int check = 0, check1 = 0; //�Űܾ� Ƚ�� ����
	int next_block_lbn = 0; //�Ű� �� ���
	const int sector_offset = lbn % SECTOR;
	const int block = lbn / SECTOR;
	const int limit = (int)(lbn / SECTOR) + 1; //���Ͱ� �ٸ� ������� �Ѿ���� �ʵ��� ���� ����
	int indicator = (mapping + block)->pbn * SECTOR + sector_offset; //���������� ������
	flash data;

	fseek(fp, indicator * sizeof(data), SEEK_SET);
	fread(data.byte, sizeof(data), 1, fp);

	//���� ���Ϳ� ����, �ٸ� ������� ���� �۾�
	if (data.byte[0] != 0x00)
	{
		//����� �� ��� �˻�
		for (i = *megabyte; i >= 0; i--)
		{
			count = 0;

			//�� ���� ã��
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

			//�� ����� ã����, �� ����� ���۷� ����
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
			//���� ���� �б�
			fseek(fp, ((mapping + block)->pbn + indicator) * sizeof(data), SEEK_SET);
			fread(data.byte, sizeof(data), 1, fp);

			//�� ���͸� �ǳʶ�
			if (data.byte[0] == 0x00)
			{
				continue;
			}

			//�ӽ� ���Ϳ� ����
			//check++;
			count++;
			//mapping + next_block_lan)->lsn
			fseek(fp, next_block_lbn * sizeof(data), SEEK_SET); //���� ������� ������� �̵�
			fwrite(data.byte, sizeof(data), 1, fp); //���� ������� ����
			(mapping + indicator)->pbn = (mapping + next_block_lbn)->lbn;//
			next_block_lbn++;

			//�ֽ� ������ �ϰ��
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

		//���� ������ �����
		flash_erase(block, megabyte);

		indicator = 0;

		//�ӽ� �����͸� ���� ������ ����
		for (next_block_lbn = i * SECTOR; next_block_lbn <= (i + 1) * SECTOR; next_block_lbn++)
		{
			fseek(fp, next_block_lbn * sizeof(data), SEEK_SET);
			fread(data.byte, sizeof(data), 1, fp);

			if (data.byte[0] == 0x00)
			{
				continue;
			}

			count++;
			fseek(fp, (limit - 1 + indicator) * sizeof(data), SEEK_SET); //���� ������� ������� �̵�
			if (sector_offset == indicator)
			{
				fwrite(string, strlen(string), 1, fp);
			}
			fwrite(data.byte, sizeof(data), 1, fp); //���� ������� ����
			indicator++;
		}

		//�ӽ� ������ �����
		flash_erase(i, megabyte);
	}
	else
	{
		//������ ���� ����
		fseek(fp, indicator * sizeof(data), SEEK_SET);
		fwrite(string, strlen(string), 1, fp);
		count1++;
		(mapping + block)->pbn = block;
	}

	flash_read(lbn, megabyte);
	printf("���� ������ %dȸ �Ͼ���ϴ�.\n", count+count1);
}

void print_table(const int inclination, int* const megabyte)
{
	int i;

	if (inclination > *megabyte * BLOCK)
	{
		return;
	}

	printf("| %4d�� ��� ���� ���̺�                        |\n", inclination);
	printf("| %10s %10s || %10s %10s |\n", "lbn", "pbn", "lbn", "pbn");

	for (i = inclination * SECTOR; i <= (inclination + 1) * SECTOR - 1-16; i++)
	{
		printf("| %10d %10d || %10d %10d |\n", mapping[i].lbn, mapping[i].pbn, mapping[i + 16].lbn, mapping[i + 16].pbn);
	}
}