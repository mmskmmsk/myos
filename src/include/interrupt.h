// interrupt.h - 割り込み処理のインターフェース
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "stdint.h"

// IDTエントリの構造体
typedef struct {
    uint16_t offset_low;   // 割り込みハンドラのアドレス（下位16ビット）
    uint16_t selector;     // コードセグメントセレクタ
    uint8_t zero;          // 予約済み（必ず0）
    uint8_t type_attr;     // タイプとアトリビュート
    uint16_t offset_high;  // 割り込みハンドラのアドレス（上位16ビット）
} __attribute__((packed)) idt_entry_t;

// IDTポインタの構造体
typedef struct {
    uint16_t limit;        // IDTのサイズ（バイト単位）
    uint32_t base;         // IDTのベースアドレス
} __attribute__((packed)) idt_ptr_t;

// 割り込み処理の初期化
void interrupt_init(void);

// 割り込みを有効化
void interrupt_enable(void);

// 割り込みを無効化
void interrupt_disable(void);

// 特定の割り込みハンドラを設定
void set_interrupt_handler(uint8_t n, uint32_t handler);

#endif // INTERRUPT_H
