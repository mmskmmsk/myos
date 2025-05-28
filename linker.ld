/* リンカースクリプト */
ENTRY(start) /* エントリーポイントを指定 */

SECTIONS {
	/* カーネルを1MBから開始（ブートローダのメモリ領域を避ける）*/
	. = 1M;
	
	/* .textセクション（コード）*/
	.text : ALIGN(4K) {
		*(.multiboot_header)
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
