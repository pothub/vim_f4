#include <stm32f4xx.h>
#include "general.h"

void General_Config(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Led0(uint8_t onoff){
	if(onoff) GPIO_SetBits(GPIOC,GPIO_Pin_13);
	else GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}
void Led1(uint8_t onoff){
	if(onoff) GPIO_SetBits(GPIOC,GPIO_Pin_14);
	else GPIO_ResetBits(GPIOC,GPIO_Pin_14);
}

uint8_t Sw0(){
	return !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_14);
}
uint8_t Sw1(){
	return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
}

