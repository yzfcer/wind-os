/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_os_prelaunch.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.01.06
**��        ��: wind os��Ԥ�����ļ�
**��        ��: ����ǰӲ����ʼ������ʼ���ⲿRAM����ʼ�����ݶΣ���ת��wind_o_lunch
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.01.06
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.01.06
** �衡��: 
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

