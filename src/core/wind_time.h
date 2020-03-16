/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_time.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_TIME_H__
#define WIND_TIME_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_DATETIME_SUPPORT

#define HOUR_TYPE_12 1//Defined as 12-hour system
#define HOUR_TYPE_24 2//Defined as 24-hour system
#define HOUR_TYPE HOUR_TYPE_24

#define DAY_AM 0
#define DAY_PM 1

typedef struct __w_time_s
{
    w_uint8_t hour;
    w_uint8_t minute;
    w_uint8_t second;
    w_uint16_t msecond;
}w_time_s;

typedef struct __w_date_s
{
    w_uint16_t year;
    w_uint8_t month;
    w_uint8_t day;
}w_date_s;

typedef enum __w_week_e
{
    SUN = 0, //Sunday
    MON = 1, //Monday
    TUS = 2, //Tuesday
    WED = 3, //Wednesday
    THU = 4, //Thursday
    FRI = 5, //Friday
    SAT = 6, //Saturday
}w_week_e;

typedef struct __datetime_s
{
    w_date_s date;
    w_time_s time;
    w_week_e week;
} datetime_s;

void _wind_datetime_mod_init(void);

w_err_t wind_datetime_setdate(w_date_s *date);
w_err_t wind_datetime_settime(w_time_s *time);
w_err_t wind_datetime_set(datetime_s *datetime);

w_err_t wind_datetime_getdate(w_date_s * date);
w_err_t wind_datetime_gettime(w_time_s *time);
w_err_t wind_datetime_get(datetime_s *datetime);

w_err_t wind_datetime_copy(datetime_s *desdt,datetime_s *srcdt);
void wind_msecond_inc(void);
void _wind_datetime_tick_isr(void);

#endif // #if WIND_DATETIME_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif  // #ifndef WIND_TIME_H__
