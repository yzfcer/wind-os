/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_core.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�ĺ����������̵߳��ȹ���
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

#ifndef WIND_CORE_H__
#define WIND_CORE_H__
#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _wind_corevar_init() 

#define _wind_thread_dispatch() 
#define _wind_switchto_thread(thread) 

#define wind_disable_switch() 
#define wind_enable_switch() 

#define wind_disable_interrupt() 
#define wind_enable_interrupt() 

#define wind_enter_irq() 
#define wind_exit_irq() 


#ifdef __cplusplus
}
#endif

#endif

