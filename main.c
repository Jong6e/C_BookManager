// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 700

// 도서 정보 구조체 정의
typedef struct
{
    int id;
    char title[50];
    char author[50];
    float rating;
} BOOK;

// 전역 변수
BOOK books[MAX_BOOKS];
int bookCount = 0;

// 도서 정보 파일 읽기 함수
void loadBooksFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("❌ 파일 열기 실패: %s\n", filename);
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
    printf("도서 %d권 로딩 완료\n", bookCount);
}

// 테스트용: 도서 목록 출력
void printAllBooks()
{
    printf("\n도서 목록:\n");
    for (int i = 0; i < bookCount; i++)
    {
        printf("%3d | %-40s | %-20s | 평점: %.2f\n",
               books[i].id, books[i].title, books[i].author, books[i].rating);
    }
}

int main()
{
    loadBooksFromFile("booklist2.txt");
    printAllBooks(); // 테스트용 출력
    return 0;
}
