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

// 서버 설정 상수
#define PORT 12345      // 서버 포트 번호
#define BUF_SIZE 65536  // 통신 버퍼 크기

// 명령어 확인 헬퍼 함수
// 클라이언트 요청이 특정 명령어와 일치하는지 정확히 확인
int isCommand(const char* buffer, const char* cmd) {
    size_t cmdLen = strlen(cmd);
    return strncmp(buffer, cmd, cmdLen) == 0 && 
           (buffer[cmdLen] == ':' || buffer[cmdLen] == '\0');
}

int main()
{
    // Winsock 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // 사용자 및 도서 데이터 로드
    loadUsersFromFile("users.txt");
    loadBooksFromFile("booklist2.txt");
    
    printf("[서버] 총 %d권의 도서 데이터 로드 완료\n", bookCount);
    
    // 서버 소켓 생성 및 설정
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {0}, cli_addr = {0};
    int cli_len = sizeof(cli_addr);
    char buffer[BUF_SIZE];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // 서버 소켓 바인딩 및 리스닝
    bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(server_fd, 5);
    printf("[서버] 클라이언트 접속 대기 중...\n");

    // 클라이언트 연결 수락
    SOCKET client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len);
    printf("[서버] 클라이언트 연결됨\n");

    // 클라이언트 요청 처리 루프
    while (1)
    {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
            break;
        buffer[bytes] = '\0';

        printf("[서버] 수신된 명령: %s\n", buffer);

        char reply[BUF_SIZE] = {0};

        // 명령어 분류 및 처리
        if (isCommand(buffer, "LOGIN") || isCommand(buffer, "REGISTER") ||
            isCommand(buffer, "DELETE") || isCommand(buffer, "FIND") ||
            isCommand(buffer, "MODIFY"))
        {
            // DELETE_BOOK이 아닌 DELETE 명령만 처리
            if (strncmp(buffer, "DELETE_BOOK", 11) == 0) {
                handleBookCommand(buffer, reply);
            } else {
                handleUserCommand(buffer, reply);
            }
        }
        else if (isCommand(buffer, "SEARCH") || isCommand(buffer, "ADD") ||
                 isCommand(buffer, "DELETE_BOOK") || isCommand(buffer, "UPDATE") ||
                 isCommand(buffer, "FIND_BOOK") || isCommand(buffer, "SORT") ||
                 isCommand(buffer, "RANK"))
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

        // 응답 전송
        printf("[서버] 처리 결과:\n%.100s...\n", reply);
        
        int replyLen = strlen(reply);
        printf("[서버] 응답 크기: %d 바이트\n", replyLen);
        
        send(client_fd, reply, replyLen, 0);
    }

    // 자원 정리 및 종료
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}