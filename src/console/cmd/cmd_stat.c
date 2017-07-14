/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_echo.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的控制台命令stat处理函数，统计资源使用情况
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.10.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.10.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_types.h"
#include "console_framework.h"
#include "wind_err.h"
#include "wind_debug.h"
#include "wind_mem.h"
#include "wind_string.h"
#include "wind_stat.h"
#include "wind_var.h"



static void core_stat_convert_num(w_int8_t *buf,w_uint32_t num)
{
    w_uint8_t i = 0;
    if(num >= 10000)
        buf[i++] = (num/10000)%10 + '0';
    if(num >= 1000)
        buf[i++] = (num/1000)%10 + '0';
    if(num >= 100)
        buf[i++] = (num/100)%10 + '0';
    if(num >= 10)
        buf[i++] = (num/10)%10 + '0';
    buf[i++] = num%10 + '0';
}
static void core_output_srcusage(w_uint16_t opt)
{
    w_int8_t str[STAT_NAME_LEN + 33];
    w_int16_t i,len;
    wind_memset(str,' ',STAT_NAME_LEN + 33);
    str[STAT_NAME_LEN + 32] = 0;
    len = wind_strlen(G_STAT[opt].name);
    for(i = 0;i < len;i ++)
        str[i] = G_STAT[opt].name[i];
    //wind_memcpy(str,G_STAT[opt].name,STAT_NAME_LEN);
    core_stat_convert_num(&str[STAT_NAME_LEN],G_STAT[opt].tot);
    core_stat_convert_num(&str[STAT_NAME_LEN + 8],G_STAT[opt].used);
    core_stat_convert_num(&str[STAT_NAME_LEN + 16],G_STAT[opt].max);
    core_stat_convert_num(&str[STAT_NAME_LEN + 24],G_STAT[opt].err);
    wind_printf("%s\r\n",str);
}
static void core_stat_show_proc(w_uint16_t opt)
{
    w_int8_t str[STAT_NAME_LEN + 33];
    w_int16_t i;
    wind_memset(str,0,STAT_NAME_LEN + 33);
    //str[STAT_NAME_LEN + 24] = 0;
    wind_memcpy(str,"source",6);
    wind_memcpy(&str[STAT_NAME_LEN],"tot",3);
    wind_memcpy(&str[STAT_NAME_LEN + 8],"used",4);
    wind_memcpy(&str[STAT_NAME_LEN + 16],"max",3);
    wind_memcpy(&str[STAT_NAME_LEN + 24],"err",3);
    for(i = 0;i < STAT_NAME_LEN + 32;i ++)
    {
        if(str[i] == 0)
            str[i] = ' ';
    }
    wind_printf("\r\n%s\r\n",str);
    
    
    if(opt != 0xffff)
    {
        core_output_srcusage(opt);
    }
    else
    {
        for(i = 0;i < STAT_CNT;i++)
        {
            core_output_srcusage(i);
        }
    }
}




w_err_t cmd_stat_show_mpool_main(w_int32_t argc,char **argv)
{
    core_stat_show_proc(0xffff);
    return ERR_OK;
}

w_err_t cmd_stat_show_cpuusage_main(w_int32_t argc,char **argv)
{
    wind_printf("cpu usage persage:%%%d\r\n",WIND_CPU_USAGE);
    return ERR_OK;    
}



cmd_s g_cmd_stat[] = 
{
    {
        NULL,"stat show mpool","to show mpools usage info.",
        "",cmd_stat_show_mpool_main
    },
    {
        NULL,"stat show cpuusage","to show current cpu usage persent.",
        "",cmd_stat_show_cpuusage_main
    }
};

void register_cmd_stat(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,g_cmd_stat,sizeof(g_cmd_stat)/sizeof(cmd_s));
}


