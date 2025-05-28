; interrupt_asm.asm - 割り込みハンドラのアセンブリ部分
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global irq0, irq1

extern fault_handler
extern irq_handler

; 例外ハンドラのマクロ（エラーコードなし）
%macro ISR_NOERRCODE 1
isr%1:
    cli
    push 0          ; ダミーのエラーコード
    push %1         ; 割り込み番号
    jmp isr_common_stub
%endmacro

; IRQハンドラのマクロ
%macro IRQ 2
irq%1:
    cli
    push 0          ; ダミーのエラーコード
    push %2         ; 割り込み番号
    jmp irq_common_stub
%endmacro

; 例外ハンドラ
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7

; IRQハンドラ
IRQ 0, 32
IRQ 1, 33

; 共通の例外ハンドラスタブ
isr_common_stub:
    pusha           ; すべてのレジスタを保存
    
    ; Cの関数を呼び出し
    push esp        ; スタックポインタを引数として渡す
    call fault_handler
    add esp, 4      ; スタックを元に戻す
    
    popa            ; すべてのレジスタを復元
    add esp, 8      ; エラーコードと割り込み番号をスタックから削除
    iret            ; 割り込みから復帰

; 共通のIRQハンドラスタブ
irq_common_stub:
    pusha           ; すべてのレジスタを保存
    
    ; Cの関数を呼び出し
    push esp        ; スタックポインタを引数として渡す
    call irq_handler
    add esp, 4      ; スタックを元に戻す
    
    popa            ; すべてのレジスタを復元
    add esp, 8      ; エラーコードと割り込み番号をスタックから削除
    iret            ; 割り込みから復帰