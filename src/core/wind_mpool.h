/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mpool.h / wind_mpool.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的内存池管理相关的功能函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.11.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.11.27
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_MPOOL_H__
#define WIND_MPOOL_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"

#ifdef __cplusplus
extern "C" {
#endif
#define WIND_MPOOL_DEBUG(...) //WIND_STD_OUT(__VA_ARGS__)
#define WIND_MPOOL_MAGIC 0x5d9c843e
#define WIND_MPOOL_BLOCK_MAGIC 0x52d6e3a9
//一个基本的内存池链结构体
typedef struct __pool_s
{
    struct __pool_s *next;
}pool_s,*ppool_s;

//内存池的头部信息结构体
typedef struct __mpoolHead_s
{
    w_uint32_t magic;//内存池被成功建立的标志
    void *head;//内存池的头部位置
    const char *name;
    w_uint32_t len;//内存池的实际可用空间大小
    w_uint32_t itemsize;//每个块的大小
    w_uint32_t num;//分成的内存块的数量
    w_uint32_t used;//已经使用的内存块的数量
    ppool_s free;//空闲块的指针
    ppool_s last;//最后一个空闲块的指针
}pool_head_s,*ppool_head_s;

#define WIND_MPOOL_ALIGN(x) ((((x)+3)/4)*4)
//定义内存池的方法
#define WIND_MPOOL(pool,num,size) w_uint8_t pool[sizeof(pool_head_s) + num * (WIND_MPOOL_ALIGN(size) + sizeof(w_uint32_t))]

w_err_t wind_pool_print(w_int8_t *name,void *pool);

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t msize,w_uint32_t itemsize);

void *wind_pool_alloc(void *mem);

w_err_t wind_pool_free(void *mem,void *block);


#ifdef __cplusplus
}
#endif


#endif

