#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_PWM_H
#define CONFIG_PWM_H

#define MAX_DUTY_PWM_motor 950 // 最大duty比（千分率）
extern void TIM_config_for_PWM_motor();
extern void set_duty_PWM_motorA(int duty);
extern void set_duty_PWM_motorB(int duty);

#endif

#ifdef __cplusplus
}
#endif
