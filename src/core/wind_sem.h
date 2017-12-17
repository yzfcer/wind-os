/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_sem.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的信号量头文件
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
#ifndef WIND_SEM_H__
#define WIND_SEM_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_SEM_SUPPORT
//#define SEM_NAME_LEN 20 //线程名的最大长度，包括 '\0'

typedef struct _wind_sem
{
    const char* name;
    dnode_s semnode;
    dlist_s waitlist;  //等待线程队列
    w_bool_t used;         //是否有效的标志
    w_uint16_t sem_tot;    //初始化的信号量的值
    w_uint16_t sem_num;    //当前的信号量的值
}sem_s;



sem_s * wind_sem_create(const char *name,w_uint16_t semValue);
w_err_t wind_sem_post(sem_s * psem);
w_err_t wind_sem_fetch(sem_s * psem,w_uint32_t timeout);
w_err_t wind_sem_tryfree(sem_s * psem);
w_err_t wind_sem_free(sem_s * psem);
w_err_t wind_sem_print(pdlist_s list);

#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_SEM_H__

