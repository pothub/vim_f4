#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_ENCODER_H
#define CONFIG_ENCODER_H

extern void TIM_config_for_encoder1();
extern void TIM_config_for_encoder2();

extern short get_cnt_encoder1();
extern short get_cnt_encoder2();

extern void set_cnt_encoder1(short cnt);
extern void set_cnt_encoder2(short cnt);

#endif

#ifdef __cplusplus
}
#endif
