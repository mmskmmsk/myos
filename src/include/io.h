// io.h - 入出力操作のための関数定義
#ifndef IO_H
#define IO_H

// ポートから1バイトのデータを読み取る
static inline unsigned char inb(unsigned short port) {
	unsigned char result;
	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

// ポートに1バイトのデータを書き込む
static inline void outb(unsigned short port, unsigned char data) {
	__asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

//ポートに2バイトのデータを読み取る
static inline unsigned short inw(unsigned short port) {
	unsigned short result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

//ポートに2バイトのデータを書き込む
static inline void outw(unsigned short port, unsigned short data) {
	__asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

#endif // IO_H
