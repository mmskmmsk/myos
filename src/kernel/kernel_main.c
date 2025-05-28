// kernel_main.c - 拡張されたカーネルのメイン関数
#include "../include/interrupt.h"
#include "../include/keyboard.h"
#include "../include/memory.h"
#include "../include/screen.h"
#include "../include/serial.h"
#include "../include/string.h"
#include "../include/timer.h"


// カーネルのメイン関数
void kernel_main(void) {
    // 割り込みを無効化
    interrupt_disable();
    
    // 画面の初期化
    screen_init();
    
    // メモリ管理の初期化
    memory_init();
    
    // 割り込み処理の初期化
    interrupt_init();
    
    // キーボードの初期化
    keyboard_init();
    
    // タイマーの初期化（100Hz）
    timer_init(100);
    
    // シリアルポートの初期化
    serial_init(SERIAL_COM1);
    
    // 割り込みを有効化
    interrupt_enable();
    
    // ウェルカムメッセージ
    screen_write("Welcome to ", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    screen_write("My", vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
    screen_write("OS", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    screen_write(" v1.0!\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    
    screen_write("A minimal operating system with advanced features.\n", 
                vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    
    // シリアルにもメッセージを送信
    serial_write(SERIAL_COM1, "MyOS v1.0 - Serial communication active\r\n");
    
    // シンプルなコマンドライン
    screen_newline();
    screen_write("Type 'help' for available commands.\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    
    char command[256];
    int cmd_pos = 0;
    
    while (1) {
        // コマンドプロンプトを表示
        screen_write("> ", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        
        // コマンド入力を処理
        cmd_pos = 0;

		// コマンド入力開始時のカースト位置を記録
		int prompt_x, prompt_y;
		screen_get_cursor(&prompt_x, &prompt_y);

        while (1) {
            // キーボード入力を処理（割り込み駆動）
            if (keyboard_has_key()) {
                char c = keyboard_get_char();
                
                // Enterキーで入力終了
                if (c == '\n') {
                    command[cmd_pos] = '\0';
                    screen_newline();
                    break;
                }
				// バックスペースで文字削除
				else if (c == '\b') {
					if (cmd_pos > 0) {
						cmd_pos--;
						// カーソル位置を取得
						int cursor_x, cursor_y;
						screen_get_cursor(&cursor_x, &cursor_y);

						// カーソル位置がプロンプトの位置より下の場合
						if (cursor_x == 0 && cursor_y > prompt_y) {
							cursor_y--;
							cursor_x = VGA_WIDTH - 1;
							screen_set_cursor(cursor_x, cursor_y);
							screen_put_char(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
							screen_set_cursor(cursor_x, cursor_y);
						}
						else if (cursor_x > 1 || cursor_y > prompt_y) {
							// 通常のバックスペース処理
							cursor_x--;
							screen_set_cursor(cursor_x, cursor_y);
							screen_put_char(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
							screen_set_cursor(cursor_x, cursor_y);
						}
						
					}
				}
                // 通常の文字入力
                else if (cmd_pos < 255 && c >= ' ' && c <= '~') {
                    command[cmd_pos++] = c;
                }
            }
            
            // CPUを少し休ませる
            asm volatile("hlt");
        }
        
        // コマンドを解釈して実行
        if (cmd_pos > 0) {
            // helpコマンド
            if (strcmp(command, "help") == 0) {
                screen_write("Available commands:\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
                screen_write("  help - Display this help message\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  clear - Clear the screen\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  echo [text] - Display text\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  info - System information\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  memory - Memory statistics\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  test - Memory allocation test\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  serial [text] - Send text via serial port\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
            }
            // clearコマンド
            else if (strcmp(command, "clear") == 0) {
                screen_clear();
            }
            // echoコマンド
            else if (strncmp(command, "echo ", 5) == 0) {
                screen_write(command + 5, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
                screen_newline();
            }
            // infoコマンド
            else if (strcmp(command, "info") == 0) {
                screen_write("MyOS - A minimal OS for learning\n", vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
                screen_write("Version: 1.0\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("Features:\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  - Dynamic memory management\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  - Interrupt handling\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  - Keyboard support\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  - VGA text mode output\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  - Timer (PIT @ 100Hz)\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                screen_write("  - Serial communication\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
            }
            // memoryコマンド
            else if (strcmp(command, "memory") == 0) {
                memory_stats();
            }
            // testコマンド
            else if (strcmp(command, "test") == 0) {
                screen_write("Memory allocation test:\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
                void* ptr1 = kmalloc(100);
                void* ptr2 = kmalloc(200);
                void* ptr3 = kmalloc(50);
                
                if (ptr1 && ptr2 && ptr3) {
                    screen_write("  Allocated 3 blocks successfully\n", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
                } else {
                    screen_write("  Allocation failed\n", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
                }
                
                kfree(ptr2);
                screen_write("  Freed middle block\n", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
                
                void* ptr4 = kmalloc(150);
                if (ptr4) {
                    screen_write("  Allocated new block successfully\n", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
                }
                
                kfree(ptr1);
                kfree(ptr3);
                kfree(ptr4);
                screen_write("  Test completed\n", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
            }
            // serialコマンド
            else if (strncmp(command, "serial ", 7) == 0) {
                serial_write(SERIAL_COM1, command + 7);
                serial_write(SERIAL_COM1, "\r\n");
                screen_write("Message sent via serial port\n", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
            }
            // 不明なコマンド
            else {
                screen_write("Unknown command: ", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
                screen_write(command, vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
                screen_newline();
            }
        }
    }
}