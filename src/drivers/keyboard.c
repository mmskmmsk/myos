// keyboard.c - キーボードドライバの実装
#include "../include/keyboard.h"
#include "../include/io.h"
#include "../include/screen.h"
#include "../include/stddef.h"



// キーバッファ（キー入力を保存する）
static char key_buffer[KEYBOARD_BUFFER_SIZE];
// バッファの先頭と末尾のインデックス
static size_t buffer_write = 0;
static size_t buffer_read = 0;

// US/UKキーボードのスキャンコードからASCIIへのマッピング
static const char scancode_to_ascii[] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
	0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// シフトキーが押されているか
static uint8_t shift_pressed = 0;

// US/UKキーボードのシフト時のスキャンコードからASCIIへのマッピング
static const char scancode_to_ascii_shift[] = {
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
	0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// キーバッファに文字を追加
static void buffer_put(char c) {
	// バッファがいっぱいの場合は何もしない
	    if ((buffer_write + 1) % KEYBOARD_BUFFER_SIZE == buffer_read) {
		    return;
	    }

	    // 文字をバッファに追加
	    key_buffer[buffer_write] = c;
	    buffer_write = (buffer_write + 1) % KEYBOARD_BUFFER_SIZE;
}

// キーバッファから文字を取得
static char buffer_get() {
	// バッファがからの場合はゼロを返す
	if (buffer_read == buffer_write) {
		return 0;
	}

	// バッファから文字を取得
	char c = key_buffer[buffer_read];
	buffer_read = (buffer_read + 1) % KEYBOARD_BUFFER_SIZE;
	return c;
}

// キーボード割り込みハンドラ
void keyboard_handler(void) {
    // キーボードからスキャンコードを取得
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // キーを離したイベント（最上位ビットが1）
    if (scancode & 0x80) {
        scancode &= 0x7F; // 最上位ビットをクリア

        // シフトキーを離した場合
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 0;
        }
    }
    // キーを押したイベント
    else {
        // シフトキーが押された場合
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {            
            shift_pressed = 1;
            // return を削除してPIC EOI送信を確実に行う
        }
        else {
            // スキャンコードをASCIIに変換
            char ascii;
            if (shift_pressed && scancode < sizeof(scancode_to_ascii_shift)) {
                ascii = scancode_to_ascii_shift[scancode];
            } else if (scancode < sizeof(scancode_to_ascii)) {
                ascii = scancode_to_ascii[scancode];
            } else {
                ascii = 0; // 不明なスキャンコード
            }

            // ASCIIが有効な場合、バッファに追加し画面に表示
            if (ascii) {
                buffer_put(ascii);
                if (ascii != '\b') {
                    screen_put_char(ascii, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
                }
            }
        }
    }
    
    // PIC EOI送信は削除（irq_handlerで処理される）
}

// キーボードを初期化
void keyboard_init(void) {
	// 特に必要な初期化はないが、関数を用意しておく
	// 実際のOSでは、ここで割り込みハンドラを設定する
}

// キー入力を処理
void keyboard_process(void) {
	// キーボードからのデータが利用可能かチェック
	if ((inb(0x64) & 0x1)) {
		keyboard_handler();
	}
}

// キー入力を松（次のキー入力があるまでブロック）
char keyboard_get_char(void) {
	char c;
	// バッファが空の間は処理を繰り返す
	while(!(c = buffer_get())) {
		keyboard_process();
	}
	return c;
}

// キーバッファに文字があるか確認
uint8_t keyboard_has_key(void) {
	return buffer_read != buffer_write;
}

