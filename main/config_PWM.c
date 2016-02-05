#include <stm32f4xx.h>
#include "config_PWM.h"

#define f_PWM_motor 200000 // PWM frequency [Hz]
uint16_t TIM_Period_PWM_motor;
#define PCLKx_Frequency_PWM_motor       PCLK2_Frequency
#define TIMx_PWM_motor                  TIM8
#define TIM_SetCompareX_motorA_forward  TIM_SetCompare1
#define TIM_SetCompareX_motorA_backward TIM_SetCompare2
#define TIM_SetCompareX_motorB_forward  TIM_SetCompare3
#define TIM_SetCompareX_motorB_backward TIM_SetCompare4
////////////////////////////////////////////////////////////////////////////////
/** @brief PWM出力用にタイマを設定する
@details
4出力可能なタイマを使ってモータ駆動用PWM波を出力するための設定を行う．
想定しているPWM周波数は20-200kHzである．
(PWM_motor1, PWM_motor2)および(PWM_motor3, PWM_motor4)の組み合わせで
モータドライバICへPWM波を送り，2個のモータを駆動する．
@attention
使用するTIM, ポートはマクロで定義すること
@return なし
*///////////////////////////////////////////////////////////////////////////////
void TIM_config_for_PWM_motor(){
	// PWM周波数からタイマのカウンタの設定値を計算する
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	if(RCC_Clocks.HCLK_Frequency == RCC_Clocks.PCLKx_Frequency_PWM_motor)
		TIM_Period_PWM_motor = RCC_Clocks.PCLKx_Frequency_PWM_motor / f_PWM_motor;
	else
		TIM_Period_PWM_motor = RCC_Clocks.PCLKx_Frequency_PWM_motor * 2 / f_PWM_motor;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	// 入出力ポートにペリフェラルを接続する
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,  GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7,  GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8,  GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9,  GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6,  GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7,  GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8,  GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9,  GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,  GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,  GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM12);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6,  GPIO_AF_TIM13);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7,  GPIO_AF_TIM14);

	// PWM出力ポートの設定
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// TIMの設定
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TIM_Period_PWM_motor - 1; // 周期の設定
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	// PWM出力の設定
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 300; // initial duty
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);
	TIM_OC1Init(TIM12, &TIM_OCInitStructure);
	TIM_OC2Init(TIM12, &TIM_OCInitStructure);
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);

	TIM_CtrlPWMOutputs(TIM8, ENABLE);	//TIM1,TIM8 output config
		
	// タイマ動作開始
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	TIM_Cmd(TIM9, ENABLE);
	TIM_Cmd(TIM12, ENABLE);
	TIM_Cmd(TIM13, ENABLE);
	TIM_Cmd(TIM14, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/** @brief motorAを制御するPWM波のduty比を変更する
@param[in] duty duty比（千分率）
@return なし
*///////////////////////////////////////////////////////////////////////////////
void set_duty_PWM_motorA(int duty)
{
	int pulse;
	if(duty >= 0){
		if(duty > MAX_DUTY_PWM_motor){
			pulse = MAX_DUTY_PWM_motor;
		}
		else{
			pulse = (int)TIM_Period_PWM_motor * duty / 1000;
		}
		TIM_SetCompareX_motorA_forward(TIMx_PWM_motor, pulse);
		TIM_SetCompareX_motorA_backward(TIMx_PWM_motor, 0);
	}
	else{
		if(duty < -MAX_DUTY_PWM_motor){
			pulse = MAX_DUTY_PWM_motor;
		}
		else{
			pulse = - (int)TIM_Period_PWM_motor * duty / 1000;
		}
		TIM_SetCompareX_motorA_forward(TIMx_PWM_motor, 0);
		TIM_SetCompareX_motorA_backward(TIMx_PWM_motor, pulse);
	}
}


////////////////////////////////////////////////////////////////////////////////
/** @brief motorBを制御するPWM波のduty比を変更する
@param[in] duty duty比（千分率）
@return なし
*///////////////////////////////////////////////////////////////////////////////
void set_duty_PWM_motorB(int duty)
{
	int pulse;
	if(duty >= 0){
		if(duty > MAX_DUTY_PWM_motor){
			pulse = MAX_DUTY_PWM_motor;
		}
		else{
			pulse = (int)TIM_Period_PWM_motor * duty / 1000;
		}
		TIM_SetCompareX_motorB_forward(TIMx_PWM_motor, pulse);
		TIM_SetCompareX_motorB_backward(TIMx_PWM_motor, 0);
	}
	else{
		if(duty < -MAX_DUTY_PWM_motor){
			pulse = MAX_DUTY_PWM_motor;
		}
		else{
			pulse = - (int)TIM_Period_PWM_motor * duty / 1000;
		}
		TIM_SetCompareX_motorB_forward(TIMx_PWM_motor, 0);
		TIM_SetCompareX_motorB_backward(TIMx_PWM_motor, pulse);
	}
}

