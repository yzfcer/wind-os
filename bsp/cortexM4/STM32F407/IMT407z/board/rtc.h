#ifndef __RTC_H
#define __RTC_H	 
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
	
u8_t RTC_InitConfig(void);						//RTC��ʼ��

u8_t RTC_GetWeek(u8_t wyear,u8_t wmonth,u8_t wday);
ErrorStatus RTC_SetTimes(u8_t year,u8_t month,u8_t date,u8_t hour,u8_t min,u8_t sec);
void RTC_GetTimes(uint32_t RTC_Format);

void RTC_SetAlarmA(u8_t week,u8_t hour,u8_t min,u8_t sec);	//��������ʱ��(����������,24Сʱ��)
void RTC_SetWakeUp(u32_t wktime,u16_t autodata);				//���û��Ѷ�ʱ����ʱ��

#endif

















