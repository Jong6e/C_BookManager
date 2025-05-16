#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

BOOK books[MAX_BOOKS];
int bookCount = 0;

void printTitle()
{
    printf("============================================\n");
    printf("=      선문대학교 도서정보관리 시스템      =\n");
    printf("============================================\n");
}

void loadBooksFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("도서 파일 열기 실패: %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (bookCount >= MAX_BOOKS)
            break;

        char *token = strtok(line, "\t");
        if (token == NULL)
            continue;
        books[bookCount].id = atoi(token);

        token = strtok(NULL, "\t");
        if (token == NULL)
            continue;
        strncpy(books[bookCount].title, token, sizeof(books[bookCount].title));

        token = strtok(NULL, "\t");
        if (token == NULL)
            continue;
        strncpy(books[bookCount].author, token, sizeof(books[bookCount].author));

        token = strtok(NULL, "\t");
        if (token == NULL)
            continue;
        books[bookCount].rating = atof(token);

        bookCount++;
    }

    fclose(fp);
}

void printAllBooks()
{
    printf("\n[도서 목록]\n");
    for (int i = 0; i < bookCount; i++)
    {
        printf("%3d | %-40s | %-20s | 평점: %.2f\n",
               books[i].id, books[i].title, books[i].author, books[i].rating);
    }
}