#include <stm32f4xx.h>
#include "adc.h"
	/*
   使用するペリフェラル
   - ADC1
   - ADC2
   - TIM1
   - TIM6
   - DMA2-0
   - DMA2-1
   - DMA2-2
   */
#define TIMx_sub_autoADC                  TIM6
#define PCLKx_Frequency_autoADC           PCLK2_Frequency
#define PCLKx_Frequency_sub_autoADC       PCLK1_Frequency
#define DMAx_IT_TCIFy_endADC_autoADC      DMA_IT_TCIF0
#define DMAx_StreamY_endADC_autoADC       DMA2_Stream0
#define DMAx_StreamY_preADC_OUT_autoADC   DMA2_Stream1
#define DMAx_StreamY_postADC_OUT_autoADC  DMA2_Stream2
#define DMA_Channel_x_endADC_autoADC      DMA_Channel_0
#define DMA_Channel_x_preADC_OUT_autoADC  DMA_Channel_6
#define DMA_Channel_x_postADC_OUT_autoADC DMA_Channel_6
struct ADCdata ADC_result[nDMAC_ADC];
uint16_t subTIM_period;
#define dt_preADC  23.E-6 // [s]
#define dt_postADC  8.E-6  // [s]

////////////////////////////////////////////////////////////////////////////////
/** @brief 自動AD変換の設定を行う
  @details
  タイマを使って一定時間ごとにAD変換を行う．
  AD変換後のデータはDMACで自動転送する．
  @return なし
  *///////////////////////////////////////////////////////////////////////////////
void ADC_Conf(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	// GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// ADCの設定
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2;
	ADC_CommonInit(&ADC_CommonInitStructure);
	ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

	// ADC1の設定
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = nADC; // 変換回数
	ADC_Init(ADC1, &ADC_InitStructure);
	// ADC2の設定
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_Init(ADC2, &ADC_InitStructure);

	ADC_DiscModeChannelCountConfig(ADC1, 1);
	ADC_DiscModeCmd(ADC1, ENABLE);
	ADC_DiscModeChannelCountConfig(ADC2, 1);
	ADC_DiscModeCmd(ADC2, ENABLE);

	// AD変換チャネルなどの設定
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 2, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 3, ADC_SampleTime_144Cycles);

	// タイマのカウンタの設定値を計算する
	uint32_t clock;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	if(RCC_Clocks.HCLK_Frequency == RCC_Clocks.PCLKx_Frequency_autoADC)
		clock = RCC_Clocks.PCLKx_Frequency_autoADC;
	else
		clock = RCC_Clocks.PCLKx_Frequency_autoADC * 2;
	uint16_t preDMAC_trig = 2;
	uint16_t ADC_trig = dt_preADC * clock + preDMAC_trig;
	uint16_t postDMAC_trig = dt_postADC * clock + ADC_trig;
	uint16_t ADC_period = dt_int * clock / nDMAC_ADC;
	// エラーチェック
	if(ADC_trig > ADC_period || postDMAC_trig > ADC_period) while(1);

	// AD変換とポート出力制御用タイマの設定
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler =  0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = ADC_period - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	// AD変換タイミング (CC3)
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ADC_trig - 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	// AD変換前DMA転送タイミング (CC1)
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = preDMAC_trig - 1;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	// AD変換後DMA転送タイミング (CC2)
	TIM_OCInitStructure.TIM_Pulse = postDMAC_trig - 1;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	// DMAC起動許可
	TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
	TIM_DMACmd(TIM1, TIM_DMA_CC2, ENABLE);

	// regular group変換後データ転送用DMACの設定
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_Channel = DMA_Channel_x_endADC_autoADC;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC->CDR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_result;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = nDMAC_ADC; // 転送回数
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMAx_StreamY_endADC_autoADC, &DMA_InitStructure);

	// ADCの有効化
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);

	// Enable DMA
	DMA_Cmd(DMAx_StreamY_endADC_autoADC, ENABLE);
	DMA_Cmd(DMAx_StreamY_preADC_OUT_autoADC, ENABLE);
	DMA_Cmd(DMAx_StreamY_postADC_OUT_autoADC, ENABLE);

	// タイマ動作開始
	TIM_Cmd(TIM1, ENABLE);
}

