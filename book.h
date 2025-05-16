#ifndef BOOK_H
#define BOOK_H

#define MAX_BOOKS 700 // 최대 700권까지 도서 저장 가능

/**
 * 도서 정보를 저장하는 구조체
 */
typedef struct
{
    int id;          // 도서 순번
    char title[50];  // 도서 제목
    char author[50]; // 도서 저자
    float rating;    // 도서 평점
} BOOK;

extern BOOK books[];  // 도서 배열
extern int bookCount; // 도서 수

void loadBooksFromFile(const char *filename);                                           // 도서 파일 읽기
void printAllBooks();                                                                   // 전체 도서 목록 출력
void printTitle();                                                                      // 타이틀 출력 (UI용)
void searchBooks(const char *field, const char *keyword, char *result, int resultSize); // 도서 검색

#endif