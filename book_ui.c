#include <stdio.h>
#include "book.h"

/**
 * 프로그램 제목 박스를 출력한다.
 */
void printTitle()
{
    printf("============================================\n");
    printf("=      선문대학교 도서정보관리 시스템      =\n");
    printf("============================================\n");
}

/**
 * 전체 도서 목록을 출력한다.
 */
void printAllBooks()
{
    printf("\n[도서 목록]\n");
    for (int i = 0; i < bookCount; i++)
    {
        printf("%3d | %-40s | %-20s | 평점: %.2f\n",
               books[i].id, books[i].title, books[i].author, books[i].rating);
    }
}
