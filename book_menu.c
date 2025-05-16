#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "book_menu.h"
#include "common_input.h"

#define BUF_SIZE 1000

void printStatusBar()
{
    printf("[ESC: 입력 취소]  [Enter: 확인]  [0: 프로그램 종료]\n");
    printf("─────────────────────────────────────────────────────\n");
}

void bookMenu(SOCKET sock)
{
    int sel;
    char buffer[BUF_SIZE];
    char field[20], keyword[50];

    while (1)
    {
        clearScreen();
        printf("=================== 도서관리 메뉴 ===================\n");
        printf("1. 도서 검색\n");
        printf("2. 도서 추가\n");
        printf("3. 도서 삭제\n");
        printf("4. 도서 수정\n");
        printf("=====================================================\n");
        printStatusBar();
        printf(">> 선택: ");
        if (scanf("%d", &sel) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar();

        if (sel == 0)
        {
            printf("[알림] 프로그램을 종료합니다.\n");
            Sleep(1000); // 1초 대기 후 종료
            exit(0); // 프로그램 종료
        }

        if (sel == 1)
        {
            char choice[10];
            getEscapableInput(choice, sizeof(choice), "검색 기준 선택\n1. 제목\n2. 저자", 0);
            if (choice[0] == '\0')
                continue;
            int fieldSel = atoi(choice);
            if (fieldSel != 1 && fieldSel != 2)
                continue;
            strcpy(field, (fieldSel == 1) ? "title" : "author");

            getEscapableInput(keyword, sizeof(keyword), "검색어", 0);
            if (keyword[0] == '\0')
                continue;

            snprintf(buffer, sizeof(buffer), "SEARCH:%s:%s", field, keyword);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            clearScreen();
            printf("[도서 목록]\n");
            printf(" ID  | 제목                                                        | 저자                             | 평점\n");
            printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
            printf("%s\n", buffer);
            printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
            getchar();
        }

        if (sel == 2)
        {
            char title[50], author[50], ratingStr[10];
            float rating;

            getEscapableInput(title, sizeof(title), "도서 제목", 0);
            if (title[0] == '\0')
                continue;

            getEscapableInput(author, sizeof(author), "도서 저자", 0);
            if (author[0] == '\0')
                continue;

            while (1)
            {
                getEscapableInput(ratingStr, sizeof(ratingStr), "도서 평점 (0.0 ~ 5.0)", 0);
                if (ratingStr[0] == '\0')
                    break;
                rating = atof(ratingStr);
                if (rating < 0.0 || rating > 5.0)
                {
                    printf("[오류] 평점은 0.0에서 5.0 사이로 입력해야 합니다.\n");
                    continue;
                }
                break;
            }
            if (ratingStr[0] == '\0')
                continue;

            snprintf(buffer, sizeof(buffer), "ADD:%s:%s:%.2f", title, author, rating);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            clearScreen();
            printf("%s\n", buffer);
            printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
            getchar();
        }

        if (sel == 3)
        {
            char idStr[10];
            int delId;
            getEscapableInput(idStr, sizeof(idStr), "삭제할 도서 ID", 0);
            if (idStr[0] == '\0')
                continue;
            delId = atoi(idStr);
            if (delId <= 0)
            {
                printf("[오류] 유효한 도서 ID를 입력하세요.\n");
                printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
                getchar();
                continue;
            }
            snprintf(buffer, sizeof(buffer), "DELETE_BOOK:%d", delId);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            clearScreen();
            printf("%s\n", buffer);
            printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
            getchar();
        }

        if (sel == 4)
        {
            char idStr[10];
            int modId;
            char newValue[100], field[10];

            getEscapableInput(idStr, sizeof(idStr), "수정할 도서 ID", 0);
            if (idStr[0] == '\0')
                continue;
            modId = atoi(idStr);
            if (modId <= 0)
            {
                printf("[오류] 유효한 도서 ID를 입력하세요.\n");
                printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
                getchar();
                continue;
            }

            snprintf(buffer, sizeof(buffer), "FIND_BOOK:%d", modId);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            clearScreen();
            printf("[도서 정보]\n");
            printf(" ID  | 제목                                                        | 저자                             | 평점\n");
            printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
            printf("%s\n", buffer);
            if (strncmp(buffer, "[오류]", 9) == 0)
            {
                printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
                getchar();
                continue;
            }

            char choice[10];
            getEscapableInput(choice, sizeof(choice), "수정할 항목 선택\n1. 제목\n2. 저자\n3. 평점", 0);
            if (choice[0] == '\0')
                continue;
            int fieldSel = atoi(choice);
            if (fieldSel < 1 || fieldSel > 3)
                continue;

            if (fieldSel == 1)
                strcpy(field, "title");
            else if (fieldSel == 2)
                strcpy(field, "author");
            else
                strcpy(field, "rating");

            getEscapableInput(newValue, sizeof(newValue), "새로운 값", 0);
            if (newValue[0] == '\0')
                continue;

            snprintf(buffer, sizeof(buffer), "UPDATE:%d:%s:%s", modId, field, newValue);
            send(sock, buffer, strlen(buffer), 0);
            bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            clearScreen();
            printf("%s\n", buffer);
            printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
            getchar();
        }
    }
}