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
    w_uint32_t *src,*dest;
    extern w_uint32_t Image$$ER_IROM1$$RW$$Base[];
    extern w_uint32_t Image$$ER_IROM1$$ZI$$Base[];

    extern w_uint32_t Image$$ER_DATA$$RW$$Base[];
    extern w_uint32_t Image$$ER_DATA$$ZI$$Base[];

    extern w_uint32_t Image$$ER_HEAP$$RW$$Base[];
    extern w_uint32_t Image$$ER_HEAP$$ZI$$Base[];
    
    src = (w_uint32_t *)Image$$ER_IROM1$$RW$$Base;
    dest = (w_uint32_t *)Image$$ER_DATA$$RW$$Base;
    limit = ((w_uint32_t)Image$$ER_HEAP$$RW$$Base - (w_uint32_t)Image$$ER_DATA$$ZI$$Base)/sizeof(w_uint32_t);
    for(i =0;i < limit;i ++)
    {
        dest[i] = src[i];
    }
    dest = Image$$ER_IROM1$$ZI$$Base;
    limit = ((w_uint32_t)Image$$ER_HEAP$$RW$$Base - (w_uint32_t)Image$$ER_DATA$$ZI$$Base)/sizeof(w_uint32_t);
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

