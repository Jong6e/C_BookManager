#ifndef USER_H
#define USER_H

#define MAX_USERS 100 // 최대 사용자 수 제한

/**
 * 사용자 구조체 정의 (ID, 비밀번호)
 */
typedef struct
{
    char id[20]; // 사용자 ID
    char pw[20]; // 사용자 비밀번호
} USER;

extern USER users[];  // 사용자 배열
extern int userCount; // 현재 사용자 수

void loadUsersFromFile(const char *filename);                         // users.txt에서 사용자 정보 읽기
int checkLogin(const char *inputId, const char *inputPw);             // ID/PW 로그인 확인
int registerUser(const char *id, const char *pw);                     // 사용자 등록
int deleteUser(const char *id, const char *pw);                       // 사용자 삭제
int findUserIndex(const char *id);                                    // 사용자 인덱스 조회
int modifyUser(const char *id, const char *oldpw, const char *newpw); // 비밀번호 변경
int saveUsersToFile(const char *filename);                            // 사용자 정보를 파일로 저장

#endif