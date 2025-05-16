#ifndef BOOK_MENU_H
#define BOOK_MENU_H

#include <winsock2.h>
#include <windows.h>

// 도서관리 메뉴 진입 함수
// 도서 검색, 추가, 수정, 삭제 등 도서 관리 기능 메뉴 표시 및 처리
void bookMenu(SOCKET sock);

#endif