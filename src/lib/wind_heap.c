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
** 描　述: 关于内存堆的分配算法，主要是移植了RT-thread系统的内存堆管理函数，为了能与wind_os实现兼容，对
**         其中的函数做了一定程度上的修改，包括堆空间的初始化，临界区保护等功能
**
** 备  注：不要在中断服务例程中分配或释放动态内存块
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*
 * File      : mhp.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-04-10     Bernard      first implementation
 * 2012-10-16     Bernard      add the mutex lock for heap object.
 * 2012-12-29     Bernard      mhp can be used as system heap.
 *                             change mutex lock to semaphore lock.
 * 2013-04-10     Bernard      add wind_heap_realloc function.
 * 2013-05-24     Bernard      fix the wind_heap_realloc issue.
 * 2013-10-25     andy zhou    修改程序以适应wind_os系统.
 */

//#include <rthw.h>
//#include <rtthread.h>

//#ifdef RT_USING_MEMHEAP



#include "wind_config.h"
#include "wind_types.h"
#include "wind_heap.h"
#include "wind_lock.h"
#include "WIND_ERR.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_list.h"
#include "wind_assert.h"

#if WIND_HEAP_SUPPORT > 0
list_s gwind_heaplist = {NULL,NULL,0};//所有内存块的入口
//内存堆的空间块的保存的位置
memheap_s gwind_heap[HEAP_BLOCK_CNT];
#define WIND_HEAP_DEBUG(...)
//#define WIND_HEAP_DEBUG(...) wind_printf(__VA_ARGS__) 
/*
 * The initialized memory pool will be:
 * +-----------------------------------+--------------------------+
 * | whole freed memory block          | Used Memory Block Tailer |
 * +-----------------------------------+--------------------------+
 *
 * block_list --> whole freed memory block
 *
 * The length of Used Memory Block Tailer is 0,
 * which is prevents block merging across list
 */
 
