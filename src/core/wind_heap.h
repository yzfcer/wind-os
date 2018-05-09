/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_heap.h / wind_heap.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统的内存堆的动态分配函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2013.11.03,初步测试通过
** 描　述: 
**
** 备  注：不要在中断服务例程中分配或释放动态内存块
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_HEAP_H__
#define WIND_HEAP_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_stati.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_HEAP_SUPPORT > 0


#define WIND_HEAP_MIN_SIZE    256
#define WIND_HEAP_MINIALLOC    12
#define WIND_HEAP_ALIGN_SIZE 4
#define WIND_HEAP_MAGIC        0xa5e87490
#define WIND_HEAPITEM_MAGIC    0x01ea01ea
#define WIND_HEAP_MASK         0xfffffff0

#define WIND_HEAP_USED         0x01
#define WIND_HEAP_PRIVATE      0x02

#define HEAP_IS_USED(h)   ((h)->magic & WIND_HEAP_USED)
#define HEAP_IS_PRIVATE(h)   ((h)->magic & WIND_HEAP_PRIVATE)

#define __ALIGN_R(size) (((size + 7) >> 3) << 3)
#define __ALIGN_L(size) (((size) >> 3) << 3)

#define WIND_HEAP_SIZE         __ALIGN_R(sizeof(heapitem_s), WIND_HEAP_ALIGN_SIZE)


//内存块的块数的定义，如果系统存在多个不连续的内存区，则应该在下面添加定义
//同时还应该在wind_heap_mutex_init函数中注册新的内存块
typedef enum __HeapBmutex_e
{
    HEAP_BLOCK_1 = 0,//第一块
    HEAP_BLOCK_CNT//块数
}HeapBmutex_e;



typedef struct __heapitem_s heapitem_s;
typedef struct __heap_s heap_s;
struct __heapitem_s
{
    w_uint32_t magic:28;
    w_uint32_t used:1;
    heap_s *heap;
    prinode_s itemnode;
    w_int32_t size;
};


struct __heap_s
{
    w_uint32_t magic;
    const char *name;
    void *addr;
    stati_s stati;
    dnode_s heapnode;
    dlist_s used_list;
    dlist_s free_list;
    void *mutex; 
};

heap_s *wind_heap_get(const char *name);

heap_s *wind_heap_create(const char *name,
             w_addr_t base,w_uint32_t size,w_uint32_t flag);

w_err_t wind_heap_destroy(w_addr_t base);


void *wind_heap_malloc(heap_s* heap, w_uint32_t size);

void *wind_heap_realloc(heap_s* heap, void* ptr, w_uint32_t newsize);

w_err_t wind_heap_free(heap_s* heap,void *ptr);

w_err_t wind_heap_print(void);

w_err_t wind_heapitem_print(dlist_s *list);


void *wind_malloc(w_uint32_t size);

void *wind_calloc(w_uint32_t count, w_uint32_t size);

void *wind_realloc(void *rmem, w_uint32_t newsize);

w_err_t wind_free(void *rmem);



#endif

#ifdef __cplusplus
}
#endif


#endif
