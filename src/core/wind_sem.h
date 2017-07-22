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
#include "wind_types.h"
//#include "wind_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_SEM_SUPPORT > 0
#define SEM_NAME_LEN 20 //线程名的最大长度，包括 '\0'

typedef struct _wind_sem
{
    char name[SEM_NAME_LEN];
    w_bool_t used;         //是否有效的标志
    w_uint16_t sem_num;     //当前的信号量的值
    w_uint16_t tot_sem_num; //初始化的信号量的值
    list_s waitlist;  //等待线程队列
}sem_s,*psem_s;



psem_s wind_sem_create(const char *name,w_uint16_t semValue);
w_err_t wind_sem_post(psem_s psem);
w_err_t wind_sem_fetch(psem_s psem,w_uint32_t timeout);
w_err_t wind_sem_tryfree(psem_s psem);
w_err_t wind_sem_free(psem_s psem);
w_err_t wind_sem_test(void);

#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_SEM_H__

