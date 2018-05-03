/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: cmd_datetime.c
**��   ��   ��: �ܽ���
**����޸�����: 2013.10.19
**��        ��: ϵͳ�Ŀ���̨����date��time������������ϵͳ�����ں�ʱ������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.10.19
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2013.10.19
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#include <stdio.h>
#include "wind_string.h"
#include "wind_cmd.h"
#include "wind_time.h"
#if WIND_DATETIME_SUPPORT

static w_err_t cmd_set_datetime(w_int32_t argc,char **argv)
{
    w_int32_t cnt;
    datetime_s dt;
    if(argc < 4)
    {
        console_printf("error:parameter is NOT enough.\r\n");
        return ERR_INVALID_PARAM;
    }
    if(wind_strlen(argv[2]) < 10)
    {
        console_printf("date format error.\r\n");
        return ERR_INVALID_PARAM;
    }
    if(wind_strlen(argv[3]) < 8)
    {
        console_printf("time format error.\r\n");
        return ERR_INVALID_PARAM;
    }
    
    cnt = sscanf(argv[2],"%4d/%02d/%02d",&dt.date.year,
                (w_uint32_t*)&dt.date.month,(w_uint32_t*)&dt.date.day);
    WIND_ASSERT_RETURN(cnt >= 3,ERR_INVALID_PARAM);
    cnt = sscanf(argv[3],"%2d:%2d:%2d",(w_uint32_t*)&dt.time.hour,
                (w_uint32_t*)&dt.time.minute,(w_uint32_t*)&dt.time.second);
    WIND_ASSERT_RETURN(cnt >= 3,ERR_INVALID_PARAM);
    dt.time.msecond = 0;
    wind_datetime_set(&dt);
    console_printf("system date:%4d/%02d/%02d %02d:%02d:%02d %4d\r\n",dt.date.year,
                dt.date.month,dt.date.day,dt.time.hour,
                dt.time.minute,dt.time.second,dt.time.msecond);
    return ERR_OK;
}


static w_err_t cmd_showdatetime(w_int32_t argc,char **argv)
{
    w_err_t err;
    datetime_s dt;
    err = wind_datetime_get(&dt);
    WIND_ASSERT_RETURN(err == ERR_OK,ERR_COMMAN);
    console_printf("system date:%4d/%02d/%02d %02d:%02d:%02d  %4d\r\n",dt.date.year,
                dt.date.month,dt.date.day,dt.time.hour,
                dt.time.minute,dt.time.second,dt.time.msecond);
    return ERR_OK;
}



COMMAND_DISC(datetime)
{
    console_printf("to set or show system date and time.\r\n");
}

COMMAND_USAGE(datetime)
{
    console_printf("datetime set <datetime>:to set the system date infomation.format:YYYY/MM/DD HH:mm:SS\r\n");
    console_printf("datetime show:to show the system date and time infomation.\r\n");
}

COMMAND_MAIN(datetime,argc,argv)
{
    if(wind_strcmp(argv[1],"set") == 0)
    {
        return cmd_set_datetime(argc,argv);
    }
    else if(wind_strcmp(argv[1],"show") == 0)
    {
        return cmd_showdatetime(argc,argv);
    }
    else
    {
        console_printf("datetime:format error.\r\n");
        return ERR_COMMAN;
    }
}

COMMAND_DEF(datetime);


#endif

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

