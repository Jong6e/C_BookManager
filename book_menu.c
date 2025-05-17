#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#include "book.h"
#include "book_menu.h"
#include "common_input.h"
#include "user_menu.h"

#define BUF_SIZE 65536 // 버퍼 크기를 64KB로 크게 증가
#define MIN_TEXT_LEN 1 // 최소 텍스트 길이 1자

void printStatusBar()
{
    printf("[ESC: 입력 취소]  [Enter: 확인]  [0: 프로그램 종료]\n");
    printf("──────────────────────────────────────────────────────\n");
}

void bookMenu(SOCKET sock)
{
    int sel;
    char buffer[BUF_SIZE];
    char field[20], keyword[50];

    while (1)
    {
        clearScreen();
        printf("┌────────────────────────────────────────────────────┐\n");
        printf("│         선문대학교 통합도서정보관리 시스템         │\n");
        printf("└────────────────────────────────────────────────────┘\n\n");
        printf("==================== 도서관리 메뉴 ===================\n");
        printf("1. 도서 검색\n");
        printf("2. 도서 추가\n");
        printf("3. 도서 삭제\n");
        printf("4. 도서 수정\n");
        printf("5. 도서 정렬\n");
        printf("6. 도서 평점 랭킹\n");
        printf("======================================================\n");
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
            exitProgram(sock); // 안전한 종료 함수 호출
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

            // 줄 나누기
            char *lines[1000];
            int lineCount = 0;
            char *start = buffer;
            char *end;

            while ((end = strchr(start, '\n')) != NULL && lineCount < 1000)
            {
                int len = end - start;
                if (len > 0)
                {
                    lines[lineCount] = (char *)malloc(len + 1);
                    if (lines[lineCount])
                    {
                        strncpy(lines[lineCount], start, len);
                        lines[lineCount][len] = '\0';
                        lineCount++;
                    }
                }
                start = end + 1;
            }

            if (*start != '\0' && lineCount < 1000)
            {
                lines[lineCount] = strdup(start);
                if (lines[lineCount])
                    lineCount++;
            }

            int currentPage = 0;
            int itemsPerPage = 10;
            int totalPages = (lineCount + itemsPerPage - 1) / itemsPerPage;
            if (totalPages == 0)
                totalPages = 1;

            while (1)
            {
                clearScreen();
                printf("[도서 검색 결과\n", currentPage + 1, totalPages);
                printf(" ID |  제목                                                        | 저자                             | 평점\n");
                printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");

                int startIdx = currentPage * itemsPerPage;
                int endIdx = startIdx + itemsPerPage;
                if (endIdx > lineCount)
                    endIdx = lineCount;

                for (int i = startIdx; i < endIdx; i++)
                {
                    printf("%s\n", lines[i]);
                }

                printf("\n[페이지: %d/%d] [←: 이전]  [→: 다음]  [ESC: 돌아가기]\n", currentPage + 1, totalPages);

                int ch = getch();
                if (ch == 27)
                    break;
                if (ch == 0 || ch == 224)
                {
                    ch = getch();
                    if (ch == 75 && currentPage > 0)
                        currentPage--;
                    else if (ch == 77 && currentPage < totalPages - 1)
                        currentPage++;
                }
            }

            for (int i = 0; i < lineCount; i++)
                free(lines[i]);
        }

        if (sel == 2)
        {
            char title[TITLE_MAX_LEN + 1], author[AUTHOR_MAX_LEN + 1], ratingStr[10];
            float rating;

            getEscapableInput(title, sizeof(title), "도서 제목", 0);
            if (title[0] == '\0')
                continue;
            
            // 도서 제목 최소 길이 체크
            if (strlen(title) < MIN_TEXT_LEN) {
                printf("[오류] 도서 제목은 최소 %d자 이상이어야 합니다.\n", MIN_TEXT_LEN);
                printf("계속하려면 아무 키나 누르세요...\n");
                getchar();
                continue;
            }

            getEscapableInput(author, sizeof(author), "도서 저자", 0);
            if (author[0] == '\0')
                continue;
            
            // 저자명 최소 길이 체크
            if (strlen(author) < MIN_TEXT_LEN) {
                printf("[오류] 저자명은 최소 %d자 이상이어야 합니다.\n", MIN_TEXT_LEN);
                printf("계속하려면 아무 키나 누르세요...\n");
                getchar();
                continue;
            }

            // 최대 길이 체크 및 조정
            if (strlen(title) > TITLE_MAX_LEN) {
                title[TITLE_MAX_LEN] = '\0';
                printf("[경고] 제목이 최대 길이(%d자)를 초과하여 잘렸습니다.\n", TITLE_MAX_LEN);
            }
            
            if (strlen(author) > AUTHOR_MAX_LEN) {
                author[AUTHOR_MAX_LEN] = '\0';
                printf("[경고] 저자명이 최대 길이(%d자)를 초과하여 잘렸습니다.\n", AUTHOR_MAX_LEN);
            }

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
            
            // 도서 정보 확인을 위한 FIND_BOOK 명령 전송
            snprintf(buffer, sizeof(buffer), "FIND_BOOK:%d", delId);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            
            // 도서를 찾지 못한 경우
            if (strstr(buffer, "[오류]") != NULL || strstr(buffer, "찾을 수 없습니다") != NULL) {
                clearScreen();
                printf("%s\n", buffer);
                printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
                getchar();
                continue;
            }
            
            // 도서 정보 출력 및 삭제 확인
            clearScreen();
            printf("[삭제할 도서 정보]\n");
            printf(" ID |  제목                                                        | 저자                             | 평점\n");
            printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
            printf("%s\n\n", buffer);
            
            // 삭제 확인
            char confirm[10];
            getEscapableInput(confirm, sizeof(confirm), "정말 이 도서를 삭제하시겠습니까? (Y/N)", 0);
            if (confirm[0] != 'Y' && confirm[0] != 'y') {
                clearScreen();
                printf("[알림] 도서 삭제가 취소되었습니다.\n");
                printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
                getchar();
                continue;
            }
            
            // 삭제 명령 전송
            snprintf(buffer, sizeof(buffer), "DELETE_BOOK:%d", delId);
            send(sock, buffer, strlen(buffer), 0);
            bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
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
            printf(" ID |  제목                                                        | 저자                             | 평점\n");
            printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
            printf("%s\n", buffer);
            
            // 오류 메시지 검사를 더 정확하게 수정
            if (strstr(buffer, "[오류]") != NULL || strstr(buffer, "찾을 수 없습니다") != NULL)
            {
                printf("[알림] 유효하지 않은 도서 ID입니다.\n");
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

            if (strcmp(field, "rating") == 0) {
                float rating;
                while (1) {
                    getEscapableInput(newValue, sizeof(newValue), "새로운 평점 (0.0 ~ 5.0)", 0);
                    if (newValue[0] == '\0')
                        break;
                    rating = atof(newValue);
                    if (rating < 0.0 || rating > 5.0) {
                        printf("[오류] 평점은 0.0에서 5.0 사이로 입력해야 합니다.\n");
                        continue;
                    }
                    break;
                }
                if (newValue[0] == '\0')
                    continue;
            } else {
                getEscapableInput(newValue, sizeof(newValue), "새로운 값", 0);
                if (newValue[0] == '\0')
                    continue;
                
                // 최소 길이 체크
                if (strlen(newValue) < MIN_TEXT_LEN) {
                    printf("[오류] %s은(는) 최소 %d자 이상이어야 합니다.\n", 
                           strcmp(field, "title") == 0 ? "도서 제목" : "저자명", MIN_TEXT_LEN);
                    printf("계속하려면 아무 키나 누르세요...\n");
                    getchar();
                    continue;
                }
                
                // 최대 길이 체크 및 조정
                if (strcmp(field, "title") == 0 && strlen(newValue) > TITLE_MAX_LEN) {
                    newValue[TITLE_MAX_LEN] = '\0';
                    printf("[경고] 제목이 최대 길이(%d자)를 초과하여 잘렸습니다.\n", TITLE_MAX_LEN);
                } else if (strcmp(field, "author") == 0 && strlen(newValue) > AUTHOR_MAX_LEN) {
                    newValue[AUTHOR_MAX_LEN] = '\0';
                    printf("[경고] 저자명이 최대 길이(%d자)를 초과하여 잘렸습니다.\n", AUTHOR_MAX_LEN);
                }
            }

            snprintf(buffer, sizeof(buffer), "UPDATE:%d:%s:%s", modId, field, newValue);
            send(sock, buffer, strlen(buffer), 0);
            bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            clearScreen();
            printf("%s\n", buffer);
            printf("(Enter를 누르면 메뉴로 돌아갑니다...)\n");
            getchar();
        }

        if (sel == 5)
        {
            char choice[10], orderStr[10];
            int fieldSel, orderSel;

            // 정렬 기준 선택
            getEscapableInput(choice, sizeof(choice), "정렬 기준 선택\n1. 제목\n2. 저자\n3. 평점", 0);
            if (choice[0] == '\0')
                continue;
            fieldSel = atoi(choice);
            if (fieldSel < 1 || fieldSel > 3)
                continue;
            const char *field = (fieldSel == 1) ? "title" : (fieldSel == 2) ? "author"
                                                                            : "rating";

            // 정렬 방식 선택
            getEscapableInput(orderStr, sizeof(orderStr), "정렬 방식 선택\n1. 내림차순\n2. 오름차순", 0);
            if (orderStr[0] == '\0')
                continue;
            orderSel = atoi(orderStr);
            if (orderSel != 1 && orderSel != 2)
                continue;
            const char *order = (orderSel == 1) ? "desc" : "asc";

            // 서버에 명령 전송
            snprintf(buffer, sizeof(buffer), "SORT:%s:%s", field, order);

            // 데이터 전송
            send(sock, buffer, strlen(buffer), 0);

            memset(buffer, 0, BUF_SIZE); // 버퍼 초기화
            int totalBytes = 0;
            int bytes;

            // 데이터 수신 (여러 번에 걸쳐 수신할 수 있도록)
            do
            {
                bytes = recv(sock, buffer + totalBytes, BUF_SIZE - totalBytes - 1, 0);
                if (bytes > 0)
                {
                    totalBytes += bytes;
                    buffer[totalBytes] = '\0';
                }
            } while (bytes > 0 && totalBytes < BUF_SIZE - 1 && bytes == 4096); // 일반적인 TCP 패킷 크기

            // 줄 단위로 분리해서 저장
            char *lines[1000]; // 최대 1000줄까지 저장 가능
            int lineCount = 0;

            // 직접 줄 파싱 구현
            char *start = buffer;
            char *end;

            while ((end = strchr(start, '\n')) != NULL && lineCount < 1000)
            {
                int len = end - start;
                if (len > 0)
                {
                    lines[lineCount] = (char *)malloc(len + 1);
                    if (lines[lineCount])
                    {
                        strncpy(lines[lineCount], start, len);
                        lines[lineCount][len] = '\0';
                        lineCount++;
                    }
                }
                start = end + 1;
            }

            // 마지막 줄 처리 (개행문자가 없는 경우)
            if (*start != '\0' && lineCount < 1000)
            {
                lines[lineCount] = strdup(start);
                if (lines[lineCount])
                {
                    lineCount++;
                }
            }

            printf("정렬된 도서 수: %d\n", lineCount);
            Sleep(1000);

            int currentPage = 0;
            int itemsPerPage = 10;                                          // 페이지당 항목 수
            int totalPages = (lineCount + itemsPerPage - 1) / itemsPerPage; // 올림 계산

            if (totalPages == 0)
                totalPages = 1; // 최소 1페이지

            // 페이지네이션 UI
            while (1)
            {
                clearScreen();
                printf("[정렬된 도서 목록]\n", currentPage + 1, totalPages);
                printf(" ID |  제목                                                        | 저자                             | 평점\n");
                printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");

                // 현재 페이지에 표시할 항목 계산
                int startIdx = currentPage * itemsPerPage;
                int endIdx = startIdx + itemsPerPage;

                if (endIdx > lineCount)
                    endIdx = lineCount;

                // 현재 페이지 항목 출력
                for (int i = startIdx; i < endIdx; i++)
                {
                    printf("%s\n", lines[i]);
                }

                printf("\n[페이지: %d/%d] ", currentPage + 1, totalPages);
                printf("[←: 이전 페이지]  [→: 다음 페이지]  [ESC: 돌아가기]\n");

                // 개선된 키 입력 처리
                int ch = getch();

                // ESC 키 처리
                if (ch == 27)
                {
                    break;
                }

                // 특수 키 처리 (화살표 키 등)
                if (ch == 0 || ch == 224)
                {
                    ch = getch();

                    // 방향키 처리
                    if (ch == 75 && currentPage > 0)
                    {
                        // 왼쪽 화살표
                        currentPage--;
                    }
                    else if (ch == 77 && currentPage < totalPages - 1)
                    {
                        // 오른쪽 화살표
                        currentPage++;
                    }
                }
            }

            // 할당한 메모리 해제
            for (int i = 0; i < lineCount; i++)
            {
                free(lines[i]);
            }
        }

        if (sel == 6)
        {
            snprintf(buffer, sizeof(buffer), "RANK");
            send(sock, buffer, strlen(buffer), 0);

            memset(buffer, 0, BUF_SIZE);
            int totalBytes = 0;
            int bytes;

            do
            {
                bytes = recv(sock, buffer + totalBytes, BUF_SIZE - totalBytes - 1, 0);
                if (bytes > 0)
                {
                    totalBytes += bytes;
                    buffer[totalBytes] = '\0';
                }
            } while (bytes > 0 && totalBytes < BUF_SIZE - 1 && bytes == 4096);

            char *lines[1000];
            int lineCount = 0;
            char *start = buffer;
            char *end;

            while ((end = strchr(start, '\n')) != NULL && lineCount < 1000)
            {
                int len = end - start;
                if (len > 0)
                {
                    lines[lineCount] = (char *)malloc(len + 1);
                    if (lines[lineCount])
                    {
                        strncpy(lines[lineCount], start, len);
                        lines[lineCount][len] = '\0';
                        lineCount++;
                    }
                }
                start = end + 1;
            }
            if (*start != '\0' && lineCount < 1000)
            {
                lines[lineCount] = strdup(start);
                if (lines[lineCount])
                    lineCount++;
            }

            int currentPage = 0;
            int itemsPerPage = 10;
            int totalPages = (lineCount + itemsPerPage - 1) / itemsPerPage;
            if (totalPages == 0)
                totalPages = 1;

            while (1)
            {
                clearScreen();
                printf("[도서 평점 랭킹]\n", currentPage + 1, totalPages);
                printf(" ID |  제목                                                        | 저자                             | 평점\n");
                printf("──────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");

                int startIdx = currentPage * itemsPerPage;
                int endIdx = startIdx + itemsPerPage;
                if (endIdx > lineCount)
                    endIdx = lineCount;

                for (int i = startIdx; i < endIdx; i++)
                {
                    printf("%s\n", lines[i]);
                }

                printf("\n[페이지: %d/%d] [←: 이전]  [→: 다음]  [ESC: 돌아가기]\n", currentPage + 1, totalPages);

                int ch = getch();
                if (ch == 27)
                    break;
                if (ch == 0 || ch == 224)
                {
                    ch = getch();
                    if (ch == 75 && currentPage > 0)
                        currentPage--;
                    else if (ch == 77 && currentPage < totalPages - 1)
                        currentPage++;
                }
            }

            for (int i = 0; i < lineCount; i++)
                free(lines[i]);
        }
    }
}