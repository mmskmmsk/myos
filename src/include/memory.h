// memory.h - メモリ管理のインターフェース
#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"
#include "stddef.h"

// メモリの初期化
void memory_init(void);

// メモリの割り当て
void* kmalloc(size_t size);

// メモリの解放
void kfree(void* ptr);

// 指定アドレスからサイズ分のメモリを0で埋める
void memset(void* ptr, int value, size_t size);

// メモリ間のコピー
void memcpy(void* dest, const void* src, size_t size);

// メモリの統計情報を表示
void memory_stats(void);

#endif // MEMORY_H
