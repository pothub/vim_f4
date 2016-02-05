#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_TIME_H
#define CONFIG_TIME_H

extern void wait(float time);
extern void start_timer();
extern float get_time();

#endif

#ifdef __cplusplus
}
#endif
