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
    
    while (1) {
        ch = getch();
        
        // Enter 키 입력 시 처리
        if (ch == '\r') {
            // 빈 입력 시 경고 메시지만 출력하고 계속 입력 대기
            if (i == 0) {
                printf("\n[경고] 값을 입력해주세요.\n");
                printf("%s\n(ESC: 취소): ", label);
                continue;
            }
            
            // 정상 입력이면 완료
            buffer[i] = '\0';
            printf("\n");
            return;
        }
        
        // ESC 키 입력 시 취소
        if (ch == 27) {
            buffer[0] = '\0';
            printf("\n[입력 취소됨]\n");
            return;
        }
        
        // Backspace 키 처리
        if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
            continue;
        }
        
        // 최대 길이 도달 시 알림만 표시하고 입력은 받지 않음
        if (i >= size - 1 && ch >= 32 && ch <= 126) {
            printf("\a"); // 경고음 출력
            continue;
        }
        
        // 일반 문자 입력 처리
        if (ch >= 32 && ch <= 126) {
            buffer[i++] = ch;
            printf(masked ? "*" : "%c", ch);
        }
    }
}