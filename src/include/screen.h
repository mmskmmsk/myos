// screen.h - 画面表示のためのインターフェース
#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"

//  画面の幅と高さ
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGAテキストモードの色定義
 #define VGA_COLOR_BLACK         0
 #define VGA_COLOR_BLUE          1
 #define VGA_COLOR_GREEN         2
 #define VGA_COLOR_CYAN          3
 #define VGA_COLOR_RED           4
 #define VGA_COLOR_MAGENTA       5
 #define VGA_COLOR_BROWN         6
 #define VGA_COLOR_LIGHT_GREY    7
 #define VGA_COLOR_DARK_GREY     8
 #define VGA_COLOR_LIGHT_BLUE    9
 #define VGA_COLOR_LIGHT_GREEN   10
 #define VGA_COLOR_LIGHT_CYAN    11
 #define VGA_COLOR_LIGHT_RED     12
 #define VGA_COLOR_LIGHT_MAGENTA 13
 #define VGA_COLOR_LIGHT_BROWN   14
 #define VGA_COLOR_WHITE         15

// 前景色と背景色から色属性を作成
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
	return fg | (bg << 4);
}

// VGAエントリ（文字と色属性）を作成
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | ((uint16_t) color << 8);
}

// 画面を初期化
void screen_init(void);

// 画面をクリア
void screen_clear(void);

// 文字を指定した色で表示
void screen_put_char(char c, uint8_t color);

// 文字列を指定した色で表示
void screen_write(const char* str, uint8_t color);

// 改行
void screen_newline(void);

// カーソル位置を設定
void screen_set_cursor(int x, int y);

// カーソル位置を取得
void screen_get_cursor(int *x, int *y);

#endif // SCREEN_h
