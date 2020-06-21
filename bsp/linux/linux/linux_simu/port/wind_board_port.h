/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_board_port.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os board adapter interface
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26    
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
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
void wind_enter_thread_hook(void);
void wind_system_reset(void);

#if WIND_HEAP_SUPPORT
void _wind_heaps_create(void);
#endif

#if WIND_MODULE_VFS_SUPPORT
void _wind_fs_mount_init(void);
#endif

#if WIND_MODULE_NET_SUPPORT
void wind_netdevs_register(void);
#endif

typedef  void (*thread_run_f)(void *pargs);
w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt,w_int32_t stk_depth);


#ifdef __cplusplus
}
#endif

#endif
