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
** 描　述: 关于内存堆的分配算法，主要是抑制了RT-thread系统的内存堆管理函数，为了能与wind_os实现兼容，对
**         启动的函数做了一定程度上的修改，包括堆空间的初始化，临界区保护等功能
**
** 备  注：不要在中断服务例程中分配或释放动态内存块
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_HEAP_H__
#define WIND_HEAP_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_lock.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_HEAP_SUPPORT > 0


#define WIND_HEAP_NAME_LEN 12
/* dynamic pool magic and mask */
#define WIND_HEAP_ALIGN_SIZE 4
#define WIND_HEAP_MAGIC        0x1ea01ea0
#define WIND_HEAP_MASK         0xfffffffe
#define WIND_HEAP_USED         0x01
#define wind_heap_FREED        0x00

#define WIND_HEAP_IS_USED(h)   ((h)->magic & WIND_HEAP_USED)
#define WIND_HEAP_MINIALLOC    12

#define WIND_HEAP_ALIGN(size,N) ((size + N -1) / N * N)
#define WIND_HEAP_SIZE         WIND_HEAP_ALIGN(sizeof(heapitem_s), WIND_HEAP_ALIGN_SIZE)
#define MEMITEM_SIZE(item)      ((w_uint32_t)item->next - (w_uint32_t)item - WIND_HEAP_SIZE)


//内存块的块数的定义，如果系统存在多个不连续的内存区，则应该在下面添加定义
//同时还应该在wind_heap_block_init函数中注册新的内存块
typedef enum __HeapBlock_e
{
    HEAP_BLOCK_1 = 0,//第一块
    HEAP_BLOCK_CNT//块数
}HeapBlock_e;

#define HEAP1_HEAD  0X68000000
#define HEAD1_LENTH (800*1024)







//struct __memheap;

typedef struct __heapitem_s
{
    w_uint32_t magic;                      /**< magic number for memheap */
    struct __memheap *pool_ptr;                   /**< point of pool */

    struct __heapitem_s *next;                       /**< next memheap item */
    struct __heapitem_s *prev;                       /**< prev memheap item */

    struct __heapitem_s *next_free;                  /**< next free memheap item */
    struct __heapitem_s *prev_free;                  /**< prev free memheap item */
}heapitem_s,*pheapitem_s;


typedef struct __memheap
{
    w_int8_t name[WIND_HEAP_NAME_LEN];
    void *start_addr;                 /**< pool start address and size */
    w_uint32_t pool_size;                  /**< pool size */
    w_uint32_t available_size;             /**< available size */
    w_uint32_t max_used_size;              /**< maximum allocated size */

    pheapitem_s block_list;                 /**< used block list */
    pheapitem_s free_list;                  /**< free block list */
    heapitem_s free_header;                /**< free block list header */
    plock_s plock;                       /**< semaphore lock */
}memheap_s,*pmemheap_s;

w_err_t wind_heap_init(pmemheap_s mhp,
                         const char *name,
                         void *start_addr,
                         w_uint32_t size);

void wind_heap_block_init(void);

void *wind_heap_alloc(pmemheap_s heap, w_uint32_t size);
void *wind_heap_alloc_default(w_uint32_t size);

void *wind_heap_realloc(pmemheap_s heap, void* ptr, w_uint32_t newsize);

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
