#include "../include/debug.h"
#include "../include/screen.h"
#include "../include/string.h"

// デバッグ行の最大数
#define DEBUG_LINES 3
// デバッグ表示開始行
#define DEBUG_START_LINE (25 - DEBUG_LINES)

// デバッグメッセージを表示
void debug_log(const char* message) {
    // 現在のカーソル位置を保存
    int save_x, save_y;
    screen_get_cursor(&save_x, &save_y);
    
    // デバッグ領域（画面下部）に移動
    screen_set_cursor(0, DEBUG_START_LINE);
    
    // 既存のデバッグメッセージを上にスクロール
    for (int i = DEBUG_START_LINE; i < DEBUG_START_LINE + DEBUG_LINES - 1; i++) {
        for (int j = 0; j < 80; j++) {
            uint16_t* vga_buffer = (uint16_t*)0xB8000;
            vga_buffer[i * 80 + j] = vga_buffer[(i + 1) * 80 + j];
        }
    }
    
    // デバッグメッセージを表示
    screen_set_cursor(0, DEBUG_START_LINE + DEBUG_LINES - 1);
    screen_write(message, vga_entry_color(VGA_COLOR_BROWN, VGA_COLOR_BLACK));
    
    // カーソル位置を元に戻す
    screen_set_cursor(save_x, save_y);
}

// 数値付きデバッグメッセージを表示
void debug_log_int(const char* message, int value) {
    // 簡易的な数値変換
    char buffer[16];
    int i = 0;
    int is_negative = 0;
    
    // 負の数処理
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }
    
    // 数値を文字に変換
    do {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);
    
    if (is_negative) {
        buffer[i++] = '-';
    }
    buffer[i] = '\0';
    
    // 文字列を反転
    for (int j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }
    
    // メッセージと数値を表示
    char full_message[80]; // 十分な長さを確保
    int pos = 0;
    
    // メッセージをコピー
    while (*message) {
        full_message[pos++] = *message++;
    }
    
    // 区切り
    full_message[pos++] = ':';
    full_message[pos++] = ' ';
    
    // 数値を追加
    for (int j = 0; buffer[j]; j++) {
        full_message[pos++] = buffer[j];
    }
    
    full_message[pos] = '\0';
    
    // デバッグ表示
    debug_log(full_message);
}