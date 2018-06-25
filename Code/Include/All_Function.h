#ifndef __ALL_FUNCTION_H__
#define __ALL_FUNCTION_H__

#include "Timer0_sysTick.h"
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "stdio.h"

#define UART0_ISR	4
#define TIMER0_ISR	1
#define UART1_ISR	15
#define PWM_ISR		13
#define SPI_ISR		9

void Uart0_init(void);
void Uart1_init(void);
void sys_init(void);
void pwm_init(void);

void Spi_init(void);
//void spi_Read(UINT32 addr_flash , UINT8 count , UINT8 *Paddr_mcu);
//void spi_ReadInQ(UINT32 addr_flash , UINT8 count);
//UINT8 get_spiReadState(void);
//void spi_server(void);

void spi_ReadInit(UINT32 addr_flash);
UINT8 spi_ReadNextByte(void);
void spi_ReadStop(void);

void q_init(void);
void q_push(UINT8 qData);
UINT8 q_pop(UINT8 *qData);
UINT8 q_get_dataSize(void);

void set_pwmDuty(UINT8 *pBuffer);
void start_pwm(void);
void stop_pwm(void);
void pwm_server(void);

void getVoiceNextData(void);
void play_voice(UINT8 index);
#endif
