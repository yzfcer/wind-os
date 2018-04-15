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
** 描　述: 关于内存堆的分配算法
**
** 备  注：不要在中断服务例程中分配或释放动态内存块
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_heap.h"
#include "wind_mutex.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_var.h"
#include "wind_core.h"

#if WIND_HEAP_SUPPORT

#define WIND_HEAP_DEBUG(...)
#define OFFSET_ADDR(base,offset) (void*)(((char*)(base))+(offset))
#define ITEM_FROM_PTR(ptr) (void*)((w_uint32_t)ptr - sizeof(heapitem_s))

static void heapitem_init(heapitem_s *item,heap_s *hp,w_int32_t size)
{
    item->magic = WIND_HEAPITEM_MAGIC;
    item->pheap = hp;
    PRIO_DNODE_INIT(item->itemnode);
    item->size = size;
}

w_err_t wind_heap_create(const char *name,w_addr_t base,w_uint32_t size)
{
    heapitem_s* item;
    heap_s* hp;
    w_uint32_t hpsize;
    
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > sizeof(heap_s),ERR_COMMAN);
    
    wind_notice("create hp:%s,base:0x%X,lenth:%d",name,base,size);
    hp = (heap_s*)(__ALIGN_R((w_addr_t)base,8));
    hpsize = size;
    hpsize += (w_uint32_t)(((w_addr_t)hp) - base);
    hpsize = __ALIGN_L(hpsize,4);
    
    hp->magic = WIND_HEAP_MAGIC;
    hp->name = name;
    hp->addr = OFFSET_ADDR(hp,sizeof(heap_s));;
    hp->size = hpsize;
    hp->rest = hp->size - sizeof(heap_s);
    hp->max_used = sizeof(heap_s);
    DNODE_INIT(hp->heapnode);
    DLIST_INIT(hp->used_list);
    DLIST_INIT(hp->free_list);
    hp->pmutex = wind_mutex_create(name);
    WIND_ASSERT_RETURN(hp->pmutex != NULL,ERR_MEM);
    item = hp->addr;
    heapitem_init(item,hp,hp->rest);
    wind_disable_switch();
    dlist_insert_head(&hp->free_list,&item->itemnode.node);
    dlist_insert_tail(&g_core.heaplist,&hp->heapnode);
    wind_enable_switch();

    return ERR_OK;
}

