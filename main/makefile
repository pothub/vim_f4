#------------------------------------------------------------------------------
# STM32F4 main コンパイル用MAKEFILE
#------------------------------------------------------------------------------

# ターゲット名と生成されたファイルを格納するディレクトリ
OUT_NAME := stm32
OUT_FILE := $(OUT_DIR)$(OUT_NAME)

# ディレクトリ内の全てのcソースをリストアップ
C_SRCS := $(wildcard $(SRC_DIR)*.c)
C_OBJS := $(C_SRCS:$(SRC_DIR)%.c=$(OUT_DIR)%.o)
# ディレクトリ内の全てのcppソースをリストアップ
CPP_SRCS := $(wildcard $(SRC_DIR)*.cpp)
CPP_OBJS := $(CPP_SRCS:$(SRC_DIR)%.cpp=$(OUT_DIR)%.o)
# ディレクトリ内の全てのヘッダファイルをリストアップ
HEADERS := $(wildcard $(INC_DIR)*.h)

# staticファイルのパス
STATIC_DIR := ../static/
STATIC_ELF := $(STATIC_DIR)$(OUT_DIR)static.elf

# staticディレクトリのヘッダファイルを追加
HEADERS += $(wildcard $(STATIC_DIR)$(INC_DIR)*.h)

# スタートアップファイル
STARTUP_OBJ := $(STARTUP:%.s=%.o)

# リンカスクリプト
LDSCRIPT := $(LD_DIR)stm32f4xx.ld

# 各ヘッダのパスを指定する
CFLAGS += -I$(STMLIB_DIR) -I$(STATIC_DIR)$(INC_DIR) -I$(INC_DIR) -I.

#-------- リンカオプション --------
# リンカスクリプトを指定
LDFLAGS += -T$(LDSCRIPT)
# マップファイルを生成
LDFLAGS += -Wl,-Map=$(OUT_FILE).map
# staticのコードと合わせる
LDFLAGS += -Wl,--just-symbols=$(STATIC_ELF)

# ターゲットの指定
all : $(OUT_DIR:/=) $(OUT_FILE).mot $(OUT_FILE).hex
# ビルドディレクトリ作成
$(OUT_DIR:/=) :
	mkdir $(OUT_DIR:/=)
# モトローラのS形式およびインテルhex形式に変換
$(OUT_FILE).mot : $(OUT_FILE).elf
	$(OBJCOPY) -O srec $< $@
$(OUT_FILE).hex : $(OUT_FILE).elf
	$(OBJCOPY) -O ihex $< $@
# リンク
$(OUT_FILE).elf : $(STARTUP_OBJ) $(C_OBJS) $(CPP_OBJS) $(LIB_STM32) $(LDSCRIPT) $(MEMORY_DEF) $(STATIC_ELF)
	$(CC) $(LDFLAGS) -o $@ $(STARTUP_OBJ) $(C_OBJS) $(CPP_OBJS) $(LIB_STM32)
	$(SIZE) -Ax $@
# コンパイル
$(OUT_DIR)%.o : $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@
#	$(CC) $(CFLAGS) -S $<
$(OUT_DIR)%.o : $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
#	$(CC) $(CFLAGS) -S $<
# スタートアップルーチンのコンパイル
$(STARTUP_OBJ) : $(STARTUP)
	$(CC) $(CFLAGS) -c $< -o $@
# 上記以外の依存関係
$(C_OBJS) $(CPP_OBJS) $(STARTUP_OBJ) : $(HEADERS) makefile
# 他のファイル全てに依存する
$(OUT_DIR)date_str.o : $(STARTUP_OBJ) $(C_OBJS:$(OUT_DIR)date_str.o=) $(CPP_OBJS) $(LIB_STM32) $(LDSCRIPT) $(MEMORY_DEF) $(STATIC_ELF)


#------------------------------------------------------------------------------
# クリーンアップ
#------------------------------------------------------------------------------
clean:
	$(RM) $(STARTUP_OBJ) $(C_OBJS) $(CPP_OBJS) $(OUT_FILE).map $(OUT_FILE).elf $(OUT_FILE).mot $(OUT_FILE).hex

t :
	$(MAKE) -C ../

f :
	python ../write_main_121028.py
	sudo chmod 666 /dev/ttyUSB0
	cu -s 115200 -l /dev/ttyUSB0

ff :
	python ../write_main_121028.py

