#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "book.h"

BOOK books[MAX_BOOKS];
int bookCount = 0;

void loadBooksFromFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("[서버] 도서 파일 열기 실패");
        exit(1);
    }

    char line[256];
    bookCount = 0;
    while (fgets(line, sizeof(line), fp) && bookCount < MAX_BOOKS) {
        char *token = strtok(line, "\t");
        if (!token) continue;
        books[bookCount].id = atoi(token);

        token = strtok(NULL, "\t");
        if (!token) continue;
        strncpy(books[bookCount].title, token, sizeof(books[bookCount].title));

        token = strtok(NULL, "\t");
        if (!token) continue;
        strncpy(books[bookCount].author, token, sizeof(books[bookCount].author));

        token = strtok(NULL, "\t\n");
        if (!token) continue;
        books[bookCount].rating = atof(token);

        bookCount++;
    }
    fclose(fp);
}

void toLowerCase(const char *src, char *dest) {
    for (int i = 0; src[i]; i++) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[strlen(src)] = '\0';
}

void searchBooks(const char *field, const char *keyword, char *result, int resultSize) {
    char lowerKeyword[50];
    toLowerCase(keyword, lowerKeyword);

    int found = 0;
    result[0] = '\0';

    for (int i = 0; i < bookCount; i++) {
        char target[100];
        if (strcmp(field, "title") == 0)
            toLowerCase(books[i].title, target);
        else if (strcmp(field, "author") == 0)
            toLowerCase(books[i].author, target);
        else {
            snprintf(result, resultSize, "검색 기준 오류\n");
            return;
        }

        if (strstr(target, lowerKeyword)) {
            char line[256];
            snprintf(line, sizeof(line), "%3d | %-60.60s | %-32.32s | 평점: %.2f\n",
                     books[i].id, books[i].title, books[i].author, books[i].rating);
            strncat(result, line, resultSize - strlen(result) - 1);
            found++;
        }
    }

    if (found == 0) {
        strncat(result, "일치하는 도서가 없습니다.\n", resultSize - strlen(result) - 1);
    }
}

void sortBooks(const char *field, const char *order, char *result, int resultSize) {
    BOOK sorted[MAX_BOOKS];
    memcpy(sorted, books, sizeof(BOOK) * bookCount);

    bool desc = strcmp(order, "desc") == 0;

    for (int i = 0; i < bookCount - 1; i++) {
        for (int j = i + 1; j < bookCount; j++) {
            int cmp = 0;
            if (strcmp(field, "title") == 0)
                cmp = strcmp(sorted[i].title, sorted[j].title);
            else if (strcmp(field, "author") == 0)
                cmp = strcmp(sorted[i].author, sorted[j].author);
            else if (strcmp(field, "rating") == 0)
                cmp = (sorted[i].rating < sorted[j].rating) ? -1 : (sorted[i].rating > sorted[j].rating) ? 1 : 0;

            if ((desc && cmp < 0) || (!desc && cmp > 0)) {
                BOOK temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    result[0] = '\0';
    for (int i = 0; i < bookCount; i++) {
        char line[256];
        snprintf(line, sizeof(line), "%3d | %-60.60s | %-32.32s | 평점: %.2f\n",
                 sorted[i].id, sorted[i].title, sorted[i].author, sorted[i].rating);
        strncat(result, line, resultSize - strlen(result) - 1);
    }
}