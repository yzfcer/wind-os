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
#include "wind_lock.h"
#include "wind_debug.h"
#include "wind_string.h"


#if WIND_HEAP_SUPPORT
dlist_s gwind_heaplist = {NULL,NULL};//所有内存块的入口
#define WIND_HEAP_DEBUG(...)
#define OFFSET_ADDR(base,offset) (void*)(((char*)(base))+(offset))
#define ITEM_FROM_PTR(ptr) (void*)((w_uint32_t)ptr - sizeof(heapitem_s))


w_err_t wind_heap_init(const char *name,void *base,w_uint32_t size)
{
    pheapitem_s item;
    pheap_s mhp;
    static plock_s lock = NULL;
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(base != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > sizeof(heap_s),ERR_COMMAN);

    mhp = (pheap_s)base;
    mhp->magic = WIND_HEAP_MAGIC;
    mhp->name = name;
    mhp->addr = OFFSET_ADDR(base,sizeof(heap_s));;
    mhp->size = __ALIGN_L(size, WIND_HEAP_ALIGN_SIZE);
    mhp->rest = mhp->size - sizeof(heap_s);
    mhp->max_used = mhp->size - mhp->rest;
    DNODE_INIT(mhp->heap_node);
    DLIST_INIT(mhp->list);
    DLIST_INIT(mhp->free_list);
    lock = wind_lock_create(name);
    mhp->plock = lock;//wind_lock_create(name);
    
    item = mhp->addr;
    item->magic = WIND_HEAPITEM_MAGIC;
    item->pheap = mhp;
    PRIO_DNODE_INIT(item->itemnode);
    item->size = mhp->rest;
    wind_lock_close(mhp->plock);
    dlist_insert_head(&mhp->free_list,&item->itemnode.node);
    wind_lock_open(mhp->plock);

    return ERR_OK;
}


//堆可自由分配的内存空间进行初始化
void wind_heaps_init(void)
{
    pheap_s pheap;
    WIND_INFO("memory heap initializing...\r\n");
    wind_heap_init("heap0",
                (void *)HEAP1_HEAD,
                HEAD1_LENTH);
    pheap = (pheap_s)HEAP1_HEAD;
    dlist_insert_tail(&gwind_heaplist,&pheap->heap_node);
    wind_heap_showinfo();
}

static void *alloc_from_item(pheap_s heap,pheapitem_s freeitem,w_uint32_t size)
{
    void *p = NULL;
    pheapitem_s item;
    w_uint32_t rest;
    pheap_s mhp = heap;
    rest = item->size - size;
    if(rest > sizeof(heapitem_s))
    {
        freeitem = OFFSET_ADDR(item,sizeof(heapitem_s));
        item->size = size;
    }
    else
    {
        freeitem = NULL;
    }

    item->magic = WIND_HEAPITEM_MAGIC | WIND_HEAP_USED;
    item->pheap = heap;
    p = OFFSET_ADDR(item,sizeof(heapitem_s));
    dlist_insert_prio(&mhp->list,&item->itemnode,(w_uint32_t)item);

    if(freeitem)
    {
        freeitem->magic = WIND_HEAPITEM_MAGIC;
        freeitem->pheap = mhp;
        PRIO_DNODE_INIT(freeitem->itemnode);
        freeitem->size = rest;
        dlist_insert_prio(&mhp->free_list,&freeitem->itemnode,(w_uint32_t)freeitem);
    }
    return p;
}


static w_err_t combine_heapitem(pheapitem_s item1,pheapitem_s item2)
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
void *wind_heap_alloc(pheap_s heap,w_uint32_t size)
{
    void *p = NULL;
    pheap_s mhp = heap;
    pdnode_s pdnode;
    pheapitem_s freeitem;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    WIND_ASSERT_RETURN(size > 0,NULL);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,NULL);
    if (size < WIND_HEAP_MINIALLOC)
        size = WIND_HEAP_MINIALLOC;
    size = __ALIGN_R(size, WIND_HEAP_ALIGN_SIZE) + sizeof(heapitem_s);
    
    WIND_HEAP_DEBUG("allocate %d on heap\r\n",size);
    WIND_HEAP_DEBUG("pool_size:0x%x\r\n",heap->size);
    WIND_HEAP_DEBUG("available_size:0x%x\r\n",heap->rest);

    wind_lock_close(mhp->plock);
    foreach_node(pdnode,&mhp->free_list)
    {
        freeitem = PRI_DLIST_OBJ(pdnode,heapitem_s,itemnode);
        if(size <= freeitem->size)
        {
            dlist_remove(&mhp->free_list,&freeitem->itemnode.node);
            p = alloc_from_item(heap,freeitem,size);
            break;
        }
    }
    wind_lock_open(mhp->plock);
    return p;
}

