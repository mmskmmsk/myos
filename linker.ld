/* リンカースクリプト */
ENTRY(start) /* エントリーポイントを指定 */

SECTIONS {
    /* カーネルを1MBから開始（ブートローダのメモリ領域を避ける）*/
    . = 1M;
    
    /* マルチブートヘッダーを最初に配置（重要！）*/
    .multiboot_header : {
        *(.multiboot_header)
    }
    
    /* .textセクション（コード）- startラベルが最初に来る */
    .text : ALIGN(4K) {
        *(.text)
    }

    /* .rodataセクション（読み取り専用データ）*/
    .rodata : ALIGN(4K) {
        *(.rodata)
    }

    /* .dataセクション（初期化済みデータ）*/
    .data : ALIGN(4K) {
        *(.data)
    }

    /* .bssセクション（未初期化データ）*/
    .bss : ALIGN(4K) {
        *(.bss)
    }
}
