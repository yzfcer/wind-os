/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_sysinfo.h / wind_sysinfo.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的核心代码
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_sysinfo.h"
#include "wind_debug.h"
#include "wind_board_port.h"
#include "wind_core.h"
const char* versioninfo = \
"E-mail:yzfcer@163.com.\r\n";

w_sysinfo_s g_sysinfo = 
{
    ARCH_NAME,
    CPU_NAME,
    BOARD_NAME,
    WIND_OS_VERSION,
    HW_VERSION,
    SOFT_VERSION
};

void _wind_os_print_logo(void)
{
    int i;
    char *logo[4] = 
    {
        " _    __    _",
        " \\\\  //\\\\  // ",
        "  \\\\//  \\\\//  ",
        "   \\/    \\/   "   
    };
    wind_printf("\r\n");
    wind_printf("system start ...\r\n");
    wind_printf("*** Only simple can be the perfect ***\r\n");
		
    for(i = 0;i < 4;i++)
    {
        wind_printf("%s\r\n",logo[i]);
    }	    
}

void _wind_print_sysinfo(void)
{
    w_uint32_t sec;
    wind_printf("ARCH         : %s\r\n",g_sysinfo.archname);
    wind_printf("CPU          : %s\r\n",g_sysinfo.cpuname);
    wind_printf("BOARD        : %s\r\n",g_sysinfo.boardname);
    wind_printf("os ver       : %d.%d.%d\r\n",(g_sysinfo.os_ver>>16)&0xff,
        (g_sysinfo.os_ver>>8)&0xff,(g_sysinfo.os_ver>>0)&0xff);
    wind_printf("soft ver     : %d.%d.%d\r\n",(g_sysinfo.soft_ver>>16)&0xff,
        (g_sysinfo.soft_ver>>8)&0xff,(g_sysinfo.soft_ver>>0)&0xff);
    wind_printf("hard ver     : %d.%d.%d\r\n",(g_sysinfo.hw_ver>>16)&0xff,
        (g_sysinfo.hw_ver>>8)&0xff,(g_sysinfo.hw_ver>>0)&0xff);
    wind_printf("build time   : %s %s\r\n",__TIME__,__DATE__);
    sec = wind_get_seconds();
    wind_printf("running time : %d:%d:%d\r\n\r\n",sec/3600,sec%3600/60,sec%60);
}

