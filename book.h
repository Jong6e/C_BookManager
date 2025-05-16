#ifndef BOOK_H
#define BOOK_H

#define MAX_BOOKS 700

typedef struct {
    int id;
    char title[50];
    char author[50];
    float rating;
} BOOK;

extern BOOK books[];
extern int bookCount;

void loadBooksFromFile(const char *filename);
void printAllBooks();
void printTitle();

#endif
