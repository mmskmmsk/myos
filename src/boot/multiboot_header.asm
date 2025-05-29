; multiboot_header.asm - 修正されたマルチブート2ヘッダー
section .multiboot_header
align 8
header_start:
    ; マルチブート2のマジックナンバー
    dd 0xe85250d6                ; マジック（リトルエンディアン）
    dd 0                         ; アーキテクチャ（i386 = 0）
    dd header_end - header_start ; ヘッダー長
    dd -(0xe85250d6 + 0 + (header_end - header_start))  ; チェックサム

    ; 終了タグ（必須）
    align 8
    dw 0    ; タイプ（終了）
    dw 0    ; フラグ
    dd 8    ; サイズ
header_end: