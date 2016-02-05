#ifdef __cplusplus
extern "C" {
#endif

#ifndef USART_H
#define USART_H

extern void USART1_Conf();
extern void outbyte(char c);
extern volatile char inkey();
extern void clear_rxb();
extern char inbyte();

#endif

#ifdef __cplusplus
}
#endif
