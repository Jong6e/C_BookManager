#include <stdio.h>
#include <string.h>
#include "user.h"

void handleUserCommand(const char *request, char *reply)
{
    char buffer[1000];
    strncpy(buffer, request, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *cmd = strtok(buffer, ":");

    if (strcmp(cmd, "LOGIN") == 0)
    {
        char *id = strtok(NULL, ":");
        char *pw = strtok(NULL, "\n");
        const char *result = checkLogin(id, pw) ? "로그인 성공." : "ID 또는 비밀번호가 잘못되었습니다.";
        strncpy(reply, result, 1000);
    }
    else if (strcmp(cmd, "REGISTER") == 0)
    {
        char *id = strtok(NULL, ":");
        char *pw = strtok(NULL, "\n");
        const char *result = registerUser(id, pw) ? "회원가입 완료." : "이미 존재하는 ID입니다.";
        strncpy(reply, result, 1000);
    }
    else if (strcmp(cmd, "DELETE") == 0)
    {
        char *id = strtok(NULL, ":");
        char *pw = strtok(NULL, "\n");
        const char *result = deleteUser(id, pw) ? "삭제 완료." : "비밀번호가 일치하지 않거나 사용자 없음.";
        strncpy(reply, result, 1000);
    }
    else if (strcmp(cmd, "FIND") == 0)
    {
        char *id = strtok(NULL, "\n");
        const char *result = findUserIndex(id) != -1 ? "사용자가 존재합니다." : "존재하지 않는 사용자입니다.";
        strncpy(reply, result, 1000);
    }
    else if (strcmp(cmd, "MODIFY") == 0)
    {
        char *id = strtok(NULL, ":");
        char *oldpw = strtok(NULL, ":");
        char *newpw = strtok(NULL, "\n");
        const char *result = modifyUser(id, oldpw, newpw) ? "비밀번호 변경 완료." : "기존 비밀번호 불일치 또는 사용자 없음.";
        strncpy(reply, result, 1000);
    }
    else
    {
        strncpy(reply, "[오류] 사용자 명령 처리 실패", 1000);
    }
}