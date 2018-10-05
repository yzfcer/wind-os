/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_time.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os��ʱ��������ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_TIME_H__
#define WIND_TIME_H__

#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_DATETIME_SUPPORT

#define HOUR_TYPE_12 1//����Ϊ12Сʱ��
#define HOUR_TYPE_24 2//����Ϊ24Сʱ��
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
    SUN = 0,
    MON = 1,
    TUS = 2,
    WED = 3,
    THU = 4,
    FRI = 5,
    SAT = 6,
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
void wind_second_inc(void);
void wind_msecond_inc(void);
#else
#define _wind_datetime_init() 
#endif

w_err_t _wind_tick_init(void);
w_uint32_t wind_get_tick(void);
w_uint32_t wind_get_seconds(void);

#ifdef __cplusplus
}
#endif

#endif  //WIND_TIME_H__
