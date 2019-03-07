/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mpool.h / wind_mpool.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的内存池管理相关的功能函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2013.11.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2013.11.27
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_POOL_H__
#define WIND_POOL_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_stati.h"
#ifdef __cplusplus
extern "C" {
#endif
#define WIND_POOL_MAGIC 0x5d9c843e
#define WIND_POOLITEM_MAGIC 0x507c

#define F_POOLITEM_USED (0x01 << 0) //标记sem对象是否通过内存池分配
#define IS_F_POOLITEM_USED(item) ((item->head.flag & F_POOLITEM_USED) == F_POOLITEM_USED)
#define SET_F_POOLITEM_USED(item) (item->head.flag |= F_POOLITEM_USED)
#define CLR_F_POOLITEM_USED(item) (item->head.flag &= (~F_POOLITEM_USED))
#define POOL_ITEM(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
typedef struct __w_poolitem_s w_poolitem_s;
typedef struct __w_pool_s w_pool_s;
#define WIND_POOL_ALIGN(size) (((size)+7) & (~0x07))

typedef struct
{
    w_uint16_t magic;
    w_uint16_t flag;
    w_poolitem_s* next;
}w_pihead_s;

//一个基本的内存池对象结构体
struct __w_poolitem_s
{
    w_pihead_s head;
    w_uint8_t buff[8];
};

//内存池的基本描述信息结构体
struct __w_pool_s
{
    w_obj_s obj;
    w_stati_s stati;
    void *head;//内存池的头部位置
    w_uint32_t size;//内存池的实际可用空间大小
    w_uint32_t itemsize;//每个块的大小
    w_int32_t itemnum;//分成的内存块的数量
    w_poolitem_s* free_head;//空闲块的指针
    w_poolitem_s* free_end;//最后一个空闲块的指针
    w_int32_t pad;
};

//定义内存池的方法
#define WIND_POOL(pool,itemnum,itemsize) w_uint8_t pool[sizeof(w_pool_s) + itemnum *(WIND_POOL_ALIGN(itemsize) + sizeof(w_pihead_s))+8]

#define NODE_TO_POOL(node) (w_pool_s*)(((char*)(node))-((w_uint32_t)&(((w_pool_s*)0)->obj.objnode)))

w_err_t _wind_pool_mod_init(void);
w_err_t _wind_pool_diagnose_init(void);

w_pool_s *wind_pool_get_by_mem(void *mem);

w_pool_s *wind_pool_get(const char *name);

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t obj_size);

w_err_t wind_pool_destroy(void *mem);

void   *wind_pool_malloc(void *mem);

w_err_t wind_pool_free(void *mem,void *block);

void wind_pool_print_list(void);

void wind_pool_stati_print(void);

#ifdef __cplusplus
}
#endif


#endif

