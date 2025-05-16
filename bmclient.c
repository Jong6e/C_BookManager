#include <stdio.h>
#include <winsock2.h>
#include "user_menu.h"
#include "book.h"  // freeAllBooks 함수 사용을 위한 추가

#pragma comment(lib, "ws2_32.lib")

// 종료 함수 선언
// user_menu.c에 구현된 종료 함수를 사용하기 위한 외부 선언
extern void exitProgram(SOCKET sock);

int main()
{
    // 콘솔 출력 인코딩 설정
    SetConsoleOutputCP(CP_UTF8);
    
    // Winsock 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // 서버 연결 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {0};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(12345);

    // 서버 연결 시도
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("서버 연결 실패\n");
        WSACleanup();
        return 1;
    }

    // 사용자 메뉴 진입
    userMenu(sock); // 사용자 메뉴 진입
    
    // 프로그램 정상 종료 처리
    // 정상적인 종료라면 이 부분은 도달하지 않음 (userMenu 내에서 exitProgram 호출)
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
    WSACleanup();
    return 0;
}