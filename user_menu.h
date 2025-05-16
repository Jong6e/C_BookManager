#ifndef USER_MENU_H
#define USER_MENU_H

#include <winsock2.h>
#include <windows.h>

// 사용자 메뉴 진입 함수
// 로그인, 회원가입 등 사용자 관리 메뉴를 표시하고 처리
void userMenu(SOCKET sock);

// 프로그램 종료 함수
// 모든 자원을 정리하고 프로그램을 안전하게 종료
void exitProgram(SOCKET sock);

#endif