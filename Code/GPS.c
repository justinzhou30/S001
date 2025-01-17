#include "All_Function.h"


#define GPS_NO_SIGNAL_TIME		160	//3000
code char *gpsDataGPVTG = "GPVTG";

static UINT8 gps_returnData;
static UINT8 gps_returnFlag;	//是否有GPS信号，有则为非0，无则为0

static UINT8 gps_speedData;
//static UINT8 gps_signalFlag;	//是否有GPS信号，有则为非0，无则为0

#define GPS_CONNECT	1
#define GPS_NOT_CONNECT	2
static UINT8 gps_connectFlag;		//检测GPS天线是否插上

static UINT8 gps_flag;					// 接收GPS发来的$符号来判断GPS天线是否插上
//extern UINT8 uart1RiFlag;

extern UINT8 sys_status;

void gps_init(void)
{
	gps_returnData = 0;
	gps_returnFlag = 0;
	gps_speedData = 0;
	gps_connectFlag = 0;
//	uart1RiFlag = 0;
}

void gps_rxDataServer(void)
{
	static UINT8 rxDataStatus;
	static UINT8 temp0;
	static UINT8 temp1;
	static UINT8 Uart1RxData[10];
//	static UINT8 temp_returnData;
//	static UINT8 temp_returnFlag;
	
	
	UINT8 temp_RxData;
	
	if(q_pop(&temp_RxData))
	{
		switch(rxDataStatus)
		{
			case 0:
				if(temp_RxData == '$')
				{
					rxDataStatus++;
					temp0 = 0;
					//gps_flag = '$';
				}
				break;
			
			
			case 1:
				Uart1RxData[temp0++] = temp_RxData;		//接收GPS特征字
			
				if(temp0 == 5)							//接收完GPS特征字
				{
					for(temp1 = 0 ; temp1 < temp0 ; temp1++)
					{
						if(Uart1RxData[temp1] != gpsDataGPVTG[temp1])		//比较是否需要的GPS特征字
						{
							rxDataStatus = 0;			//不是需要的特征字，退出
							break;
						}
					}

					if(temp1 == 5)				//符合GPS首部特征字
					{
						temp0 = 0;
						temp1 = 0;
						rxDataStatus++;
					}
				}
				
				break;
				
				
			case 2:							//筛选GPS速度所在字节
				if(temp_RxData == ',')
					temp0++;
				
				if(temp0 == 7)				//GPS速度所在位置
				{
					temp0 = 0;
					temp1 = 0;
					rxDataStatus++;
				}
				break;
				
			case 3:							//截取速度并转化存储
				if((temp_RxData == ',') || (temp_RxData == '.'))
				{
					if(temp0 != 0)			//没有测到速度
					{
						gps_flag = '$';		//检测到有GPS信号
						
						if(temp0 == 3)
						{
							gps_returnData = Uart1RxData[0]*100 + Uart1RxData[1]*10 + Uart1RxData[2];
							gps_returnFlag = 0xff;
						}
						
						if(temp0 == 2)
						{
							gps_returnData = Uart1RxData[0]*10 + Uart1RxData[1];
							gps_returnFlag = 0xff;
						}
						
						if(temp0 == 1)
						{
							gps_returnData = Uart1RxData[0];
							gps_returnFlag = 0xff;
						}

					}
					
					temp0 = 0;
					temp1 = 0;
					rxDataStatus = 0;
					
					break;
				}
				
				Uart1RxData[temp0++] = temp_RxData-'0';		//接收速度字节
				
				break;
				
			default:
				break;
		}
		
	}
	
}

UINT8 get_gpsSpeed(void)
{
	//GPS无信号则返回0xff
	//其他值则为速度值
	return gps_speedData;
}


void gps_LED_Flash(void)
{
	static UINT8 temp_time;
	
	if((gps_connectFlag == GPS_NOT_CONNECT) && (sys_status & 0x01 /*SYS_OPEN*/))
	{
		if(++temp_time > 100)
		{
			temp_time = 0;
			P12 = ~P12;					//没有GPS信号时，电源灯闪烁
		}
	}
}


void gps_Server_10ms(void)
{
	static UINT16 temp_time;
	static UINT8 temp_time2;
	
	if(gps_returnFlag)			//有GPS速度数据
	{
		gps_returnFlag = 0;
		temp_time = 0;
		gps_speedData = gps_returnData;
	}
	else
	{
		if(++temp_time > GPS_NO_SIGNAL_TIME)
		{
			temp_time = GPS_NO_SIGNAL_TIME + 10;
			gps_speedData = 0xff;			//超时无GPS信号
		}
//		else
//			gps_speedData = 100;
	}
	
	if(++temp_time2 > 160)
	{
		temp_time2 = 0;
		
		if(gps_connectFlag == GPS_CONNECT)
		{
			if(gps_flag == '$')
			{
				gps_flag = 0;
			}
			else
			{
				gps_flag = 0;
				gps_connectFlag = GPS_NOT_CONNECT;
				play_voice(VOICE_INDEX_GPS_NOT_CONNECT);
			}
		}
		else if(gps_connectFlag == GPS_NOT_CONNECT)
		{
			if(gps_flag == '$')
			{
				gps_flag = 0;
				gps_connectFlag = GPS_CONNECT;
				play_voice(VOICE_INDEX_GPS_CONNECT);
				P12 = 1;
			}
			else
			{
				gps_flag = 0;				
			}
		}
		else
		{
			if(gps_flag == '$')
			{
				gps_flag = 0;
				gps_connectFlag = GPS_CONNECT;
				play_voice(VOICE_INDEX_GPS_CONNECT);
				P12 = 1;
			}
			else
			{
				gps_flag = 0;	
				gps_connectFlag = GPS_NOT_CONNECT;
				play_voice(VOICE_INDEX_GPS_NOT_CONNECT);
			}			
		}
	}
	
	gps_LED_Flash();
}

void gps_server(void)
{
	gps_rxDataServer();
}

