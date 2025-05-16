#include <stdio.h>
#include <winsock2.h>
#include "user_menu.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup 실패: %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {0};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(12345);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("서버 연결 실패\n");
        return 1;
    }

    userMenu(sock); // 사용자 메뉴 진입

    closesocket(sock);
    WSACleanup();
    return 0;
}