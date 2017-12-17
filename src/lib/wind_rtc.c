/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_rtc.c
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

#include "wind_type.h"
#include "wind_config.h"
#include "wind_rtc.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
//#include "S3C2440addr.h"
#if WIND_RTC_SUPPORT
datetime_s G_DATETIME;//当前日期和时间
w_uint8_t g_half_day = DAY_AM;//当前是上午还是下午
static w_uint8_t g_daysofmonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

//-------------------------------------------------------------------------------------

#define BCD2Num(x) (((x) >> 4) *10 + ((x) & 0x0f))
#define Num3BCD(x) ((((x) / 10) << 4) + ((x) % 10))
static void hwrtc_set_datetime(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    rRTCCON = 1 ;        //RTC read and write enable
    rBCDYEAR = Num3BCD(dt->date.year - 2000);        //年
    rBCDMON  = Num3BCD(dt->date.month);        //月
    rBCDDATE = Num3BCD(dt->date.day) ;        //日    
    rBCDDAY  = Num3BCD(dt->week) ;    //星期
    rBCDHOUR = Num3BCD(dt->time.hour) ;        //小时
    rBCDMIN  = Num3BCD(dt->time.minute) ;        //分
    rBCDSEC  = Num3BCD(dt->time.second) ;        //秒
    rRTCCON &= ~1 ;        //RTC read and write disable
}

static void hwrtc_set_date(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    rRTCCON = 1 ;        //RTC read and write enable
    rBCDYEAR = Num3BCD(dt->date.year - 2000);        //年
    rBCDMON  = Num3BCD(dt->date.month) ;        //月
    rBCDDATE = Num3BCD(dt->date.day);        //日    
    wind_error("%d,%d,%d",rBCDYEAR,rBCDMON,rBCDDATE);
    rRTCCON &= ~1 ;        //RTC read and write disable
}

static void hwrtc_set_time(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    rRTCCON = 1 ;        //RTC read and write enable
    rBCDHOUR = Num3BCD(dt->time.hour) ;        //小时
    rBCDMIN  = Num3BCD(dt->time.minute) ;        //分
    rBCDSEC  = Num3BCD(dt->time.second) ;        //秒
    rRTCCON &= ~1 ;        //RTC read and write disable
}

static void hwrtc_set_week(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    rRTCCON = 1 ;        //RTC read and write enable
    rBCDDAY  = Num3BCD(dt->week) ;    //星期
    rRTCCON &= ~1 ;        //RTC read and write disable
}
static void hwrtc_get_datetime(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    rRTCCON = 1 ;        //RTC read and write enable
    dt->date.year = BCD2Num(rBCDYEAR)+2000;        //年
    dt->date.month = BCD2Num(rBCDMON)  ;        //月
    dt->date.day = BCD2Num(rBCDDATE)  ;        //日    
    dt->week = BCD2Num(rBCDDAY);        //星期
    dt->time.hour = BCD2Num(rBCDHOUR)  ;        //小时
    dt->time.minute = BCD2Num(rBCDMIN)  ;        //分
    dt->time.second = BCD2Num(rBCDSEC)  ;        //秒
    rRTCCON &= ~1 ;        //RTC read and write disable
}
//-------------------------------------------------------------------------------------

static w_uint8_t is_leap(w_uint16_t year)
{
    if(year & 0x03)
        return ERR_OK;
    else if((year % 400 == 0))
        return ERR_OK;
    else
        return 1;
}

void wind_set_date(w_uint16_t year,w_uint8_t month,w_uint8_t day)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    if(is_leap(year) && (month == 2) && (day > 29))
        return;
    else if((month >= 13)
    || (month == 0)
    || (day == 0)
    || (day > g_daysofmonth[month - 1]))
    return;
    dt->date.year = year;
    dt->date.month = month;
    dt->date.day = day;
    hwrtc_set_date();
}

void wind_set_time(w_uint8_t hour,w_uint8_t minute,w_uint8_t second,w_uint16_t msecond)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    if(hour >= 24
    || minute >= 60
    || second >= 60
    || msecond >= 1000)
    return;
    dt->time.hour = hour;
    dt->time.minute = minute;
    dt->time.second = second;
    dt->time.msecond = msecond;
    hwrtc_set_time();
}

void wind_set_datetime(w_uint16_t year,w_uint8_t month,w_uint8_t day,w_uint8_t hour,w_uint8_t minute,w_uint8_t second,w_uint16_t msecond,week_e week,w_uint8_t half_day)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    wind_set_date(year,month,day);
    wind_set_time(hour,minute,second,msecond);
    dt->week = week;
    hwrtc_set_week();
    g_half_day = half_day;
}

pdate_s wind_get_date(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    hwrtc_get_datetime();
    return &dt->date;
}

ptime_s wind_get_time(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    hwrtc_get_datetime();
    return &dt->time;
}

pdatetime_s wind_get_datetime(void)
{
    hwrtc_get_datetime();
    return &G_DATETIME;
}



void wind_datetime_init(void)
{
    //wind_set_datetime(2013,10,19,23,34,35,0,5,1);
}
w_err_t wind_copy_datetime(pdatetime_s desdt,pdatetime_s srcdt)
{
    if(!desdt || !srcdt)
        return ERR_INVALID_PARAM;
    desdt->date.year = srcdt->date.year;
    desdt->date.month = srcdt->date.month;
    desdt->date.day = srcdt->date.day;
    desdt->time.hour = srcdt->time.hour;
    desdt->time.minute = srcdt->time.minute;
    desdt->time.second = srcdt->time.second;
    desdt->time.msecond = srcdt->time.msecond;
    desdt->week = srcdt->week;
    return ERR_OK;
}



void wind_second_inc(void)
{
    w_uint8_t dayofmonth;
    datetime_s *dt;
    dt = &G_DATETIME;
    wind_close_interrupt();
    dt->time.second ++;
    if(dt->time.second >= 60)
    {
        dt->time.second = 0;
        dt->time.minute ++;
        if(dt->time.minute >= 60)
        {
            dt->time.minute = 0;
            dt->time.hour ++;
#if HOUR_TYPE == HOUR_TYPE_24
            if(dt->time.hour >= 24)
            {
                dt->time.hour = 0;
#else
            if(dt->time.hour >= 12)
            {
                dt->time.hour = 0;
                g_half_day = 1 - g_half_day;
                if(g_half_day)
                {
                    wind_open_interrupt();
                    return;
                }
#endif
                dt->date.day ++;
                dt->week ++;
                if(dt->week > SUN)
                    dt->week = MON;
                dayofmonth = g_daysofmonth[dt->date.month - 1];
                if(dt->date.month == 2)
                    dayofmonth += is_leap(dt->date.year);
                if(dt->date.day >= dayofmonth)
                {
                    dt->date.day = 1;
                    dt->date.month ++;
                    if(dt->date.month >= 13)
                    {
                        dt->date.month = 1;
                        dt->date.year ++;
                    }
                }
            }
        }
    }
    wind_open_interrupt();
}

void wind_msecond_inc(void)
{
    datetime_s *dt;
    dt = &G_DATETIME;
    dt->time.msecond ++;
    if(dt->time.msecond >= 1000)
    {
        dt->time.msecond = 0;
        wind_second_inc();
    }
}
#endif//WIND_RTC_SUPPORT

