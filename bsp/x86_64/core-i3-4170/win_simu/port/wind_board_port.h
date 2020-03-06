/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_board_port.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_BOARD_PORT_H_
#define WIND_BOARD_PORT_H_
#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void wind_init_hook(void);
void wind_system_reset(void);

#if WIND_HEAP_SUPPORT
void _wind_heaps_create(void);
#endif

#if WIND_MODULE_VFS_SUPPORT
void _wind_fs_mount_init(void);
#endif

typedef  void (*thread_run_f)(void *pargs);
//线程堆栈的初始化入口，移植需要重新实现
w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt,w_int32_t stk_depth);


#ifdef __cplusplus
}
#endif

#endif
