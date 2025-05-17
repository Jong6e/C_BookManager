#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "book.h"

// 응답 버퍼 크기 정의
#define REPLY_SIZE 65536 // 최대 64KB 응답 크기

// 도서 관련 명령어 처리 함수
// 클라이언트로부터 받은 요청을 분석하고 적절한 도서 관리 기능 실행
void handleBookCommand(const char *request, char *reply)
{
    char buffer[1000];
    strncpy(buffer, request, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *cmd = strtok(buffer, ":");

    if (strcmp(cmd, "SEARCH") == 0)
    {
        char *field = strtok(NULL, ":");
        char *keyword = strtok(NULL, "\n");
        searchBooks(field, keyword, reply, REPLY_SIZE);
    }
    else if (strcmp(cmd, "ADD") == 0)
    {
        char *title = strtok(NULL, ":");
        char *author = strtok(NULL, ":");
        char *ratingStr = strtok(NULL, "\n");
        float rating = atof(ratingStr);

        int maxId = 0;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i]->id > maxId)
                maxId = books[i]->id;
        }

        if (bookCount >= MAX_BOOKS)
        {
            strncpy(reply, "[오류] 최대 도서 수를 초과했습니다.", REPLY_SIZE);
        }
        else
        {
            // 동적 할당으로 새 도서 생성
            books[bookCount] = createNewBook(maxId + 1, title, author, rating);
            
            if (!books[bookCount]) {
                strncpy(reply, "[오류] 메모리 할당 실패", REPLY_SIZE);
                return;
            }
            
            bookCount++;

            FILE *fp = fopen("booklist2.txt", "w");
            if (!fp)
            {
                strncpy(reply, "[오류] 파일 저장 실패", REPLY_SIZE);
            }
            else
            {
                for (int i = 0; i < bookCount; i++)
                {
                    fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i]->id, books[i]->title, books[i]->author, books[i]->rating);
                }
                fclose(fp);
                strncpy(reply, "도서 추가 완료.", REPLY_SIZE);
            }
        }
    }
    else if (strcmp(cmd, "DELETE_BOOK") == 0)
    {
        char *idStr = strtok(NULL, "\n");
        int delId = atoi(idStr);
        int found = 0;
        int delIndex = -1;
        char bookTitle[TITLE_MAX_LEN + 1] = {0}; // 삭제할 도서 제목 저장

        // 디버깅 메시지 추가
        printf("[서버] 삭제 요청 ID: %d (현재 도서 수: %d)\n", delId, bookCount);

        // ID 검색
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i] && books[i]->id == delId)
            {
                delIndex = i;
                found = 1;
                strncpy(bookTitle, books[i]->title, TITLE_MAX_LEN);
                printf("[서버] 삭제할 도서 찾음: [%d] %s\n", delId, books[i]->title);
                break;
            }
        }

        if (found && delIndex >= 0 && delIndex < bookCount)
        {
            // 메모리 해제 전 NULL 확인
            if (books[delIndex] == NULL) {
                strncpy(reply, "[오류] 도서 데이터가 손상되었습니다.", REPLY_SIZE);
                return;
            }
            
            // 메모리 해제
            free(books[delIndex]);
            printf("[서버] 도서 메모리 해제 완료\n");
            
            // 배열 재정렬
            for (int j = delIndex; j < bookCount - 1; j++)
            {
                books[j] = books[j + 1];
            }
            
            books[bookCount - 1] = NULL; // 마지막 포인터는 NULL로 설정
            bookCount--;
            printf("[서버] 배열 재정렬 완료, 남은 도서 수: %d\n", bookCount);
            
            FILE *fp = fopen("booklist2.txt", "w");
            if (!fp)
            {
                strncpy(reply, "[오류] 파일 저장 실패", REPLY_SIZE);
            }
            else
            {
                for (int i = 0; i < bookCount; i++)
                {
                    if (books[i]) { // NULL 확인 추가
                        fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i]->id, books[i]->title, books[i]->author, books[i]->rating);
                    }
                }
                fclose(fp);
                snprintf(reply, REPLY_SIZE, "[성공] 도서 ID: %d, 제목: \"%s\" 삭제 완료.", delId, bookTitle);
                printf("[서버] 도서 파일 저장 완료\n");
            }
        }
        else
        {
            printf("[서버] 삭제할 도서를 찾지 못함. ID: %d\n", delId);
            strncpy(reply, "[오류] 해당 ID의 도서를 찾을 수 없습니다.", REPLY_SIZE);
        }
    }
    else if (strcmp(cmd, "UPDATE") == 0)
    {
        char *idStr = strtok(NULL, ":");
        char *field = strtok(NULL, ":");
        char *newValue = strtok(NULL, "\n");
        int targetId = atoi(idStr);
        int found = 0;

        for (int i = 0; i < bookCount; i++)
        {
            if (books[i]->id == targetId)
            {
                if (strcmp(field, "title") == 0)
                    strncpy(books[i]->title, newValue, sizeof(books[i]->title) - 1);
                else if (strcmp(field, "author") == 0)
                    strncpy(books[i]->author, newValue, sizeof(books[i]->author) - 1);
                else if (strcmp(field, "rating") == 0)
                    books[i]->rating = atof(newValue);

                found = 1;
                break;
            }
        }

        if (found)
        {
            FILE *fp = fopen("booklist2.txt", "w");
            if (!fp)
            {
                strncpy(reply, "[오류] 파일 저장 실패", REPLY_SIZE);
            }
            else
            {
                for (int i = 0; i < bookCount; i++)
                {
                    fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i]->id, books[i]->title, books[i]->author, books[i]->rating);
                }
                fclose(fp);
                strncpy(reply, "도서 정보 수정 완료.", REPLY_SIZE);
            }
        }
        else
        {
            strncpy(reply, "[오류] 해당 ID의 도서를 찾을 수 없습니다.", REPLY_SIZE);
        }
    }
    else if (strcmp(cmd, "FIND_BOOK") == 0)
    {
        char *idStr = strtok(NULL, "\n");
        int findId = atoi(idStr);
        int found = 0;

        for (int i = 0; i < bookCount; i++)
        {
            if (books[i]->id == findId)
            {
                snprintf(reply, REPLY_SIZE, "%3d | %-60.60s | %-32.32s | 평점: %.2f",
                         books[i]->id, books[i]->title, books[i]->author, books[i]->rating);
                found = 1;
                break;
            }
        }

        if (!found)
        {
            strncpy(reply, "[오류] 해당 ID의 도서를 찾을 수 없습니다.", REPLY_SIZE);
        }
    }
    else if (strcmp(cmd, "SORT") == 0)
    {
        char *field = strtok(NULL, ":");
        char *order = strtok(NULL, "\n");

        if (!field || !order)
        {
            strncpy(reply, "[오류] 정렬 기준 또는 방식이 잘못되었습니다.", REPLY_SIZE);
        }
        else
        {
            sortBooks(field, order, reply, REPLY_SIZE);
        }
    }
    else if (strcmp(cmd, "RANK") == 0)
    {
        // 평점 기준 내림차순 정렬
        sortBooks("rating", "desc", reply, REPLY_SIZE);
    }
    else
    {
        strncpy(reply, "[오류] 도서 명령 처리 실패", REPLY_SIZE);
    }
}