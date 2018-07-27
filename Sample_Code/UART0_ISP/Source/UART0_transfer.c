/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2015 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/21/2015
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: N76E885 UART-0 Mode2 demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Typedef.h"
#include "Define.h"
#include "SFR_Macro.h"
#include "UART0_transfer.h"
#include "ISP_USER.h"

  xdata volatile uint8_t uart_rcvbuf[64]; 
  xdata volatile uint8_t uart_txbuf[64];
 data volatile uint8_t bufhead;
  data volatile uint16_t flash_address; 
  data volatile uint16_t AP_size;
  data volatile uint8_t g_timer1Counter;
  data volatile uint8_t count; 
  data volatile uint16_t g_timer0Counter;
  data volatile uint32_t g_checksum;
  data volatile uint32_t g_totalchecksum;
  bit volatile bUartDataReady;
  bit volatile g_timer0Over;
  bit volatile g_timer1Over;
  bit volatile g_progarmflag;

bit BIT_TMP;


void TM0_ini(void)
{		
	TMOD |= 0x02;
	TH0=TL0=0; //interrupt timer 140us
	set_TR0;   //Start timer0
	set_PSH_1;   // Serial port 0 interrupt level2
	set_ES_1;
	clr_ES;
	set_ET0;
	set_EA;
//	IE = 0x92;                                      //EA=1, ES=1,ET0=1
}

void InitialUART1_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
{
//		P02_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
//		P16_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
	
	  SCON_1 = 0x50;   	//UART1 Mode1,REN_1=1,TI_1=1
    T3CON = 0x08;   	//T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1), UART1 in MODE 1
		clr_BRCK;
	

		RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  		/*16 MHz */
		RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);			/*16 MHz */

    set_TR3;         //Trigger Timer3
}

void UART0_ini(void)
{
	InitialUART1_Timer3(38400);

//	TMOD |= 0x02;
//	SCON &= 0xef;
//	set_T1M;
//	return;
	
		SCON = 0x42;     //UART0 Mode1,REN=1,TI=1
		TMOD |= 0x22;    //Timer1 Mode1, TIMER 0 MODE2
				
		set_SMOD;
		set_T1M;
		clr_BRCK;        //Serial port 0 baud rate clock source = Timer1
		TH1 = 256 - (1000000/38400);												/*16 MHz to 115200 Baudrate*/
		set_TR1; 					//Start timer1
}

void Send_64byte_To_UART0(void)
{
   for(count=0;count<64;count++)
	{
   	TI_1 = 0;	
    SBUF_1 = uart_txbuf[count];
    while(TI_1==0);
	}
}

void Serial_ISR_1 (void) interrupt 15 
{
    if (RI_1 == 1)
    {   
			uart_rcvbuf[bufhead++]=	SBUF_1;		
			clr_RI_1;                                         // Clear RI (Receive Interrupt).
    }
    if (TI_1 == 1)
    {       
        clr_TI_1;                                         // Clear TI (Transmit Interrupt).
    }
		if(bufhead ==1)
		{
			g_timer1Over=0;
			g_timer1Counter=90; //for check uart timeout using
		}
	if(bufhead == 64)
		{
			
			bUartDataReady = TRUE;
			g_timer1Counter=0;
			g_timer1Over=0;
			bufhead = 0;
		}		
}


void Serial_ISR (void) interrupt 4
{
	unsigned char n;
	if(RI == 1)
	{
		RI = 0;
		n = SBUF;
	}
	
	if(TI == 0)
		TI = 0;

//{
//    if (RI == 1)
//    {   
//			uart_rcvbuf[bufhead++]=	SBUF;		
//			clr_RI;                                         // Clear RI (Receive Interrupt).
//    }
//    if (TI == 1)
//    {       
//        clr_TI;                                         // Clear TI (Transmit Interrupt).
//    }
//		if(bufhead ==1)
//		{
//			g_timer1Over=0;
//			g_timer1Counter=90; //for check uart timeout using
//		}
//	if(bufhead == 64)
//		{
//			
//			bUartDataReady = TRUE;
//			g_timer1Counter=0;
//			g_timer1Over=0;
//			bufhead = 0;
//		}		
}

void Timer0_ISR (void) interrupt 1
{
if(g_timer0Counter)
  {
	g_timer0Counter--;
		if(!g_timer0Counter)
		{
		g_timer0Over=1;
		}
	}
	
	if(g_timer1Counter)
  {
	g_timer1Counter--;
		if(!g_timer1Counter)
		{
		g_timer1Over=1;
		}
	}
} 


