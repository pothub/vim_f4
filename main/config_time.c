////////////////////////////////////////////////////////////////////////////////
/** @brief SysTick Timerを使った多目的時間計測用の関数
@date 2012/3/10
*///////////////////////////////////////////////////////////////////////////////
#include <stm32f4xx.h>

////////////////////////////////////////////////////////////////////////////////
/** @brief 指定時間処理を止める
@details
SysTickを使って指定時間処理を止める．割り込みは使用していない．
@note
割り込みがおよそ0.5sec以上持続するような状況下では正しくカウントできない．
*///////////////////////////////////////////////////////////////////////////////
void wait(float time)
{
	// カウンタの設定値を計算する
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	uint64_t val = RCC_Clocks.HCLK_Frequency / 8 * time;
	uint32_t count = val / 0x1000000 + 1; // アンダーフロー回数
	uint32_t load = val / count;

	// SysTickの設定
	SysTick->CTRL = 0;
	SysTick->LOAD = load - 1;
	SysTick->VAL  = 0;
	SysTick->CTRL = 1;

	int i;
	for(i=0;i<count;i++){
		while((SysTick->CTRL & 0x00010000) == 0);
	}
	SysTick->CTRL = 0;
}


////////////////////////////////////////////////////////////////////////////////
/** @brief 短時間計測用にSysTickをスタートさせる
@details
SysTickをHCLKで最大カウント値からダウンカウント開始する．割り込みはしない．
概ね0.8sec以下の短時間高分解能計測に用いる．
@return なし
*///////////////////////////////////////////////////////////////////////////////
void start_timer()
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL  = 0;
	SysTick->CTRL = 1;
}


////////////////////////////////////////////////////////////////////////////////
/** @brief start_timer()でスタートさせたタイマの値を読む
@attention
start_timer()実行後にwait()が呼ばれた場合は正しい値を返さない．
@return start_timer実行後からの時間 [s]を返す．
ただし，SysTickがアンダーフローしたときは-1.0を返す．
*///////////////////////////////////////////////////////////////////////////////
float get_time()
{
	int x = SysTick->VAL;
	if(SysTick->CTRL & 0x00010000) return -1.0;
	if(x == 0) x = 0x01000000;

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	return (float)(0x01000000 - x) / (RCC_Clocks.HCLK_Frequency / 8);
}

