#ifdef __cplusplus
extern "C" {
#endif

#ifndef GENERAL_H
#define GENERAL_H

extern void General_Config();
void Led0(uint8_t onoff);
void Led1(uint8_t onoff);
uint8_t Sw0();
uint8_t Sw1();

#endif

#ifdef __cplusplus
}
#endif
