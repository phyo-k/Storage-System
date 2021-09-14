#include "sector.h"

table* mapping;

void init(const int inclination, int* const megabyte)
{
	int i = 0;
	flash data;
	FILE* fp = fopen("static_sector_flash_memory.txt", "wb");

	if (fp == NULL)
	{
		printf("������ �� �� �����ϴ�.\n");
		return;
	}

	if (inclination <= 0)//�뷮 ����
	{
		printf("�뷮�� 1�� �Է��ؾ� �����˴ϴ�.\n");
		return;
	}

	//�Ҵ��� MB : ���� * ��� * ����� ����
	*megabyte = SECTOR * BLOCK * inclination;

	//���� ���̺� ����
	mapping = (table*)malloc(*megabyte * sizeof(table));

	//�⺻ ���� ���̺� �ο�, ���� ���� �ѹ��� ���� �������� �ʾ����Ƿ� FACTORY_RESET�� ����
	for (i = 0; i <= *megabyte - 1; i++)
	{
		if (mapping != NULL) {
			(mapping + i)->lsn = i;
			(mapping + i)->psn = RESET;
		}
	}

	//���� hex���� 00���� �ʱ�ȭ�մϴ�.
	memset(data.byte, 0x00, sizeof(data.byte));

	//������ ó������ ����. SEEK_SET
	fseek(fp, 0, SEEK_SET);

	//�����͸� ���Ͽ� ����. 16����Ʈ�� �� �ٷ� ����
	for (i = 0; i < *megabyte; i++)
	{
		//���� 1���� ������ ũ��� 16����Ʈ ũ���̸�, �� ���Ҵ� data.byte �迭�� ����. �� �迭�� fp�� ��
		fwrite(&data, sizeof(data), 1, fp);
	}

	printf("%d MB flash memory\n", inclination);

	fclose(fp);
}

