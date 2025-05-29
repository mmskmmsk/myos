#	コンパイラとツール
ASM=nasm
CC=gcc
LD=ld
QEMU=qemu-system-i386

#	フラグ
ASMFLAGS=-felf32
CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding -Wall -Wextra
LDFLAGS=-m elf_i386 -T linker.ld

#	ディレクトリ
SRC_DIR=src
BUILD_DIR=build
ISO_DIR=iso

#	ソースファイル
ASM_SRC=$(wildcard $(SRC_DIR)/boot/*.asm)
C_SRC=$(wildcard $(SRC_DIR)/kernel/*.c) $(wildcard $(SRC_DIR)/drivers/*.c)

#	オブジェクトファイル
ASM_OBJ=$(patsubst $(SRC_DIR)/boot/%.asm, $(BUILD_DIR)/%.o, $(ASM_SRC))
C_OBJ=$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRC))
OBJ=$(ASM_OBJ) $(C_OBJ)

#	ターゲット
.PHONY: all clean run

#	デフォルトターゲット
all: $(BUILD_DIR)/myos.iso

#	ビルドディレクトリ作成
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/kernel
	mkdir -p $(BUILD_DIR)/drivers

#	ISOディレクトリ作成
$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

#	アセンブリのコンパイル
$(BUILD_DIR)/%.o: $(SRC_DIR)/boot/%.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

#	Cファイルのコンパイル
$(BUILD_DIR)/kernel/%.o: $(SRC_DIR)/kernel/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/%.o: $(SRC_DIR)/drivers/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#	カーネルのリンク
$(BUILD_DIR)/kernel.bin: $(OBJ) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

#	ISOイメージの作成
$(BUILD_DIR)/myos.iso: $(BUILD_DIR)/kernel.bin $(ISO_DIR)
	cp $(BUILD_DIR)/kernel.bin $(ISO_DIR)/boot/
	echo 'set timeout=3' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "MyOS v1.0" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    multiboot2 /boot/kernel.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR)

#	実行
# 実行部分を以下に置き換え
run: $(BUILD_DIR)/myos.iso
	$(QEMU) -cdrom $(BUILD_DIR)/myos.iso -boot d -m 512

# デバッグ用の実行
run-debug: $(BUILD_DIR)/myos.iso
	$(QEMU) -cdrom $(BUILD_DIR)/myos.iso -boot d -m 512 -monitor stdio

# シリアルポート付きで実行
run-serial: $(BUILD_DIR)/myos.iso
	$(QEMU) -cdrom $(BUILD_DIR)/myos.iso -boot d -m 512 -serial stdio

#	クリーン
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)
