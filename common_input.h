#ifndef COMMON_INPUT_H
#define COMMON_INPUT_H

#include <stddef.h>

// 콘솔 화면 초기화 함수
void clearScreen();

// 사용자 입력 (ESC 취소 포함, * 마스킹 여부는 masked)
void getEscapableInput(char *buffer, size_t size, const char *label, int masked);

#endif