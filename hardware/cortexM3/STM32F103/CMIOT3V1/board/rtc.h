#ifndef _RTC_H_
#define _RTC_H_




typedef struct
{

	unsigned char rtc_alarm_wakeup : 1;
	
	unsigned char reverse;

} RTC_INFO;

extern RTC_INFO rtcInfo;


#define USE_EXT_RCC		0


_Bool RTC_Init(void);

void RTC_NVIC_Init(void);

void RTC_SetTime(unsigned int sec);

void RTC_AlarmSet(unsigned int sec);

void RTC_AlarmReSet(unsigned int sec);


#endif
