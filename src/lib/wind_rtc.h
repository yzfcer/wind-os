/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_rtc.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: ����ʵʱʱ�ӵ�ģ�飬���ģ����Ҫ�ⲿ��Ӳ��ʵʱʱ��ģ���֧�֣����򽫲���������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.06.23
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.06.23
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_RTC_H__
#define WIND_RTC_H__

#include "wind_config.h"
#include "wind_type.h"
#ifdef c_plusplus__
extern "C" {
#endif

#if WIND_RTC_SUPPORT
#define HOUR_TYPE_12 1//����Ϊ12Сʱ��
#define HOUR_TYPE_24 2//����Ϊ24Сʱ��
#define HOUR_TYPE HOUR_TYPE_24

#define DAY_AM 0
#define DAY_PM 1

typedef struct __time_s
{
    w_uint8_t hour;
    w_uint8_t minute;
    w_uint8_t second;
    w_uint16_t msecond;
}time_s,*ptime_s;

typedef struct __date_s
{
    w_uint16_t year;
    w_uint8_t month;
    w_uint8_t day;
}date_s,*pdate_s;

typedef enum __week_e
{
    MON = 1,
    TUS = 2,
    WED = 3,
    THU = 4,
    FRI = 5,
    SAT = 6,
    SUN = 7
}week_e,*pweek_e;

typedef struct __datetime_s
{
    date_s date;
    time_s time;
    week_e week;
} datetime_s,*pdatetime_s;

void wind_set_date(w_uint16_t year,w_uint8_t month,w_uint8_t day);
void wind_set_time(w_uint8_t hour,w_uint8_t minute,w_uint8_t second,w_uint16_t msecond);
void wind_set_datetime(w_uint16_t year,w_uint8_t month,w_uint8_t day,w_uint8_t hour,w_uint8_t minute,w_uint8_t second,w_uint16_t msecond,week_e week,w_uint8_t half_day);
pdate_s wind_get_date(void);
ptime_s wind_get_time(void);
pdatetime_s wind_get_datetime(void);
w_err_t wind_copy_datetime(pdatetime_s desdt,pdatetime_s srcdt);
void _wind_datetime_init(void);
void wind_second_inc(void);
void wind_msecond_inc(void); 


#endif//WIND_RTC_SUPPORT
#ifdef c_plusplus__
}
#endif//c_plusplus__


#endif
