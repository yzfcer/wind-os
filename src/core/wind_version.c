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
#include "wind_types.h"
#include "wind_version.h"
#include "wind_debug.h"
const s8_t versioninfo[] = \
"yzfcer@163.com,thank you for your support!\r\n";

u8_t *g_wind_logo[4] = 
{
    " _    __    _",
    " \\\\  //\\\\  // ",
    "  \\\\//  \\\\//  ",
    "   \\/    \\/   "   
};
const s8_t *wind_get_core_version_info(void)
{
    return versioninfo;
}

const s8_t *wind_get_core_version(void)
{
    return WIND_CORE_VERSION;
}
static void output_logo(void)
{
    int i;
    for(i = 0;i < 4;i++)
    {
        WIND_INFO("%s\r\n",g_wind_logo[i]);
    }
}

void wind_output_product_info(void)
{
    const s8_t *str;
    WIND_INFO("\r\n\r\nwind_os version:%s\r\n",wind_get_core_version());
    output_logo();
    WIND_INFO("core created on:%s @ chengdu,sichuan province,china.\r\n",__DATE__);
    str = wind_get_core_version_info();
    WIND_INFO(str);
    WIND_INFO("core is built at %s %s\r\n",__TIME__,__DATE__);
}

