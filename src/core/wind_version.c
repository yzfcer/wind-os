/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_version.h / wind_version.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的核心代码
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_version.h"
#include "wind_debug.h"
const char* versioninfo = \
"E-mail:yzfcer@163.com.\r\n";

w_uint8_t *g_wind_logo[4] = 
{
    " _    __    _",
    " \\\\  //\\\\  // ",
    "  \\\\//  \\\\//  ",
    "   \\/    \\/   "   
};

const w_int8_t *wind_get_core_version_info(void)
{
    return versioninfo;
}

const w_int8_t *wind_get_core_version(void)
{
    return WIND_CORE_VERSION;
}
static void output_logo(void)
{
    int i;
    for(i = 0;i < 4;i++)
    {
        wind_printf("%s\r\n",g_wind_logo[i]);
    }
}

void _wind_print_os_info(void)
{
    const char *str;
    wind_printf("\r\n\r\nwind_os version:%s\r\n",wind_get_core_version());
    output_logo();
    wind_printf("core created on:%s @ chongqing,china.\r\n",__DATE__);
    str = wind_get_core_version_info();
    wind_printf(str);
    wind_printf("core is built at %s %s\r\n\r\n",__TIME__,__DATE__);
}

