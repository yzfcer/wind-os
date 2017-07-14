/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_rtc.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 关于实时时钟的模块，这个模块需要外部的硬件实时时钟模块的支持，否则将不会有作用
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.06.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.06.23
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_RTC_H__
#define WIND_RTC_H__

#include "wind_config.h"
#include "wind_types.h"
#ifdef c_plusplus__
extern "C" {
#endif

#if WIND_RTC_SUPPORT > 0
#define HOUR_TYPE_12 1//定义为12小时制
#define HOUR_TYPE_24 2//定义为24小时制
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
void wind_datetime_init(void);
void wind_second_inc(void);
void wind_msecond_inc(void); 


#endif//WIND_RTC_SUPPORT > 0
#ifdef c_plusplus__
}
#endif//c_plusplus__


#endif
