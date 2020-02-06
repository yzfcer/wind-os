/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_sysinfo.h / wind_sysinfo.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�ĺ��Ĵ���
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

