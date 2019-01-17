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

void wind_system_reset(void);

#if WIND_HEAP_SUPPORT
void _wind_heaps_create(void);
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
