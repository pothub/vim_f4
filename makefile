#------------------------------------------------------------------------------
# STM32F4 program 親MAKEFILE
#------------------------------------------------------------------------------
# コンパイラパスを指定（システムパスが通っている場所のコマンドを利用する場合は空にする）
export BIN_PATH :=
# コマンド名
export CC      := $(BIN_PATH)arm-none-eabi-gcc
export AR      := $(BIN_PATH)arm-none-eabi-ar
export OBJCOPY := $(BIN_PATH)arm-none-eabi-objcopy
export SIZE    := $(BIN_PATH)arm-none-eabi-size

# デバイスを指定
DEVICE := STM32F4XX

# HSE（外付けクリスタル）の周波数 [Hz]
HSE_VALUE := 25000000

# スタートアップファイル
STARTUPFILE := startup_stm32f4xx.s

# ファイル格納ディレクトリ
export LD_DIR:=ldscript/
export SRC_DIR:=
export INC_DIR:=
export OUT_DIR:=build/

export MEMORY_DEF :=../memory.ld
export INCLUDE_SYM :=includes.txt

# STMライブラリ
export STMLIB_DIR := ../Libraries/
export CORE_SRC_PATH :=
export CORE_HEADER_PATH := $(STMLIB_DIR)CMSIS/Include/
export DEVICE_HEADER_PATH := $(STMLIB_DIR)CMSIS/Device/ST/STM32F4xx/Include
export PERIPH_SRC_PATH := $(STMLIB_DIR)STM32F4xx_StdPeriph_Driver/src/
export PERIPH_HEADER_PATH := $(STMLIB_DIR)STM32F4xx_StdPeriph_Driver/inc/
export LIB_STM32 := $(STMLIB_DIR)libstm32.a
export STARTUP := $(STMLIB_DIR)CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/$(STARTUPFILE)

#-------- コンパイルオプション --------
# デバイスの種類を設定（#define $(DEVICE) と等価）
CFLAGS := -D $(DEVICE)
# HSEの値を設定
CFLAGS += -D HSE_VALUE=$(HSE_VALUE)
# ライブラリを使用する（#define USE_STDPERIPH_DRIVER と等価）
CFLAGS += -D USE_STDPERIPH_DRIVER
# cortex-m4用のコードを生成
CFLAGS += -mcpu=cortex-m4 -mthumb
# FPUを使用する
#CFLAGS += -mfloat-abi=soft # 整数演算だけで浮動小数点演算を行う
CFLAGS += -mfloat-abi=softfp -mfpu=fpv4-sp-d16 # FPUを使用する（汎用レジスタ）
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 # FPUを使用する（浮動小数点レジスタ）
# 全ての警告を表示
CFLAGS += -Wall
# 最適化レベルを2に設定(デバッグ時は"-O0"を指定し最適化なしとすればよい)
CFLAGS += -O2
# C++の例外は実装しない
CFLAGS += -fno-exceptions
# 浮動小数点定数は単精度(float型)とする
CFLAGS += -fsingle-precision-constant
# 各ヘッダのパスを指定する
CFLAGS += -I $(CORE_HEADER_PATH) -I $(DEVICE_HEADER_PATH) -I $(PERIPH_HEADER_PATH)
export CFLAGS

#-------- リンカオプション --------
# cortex-m4用のコードを生成
LDFLAGS := -mcpu=cortex-m4 -mthumb
# 標準ライブラリをリンクする
LDFLAGS += -lm -lc -lgcc
# 標準のスタートアップファイルは使わない
LDFLAGS += -nostartfiles
export LDFLAGS

#-------- 削除コマンド --------
ifeq "$(findstring cs-make, $(MAKE))" "cs-make"
	RM=cs-rm -f
else
	RM=rm -f
endif

#------------------------------------------------------------------------------
# コンパイル
#------------------------------------------------------------------------------
.PHONY : main static Libraries
all : main

main : static
	$(MAKE) -C $@

static : Libraries
	$(MAKE) -C $@

Libraries :
	$(MAKE) -C $@


#------------------------------------------------------------------------------
# クリーンアップ
#------------------------------------------------------------------------------
clean : clean_Libraries clean_static clean_main

clean_Libraries :
	$(MAKE) -C Libraries clean

clean_static :
	$(MAKE) -C static clean

clean_main :
	$(MAKE) -C main clean

