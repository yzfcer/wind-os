/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_time.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
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
#endif


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
}time_s;

typedef struct __date_s
{
    w_uint16_t year;
    w_uint8_t month;
    w_uint8_t day;
}date_s;

typedef enum __week_e
{
    MON = 1,
    TUS = 2,
    WED = 3,
    THU = 4,
    FRI = 5,
    SAT = 6,
    SUN = 7
}week_e;

typedef struct __datetime_s
{
    date_s date;
    time_s time;
    week_e week;
} datetime_s;

w_err_t wind_set_date(date_s *date);
w_err_t wind_set_time(time_s *time);
w_err_t wind_set_datetime(datetime_s *datetime);
w_err_t wind_get_date(date_s * date);
w_err_t wind_get_time(time_s *time);
w_err_t wind_get_datetime(datetime_s *datetime);
w_err_t wind_copy_datetime(datetime_s *desdt,datetime_s *srcdt);
void _wind_datetime_init(void);
void wind_second_inc(void);
void wind_msecond_inc(void); 

w_err_t _wind_time_init(void);
w_uint32_t wind_get_tick_count(void);

#ifdef __cplusplus
}
#endif

#endif  //WIND_TIME_H__
