// memory.c - シンプルなメモリ管理の実装

#include "../include/memory.h"
#include "../include/screen.h"

// メモリプールの開始アドレス
#define MEMORY_START 0x100000
// メモリプールのサイズ(1MB)
#define MEMORY_SIZE 0x100000
// メモリブロックの最小サイズ
#define MIN_BLOCK_SIZE 16

// メモリブロックの状態
#define BLOCK_FREE 0
#define BLOCK_USED 1

// メモリブロックのヘッダ構造体
typedef struct block_header {
    size_t size; // ブロックのサイズ
    uint8_t status;  // ブロックの状態 (使用中/未使用)
    struct block_header* next; // 次のブロックへのポインタ
} block_header_t;

// メモリプールの開始アドレス
static void* memory_pool = (void*)MEMORY_START;
// 最初のブロックヘッダへのポインタ
static block_header_t* first_block = NULL;
// 割り当てられたメモリの合計サイズ
static size_t allocated_memory = 0;
// 利用可能なメモリの合計サイズ
static size_t free_memory = 0;

// メモリの初期化
void memory_init(void) {
    // 最初のブロックを設定
    first_block = (block_header_t*)memory_pool;
    first_block->size = MEMORY_SIZE - sizeof(block_header_t);
    first_block->status = BLOCK_FREE;
    first_block->next = NULL;

    free_memory = MEMORY_SIZE - sizeof(block_header_t);
    allocated_memory = 0;
}

// メモリの割り当て（単純なファーストフィット方式）
void* kmalloc(size_t size) {
    // サイズを最小ブロックサイズにアライン
    if (size < MIN_BLOCK_SIZE) {
        size = MIN_BLOCK_SIZE;
    }
    
    // 4バイト境界にアライン
    size = (size + 3) & ~3;
    
    block_header_t* current = first_block;
    
    // 利用可能なブロックを探す
    while (current != NULL) {
        if (current->status == BLOCK_FREE && current->size >= size) {
            // 十分な大きさのフリーブロックを発見
            
            // ブロックを分割するかどうか決定
            if (current->size > size + sizeof(block_header_t) + MIN_BLOCK_SIZE) {
                // ブロックを分割
                block_header_t* new_block = (block_header_t*)((char*)current + sizeof(block_header_t) + size);
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->status = BLOCK_FREE;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            
            // ブロックを使用中に設定
            current->status = BLOCK_USED;
            allocated_memory += current->size;
            free_memory -= current->size;
            
            // データ部分のポインタを返す
            return (void*)((char*)current + sizeof(block_header_t));
        }
        current = current->next;
    }
    
    // 利用可能なブロックが見つからない
    return NULL;
}

// メモリの解放
void kfree(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    // ブロックヘッダを取得
    block_header_t* block = (block_header_t*)((char*)ptr - sizeof(block_header_t));
    
    // ブロックをフリーに設定
    block->status = BLOCK_FREE;
    allocated_memory -= block->size;
    free_memory += block->size;
    
    // 隣接するフリーブロックと結合
    block_header_t* current = first_block;
    while (current != NULL) {
        if (current->status == BLOCK_FREE && current->next != NULL && current->next->status == BLOCK_FREE) {
            // 隣接するフリーブロックを結合
            block_header_t* next_block = current->next;
            current->size += sizeof(block_header_t) + next_block->size;
            current->next = next_block->next;
        } else {
            current = current->next;
        }
    }
}

// 指定アドレスからサイズ分のメモリを指定値で埋める
void memset(void* ptr, int value, size_t size) {
    unsigned char* p = (unsigned char*)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = (unsigned char)value;
    }
}

// メモリ間のコピー
void memcpy(void* dest, const void* src, size_t size) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

// メモリの統計情報を表示
void memory_stats(void) {
    screen_write("Memory Statistics:\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    
    // 文字列表示用の関数が必要なので、簡易的な実装
    char buffer[32];
    
    // 割り当て済みメモリを表示
    screen_write("  Allocated: ", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    print_size(allocated_memory, buffer);
    screen_write(buffer, vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    screen_write(" bytes\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    
    // フリーメモリを表示
    screen_write("  Free: ", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    print_size(free_memory, buffer);
    screen_write(buffer, vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    screen_write(" bytes\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    
    // 総メモリ容量を表示
    screen_write("  Total: ", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    print_size(MEMORY_SIZE, buffer);
    screen_write(buffer, vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    screen_write(" bytes\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

// 整数を文字列に変換する簡易関数
void int_to_string(uint32_t value, char* buffer) {
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    char temp[32];
    int index = 0;
    
    while (value > 0) {
        temp[index++] = '0' + (value % 10);
        value /= 10;
    }
    
    // 文字列を逆順にしてbufferにコピー
    for (int i = 0; i < index; i++) {
        buffer[i] = temp[index - 1 - i];
    }
    buffer[index] = '\0';
}

// サイズを人間が読みやすい形式に変換する関数
void print_size(size_t size, char* buffer) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int index = 0;
    
    while (size >= 1024 && index < 4) {
        size /= 1024;
        index++;
    }
    
    // 整数部分と小数部分を分ける
    size_t integer_part = size;
    size_t decimal_part = (size - integer_part) * 100;
    
    // 整数部分を文字列に変換
    int_to_string(integer_part, buffer);
    
    // 小数部分を文字列に追加
    if (decimal_part > 0) {
        char decimal_buffer[8];
        int_to_string(decimal_part, decimal_buffer);
        strcat(buffer, ".");
        strcat(buffer, decimal_buffer);
    }
    
    // 単位を追加
    strcat(buffer, suffixes[index]);
}