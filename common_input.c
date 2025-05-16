#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "common_input.h"

// 콘솔 화면을 초기화한다
void clearScreen() {
    system("cls");
}

// ESC 입력이 가능한 사용자 입력 함수
// masked = 1이면 * 표시, 0이면 일반 입력
void getEscapableInput(char *buffer, size_t size, const char *label, int masked) {
    int i = 0;
    char ch;
    printf("%s\n(ESC: 취소): ", label);
    while ((ch = getch()) != '\r' && i < size - 1) {
        if (ch == 27) {
            buffer[0] = '\0';
            printf("\n[입력 취소됨]\n");
            return;
        }
        if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
        } else if (ch >= 32 && ch <= 126) {
            buffer[i++] = ch;
            printf(masked ? "*" : "%c", ch);
        }
    }
    buffer[i] = '\0';
    printf("\n");
}