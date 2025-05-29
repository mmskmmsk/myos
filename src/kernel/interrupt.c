// interrupt.c - 割り込み処理の実装
#include "../include/interrupt.h"
#include "../include/io.h"
#include "../include/keyboard.h"
#include "../include/memory.h"
#include "../include/screen.h"
#include "../include/timer.h"


// IDTのエントリ数
#define IDT_SIZE 256

// IDTテーブル
static idt_entry_t idt[IDT_SIZE];
// IDTポインタ
static idt_ptr_t idtp;

// PID (Programmable Interrupt Controller) のポート
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// 外部で定義される割り込みハンドラ（アセンブリ）
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);

extern void irq0(void);
extern void irq1(void);

// IDTエントリを設定
static void idt_set_gate(uint8_t n, uint32_t handler, uint16_t sel, uint8_t flags) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
    idt[n].selector = sel;
    idt[n].zero = 0;
    idt[n].type_attr = flags;
}

// PICの再マップ
static void pic_remap(void) {
    // マスタPICの初期化
    outb(PIC1_COMMAND, 0x11);  // 初期化コマンド
    outb(PIC1_DATA, 0x20);     // ベクタオフセット（IRQ0-7を0x20-0x27にマップ）
    outb(PIC1_DATA, 0x04);     // スレーブPICはIRQ2に接続
    outb(PIC1_DATA, 0x01);     // 8086モード
    
    // スレーブPICの初期化
    outb(PIC2_COMMAND, 0x11);  // 初期化コマンド
    outb(PIC2_DATA, 0x28);     // ベクタオフセット（IRQ8-15を0x28-0x2Fにマップ）
    outb(PIC2_DATA, 0x02);     // マスタPICのIRQ2に接続
    outb(PIC2_DATA, 0x01);     // 8086モード
    
    // 全ての割り込みをマスク（無効化）
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

// 割り込み処理の初期化
void interrupt_init(void) {
    // IDTポインタを設定
    idtp.limit = (sizeof(idt_entry_t) * IDT_SIZE) - 1;
    idtp.base = (uint32_t)&idt;
    
    // IDTをクリア
    memset(&idt, 0, sizeof(idt_entry_t) * IDT_SIZE);
    
    // PICを再マップ
    pic_remap();
    
    // 例外ハンドラを設定（0-7）
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    
    // IRQハンドラを設定
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E); // タイマー
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E); // キーボード
    
    // IDTを読み込み
    asm volatile("lidt %0" : : "m" (idtp));
    
    // タイマーとキーボード割り込みを有効化
    // 0xFC → 0xFE に変更（キーボードのみ有効、タイマー無効）
    outb(PIC1_DATA, 0xFE); // IRQ1（キーボード）のみ有効
}

// 割り込みを有効化
void interrupt_enable(void) {
    asm volatile("sti");
}

// 割り込みを無効化
void interrupt_disable(void) {
    asm volatile("cli");
}

// 特定の割り込みハンドラを設定
void set_interrupt_handler(uint8_t n, uint32_t handler) {
    idt_set_gate(n, handler, 0x08, 0x8E);
}

// 例外ハンドラ
void fault_handler(uint32_t int_no, uint32_t err_code) {
    screen_write("Exception occurred: ", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    
    char buffer[32];
    int_to_string(int_no, buffer);
    screen_write(buffer, vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    
    screen_write(" Error Code: ", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    int_to_string(err_code, buffer);
    screen_write(buffer, vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    screen_newline();
    
    // システムを停止
    while (1) {
        asm volatile("hlt");
    }
}

// IRQハンドラ
void irq_handler(uint32_t int_no) {
    // IRQ0（タイマー）の処理
    if (int_no == 32) {
        timer_handler();
    }
    // IRQ1（キーボード）の処理
    else if (int_no == 33) {
        keyboard_handler();
    }
    
    // EOI（End of Interrupt）シグナルをPICに送信
    if (int_no >= 40) {
        outb(PIC2_COMMAND, 0x20); // スレーブPICにEOI
    }
    outb(PIC1_COMMAND, 0x20); // マスタPICにEOI
}