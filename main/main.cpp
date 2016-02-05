#include <stdio.h>
#include "usart.h"
#include "config_clock_flash.h"
#include "config_time.h"
#include "general.h"
#include "config_PWM.h"
#include "config_encoder.h"
#include "adc.h"
//#include "module_test.h"

extern "C" { extern void startup(); }

int main(void){
	startup();
	CLOCK_FLASH_config();
	General_Config();
	USART1_Conf();
	ADC_Conf();
	TIM_config_for_PWM_motor();
	wait(0.5);

	while(1){
		for(int i=1;i<=nDMAC_ADC;i++){
			printf("%4d\t",ave_ADCdata(i));
		}
		printf("\r\n");
		wait(0.1);
	}
	// while(1){
	// 	if(Sw0()) Led0(1);
	// 	else Led0(0);
	// 	if(Sw1()) Led1(1);
	// 	else Led1(0);
	// }
	// PWM出力の設定
	set_duty_PWM_motorA(500);
	set_duty_PWM_motorB(500);
	while(1);
	// エンコーダ入力の設定
	TIM_config_for_encoder1();
	TIM_config_for_encoder2();
	// 周期割り込み付き自動AD変換設定
	// 少し待つ（0.5秒）
	wait(0.5);

	//printf("---- Compiled @ %s %s ----\r\n",date_str,time_str);


	//module_test_low_level();

	while(1){
		printf("Hello world!\r\n");
		wait(0.5);
	}

	return 0;
}

