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
#include "wind_dlist.h"
#include "wind_stati.h"
#ifdef __cplusplus
extern "C" {
#endif
#define WIND_MPOOL_MAGIC 0x5d9c843e
#define POOL_BLK_FREE 0x52d6e300
#define POOL_BLK_USED (POOL_BLK_FREE | 0xA5)

typedef struct __pool_item_s pool_item_s;
typedef struct __mpool_s pool_s;

//一个基本的内存池链结构体
struct __pool_item_s
{
    w_uint32_t flag;
    pool_item_s* next;
};

//内存池的头部信息结构体
struct __mpool_s
{
    w_uint32_t magic;//内存池被成功建立的标志
    const char *name;
    dnode_s poolnode;
    stati_s stati;
    void *head;//内存池的头部位置
    w_uint32_t size;//内存池的实际可用空间大小
    w_uint32_t itemsize;//每个块的大小
    w_uint32_t itemnum;//分成的内存块的数量
    w_uint32_t used;//已经使用的内存块的数量
    pool_item_s* free_head;//空闲块的指针
    pool_item_s* free_end;//最后一个空闲块的指针
};

//定义内存池的方法
#define WIND_MPOOL_ALIGN(size) (((size)+3) & (~0x03))
#define WIND_MPOOL(pool,itemnum,itemsize) w_uint8_t pool[sizeof(pool_s) + itemnum *(WIND_MPOOL_ALIGN(itemsize) + sizeof(pool_item_s))+8]

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t itemsize);

void   *wind_pool_alloc(void *mem);

w_err_t wind_pool_free(void *mem,void *block);

w_err_t wind_pool_print(void *pool);

void wind_pool_print_list(dlist_s *list);

#ifdef __cplusplus
}
#endif


#endif

