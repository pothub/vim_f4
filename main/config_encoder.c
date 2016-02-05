////////////////////////////////////////////////////////////////////////////////
/** @brief タイマを使って2相エンコーダを読む関数
@date 2012/3/22
*///////////////////////////////////////////////////////////////////////////////
#include <stm32f4xx.h>

#define RCC_ClockCmd_TIMx_encoder1(x)   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, x)
#define RCC_ClockCmd_GPIOx_encoder1(x)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, x)
#define GPIOx_encoder1                  GPIOA
#define GPIO_AF_TIMx_encoder1           GPIO_AF_TIM3
#define GPIO_Pin_x_CH1_encoder1         GPIO_Pin_6
#define GPIO_Pin_x_CH2_encoder1         GPIO_Pin_7
#define GPIO_PinSourceX_CH1_encoder1    GPIO_PinSource6
#define GPIO_PinSourceX_CH2_encoder1    GPIO_PinSource7
#define TIMx_encoder1                   TIM3
#define TIM_EncoderMode_xxx_encoder1    TIM_EncoderMode_TI12
#define TIM_ICPolarity_xxx_CH1_encoder1 TIM_ICPolarity_Rising
#define TIM_ICPolarity_xxx_CH2_encoder1 TIM_ICPolarity_Rising
////////////////////////////////////////////////////////////////////////////////
/** @brief 位相係数カウンタ用にタイマを設定する
@return なし
*///////////////////////////////////////////////////////////////////////////////
void TIM_config_for_encoder1()
{
	// クロックを有効にする
	RCC_ClockCmd_TIMx_encoder1(ENABLE);
	RCC_ClockCmd_GPIOx_encoder1(ENABLE);

	// 入出力ポートにペリフェラルを接続する
	GPIO_PinAFConfig(GPIOx_encoder1, GPIO_PinSourceX_CH1_encoder1, GPIO_AF_TIMx_encoder1);
	GPIO_PinAFConfig(GPIOx_encoder1, GPIO_PinSourceX_CH2_encoder1, GPIO_AF_TIMx_encoder1);

	// エンコーダ入力ポート
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_x_CH1_encoder1;
	GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_x_CH2_encoder1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOx_encoder1, &GPIO_InitStructure);

	// 位相計数モード
	TIM_EncoderInterfaceConfig(
		TIMx_encoder1,
		TIM_EncoderMode_xxx_encoder1,
		TIM_ICPolarity_xxx_CH1_encoder1,
		TIM_ICPolarity_xxx_CH2_encoder1);

	// タイマ動作開始
	TIM_Cmd(TIMx_encoder1, ENABLE);
}


#define RCC_ClockCmd_TIMx_encoder2(x)   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, x)
#define RCC_ClockCmd_GPIOx_encoder2(x)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, x)
#define GPIOx_encoder2                  GPIOB
#define GPIO_AF_TIMx_encoder2           GPIO_AF_TIM4
#define GPIO_Pin_x_CH1_encoder2         GPIO_Pin_6
#define GPIO_Pin_x_CH2_encoder2         GPIO_Pin_7
#define GPIO_PinSourceX_CH1_encoder2    GPIO_PinSource6
#define GPIO_PinSourceX_CH2_encoder2    GPIO_PinSource7
#define TIMx_encoder2                   TIM4
#define TIM_EncoderMode_xxx_encoder2    TIM_EncoderMode_TI12
#define TIM_ICPolarity_xxx_CH1_encoder2 TIM_ICPolarity_Rising
#define TIM_ICPolarity_xxx_CH2_encoder2 TIM_ICPolarity_Rising
////////////////////////////////////////////////////////////////////////////////
/** @brief 位相係数カウンタ用にタイマを設定する
@return なし
*///////////////////////////////////////////////////////////////////////////////
void TIM_config_for_encoder2()
{
	// クロックを有効にする
	RCC_ClockCmd_TIMx_encoder2(ENABLE);
	RCC_ClockCmd_GPIOx_encoder2(ENABLE);

	// 入出力ポートにペリフェラルを接続する
	GPIO_PinAFConfig(GPIOx_encoder2, GPIO_PinSourceX_CH1_encoder2, GPIO_AF_TIMx_encoder2);
	GPIO_PinAFConfig(GPIOx_encoder2, GPIO_PinSourceX_CH2_encoder2, GPIO_AF_TIMx_encoder2);

	// エンコーダ入力ポート
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_x_CH1_encoder2;
	GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_x_CH2_encoder2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOx_encoder2, &GPIO_InitStructure);

	// 位相計数モード
	TIM_EncoderInterfaceConfig(
		TIMx_encoder2,
		TIM_EncoderMode_xxx_encoder2,
		TIM_ICPolarity_xxx_CH1_encoder2,
		TIM_ICPolarity_xxx_CH2_encoder2);

	// タイマ動作開始
	TIM_Cmd(TIMx_encoder2, ENABLE);
}

short get_cnt_encoder1(){ return TIM_GetCounter(TIMx_encoder1); }
short get_cnt_encoder2(){ return TIM_GetCounter(TIMx_encoder2); }

void set_cnt_encoder1(short cnt){ TIM_SetCounter(TIMx_encoder1, cnt); }
void set_cnt_encoder2(short cnt){ TIM_SetCounter(TIMx_encoder2, cnt); }

