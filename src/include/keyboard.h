// keyboard.h - キーボード入力のためのインターフェース
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "stdint.h"

// 特殊キーのスキャンコード
#define KEY_ESC         0x01
#define KEY_BACKSPACE   0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LCTRL       0x1D
#define KEY_LSHIFT      0x2A
#define KEY_RSHIFT      0x36
#define KEY_LALT        0x38
#define KEY_CAPSLOCK    0x3A
#define KEY_F1          0x3B
#define KEY_F2          0x3C
#define KEY_F3          0x3D
#define KEY_F4          0x3E
#define KEY_F5          0x3F
#define KEY_F6          0x40
#define KEY_F7          0x41
#define KEY_F8          0x42
#define KEY_F9          0x43
#define KEY_F10         0x44
#define KEY_NUMLOCK     0x45
#define KEY_SCROLLLOCK  0x46

// キーボードデータポート
#define KEYBOARD_DATA_PORT 0x60

// キーバッファのサイズ
#define KEYBOARD_BUFFER_SIZE 256

// キーボードを初期化
void keyboard_init(void);

// キー入力を処理
void keyboard_process(void);

// キー入力を待つ（次のキー入力があるまでブロック）
char keyboard_get_char(void);

// キーバッファに文字があるか確認
uint8_t keyboard_has_key(void);

// キーボード割り込みハンドラ
void keyboard_handler(void);



#endif // KEYBOARD_H
