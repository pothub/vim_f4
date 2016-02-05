#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_AUTOADC_H
#define CONFIG_AUTOADC_H

#include <stdint.h>

extern void ADC_Conf();
void ADC_IRQ_Conf();

#define dt_int 1.E-3 // 割り込み周期 [s]
#define nADC 3 // AD変換回数（max.: 16）
#define ADC_REPEAT 1
#define nDMAC_ADC (nADC*ADC_REPEAT)

struct ADCdata{ // little endian
	uint16_t data1;
	uint16_t data2;
};
extern struct ADCdata ADC_result[nDMAC_ADC];
#define ADC1data(x) (ADC_result[x-1].data1) // x番目の変換データ
#define ADC2data(x) (ADC_result[x-1].data2) // x = 1, 2, ..., nDMAC_ADC
#define ave_ADCdata(x) ((ADC_result[x-1].data1 + ADC_result[x-1].data2) /2) 

// reset (16bits) + set (16bits), set has priority
extern uint32_t preADC_GPIOx_OUT_table[nDMAC_ADC];
extern uint32_t postADC_GPIOx_OUT_table[nDMAC_ADC];

extern volatile float abs_time; // [s]
extern volatile uint64_t time_count;
extern int duty_int_routine; // 千分率
extern int duty_int_routine_max; // 千分率
extern void int_pre_process();
extern void int_post_process();
extern volatile int duty_here();
extern void stop_periodic_interrupt();
extern void restart_periodic_interrupt();

#endif

#ifdef __cplusplus
}
#endif

