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
#include "wind_lock.h"
#include "dlist.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_HEAP_SUPPORT > 0


#define WIND_HEAP_MINIALLOC    12
#define WIND_HEAP_ALIGN_SIZE 4
#define WIND_HEAP_MAGIC        0xa5e8749c
#define WIND_HEAPITEM_MAGIC    0x1ea01ea0
#define WIND_HEAP_MASK         0xfffffffe
#define WIND_HEAP_USED         0x05

#define HEAP_IS_USED(h)   ((h)->magic & WIND_HEAP_USED)

#define __ALIGN_R(size,N) ((size + N - 1) / N * N)
#define __ALIGN_L(size,N) ((size - N + 1) / N * N)

#define WIND_HEAP_SIZE         __ALIGN_R(sizeof(heapitem_s), WIND_HEAP_ALIGN_SIZE)


//内存块的块数的定义，如果系统存在多个不连续的内存区，则应该在下面添加定义
//同时还应该在wind_heap_block_init函数中注册新的内存块
typedef enum __HeapBlock_e
{
    HEAP_BLOCK_1 = 0,//第一块
    HEAP_BLOCK_CNT//块数
}HeapBlock_e;

#define HEAP1_HEAD  0X68000000
#define HEAD1_LENTH (800*1024)


typedef struct __heapitem_s heapitem_s,*pheapitem_s;
typedef struct __memheap_s heap_s,*pheap_s;
struct __heapitem_s
{
    w_uint32_t magic;
    heap_s *pheap;
    prinode_s itemnode;
    w_int32_t size;
};


struct __memheap_s
{
    w_uint32_t magic;
    const char *name;
    void *addr; 
    w_uint32_t size;
    w_uint32_t rest;
    w_uint32_t max_used;
    dnode_s heap_node;
    dlist_s list;
    dlist_s free_list;
    plock_s plock; 
};

w_err_t wind_heap_init(const char *name,
             void *base,w_uint32_t size);

void wind_heaps_init(void);

void *wind_heap_alloc(pheap_s heap, w_uint32_t size);
void *wind_heap_alloc_default(w_uint32_t size);

void *wind_heap_realloc(pheap_s heap, void* ptr, w_uint32_t newsize);

w_err_t wind_heap_free(void *ptr);

void *wind_hmalloc(w_uint32_t size);

w_err_t wind_hfree(void *rmem);

void *wind_hcalloc(w_uint32_t count, w_uint32_t size);

void wind_heap_showinfo(void);

#endif

#ifdef __cplusplus
}
#endif


#endif
