#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

USER users[MAX_USERS]; // 사용자 목록 저장 배열
int userCount = 0;     // 현재 사용자 수

/**
 * 사용자 정보를 users.txt 파일에서 읽어온다.
 */
void loadUsersFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("[서버] 사용자 파일 열기 실패");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL && userCount < MAX_USERS)
    {
        char *id = strtok(line, ":");
        char *pw = strtok(NULL, "\n");
        if (id && pw)
        {
            strncpy(users[userCount].id, id, sizeof(users[userCount].id));
            strncpy(users[userCount].pw, pw, sizeof(users[userCount].pw));
            userCount++;
        }
    }
    fclose(fp);
}

/**
 * 사용자 정보를 users.txt 파일에 저장한다.
 */
int saveUsersToFile(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        return 0;
    for (int i = 0; i < userCount; i++)
    {
        fprintf(fp, "%s:%s\n", users[i].id, users[i].pw);
    }
    fclose(fp);
    return 1;
}

/**
 * ID에 해당하는 사용자의 인덱스를 반환한다. 없으면 -1.
 */
int findUserIndex(const char *id)
{
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].id, id) == 0)
            return i;
    }
    return -1;
}

/**
 * 입력된 ID와 비밀번호가 일치하는지 확인한다.
 */
int checkLogin(const char *inputId, const char *inputPw)
{
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].id, inputId) == 0 && strcmp(users[i].pw, inputPw) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * 새로운 사용자를 등록한다. 중복 ID는 허용하지 않음.
 */
int registerUser(const char *id, const char *pw)
{
    if (userCount >= MAX_USERS || findUserIndex(id) != -1)
        return 0;
    strncpy(users[userCount].id, id, sizeof(users[userCount].id));
    strncpy(users[userCount].pw, pw, sizeof(users[userCount].pw));
    userCount++;
    return saveUsersToFile("users.txt");
}

/**
 * 사용자를 삭제한다. 비밀번호가 일치해야 삭제 가능.
 */
int deleteUser(const char *id, const char *pw)
{
    int idx = findUserIndex(id);
    if (idx == -1 || strcmp(users[idx].pw, pw) != 0)
        return 0;
    for (int i = idx; i < userCount - 1; i++)
    {
        users[i] = users[i + 1];
    }
    userCount--;
    return saveUsersToFile("users.txt");
}

/**
 * 비밀번호를 변경한다. 기존 비밀번호 확인 필요.
 */
int modifyUser(const char *id, const char *oldpw, const char *newpw)
{
    int idx = findUserIndex(id);
    if (idx == -1 || strcmp(users[idx].pw, oldpw) != 0)
        return 0;
    strncpy(users[idx].pw, newpw, sizeof(users[idx].pw));
    return saveUsersToFile("users.txt");
}