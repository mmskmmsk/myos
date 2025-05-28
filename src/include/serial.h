// serial.h - シリアル通信のインターフェース
#ifndef SERIAL_H
#define SERIAL_H

#include "stdint.h"

// シリアルポートの定義
#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

// シリアルポートを初期化
int serial_init(uint16_t port);

// 1文字を送信
void serial_putchar(uint16_t port, char c);

// 文字列を送信
void serial_write(uint16_t port, const char* str);

// 1文字を受信（ブロッキング）
char serial_getchar(uint16_t port);

// 文字が利用可能かチェック
int serial_received(uint16_t port);

// 送信が完了したかチェック
int serial_is_transmit_empty(uint16_t port);

#endif // SERIAL_H