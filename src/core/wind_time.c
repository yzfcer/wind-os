/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_time.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码
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
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
#include "wind_thread.h"
#include "wind_core.h"
#include "wind_time.h"
#include "wind_var.h"
#include "wind_string.h"
static w_uint32_t g_wind_time_ms_cnt = 0;//毫秒计时

w_uint32_t tick_cnt_for_time;
datetime_s g_datetime;//当前日期和时间
static w_uint8_t g_daysofmonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

//-------------------------------------------------------------------------------------

static void hwrtc_set_datetime(datetime_s *datetime)
{

}

static void hwrtc_set_date(void)
{
}

static void hwrtc_set_time(void)
{
}

static void hwrtc_set_week(void)
{
}

static void hwrtc_get_datetime(void)
{
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

w_err_t wind_set_date(date_s *date)
{
    datetime_s *dt;
    dt = &g_datetime;
    if(is_leap(date->year) && (date->month == 2) && (date->day > 29))
        return ERR_INVALID_PARAM;
    else if((date->month >= 13)
    || (date->month == 0)
    || (date->day == 0)
    || (date->day > g_daysofmonth[date->month - 1]))
    return ERR_INVALID_PARAM;
    dt->date.year = date->year;
    dt->date.month = date->month;
    dt->date.day = date->day;
    hwrtc_set_date();
    return ERR_OK;
}

w_err_t wind_set_time(time_s *time)
{
    datetime_s *dt;
    dt = &g_datetime;
    if(time->hour >= 24
    || time->minute >= 60
    || time->second >= 60
    || time->msecond >= 1000)
    return ERR_INVALID_PARAM;
    dt->time.hour = time->hour;
    dt->time.minute = time->minute;
    dt->time.second = time->second;
    dt->time.msecond = time->msecond;
    hwrtc_set_time();
    return ERR_OK;
}

w_err_t wind_set_datetime(datetime_s *datetime)
{
    datetime_s *dt;
    dt = &g_datetime;
    wind_set_date(&datetime->date);
    wind_set_time(&datetime->time);
    dt->week = datetime->week;
    hwrtc_set_week();
    return ERR_OK;
}

w_err_t wind_get_date(date_s * date)
{
    hwrtc_get_datetime();
    wind_memcpy(date,&g_datetime.date,sizeof(date_s));
    return ERR_OK;
}

w_err_t wind_get_time(time_s *time)
{
    hwrtc_get_datetime();
    wind_memcpy(time,&g_datetime.time,sizeof(time_s));
    return ERR_OK;
}

w_err_t wind_get_datetime(datetime_s *datetime)
{
    hwrtc_get_datetime();
    wind_memcpy(datetime,&g_datetime,sizeof(datetime_s));
    return ERR_OK;
}



void _wind_datetime_init(void)
{
    
}

w_err_t wind_copy_datetime(datetime_s *desdt,datetime_s *srcdt)
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
    dt = &g_datetime;
    wind_disable_interrupt();
    dt->time.second ++;
    if(dt->time.second >= 60)
    {
        dt->time.second = 0;
        dt->time.minute ++;
        if(dt->time.minute >= 60)
        {
            dt->time.minute = 0;
            dt->time.hour ++;
            if(dt->time.hour >= 24)
            {
                dt->time.hour = 0;
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
    wind_enable_interrupt();
}

void wind_msecond_inc(void)
{
    datetime_s *dt;
    dt = &g_datetime;
    dt->time.msecond ++;
    if(dt->time.msecond >= 1000)
    {
        dt->time.msecond = 0;
        wind_second_inc();
    }
}


w_err_t _wind_time_init(void)
{
    g_wind_time_ms_cnt = 0;
    return ERR_OK;
}

//获取毫秒计时
w_uint32_t wind_get_tick_count(void)
{
    return g_wind_time_ms_cnt;
}

//tick中断调用的函数
void wind_tick_callback(void)
{
    TICKS_CNT ++;//更新tick计数器
    tick_cnt_for_time ++;
    if(tick_cnt_for_time >= WIND_TICK_PER_SEC)
    {
        tick_cnt_for_time = 0;
        wind_second_inc();
    }
    _wind_thread_wakeup();
}

void wind_tick_isr(void)
{				   
    wind_enter_irq();
    wind_tick_callback();
    wind_exit_irq();       
}


