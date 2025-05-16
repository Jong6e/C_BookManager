#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "user.h"
#include "book.h"
#include "user_command.h"
#include "book_command.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUF_SIZE 65536

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup 실패: %d\n", WSAGetLastError());
        return 1;
    }

    loadUsersFromFile("users.txt");
    loadBooksFromFile("booklist2.txt");
    
    printf("[서버] 총 %d권의 도서 데이터 로드 완료\n", bookCount);
    
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {0}, cli_addr = {0};
    int cli_len = sizeof(cli_addr);
    char buffer[BUF_SIZE];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(server_fd, 5);
    printf("[서버] 클라이언트 접속 대기 중...\n");

    SOCKET client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len);
    printf("[서버] 클라이언트 연결됨\n");

    while (1)
    {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
            break;
        buffer[bytes] = '\0';

        printf("[서버] 수신된 명령: %s\n", buffer);

        char reply[BUF_SIZE] = {0};

        if (strncmp(buffer, "LOGIN", 5) == 0 || strncmp(buffer, "REGISTER", 8) == 0 ||
            strncmp(buffer, "DELETE", 6) == 0 || strncmp(buffer, "FIND", 4) == 0 ||
            strncmp(buffer, "MODIFY", 6) == 0)
        {
            handleUserCommand(buffer, reply);
        }
        else if (strncmp(buffer, "SEARCH", 6) == 0 || strncmp(buffer, "ADD", 3) == 0 ||
                 strncmp(buffer, "DELETE_BOOK", 11) == 0 || strncmp(buffer, "UPDATE", 6) == 0 ||
                 strncmp(buffer, "FIND_BOOK", 9) == 0 || strncmp(buffer, "SORT", 4) == 0)
        {
            handleBookCommand(buffer, reply);
            
            if (strncmp(buffer, "SORT", 4) == 0) {
                printf("[서버] 정렬 명령 처리, 응답 크기: %zu 바이트\n", strlen(reply));
            }
        }
        else
        {
            strncpy(reply, "[서버] 지원하지 않는 명령입니다.", sizeof(reply));
        }

        printf("[서버] 처리 결과:\n%.100s...\n", reply);
        
        int replyLen = strlen(reply);
        printf("[서버] 응답 크기: %d 바이트\n", replyLen);
        
        send(client_fd, reply, replyLen, 0);
    }

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}