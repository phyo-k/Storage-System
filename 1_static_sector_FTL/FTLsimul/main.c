// �� ����� �� ä������ ���� ���, �� ���Ϳ� ��������Ʈ�� �߻����� ��, ���ڿ� ��ȯ�� �ȵ�

#define _CRT_SECURE_NO_WARNINGS
#include "sector.h"

void title() 
{
	printf("-----------------------------------------------------------------------\n");
	printf("|                         <���� ���� ����>                            |\n");
	printf("-----------------------------------------------------------------------\n");
}

void function()
{
	printf("| -- ���� ��ȣ�� ����, �Է� �����ʹ� �����Դϴ�.                      |\n");
	printf("| ** �޸� �б��               (r ���͹�ȣ)�� �Է��ϼ���.           |\n");
	printf("| ** �޸𸮿� ������ �����      (w ���͹�ȣ ������)�� �Է��ϼ���.    |\n");
	printf("| ** �޸��� ��� ������        (e ������_��Ϲ�ȣ)�� �Է��ϼ���.    |\n");
	printf("| ** ���� ���̺� �����          (p �����_��Ϲ�ȣ)�� �Է��ϼ���.    |\n");
	printf("| ** ���α׷� �����              exit�� �Է��ϼ���.                  |\n");
	printf("-----------------------------------------------------------------------\n");
}

int main(void)
{
	int psn;
	int megabyte = 0;
	char command[10];
	char string[MAX / SIZE + 1];

	title();
	function();

	printf("| ** 1MB �޸𸮸� �����մϴ�. 1�� �Է��ϼ���.                         |\n");
	printf(">> ");
	scanf("%d", &psn);
	init(psn, &megabyte);

	while (TRUE)
	{
		printf(">> ");
		scanf("%s", command);

		if (strcmp(command, "r") == 0)
		{
			scanf("%d", &psn);
			flash_read(psn, &megabyte);
		}
		else if (strcmp(command, "w") == 0)
		{
			scanf("%d  %s", &psn, string);
			flash_write(psn, string, &megabyte);
		}
		else if (strcmp(command, "e") == 0)
		{
			scanf("%d", &psn);
			flash_erase(psn, &megabyte);
		}
		else if (strcmp(command, "p") == 0)
		{
			scanf("%d", &psn);
			print_table(psn, &megabyte);
		}
		else if ((strcmp(command, "exit") == 0)) break;
		else printf("��ɾ �߸��Ǿ����ϴ�.\n");
	}
	return 0;
}