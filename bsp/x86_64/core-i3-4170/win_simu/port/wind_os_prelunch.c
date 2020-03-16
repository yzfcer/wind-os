/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_os_prelaunch.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.01.06
**描        述: wind os的预启动文件
**功        能: 启动前硬件初始化，初始化外部RAM，初始化数据段，跳转到wind_o_lunch
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.01.06
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.01.06
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include <windows.h>
 
CRITICAL_SECTION CriticalSection;

#ifdef __cplusplus
extern "C" {
#endif

static void hw_preinit(void)
{
    InitializeCriticalSection(&CriticalSection);
}

#if defined(__CC_ARM)
void data_bss_init(void)
{
    return;
}
#else
static void data_bss_init(void)
{
}
#endif
static void exram_init(void)
{
}

extern void wind_os_launch(void);
void wind_os_prelaunch(void)
{
    hw_preinit();
    exram_init();
    data_bss_init();
    wind_os_launch();
}


#ifdef __cplusplus
}
#endif

