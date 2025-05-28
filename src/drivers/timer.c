// timer.c - PIT (Programmable Interval TImer) ドライバの実装
#include "../include/timer.h"
#include "../include/io.h"

// PITの制御ポート
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

// PIT入力クロック（1.193182MHz）
#define PIT_CLOCK 1193182

// タイマーのティック（割り込み）カウント
static volatile uint32_t timer_ticks = 0;

// タイマーを初期化（周波数をHz単位で指定）
void timer_init(uint32_t frequency) {
	// 分周比を計算
	uint32_t divisor = PIT_CLOCK / frequency;
	// PITに制御ワードを送信
	// 0x36 = 00110110b
	// - 00: チャンネル0を選択
	// - 11: 下位バイト、上位バイトの順に送信
	// - 011: モード3（矩形波）
	// - 0: 2進数カウント
	outb(PIT_COMMAND, 0x36);

	// 分周比の下位バイトと上位バイトを送信
	outb(PIT_CHANNEL0, divisor & 0xFF);		// 下位8ビット
	outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);	// 上位8ビット
}

// タイマーの割り込みハンドラ
void timer_handler(void) {
	timer_ticks++;
}

// 指定したミリ秒待機
void timer_sleep(uint32_t ms) {
	// ミリ秒からタイマーティック数に変換（100Hzを仮定）
	uint32_t ticks = ms / 10;
	uint32_t target_ticks = timer_ticks + ticks;

	// 目標のティック数に達するまで待機
	while (timer_ticks < target_ticks) {
	// 割り込みがないシンプルなOSでは、ポーリングでタイマーを更新
	timer_handler();
	}
}

// タイマーカウントを取得
uint32_t timer_get_ticks(void) {
	return timer_ticks;
}

