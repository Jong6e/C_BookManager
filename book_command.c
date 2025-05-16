#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "book.h"

#define REPLY_SIZE 65536  // 응답 버퍼 크기를 클라이언트 버퍼와 동일하게 설정

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
            if (books[i].id > maxId)
                maxId = books[i].id;
        }

        if (bookCount >= MAX_BOOKS)
        {
            strncpy(reply, "[오류] 최대 도서 수를 초과했습니다.", REPLY_SIZE);
        }
        else
        {
            books[bookCount].id = maxId + 1;
            strncpy(books[bookCount].title, title, sizeof(books[bookCount].title));
            strncpy(books[bookCount].author, author, sizeof(books[bookCount].author));
            books[bookCount].rating = rating;
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
                    fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i].id, books[i].title, books[i].author, books[i].rating);
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

        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == delId)
            {
                for (int j = i; j < bookCount - 1; j++)
                {
                    books[j] = books[j + 1];
                }
                bookCount--;
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
                    fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i].id, books[i].title, books[i].author, books[i].rating);
                }
                fclose(fp);
                strncpy(reply, "도서 삭제 완료.", REPLY_SIZE);
            }
        }
        else
        {
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
            if (books[i].id == targetId)
            {
                if (strcmp(field, "title") == 0)
                    strncpy(books[i].title, newValue, sizeof(books[i].title));
                else if (strcmp(field, "author") == 0)
                    strncpy(books[i].author, newValue, sizeof(books[i].author));
                else if (strcmp(field, "rating") == 0)
                    books[i].rating = atof(newValue);

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
                    fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i].id, books[i].title, books[i].author, books[i].rating);
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
            if (books[i].id == findId)
            {
                snprintf(reply, REPLY_SIZE, "%3d | %-60.60s | %-32.32s | 평점: %.2f",
                         books[i].id, books[i].title, books[i].author, books[i].rating);
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
            // 서버 로그에만 출력하고 응답에는 포함시키지 않음
            printf("[서버] 정렬 시작: 총 도서 수: %d\n", bookCount);
            
            // 정렬 결과만 응답에 포함
            sortBooks(field, order, reply, REPLY_SIZE);
        }
    }
    else
    {
        strncpy(reply, "[오류] 도서 명령 처리 실패", REPLY_SIZE);
    }
}