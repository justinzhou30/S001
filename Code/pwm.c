#include "All_Function.h"

UINT8 pwm_intFlag;

void pwm_init(void)
{
	PWM3_P04_OUTPUT_ENABLE;
	PWM_CLOCK_FSYS;
	PWM_INT_PWM3;
	PWM_RISING_INT;
//	PWM_CLOCK_DIV_2;
	PWMPH = 16000000/16000/256;
	PWMPL = (16000000/16000)%256;
	
//	PWM3H = 0x01;
//	PWM3L = 0xf4;
	
	clr_EPWM;
//	clr_LOAD;
	clr_PWMRUN;
}

void Pwm_ISR (void) interrupt PWM_ISR
{
//	static UINT16 temp;
  clr_PWMF;               // clear PWM interrupt flag

//	if(++PWM3L == 0xe7)
//	{
//		PWM3L = 1;
//		if(++PWM3H == 4)
//			PWM3H = 0;

//	}
//	set_LOAD;
//	if(++temp == 16000)
//	{
//		Send_Data_To_UART0(0x34);
//		temp = 0;
//	}
	P12 = ~P12;
//	pwm_intFlag = 0xff;
	getVoiceNextData();
}

void start_pwm(void)
{
//	printf("\nPWM_start");
	set_EPWM;
	set_LOAD;
	set_PWMRUN;
}

void stop_pwm(void)
{
	clr_EPWM;
	clr_PWMRUN;
	clr_PWMF;
//	printf("\nPWM_STOP");
}

void set_pwmDuty(UINT8 *pBuffer)
{
	UINT16 temp;
	
	temp = 0;
	temp |= *(pBuffer+1);
	temp <<= 8;
	temp |= *pBuffer;
	
	temp >>= 3;						//???????
	if(temp > 1000)
		temp = 1000;
	
	if(temp == 0)
		temp = 1;
	
	PWM3L = (UINT8)temp;
	PWM3H = (UINT8)(temp>>8);
	
	set_LOAD;
}

void pwm_server(void)
{
	if(pwm_intFlag)
	{
		pwm_intFlag = 0;
		getVoiceNextData();
	}
}
