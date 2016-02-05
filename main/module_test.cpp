#include <stdio.h>
#include "usart.h"
#include "config_PWM.h"
#include "config_encoder.h"
#include "adc.h"

void module_test_low_level()
{
// config_***.c内の関数，変数のみを使用する
	int duty1 = 0;
	int duty2 = 0;

	clear_rxb(); // 受信バッファをクリア
	while(1){
		// 各種変数値の表示
		int line = 0;
		printf("TIME: %8.3f [s]\r\n",abs_time); line++;
		printf("duty_int_routine: %d (max: %d)\r\n",duty_int_routine, duty_int_routine_max); line++;
		printf("PWM duty: %5d  %5d\r\n",duty1, duty2); line++;
		printf("encoder: %6d  %6d\r\n",get_cnt_encoder1(), get_cnt_encoder2()); line++;
		printf("CH   ADC1  ADC2\r\n"); line++;
		for(int i=1;i<=nDMAC_ADC;i++){
			printf("%2d:  %4d  %4d\r\n", i, ADC1data(i), ADC2data(i)); line++;
		}
		
		float time = ((int)(abs_time/0.5)+1) * 0.5;

		// キー入力の処理
		while(abs_time < time){
			char c = inkey();
			switch(c){
			case '1': duty1 += 1; break;
			case 'q': duty1 -= 1; break;
			case '2': duty1 += 10; break;
			case 'w': duty1 -= 10; break;
			case '3': duty1 += 100; break;
			case 'e': duty1 -= 100; break;
			case '7': duty2 += 1; break;
			case 'u': duty2 -= 1; break;
			case '8': duty2 += 10; break;
			case 'i': duty2 -= 10; break;
			case '9': duty2 += 100; break;
			case 'o': duty2 -= 100; break;
			case 'r': // reset
				duty1 = duty2 = 0;
				break;
			case 0x1b: goto END; // [esc] exit
			}
			// 各定数の設定
			if(duty1 >  MAX_DUTY_PWM_motor) duty1 =  MAX_DUTY_PWM_motor;
			if(duty1 < -MAX_DUTY_PWM_motor) duty1 = -MAX_DUTY_PWM_motor;
			if(duty2 >  MAX_DUTY_PWM_motor) duty2 =  MAX_DUTY_PWM_motor;
			if(duty2 < -MAX_DUTY_PWM_motor) duty2 = -MAX_DUTY_PWM_motor;
			set_duty_PWM_motorA(duty1);
			set_duty_PWM_motorB(duty2);
		}

		printf("%c[%dA",0x1b,line);
		printf("%c[0J",0x1b);
		fflush(stdout);
	}
	END:;
	clear_rxb();
}

