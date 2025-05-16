#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "user_menu.h"
#include "book_menu.h"
#include "common_input.h"
#include "book.h"

#define BUF_SIZE 1000

// 프로그램 종료 함수
// 모든 자원을 정리하고 프로그램을 안전하게 종료
void exitProgram(SOCKET sock) {
    printf("[알림] 프로그램을 종료합니다.\n");
    printf("[알림] 자원 정리 중...\n");
    
    // 도서 메모리 해제
    freeAllBooks();
    
    // 소켓 연결 종료
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        printf("[알림] 서버 연결 종료\n");
    }
    
    // Winsock 정리
    WSACleanup();
    printf("[알림] 네트워크 자원 정리 완료\n");
    
    Sleep(1000); // 1초 대기
    
    // 프로그램 종료
    exit(0);
}

void userMenu(SOCKET sock)
{
    int option;
    char id[20], pw[20], pw2[20], buffer[BUF_SIZE];

    while (1)
    {
        clearScreen();
        printf("==================== 사용자 메뉴 ====================\n");
        printf("1. 로그인\n2. 회원가입\n3. 사용자 삭제\n4. 사용자 조회\n5. 비밀번호 수정\n");
        printf("======================================================\n");
        printf("[ESC: 입력 취소]  [Enter: 확인]  [0: 프로그램 종료]\n");
        printf("──────────────────────────────────────────────────────\n");
        printf(">> 선택: ");
        if (scanf("%d", &option) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar();

        if (option == 0)
        {
            exitProgram(sock); // 안전한 종료 함수 호출
        }
        if (option < 1 || option > 5)
            continue;

        getEscapableInput(id, sizeof(id), "ID 입력", 0);
        if (id[0] == '\0')
            continue;

        if ((option == 3 || option == 4 || option == 5))
        {
            snprintf(buffer, BUF_SIZE, "FIND:%s", id);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            
            if (option == 4) {
                printf("서버 응답: %s\n", buffer);
                printf("계속하려면 아무 키나 누르세요...\n");
                getchar();
                continue;
            }
            
            if (strncmp(buffer, "사용자가 존재합니다", 24) != 0)
            {
                printf("[알림] 존재하지 않는 사용자입니다.\n");
                printf("계속하려면 아무 키나 누르세요...\n");
                getchar();
                continue;
            }
        }

        if (option == 1)
        {
            getEscapableInput(pw, sizeof(pw), "비밀번호 입력", 1);
            if (pw[0] == '\0')
                continue;
            snprintf(buffer, BUF_SIZE, "LOGIN:%s:%s", id, pw);
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            if (strncmp(buffer, "로그인 성공", 10) == 0)
            {
                bookMenu(sock);
            }
            else
            {
                printf("%s\n", buffer);
                printf("계속하려면 아무 키나 누르세요...\n");
                getchar();
            }
        }
        else if (option == 2)
        {
            getEscapableInput(pw, sizeof(pw), "비밀번호 입력", 1);
            if (pw[0] == '\0')
                continue;
            getEscapableInput(pw2, sizeof(pw2), "비밀번호 확인", 1);
            if (pw2[0] == '\0')
                continue;
            if (strcmp(pw, pw2) != 0)
            {
                printf("[오류] 비밀번호가 일치하지 않습니다.\n");
                printf("계속하려면 아무 키나 누르세요...\n");
                getchar();
                continue;
            }
            snprintf(buffer, BUF_SIZE, "REGISTER:%s:%s", id, pw);
        }
        else if (option == 3)
        {
            getEscapableInput(pw, sizeof(pw), "비밀번호 확인", 1);
            if (pw[0] == '\0')
                continue;
            snprintf(buffer, BUF_SIZE, "DELETE:%s:%s", id, pw);
        }
        else if (option == 5)
        {
            getEscapableInput(pw, sizeof(pw), "기존 비밀번호 입력", 1);
            if (pw[0] == '\0')
                continue;
            getEscapableInput(pw2, sizeof(pw2), "새 비밀번호 입력", 1);
            if (pw2[0] == '\0')
                continue;
            snprintf(buffer, BUF_SIZE, "MODIFY:%s:%s:%s", id, pw, pw2);
        }

        if (option != 1 && option != 4)
        {
            send(sock, buffer, strlen(buffer), 0);
            int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
            buffer[bytes] = '\0';
            printf("서버 응답: %s\n", buffer);
            printf("계속하려면 아무 키나 누르세요...\n");
            getchar();
        }
    }
}