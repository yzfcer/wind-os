/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_cpu_port.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os��ʱ��������ͷ�ļ�
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
#ifndef WIND_OS_HWIF_H_
#define WIND_OS_HWIF_H_
#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARCH_NAME "Cortex-M3"
#define CPU_NAME "STM42F103ZET6"
#define BOARD_NAME "ALIENTEKV3"
#define HW_VERSION 0x000003
#define SOFT_VERSION 0x000102


void _wind_enter_main_hook(void);
void wind_system_reset(void);

#if WIND_HEAP_SUPPORT
void _wind_heaps_mod_init(void);
#endif

#if WIND_FS_SUPPORT
void _wind_fs_mount_init(void);
#endif


typedef  void (*thread_run_f)(void *pargs);
//�̶߳�ջ�ĳ�ʼ����ڣ���ֲ��Ҫ����ʵ��
w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt);


#ifdef __cplusplus
}
#endif

#endif
