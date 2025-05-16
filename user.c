#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

USER users[MAX_USERS];
int userCount = 0;

void loadUsersFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("사용자 파일 열기 실패: %s\n", filename);
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (userCount >= MAX_USERS)
            break;

        char *id = strtok(line, ":");
        char *pw = strtok(NULL, "\n");

        if (id != NULL && pw != NULL)
        {
            strncpy(users[userCount].id, id, sizeof(users[userCount].id));
            strncpy(users[userCount].pw, pw, sizeof(users[userCount].pw));
            userCount++;
        }
    }

    fclose(fp);
}

int login()
{
    char inputId[20];
    char inputPw[20];
    int attempts = 0;

    while (attempts < 5)
    {
        printf("\n[로그인 시도 %d/5]\n", attempts + 1);
        printf("ID 입력: ");
        scanf("%s", inputId);
        while (getchar() != '\n')
            ;

        printf("비밀번호 입력: ");
        scanf("%s", inputPw);
        while (getchar() != '\n')
            ;

        for (int i = 0; i < userCount; i++)
        {
            if (strcmp(users[i].id, inputId) == 0 &&
                strcmp(users[i].pw, inputPw) == 0)
            {
                printf("로그인 성공: %s\n", inputId);
                return 1;
            }
        }

        printf("로그인 실패. 다시 시도하세요.\n");
        attempts++;
    }

    printf("5회 이상 실패하여 프로그램을 종료합니다.\n");
    return 0;
}