// timer.h - タイマー昨日のインターフェース
#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

// タイマーを初期化 (周波数をHz単位で指定）
void timer_init(uint32_t frequency);

// 指定したミリ秒待機
void timer_sleep(uint32_t ms);

// タイマーカウントを取得
uint32_t timer_get_ticks(void);

#endif // TIMER_H