#if WIND_HEAP_SUPPORT
void *wind_heap_alloc_default(w_uint32_t size)
{
    void *p = NULL;
    pdnode_s pdnode;
    pheap_s pheap;
    foreach_node(pdnode,&gwind_heaplist)
    {
        pheap = DLIST_OBJ(pdnode,heap_s,heap_node);
        p = wind_heap_alloc((pheap_s)pheap,size);
        if(NULL != p)
            return p;
    }
    return NULL;
}
#endif

void *wind_heap_realloc(pheap_s heap, void* ptr, w_uint32_t newsize)
{
    void *p = NULL;
    pheapitem_s old;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    WIND_ASSERT_RETURN(newsize > 0,NULL);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,NULL);
    old = ITEM_FROM_PTR(ptr);
    WIND_ASSERT_RETURN(old->magic == (WIND_HEAPITEM_MAGIC | WIND_HEAP_USED),NULL);
    WIND_ASSERT_RETURN(old->pheap == heap,NULL);
    if (newsize < WIND_HEAP_MINIALLOC)
        newsize = WIND_HEAP_MINIALLOC;
    newsize = __ALIGN_R(newsize, WIND_HEAP_ALIGN_SIZE) + sizeof(heapitem_s);
    wind_lock_close(heap->plock);
    if(old->size > newsize)
    {
        dlist_remove(&heap->list,&old->itemnode.node);
        p = alloc_from_item(heap,old,newsize);
    }
    else
    {
        p = wind_heap_alloc(heap,newsize);
        wind_heap_free(ptr);
    }
    if(p != NULL)
        wind_memcpy(p,ptr,newsize);
    wind_lock_open(heap->plock);
    return p;
}



w_err_t wind_heap_free(void *ptr)
{
    pheap_s heap;
    pheapitem_s item,tmpitem;
    pdnode_s pdnode;
    WIND_ASSERT_RETURN(ptr != NULL,ERR_NULL_POINTER);
    item = ITEM_FROM_PTR(ptr);
    WIND_ASSERT_RETURN(item->magic == (WIND_HEAPITEM_MAGIC | WIND_HEAP_USED),ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item->pheap != NULL,ERR_INVALID_PARAM);
    heap = item->pheap;
    WIND_ASSERT_RETURN(heap != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,ERR_INVALID_PARAM);
    wind_lock_close(heap->plock);
    dlist_remove(&heap->list,&item->itemnode.node);
    item->magic = WIND_HEAPITEM_MAGIC;
    dlist_insert_prio(&heap->free_list,&item->itemnode,(w_uint32_t)item);
    pdnode = dnode_next(&item->itemnode.node);
    tmpitem = PRI_DLIST_OBJ(pdnode,heapitem_s,itemnode);
    if(tmpitem != NULL)
        combine_heapitem(item,tmpitem);
    pdnode = dnode_prev(&item->itemnode.node);
    tmpitem = DLIST_OBJ(pdnode,heapitem_s,itemnode);
    if(tmpitem != NULL)
        combine_heapitem(tmpitem,item);
    wind_lock_open(heap->plock);
    return ERR_OK;
}


void *wind_hmalloc(w_uint32_t size)
{
    void *ptr = NULL;
    pheap_s heap;
    pdnode_s pnode;
    foreach_node(pnode,&gwind_heaplist)
    {
        heap = DLIST_OBJ(pnode,heap_s,heap_node);
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
    pheap_s pheap;
    pdnode_s pnode;
    if (rmem == NULL)
        return wind_hmalloc(newsize);
    foreach_node(pnode,&gwind_heaplist)
    {
        pheap = DLIST_OBJ(pnode,heap_s,heap_node);
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
    tot_size = count * size;
    ptr = wind_hmalloc(tot_size);
    WIND_ASSERT_RETURN(ptr != NULL,NULL);
    wind_memset(ptr, 0, tot_size);
    return ptr;
}


void wind_heap_showinfo(void)
{
    pdnode_s pnode;
    pheap_s heap;
    wind_printf("heap list:\r\n");
    foreach_node(pnode,&gwind_heaplist)
    {
        heap = DLIST_OBJ(pnode,heap_s,heap_node);
        wind_printf("name:%s\r\n",heap->name);
        wind_printf("start:0x%x\r\n",heap->addr);
        wind_printf("size:0x%x\r\n",heap->size);
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

