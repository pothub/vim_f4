////////////////////////////////////////////////////////////////////////////////
/** @brief クロックとFLASHの設定関数およびFLASHの消去と書き込み関数
@date 2013/9/21
*///////////////////////////////////////////////////////////////////////////////
#include <stm32f4xx.h>

#define HSICalibrationValue 0x10 // 初期値は0x10, 0x0 - 0x1Fの間で調整
//#define VoltageRange_x      VoltageRange_1 // 1.8 - 2.1V
//#define VoltageRange_x      VoltageRange_2 // 2.1 - 2.7V
#define VoltageRange_x      VoltageRange_3 // 2.7 - 3.6V
////////////////////////////////////////////////////////////////////////////////
/** @brief クロックとFLASHの設定を行う
@details
内蔵発振子を使用し，仕様上の最大値である以下の周波数に設定する
- SYSCLK = 168 MHz
- HCLK = 168 MHz (AHB clock)
- PCLK2 = 84 MHz (APB2 clock)
- PCLK1 = 42 MHz (APB1 clock)
- APB2 timer clock = 168 MHz
- APB1 timer clock = 84 MHz
また，FLASHについて以下の項目を設定する
- wait state
- Instruction prefetchを有効にする
- Instruction cacheを有効にする
- Data cacheを有効にする
@note
APBxのprescalarが/1以外のとき，timerのクロックだけはx2される．
また，電源電圧に応じてFLASHのwait state最小値とFLASH書き込み最大ビット数は制限される．
電源電圧が2.4Vより低いとSYSCLK=168MHzでFLASHからの読み込みはできない
@return なし
*///////////////////////////////////////////////////////////////////////////////
void CLOCK_FLASH_config()
{
	// クロックの状態を初期値に戻す
	RCC_DeInit();

	// wait stateの設定
	if     (VoltageRange_x == VoltageRange_2) FLASH_SetLatency(FLASH_Latency_6);
	else if(VoltageRange_x == VoltageRange_3) FLASH_SetLatency(FLASH_Latency_5);
	else while(1);

	// ART Acceleratorの設定
	FLASH_PrefetchBufferCmd(ENABLE);
	FLASH_InstructionCacheCmd(ENABLE);
	FLASH_DataCacheCmd(ENABLE);

	// HSIのキャリブレーション値を設定する
	RCC_AdjustHSICalibrationValue(HSICalibrationValue);

	// PLLの設定をする (max. 168 MHz)
	// PLLM: division factor, 2-63 --> 1-2 MHz (2MHz is recommended)
	// PLLN: multiplication factor, 64-432 --> 64-432 MHz
	// PLLP: division factor, 2, 4, 6, or 8 --> max 168 MHz
	// PLLQ: division factor, 2-15 --> 48 MHz
	RCC_PLLConfig(RCC_PLLSource_HSI, 8, 168, 2, 7); // HSI / 8 * 168 / 2 = 168 MHz
	//RCC_PLLConfig(RCC_PLLSource_HSI, 8, 168, 4, 7); // HSI / 8 * 168 / 4 = 84 MHz
	RCC_PLLCmd(ENABLE);
	while( RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET);

	// HCLKの分周比を設定する (max. 168 MHz)
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	// PCLK2の分周比を設定する (max. 84 MHz)
	RCC_PCLK2Config(RCC_HCLK_Div2);

	// PLCK1の分周比を設定する (max. 42 MHz)
	RCC_PCLK1Config(RCC_HCLK_Div4);

	// SYSCLKのクロックソースをPLLに切り替える
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
}


////////////////////////////////////////////////////////////////////////////////
/** @brief FLASHの指定したセクターを消去する
@param[in] FLASH_Sector 消去するセクター番号 (0-11)
@return なし
*///////////////////////////////////////////////////////////////////////////////
void erase_flash_sector(uint32_t FLASH_Sector)
{
	FLASH_Unlock();
	FLASH_EraseSector(FLASH_Sector, VoltageRange_x);
}


union HalfWord
{
	uint16_t half_word;
	uint8_t byte[2];
};

////////////////////////////////////////////////////////////////////////////////
/** @brief FLASHの指定したアドレスへ1バイト書き込む
@param[in] address 書き込み先のアドレス
@param[in] data 書き込む1バイト
@return なし
*///////////////////////////////////////////////////////////////////////////////
void write_1byte_to_flash(uint32_t address, uint8_t data)
{
	FLASH_ProgramByte(address, data);
}

////////////////////////////////////////////////////////////////////////////////
/** @brief FLASHの指定したアドレスへ2バイト書き込む
@details
電源電圧に応じて書き込み単位を変えて書き込む
@param[in] address 書き込み先のアドレス
@param[in] data 書き込む2バイト
@return なし
*///////////////////////////////////////////////////////////////////////////////
void write_2bytes_to_flash(uint32_t address, uint16_t data)
{
	if(VoltageRange_x == VoltageRange_4 ||
	   VoltageRange_x == VoltageRange_3 ||
	   VoltageRange_x == VoltageRange_2){
		FLASH_ProgramHalfWord(address, data);
	}
	else if(VoltageRange_x == VoltageRange_1){
		union{
			uint16_t half_word;
			uint8_t byte[2];
		} x = {data};
		FLASH_ProgramByte(address,   x.byte[0]);
		FLASH_ProgramByte(address+1, x.byte[1]);
	}
}

////////////////////////////////////////////////////////////////////////////////
/** @brief FLASHの指定したアドレスへ4バイト書き込む
@details
電源電圧に応じて書き込み単位を変えて書き込む
@param[in] address 書き込み先のアドレス
@param[in] data 書き込む4バイト
@return なし
*///////////////////////////////////////////////////////////////////////////////
void write_4bytes_to_flash(uint32_t address, uint32_t data)
{
	if(VoltageRange_x == VoltageRange_4 ||
	   VoltageRange_x == VoltageRange_3){
		FLASH_ProgramWord(address, data);
	}
	else{
		union{
			uint32_t word;
			uint16_t half_word[2];
			uint8_t byte[4];
		} x = {data};
		if(VoltageRange_x == VoltageRange_2){
			FLASH_ProgramHalfWord(address,   x.half_word[0]);
			FLASH_ProgramHalfWord(address+2, x.half_word[1]);
		}
		else if(VoltageRange_x == VoltageRange_1){
			FLASH_ProgramByte(address,   x.byte[0]);
			FLASH_ProgramByte(address+1, x.byte[1]);
			FLASH_ProgramByte(address+2, x.byte[2]);
			FLASH_ProgramByte(address+3, x.byte[3]);
		}
	}
}

