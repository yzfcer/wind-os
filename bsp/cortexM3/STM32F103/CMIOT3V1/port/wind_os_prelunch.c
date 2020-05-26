/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_os_prelaunch.c
** Author      : Jason Zhou
** Last Date   : 2019.01.06
** Description : wind-os prepare for starting
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

#ifdef __cplusplus
extern "C" {
#endif
extern void SystemInit(void);

static void hw_preinit(void)
{
    SystemInit();
}

#if defined(__CC_ARM)
void data_bss_init(void)
{
    w_uint32_t i,limit; 
    w_addr_t *src,*dest;
    extern w_addr_t Image$$ER_IROM1$$RW$$Base[];
    extern w_addr_t Image$$ER_IROM1$$ZI$$Base[];

    extern w_addr_t Image$$ER_DATA$$RW$$Base[];
    extern w_addr_t Image$$ER_DATA$$ZI$$Base[];

    extern w_addr_t Image$$ER_HEAP$$RW$$Base[];
    extern w_addr_t Image$$ER_HEAP$$ZI$$Base[];
    
    src = (w_addr_t *)Image$$ER_IROM1$$RW$$Base;
    dest = (w_addr_t *)Image$$ER_DATA$$RW$$Base;
    limit = (w_uint32_t)((w_addr_t)Image$$ER_HEAP$$RW$$Base - (w_addr_t)Image$$ER_DATA$$ZI$$Base)/sizeof(w_addr_t);
    for(i =0;i < limit;i ++)
    {
        dest[i] = src[i];
    }
    dest = Image$$ER_IROM1$$ZI$$Base;
    limit = (w_uint32_t)((w_addr_t)Image$$ER_HEAP$$RW$$Base - (w_addr_t)Image$$ER_DATA$$ZI$$Base)/sizeof(w_addr_t);
    for(i =0;i < limit;i ++)
    {
        dest[i] = 0;
    }
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

