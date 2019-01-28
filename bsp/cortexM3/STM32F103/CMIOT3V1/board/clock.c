/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	clock.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-04
	*
	*	版本： 		V1.0
	*
	*	说明： 		网络时间获取、解析
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//硬件驱动
#include "clock.h"

//C库
#include <string.h>
#include <stdlib.h>


/*
************************************************************
*	函数名称：	CLOCK_GetTime
*
*	函数功能：	解码时间
*
*	入口参数：	dataPtr：时间字符串缓存
*				localTime：时间结构体
*
*	返回参数：	无
*
*	说明：		适用于这种形式的字符串时间解码：
*				57877 17-05-04 01:16:31 50 0 0 344.5 UTC(NIST) * 
************************************************************
*/
void CLOCK_GetTime(unsigned char *dataPtr, struct tm *localTime)
{
	char time[3] = {0, 0, 0};								//时间缓存
	
	//UsartPrintf(USART_DEBUG, "\r\nTime: %s\r\n", dataPtr);
	dataPtr = (unsigned char *)strchr((char *)dataPtr, ' ');
	dataPtr++;
	
	//年============================================================================================
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_year = atoi(time) + 100;				//time.h从1900年开始的
	
	//月============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_mon = atoi(time) - 1;					//0~11
	
	//日============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_mday = atoi(time);
	
	//时============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_hour = atoi(time);
	
	//分============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_min = atoi(time);
	
	//秒============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_sec = atoi(time);
	
	localTime->tm_isdst = 1;
	
	//UsartPrintf(USART_DEBUG, "\r\n%d-%d-%d %d:%d:%d\r\n", localTime->tm_year, localTime->tm_mon, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
	//UsartPrintf(USART_DEBUG, "\r\n%s\r\n", asctime(localTime));
	
}
