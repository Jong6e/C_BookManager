#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "user.h"
#include "book.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUF_SIZE 1000

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup 실패: %d\n", WSAGetLastError());
        return 1;
    }

    loadUsersFromFile("users.txt");
    loadBooksFromFile("booklist2.txt");

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

        char *cmd = strtok(buffer, ":");
        const char *result = "";
        char reply[BUF_SIZE] = {0};

        if (strcmp(cmd, "LOGIN") == 0)
        {
            char *id = strtok(NULL, ":");
            char *pw = strtok(NULL, "\n");
            result = checkLogin(id, pw) ? "로그인 성공." : "ID 또는 비밀번호가 잘못되었습니다.";
            strncpy(reply, result, sizeof(reply));
        }
        else if (strcmp(cmd, "REGISTER") == 0)
        {
            char *id = strtok(NULL, ":");
            char *pw = strtok(NULL, "\n");
            result = registerUser(id, pw) ? "회원가입 완료." : "이미 존재하는 ID입니다.";
            strncpy(reply, result, sizeof(reply));
        }
        else if (strcmp(cmd, "DELETE") == 0)
        {
            char *id = strtok(NULL, ":");
            char *pw = strtok(NULL, "\n");
            result = deleteUser(id, pw) ? "삭제 완료." : "비밀번호가 일치하지 않거나 사용자 없음.";
            strncpy(reply, result, sizeof(reply));
        }
        else if (strcmp(cmd, "FIND") == 0)
        {
            char *id = strtok(NULL, "\n");
            result = findUserIndex(id) != -1 ? "사용자가 존재합니다." : "존재하지 않는 사용자입니다.";
            strncpy(reply, result, sizeof(reply));
        }
        else if (strcmp(cmd, "MODIFY") == 0)
        {
            char *id = strtok(NULL, ":");
            char *oldpw = strtok(NULL, ":");
            char *newpw = strtok(NULL, "\n");
            result = modifyUser(id, oldpw, newpw) ? "비밀번호 변경 완료." : "기존 비밀번호 불일치 또는 사용자 없음.";
            strncpy(reply, result, sizeof(reply));
        }
        else if (strcmp(cmd, "SEARCH") == 0)
        {
            char *field = strtok(NULL, ":");
            char *keyword = strtok(NULL, "\n");
            searchBooks(field, keyword, reply, sizeof(reply));
        }
        else if (strcmp(cmd, "ADD") == 0)
        {
            char *title = strtok(NULL, ":");
            char *author = strtok(NULL, ":");
            char *ratingStr = strtok(NULL, "\n");
            float rating = atof(ratingStr);
            int maxId = 0;
            for (int i = 0; i < bookCount; i++)
            {
                if (books[i].id > maxId)
                    maxId = books[i].id;
            }
            if (bookCount >= MAX_BOOKS)
            {
                strncpy(reply, "[오류] 최대 도서 수를 초과했습니다.", sizeof(reply));
            }
            else
            {
                books[bookCount].id = maxId + 1;
                strncpy(books[bookCount].title, title, sizeof(books[bookCount].title));
                strncpy(books[bookCount].author, author, sizeof(books[bookCount].author));
                books[bookCount].rating = rating;
                bookCount++;
                FILE *fp = fopen("booklist2.txt", "w");
                if (!fp)
                {
                    strncpy(reply, "[오류] 파일 저장 실패", sizeof(reply));
                }
                else
                {
                    for (int i = 0; i < bookCount; i++)
                    {
                        fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i].id, books[i].title, books[i].author, books[i].rating);
                    }
                    fclose(fp);
                    strncpy(reply, "도서 추가 완료.", sizeof(reply));
                }
            }
        }
        else if (strcmp(cmd, "DELETE_BOOK") == 0)
        {
            char *idStr = strtok(NULL, "\n");
            int delId = atoi(idStr);
            int found = 0;
            for (int i = 0; i < bookCount; i++)
            {
                if (books[i].id == delId)
                {
                    for (int j = i; j < bookCount - 1; j++)
                    {
                        books[j] = books[j + 1];
                    }
                    bookCount--;
                    found = 1;
                    break;
                }
            }
            if (found)
            {
                FILE *fp = fopen("booklist2.txt", "w");
                if (!fp)
                {
                    strncpy(reply, "[오류] 파일 저장 실패", sizeof(reply));
                }
                else
                {
                    for (int i = 0; i < bookCount; i++)
                    {
                        fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i].id, books[i].title, books[i].author, books[i].rating);
                    }
                    fclose(fp);
                    strncpy(reply, "도서 삭제 완료.", sizeof(reply));
                }
            }
            else
            {
                strncpy(reply, "[오류] 해당 ID의 도서를 찾을 수 없습니다.", sizeof(reply));
            }
        }
        else if (strcmp(cmd, "UPDATE") == 0)
        {
            char *idStr = strtok(NULL, ":");
            char *field = strtok(NULL, ":");
            char *newValue = strtok(NULL, "\n");
            int targetId = atoi(idStr);
            int found = 0;
            for (int i = 0; i < bookCount; i++)
            {
                if (books[i].id == targetId)
                {
                    if (strcmp(field, "title") == 0)
                    {
                        strncpy(books[i].title, newValue, sizeof(books[i].title));
                    }
                    else if (strcmp(field, "author") == 0)
                    {
                        strncpy(books[i].author, newValue, sizeof(books[i].author));
                    }
                    else if (strcmp(field, "rating") == 0)
                    {
                        books[i].rating = atof(newValue);
                    }
                    found = 1;
                    break;
                }
            }
            if (found)
            {
                FILE *fp = fopen("booklist2.txt", "w");
                if (!fp)
                {
                    strncpy(reply, "[오류] 파일 저장 실패", sizeof(reply));
                }
                else
                {
                    for (int i = 0; i < bookCount; i++)
                    {
                        fprintf(fp, "%d\t%s\t%s\t%.2f\n", books[i].id, books[i].title, books[i].author, books[i].rating);
                    }
                    fclose(fp);
                    strncpy(reply, "도서 정보 수정 완료.", sizeof(reply));
                }
            }
            else
            {
                strncpy(reply, "[오류] 해당 ID의 도서를 찾을 수 없습니다.", sizeof(reply));
            }
        }
        else if (strcmp(cmd, "FIND_BOOK") == 0)
        {
            char *idStr = strtok(NULL, "\n");
            int findId = atoi(idStr);
            int found = 0;
            for (int i = 0; i < bookCount; i++)
            {
                if (books[i].id == findId)
                {
                    snprintf(reply, sizeof(reply), "%3d | %-60.60s | %-32.32s | 평점: %.2f",
                             books[i].id, books[i].title, books[i].author, books[i].rating);
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                strncpy(reply, "[오류] 해당 ID의 도서를 찾을 수 없습니다.", sizeof(reply));
            }
        }

        printf("[서버] 처리 결과:\n%s\n", reply);
        send(client_fd, reply, strlen(reply), 0);
    }

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