static w_err_t wind_heap_reg(pmemheap_s mhp)
{

    pnode_s pnode;
    WIND_ASSERT_RETURN(mhp != NULL,ERR_NULL_POINTER);
    pnode = wind_node_malloc(CORE_TYPE_HEAP);
    WIND_ASSERT_RETURN(pnode != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    wind_node_bindobj(pnode,CORE_TYPE_HEAP,0,mhp);
    wind_list_inserttoend(&gwind_heaplist,pnode);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_heap_init(pmemheap_s mhp,
                         const char *name,
                         void *start_addr,
                         w_uint32_t size)
{
    w_err_t err;
    pheapitem_s item;
    WIND_ASSERT_RETURN(mhp != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(start_addr != NULL,ERR_NULL_POINTER);
    err = wind_heap_reg(mhp);
    WIND_ASSERT_RETURN(err == ERR_OK,ERR_COMMAN);
    wind_memset(mhp->name,0,WIND_HEAP_NAME_LEN);
    wind_memcpy(mhp->name,name,WIND_HEAP_NAME_LEN - 1);
    mhp->start_addr = start_addr;
    mhp->pool_size = WIND_HEAP_ALIGN(size, WIND_HEAP_ALIGN_SIZE);//
    mhp->available_size = mhp->pool_size - (2 * WIND_HEAP_SIZE);
    mhp->max_used_size = mhp->pool_size - mhp->available_size;

    item = &(mhp->free_header);
    item->magic = WIND_HEAP_MAGIC;
    item->pool_ptr = mhp;
    item->next = NULL;
    item->prev = NULL;
    item->next_free = item;
    item->prev_free = item;

    mhp->free_list = item;

    item = (pheapitem_s )start_addr;
    item->magic = WIND_HEAP_MAGIC;
    item->pool_ptr = mhp;
    item->next = NULL;
    item->prev = NULL;
    item->next_free = item;
    item->prev_free = item;

    item->next = (pheapitem_s)((w_uint8_t *)item + mhp->available_size + WIND_HEAP_SIZE);
    item->prev = item->next;

    mhp->block_list = item;


    item->next_free = mhp->free_list->next_free;
    item->prev_free = mhp->free_list;
    mhp->free_list->next_free->prev_free = item;
    mhp->free_list->next_free  = item;


    item = item->next;

    item->magic = WIND_HEAP_MAGIC | WIND_HEAP_USED;
    item->pool_ptr = mhp;
    item->next = (pheapitem_s )start_addr;
    item->prev = (pheapitem_s )start_addr;

    item->next_free = item->prev_free = NULL;


    //rt_sem_init(&(mhp->lock), name, 1, RT_IPC_FLAG_FIFO);
    mhp->plock = wind_lock_create(name);
    //WIND_HEAP_DEBUG("memory heap: \r\nstart addr 0x%x\r\nsize 0x%x\r\nfree list header 0x%x\r\n",
    //              start_addr, size, &(mhp->free_header));
    //WIND_HEAP_DEBUG("pool_size:0x%x\r\n",mhp->pool_size);
    //WIND_HEAP_DEBUG("available_size:0x%x\r\n",mhp->available_size);
    //WIND_HEAP_DEBUG("gwind_heaplist->obj:0x%x,0x%x\r\n",gwind_heaplist->obj,mhp);
    return ERR_OK;
}



//堆可自由分配的内存空间进行初始化
void wind_heap_block_init(void)
{
    WIND_INFO("memory heap initializing...\r\n");
    wind_heap_init(&gwind_heap[0],
                    "heap0",
                    (void *)HEAP1_HEAD,
                    HEAD1_LENTH);
    wind_heap_showinfo();
}

void *wind_heap_alloc_default(w_uint32_t size)
{
    int i;
    void *p;
    for(i = 0;i < HEAP_BLOCK_CNT;i ++)
    {
        p = wind_heap_alloc(&gwind_heap[0],size);
        if(NULL != p)
            return p;
    }
    return p;
}

//从内存堆中分出一块空间
void *wind_heap_alloc(pmemheap_s heap, w_uint32_t size)
{
    w_err_t result;
    w_uint32_t free_size;
    pheapitem_s header_ptr;
    pheapitem_s new_ptr;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    size = WIND_HEAP_ALIGN(size, WIND_HEAP_ALIGN_SIZE);
    if (size < WIND_HEAP_MINIALLOC)
        size = WIND_HEAP_MINIALLOC;
    
    WIND_HEAP_DEBUG("allocate %d on heap\r\n",size);
    WIND_HEAP_DEBUG("pool_size:0x%x\r\n",heap->pool_size);
    WIND_HEAP_DEBUG("available_size:0x%x\r\n",heap->available_size);
    
    //WIND_HEAP_DEBUG("free size:0x%x\r\n",heap->available_size);
    //WIND_HEAP_DEBUG("free size:0x%x\r\n",heap->available_size);
    if (size < heap->available_size)
    {

        free_size = 0;
        

        //result = rt_sem_take(&(heap->lock), RT_WAITING_FOREVER);
        result = wind_lock_close(heap->plock);
        if (result != ERR_OK)
        {
            WIND_HEAP_DEBUG("lock close err\r\n");
            return NULL;
        }
        //WIND_HEAP_DEBUG("allc1\r\n");

        header_ptr = heap->free_list->next_free;
        
        while (header_ptr != heap->free_list && free_size < size)
        {
            /* get current freed memory block size */
            free_size = MEMITEM_SIZE(header_ptr);
            if (free_size < size)
            {
                /* move to next free memory block */
                header_ptr = header_ptr->next_free;
            }
            //WIND_HEAP_DEBUG("loop1\r\n");
        }
        //WIND_HEAP_DEBUG("allc2\r\n");


        if (free_size >= size)
        {

            if (free_size >= (size + WIND_HEAP_SIZE + WIND_HEAP_MINIALLOC))
            {

                new_ptr = (pheapitem_s)
                          (((w_uint8_t *)header_ptr) + size + WIND_HEAP_SIZE);

                WIND_HEAP_DEBUG("split: block[0x%x] nextm[0x%x] prevm[0x%x] to new[0x%x]\r\n",
                              header_ptr,
                              header_ptr->next,
                              header_ptr->prev,
                              new_ptr);


                new_ptr->magic = WIND_HEAP_MAGIC;


                new_ptr->pool_ptr = heap;


                new_ptr->prev          = header_ptr;
                new_ptr->next          = header_ptr->next;
                header_ptr->next->prev = new_ptr;
                header_ptr->next       = new_ptr;


                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;


                new_ptr->next_free = heap->free_list->next_free;
                new_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = new_ptr;
                heap->free_list->next_free            = new_ptr;
                /*RT_DEBUG_LOG(RT_DEBUG_MEMHEAP, ("new ptr: next_free 0x%08x, prev_free 0x%08x",
                                                new_ptr->next_free,
                                                new_ptr->prev_free));*/


                heap->available_size = heap->available_size -
                                       size -
                                       WIND_HEAP_SIZE;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;
                //WIND_HEAP_DEBUG("allc3\r\n");
            }
            else
            {

                heap->available_size = heap->available_size - free_size;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;



                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;
                //WIND_HEAP_DEBUG("allc4\r\n");
            }


            header_ptr->magic |= WIND_HEAP_USED;


            //rt_sem_release(&(heap->lock));
            wind_lock_open(heap->plock);
            //WIND_HEAP_DEBUG("allc5\r\n");


            WIND_HEAP_DEBUG("alloc mem: memory[0x%08x], heap[0x%08x], size: %d\r\n",
                          (void *)((w_uint8_t *)header_ptr + WIND_HEAP_SIZE),
                          header_ptr,
                          size);


            return (void *)((w_uint8_t *)header_ptr + WIND_HEAP_SIZE);
        }
        wind_lock_open(heap->plock);
    }
    return NULL;
}


void *wind_heap_realloc(pmemheap_s heap, void* ptr, w_uint32_t newsize)
{
    w_err_t result;
    w_uint32_t oldsize;
    pheapitem_s header_ptr;
    pheapitem_s new_ptr;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    if (newsize == 0)
    {
        wind_heap_free(ptr);
        return NULL;
    }

    newsize = WIND_HEAP_ALIGN(newsize, WIND_HEAP_ALIGN_SIZE);
    if (newsize < WIND_HEAP_MINIALLOC)
        newsize = WIND_HEAP_MINIALLOC;

    if (ptr == NULL)
    {
        return wind_heap_alloc(heap, newsize);
    }


    header_ptr = (heapitem_s*)((w_uint8_t *)ptr - WIND_HEAP_SIZE);
    oldsize = MEMITEM_SIZE(header_ptr);

    if (newsize > oldsize)
    {
        void* new_ptr;

        new_ptr = (void*)wind_heap_alloc(heap, newsize);
        if (new_ptr != NULL)
        {
            wind_memcpy(new_ptr, ptr, oldsize < newsize ? oldsize : newsize);
            wind_heap_free(ptr);
        }

        return new_ptr;
    }


    result = wind_lock_close(heap->plock);
    if (result != ERR_OK)
    {
        return NULL;
    }


    new_ptr = (pheapitem_s)(((w_uint8_t *)header_ptr) + newsize + WIND_HEAP_SIZE);




    new_ptr->magic = WIND_HEAP_MAGIC;

    new_ptr->pool_ptr = heap;


    new_ptr->prev           = header_ptr;
    new_ptr->next           = header_ptr->next;
    header_ptr->next->prev = new_ptr;
    header_ptr->next       = new_ptr;


    if (!WIND_HEAP_IS_USED(new_ptr->next))
    {
        pheapitem_s free_ptr;


        free_ptr = new_ptr->next;
        heap->available_size = heap->available_size - MEMITEM_SIZE(free_ptr);



        free_ptr->next->prev = new_ptr;
        new_ptr->next    = free_ptr->next;


        free_ptr->next_free->prev_free = free_ptr->prev_free;
        free_ptr->prev_free->next_free = free_ptr->next_free;
    }


    new_ptr->next_free = heap->free_list->next_free;
    new_ptr->prev_free = heap->free_list;
    heap->free_list->next_free->prev_free = new_ptr;
    heap->free_list->next_free              = new_ptr;

    heap->available_size = heap->available_size + MEMITEM_SIZE(new_ptr);


    wind_lock_open(heap->plock);
    return ptr;
}


w_err_t wind_heap_free(void *ptr)
{
    w_err_t result;
    pmemheap_s heap;
    pheapitem_s header_ptr, new_ptr;
    w_uint32_t insert_header;
    WIND_ASSERT_RETURN(ptr != NULL,ERR_NULL_POINTER);


    insert_header = 1;
    new_ptr       = NULL;
    header_ptr    = (pheapitem_s )((w_uint8_t *)ptr - WIND_HEAP_SIZE);


    heap = header_ptr->pool_ptr;


    result = wind_lock_close(heap->plock);
    if (result != ERR_OK)
    {

        return result;
    }


    header_ptr->magic &= ~WIND_HEAP_USED;

    heap->available_size = heap->available_size + MEMITEM_SIZE(header_ptr);


    if (!WIND_HEAP_IS_USED(header_ptr->prev))
    {

        heap->available_size = heap->available_size + WIND_HEAP_SIZE;


        (header_ptr->prev)->next = header_ptr->next;
        (header_ptr->next)->prev = header_ptr->prev;


        header_ptr = header_ptr->prev;

        insert_header = 0;
    }


    if (!WIND_HEAP_IS_USED(header_ptr->next))
    {

        heap->available_size = heap->available_size + WIND_HEAP_SIZE;


        new_ptr = header_ptr->next;



        new_ptr->next->prev = header_ptr;
        header_ptr->next    = new_ptr->next;


        new_ptr->next_free->prev_free = new_ptr->prev_free;
        new_ptr->prev_free->next_free = new_ptr->next_free;
    }

    if (insert_header)
    {

        header_ptr->next_free = heap->free_list->next_free;
        header_ptr->prev_free = heap->free_list;
        heap->free_list->next_free->prev_free = header_ptr;
        heap->free_list->next_free            = header_ptr;


    }


    result = wind_lock_open(heap->plock);
    return result;
}


void *wind_hmalloc(w_uint32_t size)
{
    void *ptr = NULL;
    pmemheap_s heap;
    plist_s list = &gwind_heaplist;
    pnode_s pnode = list->head;
    if(!pnode)
    {
        WIND_HEAP_DEBUG("NULL pnode in heap\r\n");
        return NULL;
    }
    while(pnode)
    {
        heap = (pmemheap_s)pnode->obj;
        
        WIND_HEAP_DEBUG("malloc in heap:0x%x\r\n",heap);
        WIND_HEAP_DEBUG("pool_size:0x%x\r\n",heap->pool_size);
        WIND_HEAP_DEBUG("available_size:0x%x\r\n",heap->available_size);
        ptr = wind_heap_alloc(heap, size);
        if(ptr)
        {
            break;
        }
        pnode= pnode->next;
        
    }
    if(!ptr)
    {
        WIND_HEAP_DEBUG("NULL ptr in heap\r\n");
    }
    return ptr;
}


w_err_t wind_hfree(void *rmem)
{
    WIND_ASSERT_RETURN(rmem != NULL,ERR_NULL_POINTER);
    return wind_heap_free(rmem);
}


void *wind_hrealloc(void *rmem, w_uint32_t newsize)
{
    void *new_ptr;
    pheapitem_s header_ptr;
    w_uint32_t oldsize;
    if (rmem == NULL) 
        return wind_hmalloc(newsize);


    header_ptr = (pheapitem_s )((w_uint8_t *)rmem - WIND_HEAP_SIZE);

    new_ptr = wind_heap_realloc(header_ptr->pool_ptr, rmem, newsize);
    if (new_ptr == NULL && newsize != 0)
    {

        new_ptr = wind_hmalloc(newsize);
        if (new_ptr != NULL && rmem != NULL)
        {

            oldsize = MEMITEM_SIZE(header_ptr);
            if (newsize > oldsize) wind_memcpy(new_ptr, rmem, oldsize);
            else wind_memcpy(new_ptr, rmem, newsize);
        }
    }

    return new_ptr;
}


void *wind_hcalloc(w_uint32_t count, w_uint32_t size)
{
    void *ptr;
    w_uint32_t total_size;
    WIND_ASSERT_RETURN(count > 0,NULL);
    WIND_ASSERT_RETURN(size > 0,NULL);
    total_size = count * size;
    ptr = wind_hmalloc(total_size);
    WIND_ASSERT_RETURN(ptr != NULL,NULL);
    wind_memset(ptr, 0, total_size);
    return ptr;
}


void wind_heap_showinfo(void)
{
    pnode_s pnode;
    pmemheap_s heap;
    wind_printf("heap list:\r\n");
    pnode = gwind_heaplist.head;
    while(pnode)
    {
        heap = (pmemheap_s)pnode->obj;
        wind_printf("name:%s\r\n",heap->name);
        wind_printf("start:0x%x\r\n",heap->start_addr);
        wind_printf("size:0x%x\r\n",heap->pool_size);
        //wind_printf("available_size:0x%x\r\n\r\n",heap->available_size);
        pnode = pnode->next;
    }
}

w_err_t wind_heap_test(void)
{
    w_uint8_t *p1,*p2,*p3;
    p1 = wind_hmalloc(120);
    if(p1)
    {
        wind_printf("p1:0x%x,size:120\r\n",p1);
    }
    else
    {
        WIND_ERROR("p1:malloc err\r\n");
    }
    p2 = wind_hmalloc(1024);
    if(p2)
    {
        wind_printf("p2:0x%x,size:1024\r\n",p2);
    }
    else
    {
        WIND_ERROR("p2:malloc err\r\n");
    }
    p3 = wind_hmalloc(0x1000000);
    if(p3)
    {
        wind_printf("p3:0x%x,size:0x10000\r\n",p3);
    }
    else
    {
        WIND_ERROR("p3:malloc err\r\n");
    }
    wind_printf("memory free:p1\r\n");
    wind_hfree(p1);
    wind_printf("memory free:p2\r\n");
    wind_hfree(p2);
    wind_printf("memory free:p3\r\n");
    wind_hfree(p3);
    return ERR_OK;
    
}
#endif

