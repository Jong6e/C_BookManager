#ifndef BOOK_H
#define BOOK_H

// 최대 도서 수 제한 상수
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

extern BOOK *books[MAX_BOOKS];  // 도서 포인터 배열
extern int bookCount; // 도서 수

// 도서 정보 파일에서 로드
// 프로그램 시작 시 booklist2.txt에서 도서 데이터를 읽어옴
void loadBooksFromFile(const char *filename);

// 전체 도서 목록 출력
// 현재 메모리에 로드된 모든 도서 정보를 화면에 표시
void printAllBooks();

// 타이틀 출력
// UI 상단에 표시할 타이틀 정보 출력
void printTitle();

// 도서 검색 기능
// 지정된 필드(제목/저자)에서 키워드를 검색하여 결과 반환
void searchBooks(const char *field, const char *keyword, char *result, int resultSize);

// 도서 정렬 기능
// 지정된 필드와 정렬 방식으로 도서 목록 정렬하여 결과 반환
void sortBooks(const char *field, const char *order, char *result, int resultSize);

// 모든 도서 메모리 해제
// 프로그램 종료 시 할당된 도서 메모리를 모두 해제
void freeAllBooks();

// 새 도서 생성 함수
// 입력받은 정보로 새 도서 구조체를 동적 할당하여 생성
BOOK* createNewBook(int id, const char* title, const char* author, float rating);

#endif