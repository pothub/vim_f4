#------------------------------------------------------------------------------
# STM32F4 static program コンパイル用 MAKEFILE
#------------------------------------------------------------------------------

# ターゲット名と生成されたファイルを格納するディレクトリ
OUT_NAME := static
OUT_FILE := $(OUT_DIR)$(OUT_NAME)

# ディレクトリ内の全てのcソースをリストアップ
C_SRCS := $(wildcard $(SRC_DIR)*.c)
C_OBJS := $(C_SRCS:$(SRC_DIR)%.c=$(OUT_DIR)%.o)
# ディレクトリ内の全てのcppソースをリストアップ
CPP_SRCS := $(wildcard $(SRC_DIR)*.cpp)
CPP_OBJS := $(CPP_SRCS:$(SRC_DIR)%.cpp=$(OUT_DIR)%.o)
# ディレクトリ内の全てのヘッダファイルをリストアップ
HEADERS := $(wildcard $(INC_DIR)*.h)

# リンカスクリプト
LDSCRIPT := $(LD_DIR)stm32f4xx_static.ld

# 各ヘッダのパスを指定する
CFLAGS += -I$(STMLIB_DIR) -I$(INC_DIR) -I.

#-------- リンカオプション --------
# リンカスクリプトを指定
LDFLAGS += -T$(LDSCRIPT)
# マップファイルを生成
LDFLAGS += -Wl,-Map=$(OUT_FILE).map

# ターゲットの指定
all : $(OUT_DIR:/=) $(OUT_FILE).mot $(OUT_FILE).hex
# ビルドディレクトリ作成
$(OUT_DIR:/=) :
	mkdir $(OUT_DIR:/=)
# モトローラS形式およびインテルhex形式に変換
$(OUT_FILE).mot : $(OUT_FILE).elf
	$(OBJCOPY) -O srec $< $@
$(OUT_FILE).hex : $(OUT_FILE).elf
	$(OBJCOPY) -O ihex $< $@
# リンク
$(OUT_FILE).elf : $(C_OBJS) $(CPP_OBJS) $(LIB_STM32) $(LDSCRIPT) $(MEMORY_DEF) $(INCLUDE_SYM) makefile
	$(CC) $(LDFLAGS) -o $@ $(C_OBJS) $(CPP_OBJS) $(LIB_STM32)
	$(SIZE) -Ax $@
# コンパイル
$(OUT_DIR)%.o : $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@
#	$(CC) $(CFLAGS) -S $<
$(OUT_DIR)%.o : $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
#	$(CC) $(CFLAGS) -S $<
# 上記以外の依存関係
$(C_OBJS) $(CPP_OBJS) : $(HEADERS) makefile


#------------------------------------------------------------------------------
# クリーンアップ
#------------------------------------------------------------------------------
clean:
	$(RM) $(C_OBJS) $(CPP_OBJS) $(OUT_FILE).map $(OUT_FILE).elf $(OUT_FILE).mot $(OUT_FILE).hex
