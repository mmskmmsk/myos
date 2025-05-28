; multiboot_header.asm - GRUBがカーネルを認識するためのマルチブートヘッダ
section .multiboot_header
header_start:
    ; マジックナンバー
    dd 0xe85250d6                ; マルチブート2のマジック
    dd 0                         ; アーキテクチャ 0 (32ビット保護モード i386)
    dd header_end - header_start ; ヘッダのサイズ
    ; チェックサム
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; 必須の終了タグ
    dw 0    ; タイプ
    dw 0    ; フラグ
    dd 8    ; サイズ
header_end:
