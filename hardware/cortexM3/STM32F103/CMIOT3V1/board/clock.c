/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	clock.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-04
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����ʱ���ȡ������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Ӳ������
#include "clock.h"

//C��
#include <string.h>
#include <stdlib.h>


/*
************************************************************
*	�������ƣ�	CLOCK_GetTime
*
*	�������ܣ�	����ʱ��
*
*	��ڲ�����	dataPtr��ʱ���ַ�������
*				localTime��ʱ��ṹ��
*
*	���ز�����	��
*
*	˵����		������������ʽ���ַ���ʱ����룺
*				57877 17-05-04 01:16:31 50 0 0 344.5 UTC(NIST) * 
************************************************************
*/
void CLOCK_GetTime(unsigned char *dataPtr, struct tm *localTime)
{
	char time[3] = {0, 0, 0};								//ʱ�仺��
	
	//UsartPrintf(USART_DEBUG, "\r\nTime: %s\r\n", dataPtr);
	dataPtr = (unsigned char *)strchr((char *)dataPtr, ' ');
	dataPtr++;
	
	//��============================================================================================
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_year = atoi(time) + 100;				//time.h��1900�꿪ʼ��
	
	//��============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_mon = atoi(time) - 1;					//0~11
	
	//��============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_mday = atoi(time);
	
	//ʱ============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_hour = atoi(time);
	
	//��============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_min = atoi(time);
	
	//��============================================================================================
	dataPtr++;
	time[0] = *dataPtr++;time[1] = *dataPtr++;
	localTime->tm_sec = atoi(time);
	
	localTime->tm_isdst = 1;
	
	//UsartPrintf(USART_DEBUG, "\r\n%d-%d-%d %d:%d:%d\r\n", localTime->tm_year, localTime->tm_mon, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
	//UsartPrintf(USART_DEBUG, "\r\n%s\r\n", asctime(localTime));
	
}
