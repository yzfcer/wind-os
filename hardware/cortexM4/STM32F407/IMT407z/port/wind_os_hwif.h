/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_cpu_port.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码头文件
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
#ifndef WIND_OS_HWIF_H_
#define WIND_OS_HWIF_H_
#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif
void _wind_target_init(void);
void wind_system_reset(void);

#if WIND_HEAP_SUPPORT
void _wind_heaps_mod_init(void);
#endif

#if WIND_FS_SUPPORT
void _wind_fs_mount_init(void);
#endif

typedef  void (*thread_run_f)(void *pargs);
//线程堆栈的初始化入口，移植需要重新实现
w_pstack_t _wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_pstack_t pstkbt);


#ifdef __cplusplus
}
#endif

#endif
