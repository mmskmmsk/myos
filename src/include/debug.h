#ifndef DEBUG_H
#define DEBUG_H

#include "stdint.h"

// デバッグメッセージを表示
void debug_log(const char* message);

// 数値付きデバッグメッセージを表示
void debug_log_int(const char* message, int value);

#endif // DEBUG_H