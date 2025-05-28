// screen.c - VGAテキストモードのドライバ実装
#include "../include/screen.h"
#include "../include/io.h"
#include "../include/stddef.h"

// VGAテキストモードのバッファアドレス
#define VGA_BUFFER 0xB8000


// カーソル位置
static int cursor_x = 0;
static int cursor_y = 0;
// VGAバッファのポインタ
static uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;

// カーソル位置をハードウェアに更新する
static void update_cursor() {
	// カーソル位置を計算
	uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
	
	// カーソル位置の下位バイトを送信
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));

	// カーソル位置の上位バイトを送信
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

// 画面をスクロールする
static void scroll() {
	uint8_t blank_attr = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	uint16_t blank = vga_entry(' ', blank_attr);

	//一番上の行が画面から出る場合
	if (cursor_y >= VGA_HEIGHT) {
		// テキストを1行分上にコピー
		for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
			vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
		}

		// 最後の行をクリア
		for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
			vga_buffer[i] = blank;
		}

		// カーソル位置を調整
		cursor_y = VGA_HEIGHT - 1;
	}
}

// 画面を初期化
void screen_init(void) {
	// 画面をクリア
	screen_clear();

	// カーソルを更新
	update_cursor();
}

// 画面をクリア
void screen_clear(void) {
	uint8_t blank_attr = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	uint16_t blank = vga_entry(' ', blank_attr);

	// 全ての文字位置を空白で埋める
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		vga_buffer[i] = blank;
	}

	// カーソルを左上に戻す
	cursor_x = 0;
	cursor_y = 0;
	update_cursor();
}

// 文字を指定した色で表示
void screen_put_char(char c, uint8_t color) {
    // バックスペース処理
    if (c == '\b' && cursor_x > 0) {
        cursor_x--;
    }
	// タブ処理（8スペース）
	else if (c == '\t') {
		cursor_x = (cursor_x + 8) & ~(8 - 1);
	}
	// キャリッジリターン処理
	else if (c == '\r') {
		cursor_x = 0;
	}
	// 改行処理
	else if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
	}
	// 表示可能な文字の場合
	else if (c >= ' ') {
		// 文字と色属性を書き込む
		vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = vga_entry(c, color);
		cursor_x++;
	}
	
	// 行末まで来たら次の行へ
	if (cursor_x >= VGA_WIDTH) {
		cursor_x = 0;
		cursor_y++;
	}

	// 必要に応じてスクロール
	scroll();

	// カーソル位置を更新
	update_cursor();

}

// 文字列を指定した色で表示
void screen_write(const char* str, uint8_t color) {
	for (size_t i = 0; str[i] != '\0'; i++) {
		screen_put_char(str[i], color);
	}
}

// 改行
void screen_newline(void) {
	cursor_x = 0;
	cursor_y++;
	scroll();
	update_cursor();
}

// カーソル位置を設定
void screen_set_cursor(int x, int y) {
	cursor_x = x;
	cursor_y = y;

	// 範囲を制限
	if (cursor_x < 0) cursor_x = 0;
	if (cursor_x >= VGA_WIDTH) cursor_x = VGA_WIDTH - 1;
	if (cursor_y < 0) cursor_y = 0;
	if (cursor_y >= VGA_HEIGHT) cursor_y = VGA_HEIGHT - 1;

	update_cursor();
}

// カーソル位置を取得
void screen_get_cursor(int *x, int *y) {
	if (x) *x = cursor_x;
	if (y) *y = cursor_y;
}