void ADC_IRQ_Conf(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMAx_StreamY_endADC_autoADC, DMA_IT_TC, ENABLE);

	// 周期割り込み補助タイマ
	uint32_t clock;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	if(RCC_Clocks.HCLK_Frequency == RCC_Clocks.PCLKx_Frequency_sub_autoADC)
		clock = RCC_Clocks.PCLKx_Frequency_sub_autoADC;
	else
		clock = RCC_Clocks.PCLKx_Frequency_sub_autoADC * 2;
	uint32_t period = dt_int * clock;
	uint16_t subTIM_prescalar = 1;
	while(1){
		subTIM_period = period / subTIM_prescalar;
		if(subTIM_period < 0x800) break;
		subTIM_prescalar <<= 1;
	}
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = subTIM_prescalar - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIMx_sub_autoADC, &TIM_TimeBaseStructure);
	TIM_Cmd(TIMx_sub_autoADC, ENABLE);
}

volatile float abs_time = 0; // [s]
volatile uint64_t time_count = 0;
int duty_int_routine;
int duty_int_routine_max = 0;
int subTIM_value;
////////////////////////////////////////////////////////////////////////////////
/** @brief 割り込みルーチンのはじめに実行されるべき関数
  @details
  - 割り込みフラグのクリア
  - 時間の更新
  を行う
  @return
  なし
  *///////////////////////////////////////////////////////////////////////////////
void int_pre_process()
{
	// 割り込みフラグのクリア
	DMA_ClearITPendingBit(DMAx_StreamY_endADC_autoADC, DMAx_IT_TCIFy_endADC_autoADC);

	// 時間の更新
	static uint16_t value_old = 0;
	subTIM_value = TIM_GetCounter(TIMx_sub_autoADC);
	time_count += (uint16_t)(subTIM_value - value_old);
	value_old = subTIM_value;
	abs_time = time_count / subTIM_period * dt_int;
}


////////////////////////////////////////////////////////////////////////////////
/** @brief 割り込み周期に占める呼び出し位置までの処理時間の割合
  @details
  割り込み内の処理にかかった時間を計算する
  @return
  なし
  *///////////////////////////////////////////////////////////////////////////////
volatile int duty_here()
{
	return (uint16_t)(TIM_GetCounter(TIMx_sub_autoADC) - subTIM_value)
		* 1000 / subTIM_period;
}


////////////////////////////////////////////////////////////////////////////////
/** @brief 割り込みルーチンの終わりに実行されるべき関数
  @details
  割り込み内の処理にかかった時間を計算する
  @return
  なし
  *///////////////////////////////////////////////////////////////////////////////
void int_post_process()
{
	// 処理の重さの計算
	duty_int_routine = duty_here();
	if(duty_int_routine > duty_int_routine_max) duty_int_routine_max = duty_int_routine;
}


////////////////////////////////////////////////////////////////////////////////
/** @brief 周期割り込みを禁止する
  @details
  FLASHへの書き込み中に割り込みがかかると長時間保留される可能性がある．
  このときに生じうる不具合を防ぐためにあらかじめ割り込みを禁止するために使用する．
  @return
  なし
  *///////////////////////////////////////////////////////////////////////////////
void stop_periodic_interrupt()
{
	DMA_ITConfig(DMAx_StreamY_endADC_autoADC, DMA_IT_TC, DISABLE);
}


////////////////////////////////////////////////////////////////////////////////
/** @brief stop_periodic_interrupt()で禁止した割り込みを再度許可する
  @return
  なし
  *///////////////////////////////////////////////////////////////////////////////
void restart_periodic_interrupt()
{
	DMA_ClearITPendingBit(DMAx_StreamY_endADC_autoADC, DMAx_IT_TCIFy_endADC_autoADC); // 割り込みフラグのクリア
	DMA_ITConfig(DMAx_StreamY_endADC_autoADC, DMA_IT_TC, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/** @brief autoADC内で設定されたDMA転送が規定回数行われたときに呼ばれる割り込み関数
@details
一定周期で起こるように設定された割り込みである．
なお，関数名はスタートアップルーチンで指定されているので変更できない
@return なし
*///////////////////////////////////////////////////////////////////////////////
void DMA2_Stream0_IRQHandler() // Don't change the function name
{
	int_pre_process(); // 割り込み前処理

	// ここに処理を書く
	
	int_post_process(); // 割り込み後処理
}
