/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_os_prelaunch.c
** Author      : Jason Zhou
** Last Date   : 2019.01.06
** Description : wind os prepare for starting
** Function    : Hardware initialization, external RAM initialization, data segment initialization, and 
**               jump to wind_o_lunch
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.01.06
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.01.06
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include <stdio.h>
 
//CRITICAL_SECTION CriticalSection;

#ifdef __cplusplus
extern "C" {
#endif

static void hw_preinit(void)
{
    //InitializeCriticalSection(&CriticalSection);
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