w_err_t wind_heap_destroy(w_addr_t base)
{
    heap_s *heap;
    heap = (heap_s*)(__ALIGN_R((w_addr_t)base,8));
    WIND_ASSERT_RETURN(heap != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(heap->size > sizeof(heap_s),ERR_INVALID_PARAM);
    wind_disable_switch();
    dlist_remove(&g_core.heaplist,&heap->heapnode);
    wind_enable_switch();
    wind_mutex_destroy(heap->pmutex);
    wind_memset(heap,0,sizeof(heap_s));
    return ERR_OK;
}



static void *alloc_from_item(heap_s* heap,heapitem_s* freeitem,w_uint32_t size)
{
    void *p = NULL;
    heapitem_s* item = freeitem;
    w_uint32_t rest;
    heap_s* hp = heap;
    item = freeitem;
    rest = freeitem->size - size;
    if(rest > sizeof(heapitem_s))
    {
        freeitem = OFFSET_ADDR(item,size);
        item->size = size;
    }
    else
    {
        freeitem = NULL;
        item->size = freeitem->size;
    }

    item->magic = WIND_HEAPITEM_MAGIC | WIND_HEAP_USED;
    item->pheap = heap;
    p = OFFSET_ADDR(item,sizeof(heapitem_s));
    dlist_insert_prio(&hp->used_list,&item->itemnode,(w_uint32_t)&item);

    if(freeitem)
    {
        heapitem_init(freeitem,heap,rest);
        dlist_insert_prio(&hp->free_list,&freeitem->itemnode,(w_uint32_t)freeitem);
    }
    return p;
}


static w_err_t combine_heapitem(heapitem_s* item1,heapitem_s* item2)
{
    WIND_ASSERT_RETURN(item1 != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(item2 != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(item1->pheap == item2->pheap,ERR_INVALID_PARAM);
    
    if(item1->size + (w_uint32_t)item1 == (w_uint32_t)item2)
    {
        dlist_remove(&item1->pheap->free_list,&item2->itemnode.node);
        item1->size += item2->size;
        item2->magic = 0;
        item2->pheap = NULL;
        PRIO_DNODE_INIT(item2->itemnode);
    }
    return ERR_OK;
}

//从内存堆中分出一块空间
void *wind_heap_alloc(heap_s* heap,w_uint32_t size)
{
    void *p = NULL;
    heap_s* hp = heap;
    dnode_s *pdnode;
    heapitem_s* freeitem;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    WIND_ASSERT_RETURN(size > 0,NULL);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,NULL);
    if (size < WIND_HEAP_MINIALLOC)
        size = WIND_HEAP_MINIALLOC;
    size = __ALIGN_R(size, WIND_HEAP_ALIGN_SIZE) + sizeof(heapitem_s);
    
    WIND_HEAP_DEBUG("allocate %d on heap\r\n",size);
    WIND_HEAP_DEBUG("pool_size:0x%x\r\n",heap->size);
    WIND_HEAP_DEBUG("available_size:0x%x\r\n",heap->rest);

    wind_mutex_lock(hp->pmutex);
    foreach_node(pdnode,&hp->free_list)
    {
        freeitem = PRI_DLIST_OBJ(pdnode,heapitem_s,itemnode);
        if(size <= freeitem->size)
        {
            dlist_remove(&hp->free_list,&freeitem->itemnode.node);
            p = alloc_from_item(heap,freeitem,size);
            break;
        }
    }
    wind_mutex_unlock(hp->pmutex);
    return p;
}

void *wind_heap_alloc_default(w_uint32_t size)
{
    void *p = NULL;
    dnode_s *pdnode;
    heap_s* pheap;
    foreach_node(pdnode,&g_core.heaplist)
    {
        pheap = DLIST_OBJ(pdnode,heap_s,heapnode);
        p = wind_heap_alloc((heap_s*)pheap,size);
        if(NULL != p)
            return p;
    }
    return NULL;
}

void *wind_heap_realloc(heap_s* heap, void* ptr, w_uint32_t newsize)
{
    void *p = NULL;
    heapitem_s* old;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    WIND_ASSERT_RETURN(newsize > 0,NULL);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,NULL);
    old = ITEM_FROM_PTR(ptr);
    WIND_ASSERT_RETURN(old->magic == (WIND_HEAPITEM_MAGIC | WIND_HEAP_USED),NULL);
    WIND_ASSERT_RETURN(old->pheap == heap,NULL);
    if (newsize < WIND_HEAP_MINIALLOC)
        newsize = WIND_HEAP_MINIALLOC;
    newsize = __ALIGN_R(newsize, WIND_HEAP_ALIGN_SIZE) + sizeof(heapitem_s);
    wind_mutex_lock(heap->pmutex);
    if(old->size > newsize)
    {
        dlist_remove(&heap->used_list,&old->itemnode.node);
        p = alloc_from_item(heap,old,newsize);
    }
    else
    {
        p = wind_heap_alloc(heap,newsize);
        wind_heap_free(ptr);
    }
    if(p != NULL)
        wind_memcpy(p,ptr,newsize);
    wind_mutex_unlock(heap->pmutex);
    return p;
}



w_err_t wind_heap_free(void *ptr)
{
    heap_s* heap;
    heapitem_s* item,*tmpitem;
    dnode_s *pdnode;
    WIND_ASSERT_RETURN(ptr != NULL,ERR_NULL_POINTER);
    item = ITEM_FROM_PTR(ptr);
    WIND_ASSERT_RETURN(item->magic == (WIND_HEAPITEM_MAGIC | WIND_HEAP_USED),ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item->pheap != NULL,ERR_INVALID_PARAM);
    heap = item->pheap;
    WIND_ASSERT_RETURN(heap != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,ERR_INVALID_PARAM);
    wind_mutex_lock(heap->pmutex);
    dlist_remove(&heap->used_list,&item->itemnode.node);
    item->magic = WIND_HEAPITEM_MAGIC;
    dlist_insert_prio(&heap->free_list,&item->itemnode,(w_uint32_t)item);
    pdnode = dnode_next(&item->itemnode.node);
    if(pdnode != NULL)
    {
        tmpitem = PRI_DLIST_OBJ(pdnode,heapitem_s,itemnode);
        if(tmpitem != NULL)
            combine_heapitem(item,tmpitem);
    }
    pdnode = dnode_prev(&item->itemnode.node);
    if(pdnode != NULL)
    {
        tmpitem = DLIST_OBJ(pdnode,heapitem_s,itemnode);
        if(tmpitem != NULL)
            combine_heapitem(tmpitem,item);
    }
    wind_mutex_unlock(heap->pmutex);
    return ERR_OK;
}


void *wind_hmalloc(w_uint32_t size)
{
    void *ptr = NULL;
    heap_s* heap;
    dnode_s *pnode;
    foreach_node(pnode,&g_core.heaplist)
    {
        heap = DLIST_OBJ(pnode,heap_s,heapnode);
        WIND_HEAP_DEBUG("malloc in heap:0x%x\r\n",heap);
        WIND_HEAP_DEBUG("pool_size:0x%x\r\n",heap->size);
        WIND_HEAP_DEBUG("available_size:0x%x\r\n",heap->rest);
        ptr = wind_heap_alloc(heap, size);
        if(ptr)
        {
            break;
        }
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
    void *pnew;
    heap_s* pheap;
    dnode_s *pnode;
    if (rmem == NULL)
        return wind_hmalloc(newsize);
    foreach_node(pnode,&g_core.heaplist)
    {
        pheap = DLIST_OBJ(pnode,heap_s,heapnode);
        pnew = wind_heap_realloc(pheap,rmem,newsize);
        return pnew;
    }
    return NULL;
}


void *wind_hcalloc(w_uint32_t count, w_uint32_t size)
{
    void *ptr;
    w_uint32_t tot_size;
    WIND_ASSERT_RETURN(count > 0,NULL);
    WIND_ASSERT_RETURN(size > 0,NULL);
    tot_size = count *size;
    ptr = wind_hmalloc(tot_size);
    WIND_ASSERT_RETURN(ptr != NULL,NULL);
    wind_memset(ptr, 0, tot_size);
    return ptr;
}


w_err_t wind_heap_print(void)
{
    dnode_s *pnode;
    heap_s* heap;
    wind_printf("heap list:\r\n");
    foreach_node(pnode,&g_core.heaplist)
    {
        heap = DLIST_OBJ(pnode,heap_s,heapnode);
        wind_printf("magic :%x\r\n",heap->magic);
        wind_printf("name  :%s\r\n",heap->name);
        wind_printf("addr  :%x\r\n",heap->addr);
        wind_printf("size  :%d\r\n",heap->size);
        wind_printf("rest  :%d\r\n",heap->rest);
        wind_printf("used  :%d\r\n",heap->max_used);
    }
    return ERR_OK;
}

w_err_t wind_heapitem_print(dlist_s *list)
{   
    dnode_s *dnode,*dnode1;
    heap_s *heap;
    heapitem_s *heapitem;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nheapitem list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-8s %-10s\r\n","addr","size","state");
    wind_printf("----------------------------------------------\r\n");

    foreach_node(dnode,list)
    {
        heap = (heap_s*)DLIST_OBJ(dnode,heap_s,heapnode);
        foreach_node(dnode1,&heap->free_list)
        {
            heapitem = (heapitem_s *)DLIST_OBJ(dnode1,heapitem_s,itemnode.node);
            wind_printf("0x%-14x %-8d %-10s\r\n",
                heapitem,heapitem->size,(heapitem->magic & WIND_HEAP_USED)?"uesd":"free");
        }
        foreach_node(dnode1,&heap->used_list)
        {
            heapitem = (heapitem_s *)DLIST_OBJ(dnode1,heapitem_s,itemnode.node);
            wind_printf("0x%-14x %-8d %-10s\r\n",
                heapitem,heapitem->size,(heapitem->magic & WIND_HEAP_USED)?"uesd":"free");
        }
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}

#endif

