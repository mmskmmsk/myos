// serial.c - シリアル通信ドライバの実装
#include "../include/serial.h"
#include "../include/io.h"

// シリアルポートを初期化
int serial_init(uint16_t port) {
    // 割り込みを無効化
    outb(port + 1, 0x00);
    
    // DLABを有効にしてボーレート設定
    outb(port + 3, 0x80);
    // 9600bpsに設定（115200 / 9600 = 12）
    outb(port + 0, 0x0C);    // 除数の下位バイト
    outb(port + 1, 0x00);    // 除数の上位バイト
    
    // 8ビット、パリティなし、ストップビット1に設定
    outb(port + 3, 0x03);
    
    // FIFOを有効化（14バイト閾値）
    outb(port + 2, 0xC7);
    
    // IRQ有効、RTS/DTR設定
    outb(port + 4, 0x0B);
    
    // ループバックテストを行う
    outb(port + 4, 0x1E);    // ループバック有効
    outb(port + 0, 0xAE);    // テストバイト送信
    
    // 受信したバイトをチェック
    if (inb(port + 0) != 0xAE) {
        return 1; // 初期化失敗
    }
    
    // ループバック無効、正常モードに設定
    outb(port + 4, 0x0F);
    return 0; // 初期化成功
}

// 送信が完了したかチェック
int serial_is_transmit_empty(uint16_t port) {
    return inb(port + 5) & 0x20;
}

// 1文字を送信
void serial_putchar(uint16_t port, char c) {
    // 送信バッファが空になるまで待機
    while (serial_is_transmit_empty(port) == 0);
    
    // 文字を送信
    outb(port, c);
}

// 文字列を送信
void serial_write(uint16_t port, const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_putchar(port, str[i]);
    }
}

// 文字が利用可能かチェック
int serial_received(uint16_t port) {
    return inb(port + 5) & 1;
}

// 1文字を受信（ブロッキング）
char serial_getchar(uint16_t port) {
    // データが受信されるまで待機
    while (serial_received(port) == 0);
    
    // 文字を読み取り
    return inb(port);
}