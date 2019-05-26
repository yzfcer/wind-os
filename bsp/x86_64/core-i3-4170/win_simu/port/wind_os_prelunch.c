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

