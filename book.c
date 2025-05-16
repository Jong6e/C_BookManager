#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "book.h"

BOOK books[700];   // 최대 700권의 도서를 저장하는 배열
int bookCount = 0; // 현재 저장된 도서 수

/**
 * booklist2.txt 파일을 읽어 도서 배열에 저장한다.
 */
void loadBooksFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("[서버] 도서 파일 열기 실패");
        exit(1);
    }

    char line[256];
    bookCount = 0;
    while (fgets(line, sizeof(line), fp) && bookCount < 700)
    {
        char *token = strtok(line, "\t");
        if (!token)
            continue;
        books[bookCount].id = atoi(token);

        token = strtok(NULL, "\t");
        if (!token)
            continue;
        strncpy(books[bookCount].title, token, sizeof(books[bookCount].title));

        token = strtok(NULL, "\t");
        if (!token)
            continue;
        strncpy(books[bookCount].author, token, sizeof(books[bookCount].author));

        token = strtok(NULL, "\t\n");
        if (!token)
            continue;
        books[bookCount].rating = atof(token);

        bookCount++;
    }
    fclose(fp);
}

/**
 * 문자열을 소문자로 변환하여 반환한다 (부분 검색 대비용).
 */
void toLowerCase(const char *src, char *dest)
{
    for (int i = 0; src[i]; i++)
    {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[strlen(src)] = '\0';
}

/**
 * 주어진 검색 기준과 키워드에 대해 도서를 검색한 후 결과 문자열을 반환한다.
 */
void searchBooks(const char *field, const char *keyword, char *result, int resultSize)
{
    char lowerKeyword[50];
    toLowerCase(keyword, lowerKeyword);

    int found = 0;
    result[0] = '\0';

    for (int i = 0; i < bookCount; i++)
    {
        char target[100];
        if (strcmp(field, "title") == 0)
        {
            toLowerCase(books[i].title, target);
        }
        else if (strcmp(field, "author") == 0)
        {
            toLowerCase(books[i].author, target);
        }
        else
        {
            snprintf(result, resultSize, "검색 기준 오류\n");
            return;
        }

        if (strstr(target, lowerKeyword))
        {
            char line[256];
            snprintf(line, sizeof(line), "%3d | %-60.60s | %-32.32s | 평점: %.2f\n",
                     books[i].id, books[i].title, books[i].author, books[i].rating);
            strncat(result, line, resultSize - strlen(result) - 1);
            found++;
        }
    }

    if (found == 0)
    {
        strncat(result, "일치하는 도서가 없습니다.\n", resultSize - strlen(result) - 1);
    }
}