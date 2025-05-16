#include <stdio.h>
#include "book.h"
#include "user.h"

int main() {
    printTitle();
    loadBooksFromFile("booklist2.txt");
    loadUsersFromFile("users.txt");

    if (!login()) {
        return 1;
    }

    printAllBooks();
    return 0;
}