void flash_read(const int lsn, int* const megabyte)
{
	FILE* fp = fopen("static_sector_flash_memory.txt", "rb");

	if (fp == NULL)
	{
		printf("������ �� �� �����ϴ�.\n");
		return;
	}

	if (lsn <= -1)
	{
		printf("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		return;
	}
	else if (lsn > * megabyte - 1)
	{
		printf("%d�� ���Ͱ� �����ϴ�.\n", lsn);
		return;
	}
	else if ((mapping + lsn)->psn == RESET) //�ش� ��ġ�� -1�� ��. �����Ͱ� �� ���
	{
		printf("�Էµ� �����Ͱ� �����ϴ�.\n");
		return;
	}

	ftl_read(fp, lsn, megabyte);

	fclose(fp);
}

void flash_write(const int lsn, const char* string, int* const megabyte)
{
	FILE* fp = fopen("static_sector_flash_memory.txt", "rb+");

	if (fp == NULL)
	{
		printf("������ �� �� �����ϴ�.\n");
		return;
	}

	if (lsn <= -1)
	{
		printf("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		return;
	}
	else if (lsn > * megabyte - 1)
	{
		printf("%d ���Ͱ� �����ϴ�.\n", lsn);
		return;
	}

	ftl_write(fp, lsn, string, megabyte);
	//end = clock();
	//printf("���� �ð� : %f��\n", (float)(end - start) / CLOCKS_PER_SEC);

	fclose(fp);
}

void flash_erase(const int lbn, int* const megabyte)
{
	int i;
	int count = 0;
	flash data;
	FILE* fp = fopen("static_sector_flash_memory.txt", "rb+");

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

	//flash������ data�� �����ϰ� data�� byte�� 16����Ʈ �� ���� �̸� 0���� �ʱ�ȭ
	memset(data.byte, 0x00, sizeof(data.byte));

	//ó������ number��ŭ ����� �̵��Ѵ�.
	fseek(fp, SECTOR * lbn * sizeof(data), SEEK_SET);

	for (i = 0; i <= SECTOR - 1; i++)
	{
		fwrite(&data, sizeof(data), 1, fp);
		count++;
	}

	printf("block %d���� ���������ϴ�.\n", lbn);
	printf("����� ������ % dȸ �Ͼ���ϴ�.\n", count);

	fclose(fp);
}

void ftl_read(FILE* fp, const int lsn, int* const megabyte)
{
	int i;
	int cnt = 0;
	flash data;

	//����ü �ʱ�ȭ
	memset(data.byte, 0x00, sizeof(data.byte));

	//ó������ lsn��ŭ ���͸� �̵��Ѵ�.
	fseek(fp, (mapping + lsn)->psn * sizeof(data), SEEK_SET);

	//����ü�� �о�´�.
	fread(&data, sizeof(data), 1, fp);

	/*printf("���ڿ� : ");

	for (i = 0; i <= sizeof(data.byte) - 1; i++)
	{
		printf("%2c ", data.byte[i]);
	}*/

	for (int n = 0; n < *megabyte - 1; n++) {
		cnt++;
		if ((mapping + lsn)->psn == n) {
			printf("%d�� �о����ϴ�.\n", cnt);
			break;
		}
	}
}

void ftl_write(FILE* fp, const int lsn, const char* string, int* const megabyte)
{
	int i, j;
	int type = BASIC;
	int indicator;
	int count = 0;
	int check = 0; //�Űܾ� Ƚ�� ����
	int next_block_lsn = 0; //�Ű� �� ���
	const int limit = (int)(lsn / SECTOR) + 1; //���Ͱ� �ٸ� ������� �Ѿ���� �ʵ��� ���� ����
	flash data;

	fseek(fp, lsn * sizeof(data), SEEK_SET);
	fread(data.byte, sizeof(data), 1, fp);

	//���� indicator�� �� ���� �ѹ��� ���� ����Ű�� ��ġ �������̴�.
	for (indicator = lsn; (mapping + lsn)->psn <= limit * SECTOR; indicator++)
	{
		count++;

		//�� ��Ͽ��� ��� ���Ͱ� ���� ����������
		if (count == SECTOR)
		{
			//����� �� ����� ó������ ���ʴ�� �˻��Ѵ�.
			for (i = *megabyte; i >= 0; i--)
			{
				//���� ����̸� �ǳ� ��
				if (lsn / SECTOR == i)
				{
					continue;
				}

				count = 0;

				//�� ��Ͽ��� �� ���� �˻��ϴ� ����
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

				//�� ����� ������ ������� �����Ѵ�.
				if (count == SECTOR)
				{
					next_block_lsn = i * SECTOR;

					break;
				}
			}

			count = 0;

			//�� ����� ������ ������ �����Ѵ�.
			for (indicator = (limit - 1) * SECTOR; indicator <= limit * SECTOR - 1; indicator++)
			{
				count++;
				//���� �����͸� �о�´�.
				fseek(fp, (mapping + indicator)->psn * sizeof(data), SEEK_SET);
				fread(data.byte, sizeof(data), 1, fp);

				if (data.byte[0] == 0x00)
				{
					next_block_lsn++;
					continue;
				}

				//NULL���� �ƴϸ� check�� +1, ���ο� ������� �����͸� �����Ѵ�.
				check++;
				fseek(fp, (mapping + next_block_lsn)->lsn * sizeof(data), SEEK_SET); //���� ������� ������� �̵�
				fwrite(data.byte, sizeof(data), 1, fp); //���� ������� ����
				(mapping + indicator)->psn = (mapping + next_block_lsn)->lsn;
				next_block_lsn++;

				if (count == SECTOR)
				{
					break;
				}
			}

			type = SHIFT;
			break;
		}
		//�� ��Ͽ��� ��� ���͸� ��� ������ ���
		else
		{
			if (indicator >= limit * SECTOR)
			{
				indicator = (limit - 1) * SECTOR;
			}

			fseek(fp, indicator * sizeof(data), SEEK_SET);
			fread(data.byte, sizeof(data), 1, fp);


			//�����Ͱ� ��ϵǾ����� ��� ���� ���Ϳ� ����, ���� ��� �� ������ ���� ����.
			if (data.byte[0] != 0x00)
			{
				//continue;
				//����� �� ����� ó������ ���ʴ�� �˻��Ѵ�.
				for (i = *megabyte; i >= 0; i--)
				{
					//���� ����̸� �ǳ� ��
					if (lsn / SECTOR == i)
					{
						continue;
					}

					count = 0;

					//�� ��Ͽ��� �� ���� �˻��ϴ� ����
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

					//�� ����� ������ ������� �����Ѵ�.
					if (count == SECTOR)
					{
						next_block_lsn = i * SECTOR;

						break;
					}
				}

				count = 0;

				//�� ����� ������ ������ �����Ѵ�.
				for (indicator = (limit - 1) * SECTOR; indicator <= limit * SECTOR - 1; indicator++)
				{
					count++;
					//���� �����͸� �о�´�.
					fseek(fp, (mapping + indicator)->psn * sizeof(data), SEEK_SET);
					fread(data.byte, sizeof(data), 1, fp);

					if (data.byte[0] == 0x00)
					{
						next_block_lsn++;
						continue;
					}

					//NULL���� �ƴϸ� check�� +1, ���ο� ������� �����͸� �����Ѵ�.
					check++;
					fseek(fp, (mapping + next_block_lsn)->lsn * sizeof(data), SEEK_SET); //���� ������� ������� �̵�
					fwrite(data.byte, sizeof(data), 1, fp); //���� ������� ����
					(mapping + indicator)->psn = (mapping + next_block_lsn)->lsn;
					next_block_lsn++;

					if (count == SECTOR)
					{
						break;
					}
				}
				type = SHIFT;
				break;
			}
			else
			{
				(mapping + lsn)->psn = indicator; //�������̺� �ֽ�
				//TODO : �Űܾ��� ����, ���� ���Ϳ� �����͸� ����, ���������ʹ� �����, �������̺��� -1�� �ٲ۴�. 
				type = BASIC;

				break;
			}

		}
	}

	//��ɿ��� �Է��� ������ �ִ� ����
	if (type == SHIFT)
	{
		fseek(fp, next_block_lsn * sizeof(data), SEEK_SET);
		fwrite(string, strlen(string), 1, fp);
		(mapping + indicator)->psn = (mapping + next_block_lsn)->lsn;

		//���� ����� �����.
		flash_erase(lsn / SECTOR, megabyte);

		printf("%d�� ������� �Ű������ϴ�.\n", next_block_lsn / SECTOR);
	}
	else if (type == BASIC)
	{
		fseek(fp, indicator * sizeof(data), SEEK_SET);
		fwrite(string, strlen(string), 1, fp);
	}

	flash_read(lsn, megabyte);
	check++;
	printf("���� ������ %dȸ �Ͼ���ϴ�.\n", check);
}

void print_table(const int inclination, int* const megabyte)
{
	//�� �Լ��� �⺻ ����Ʈ ũ�Ⱑ 16����Ʈ�� ����
	assert(MAX / SIZE == 16);

	int i;

	if (inclination <= -1 || inclination >= *megabyte / SECTOR)
	{
		printf("���� �ùٸ��� �ʽ��ϴ�.\n");

		return;
	}

	printf("| %4d�� ��� ���� ���̺�                        |\n", inclination);
	printf("| %10s %10s || %10s %10s |\n", "lsn", "psn", "lsn", "psn");

	for (i = inclination * SECTOR; i <= (inclination + 1) * SECTOR - 1 - 16; i++)
	{
		printf("| %10d %10d || %10d %10d |\n", mapping[i].lsn, mapping[i].psn, mapping[i + 16].lsn, mapping[i + 16].psn);
	}
}