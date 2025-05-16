#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "book.h"

// 도서 정보를 저장하는 전역 배열
BOOK *books[MAX_BOOKS]; // 도서 포인터 배열
int bookCount = 0;      // 현재 등록된 도서 수

// 새 도서 동적 할당 생성
// 도서 추가 시 새로운 도서 정보를 위한 메모리를 할당하고 초기화함
BOOK* createNewBook(int id, const char* title, const char* author, float rating) {
    // 메모리 할당
    BOOK* newBook = (BOOK*)malloc(sizeof(BOOK));
    if (!newBook) {
        printf("[오류] 메모리 할당 실패\n");
        return NULL;
    }
    
    // 데이터 초기화
    newBook->id = id;
    strncpy(newBook->title, title, sizeof(newBook->title) - 1);
    newBook->title[sizeof(newBook->title) - 1] = '\0';
    strncpy(newBook->author, author, sizeof(newBook->author) - 1);
    newBook->author[sizeof(newBook->author) - 1] = '\0';
    newBook->rating = rating;
    
    return newBook;
}

// 모든 도서 메모리 해제
// 프로그램 종료 시 또는 도서 정보 재로드 시 할당된 메모리 정리
void freeAllBooks() {
    printf("[알림] 할당된 도서 메모리 해제 중...\n");
    for (int i = 0; i < bookCount; i++) {
        if (books[i]) {
            free(books[i]);
            books[i] = NULL;
        }
    }
    bookCount = 0;
    printf("[알림] 메모리 해제 완료\n");
}

// 도서 파일에서 데이터 로드
// 프로그램 시작 시 또는 도서 정보 갱신이 필요할 때 호출됨
void loadBooksFromFile(const char *filename) {
    // 기존 도서 메모리 해제
    freeAllBooks();
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("[서버] 도서 파일 열기 실패");
        exit(1);
    }

    char line[256];
    bookCount = 0;
    printf("[서버] 도서 파일 로드 시작\n");
    
    while (fgets(line, sizeof(line), fp) && bookCount < MAX_BOOKS) {
        int id;
        char title[50], author[50];
        float rating;
        
        char *token = strtok(line, "\t");
        if (!token) {
            printf("[서버] 도서 ID 파싱 실패: %s\n", line);
            continue;
        }
        id = atoi(token);
        if (id <= 0) {
            printf("[서버] 유효하지 않은 도서 ID: %d\n", id);
            continue;
        }

        token = strtok(NULL, "\t");
        if (!token) {
            printf("[서버] 도서 제목 파싱 실패, ID: %d\n", id);
            continue;
        }
        strncpy(title, token, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';

        token = strtok(NULL, "\t");
        if (!token) {
            printf("[서버] 도서 저자 파싱 실패, ID: %d\n", id);
            continue;
        }
        strncpy(author, token, sizeof(author) - 1);
        author[sizeof(author) - 1] = '\0';

        token = strtok(NULL, "\t\n");
        if (!token) {
            printf("[서버] 도서 평점 파싱 실패, ID: %d\n", id);
            continue;
        }
        rating = atof(token);
        if (rating < 0 || rating > 5) {
            printf("[서버] 유효하지 않은 평점: %.2f, ID: %d\n", rating, id);
            rating = 0.0; // 기본값 설정
        }

        // 동적 할당으로 도서 생성 및 배열에 저장
        books[bookCount] = createNewBook(id, title, author, rating);
        if (books[bookCount]) {
            bookCount++;
        }
        else {
            printf("[서버] 도서 생성 실패, ID: %d\n", id);
        }
    }
    
    printf("[서버] 도서 총 %d권 로드 완료\n", bookCount);
    fclose(fp);
}

// 문자열을 소문자로 변환
// 검색 시 대소문자 구분 없이 비교하기 위해 사용
void toLowerCase(const char *src, char *dest) {
    for (int i = 0; src[i]; i++) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[strlen(src)] = '\0';
}

// 도서 검색 기능 구현
// 제목 또는 저자 필드에서 키워드로 도서 검색
void searchBooks(const char *field, const char *keyword, char *result, int resultSize) {
    char lowerKeyword[50];
    toLowerCase(keyword, lowerKeyword);

    int found = 0;
    result[0] = '\0';

    for (int i = 0; i < bookCount; i++) {
        char target[100];
        if (strcmp(field, "title") == 0)
            toLowerCase(books[i]->title, target);
        else if (strcmp(field, "author") == 0)
            toLowerCase(books[i]->author, target);
        else {
            snprintf(result, resultSize, "검색 기준 오류\n");
            return;
        }

        if (strstr(target, lowerKeyword)) {
            char line[256];
            snprintf(line, sizeof(line), "%3d | %-60.60s | %-32.32s | 평점: %.2f\n",
                     books[i]->id, books[i]->title, books[i]->author, books[i]->rating);
            strncat(result, line, resultSize - strlen(result) - 1);
            found++;
        }
    }

    if (found == 0) {
        strncat(result, "일치하는 도서가 없습니다.\n", resultSize - strlen(result) - 1);
    }
}

// 도서 정렬 기능 구현
// 지정된 필드(제목/저자/평점)와 정렬 방식(오름차순/내림차순)으로 도서 목록 정렬
void sortBooks(const char *field, const char *order, char *result, int resultSize) {
    // 포인터 배열로 복사
    BOOK *sorted[MAX_BOOKS];
    for (int i = 0; i < bookCount; i++) {
        sorted[i] = books[i];
    }

    bool desc = strcmp(order, "desc") == 0;

    for (int i = 0; i < bookCount - 1; i++) {
        for (int j = i + 1; j < bookCount; j++) {
            int cmp = 0;
            if (strcmp(field, "title") == 0)
                cmp = strcmp(sorted[i]->title, sorted[j]->title);
            else if (strcmp(field, "author") == 0)
                cmp = strcmp(sorted[i]->author, sorted[j]->author);
            else if (strcmp(field, "rating") == 0)
                cmp = (sorted[i]->rating < sorted[j]->rating) ? -1 : 
                      (sorted[i]->rating > sorted[j]->rating) ? 1 : 0;

            if ((desc && cmp < 0) || (!desc && cmp > 0)) {
                BOOK *temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    // 정렬된 결과를 응답에 저장
    result[0] = '\0';
    for (int i = 0; i < bookCount; i++) {
        char line[256];
        snprintf(line, sizeof(line), "%3d | %-60.60s | %-32.32s | 평점: %.2f\n",
                 sorted[i]->id, sorted[i]->title, sorted[i]->author, sorted[i]->rating);
        strncat(result, line, resultSize - strlen(result) - 1);
    }
}