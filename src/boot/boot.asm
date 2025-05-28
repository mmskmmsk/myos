; boot.asm - OSのエントリーポイント
global start
extern kernel_main  ; C言語のカーネル関数

section .text
bits 32
start:
    ; スタックポインタを設定
    mov esp, stack_top

    ; カーネル関数を呼び出す
    call kernel_main

    ; カーネルが戻ってきた場合（通常は戻ってこない）
    ; CPUを停止
.hang:
    cli      ; 割り込み無効化
    hlt      ; CPUを停止
    jmp .hang ; 万が一HLTから復帰した場合に備えて無限ループ

; スタック用のメモリ領域（16KB）
section .bss
align 4096
stack_bottom:
    resb 16384 ; 16 KB
stack_top:
