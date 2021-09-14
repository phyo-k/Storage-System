// 한 블록이 다 채워지지 않은 경우, 한 섹터에 오버라이트가 발생했을 때, 문자열 반환이 안됨

#define _CRT_SECURE_NO_WARNINGS
#include "sector.h"

void title() 
{
	printf("-----------------------------------------------------------------------\n");
	printf("|                         <정적 섹터 매핑>                            |\n");
	printf("-----------------------------------------------------------------------\n");
}

void function()
{
	printf("| -- 섹터 번호는 정수, 입력 데이터는 문자입니다.                      |\n");
	printf("| ** 메모리 읽기는               (r 섹터번호)를 입력하세요.           |\n");
	printf("| ** 메모리에 데이터 쓰기는      (w 섹터번호 데이터)를 입력하세요.    |\n");
	printf("| ** 메모리의 블록 삭제는        (e 삭제할_블록번호)를 입력하세요.    |\n");
	printf("| ** 매핑 테이블 출력은          (p 출력할_블록번호)를 입력하세요.    |\n");
	printf("| ** 프로그램 종료는              exit를 입력하세요.                  |\n");
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

	printf("| ** 1MB 메모리를 생성합니다. 1을 입력하세요.                         |\n");
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
		else printf("명령어가 잘못되었습니다.\n");
	}
	return 0;
}