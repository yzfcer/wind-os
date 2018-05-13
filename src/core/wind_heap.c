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
#include "wind_heap.h"
#include "wind_mutex.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_var.h"
#include "wind_core.h"

#if WIND_HEAP_SUPPORT

#define OFFSET_ADDR(base,offset) (void*)(((char*)(base))+(offset))
#define ITEM_FROM_PTR(ptr) (void*)((w_uint32_t)ptr - sizeof(heapitem_s))
#if 0
static void printf_list_addr(dlist_s *list)
{
    dnode_s *node;
    foreach_node(node,list)
    {
        wind_printf("0x%x,",node);
    }
}
#endif

static void heapitem_init(heapitem_s *item,heap_s *hp,w_int32_t size,w_uint32_t used)
{
    item->magic = WIND_HEAPITEM_MAGIC;
    item->used = used;
    item->heap = hp;
    PRIO_DNODE_INIT(item->itemnode);
    item->size = size;
}

heap_s *wind_heap_get(const char *name)
{
    heap_s *heap;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.heaplist)
    {
        heap = DLIST_OBJ(dnode,heap_s,heapnode);
        if(wind_strcmp(name,heap->name) == 0)
        {
            wind_enable_switch();
            return heap;
        }
    }
    wind_enable_switch();
    return NULL;
}


heap_s *wind_heap_create(const char *name,w_addr_t base,w_uint32_t size,w_uint32_t is_private)
{
    heapitem_s* item;
    heap_s* hp;
    w_uint32_t hpsize;
    
    WIND_ASSERT_RETURN(name != NULL,NULL);
    WIND_ASSERT_RETURN(size > WIND_HEAP_MIN_SIZE,NULL);
    
    wind_notice("create hp:%s,base:0x%X,lenth:%d",name,base,size);
    hp = (heap_s*)(__ALIGN_R((w_addr_t)base));
    hpsize = size;
    hpsize += (w_uint32_t)(((w_addr_t)hp) - base);
    hpsize = __ALIGN_L(hpsize);
    
    hp->magic = WIND_HEAP_MAGIC;
    hp->is_private = is_private?1:0;
    //if(flag & WIND_HEAP_PRIVATE)
    //    hp->magic |= WIND_HEAP_PRIVATE;
    hp->name = name;
    hp->addr = OFFSET_ADDR(hp,sizeof(heap_s));
    WIND_STATI_INIT(hp->stati,hpsize);
    WIND_STATI_ADD(hp->stati,sizeof(heap_s));
    DNODE_INIT(hp->heapnode);
    DLIST_INIT(hp->used_list);
    DLIST_INIT(hp->free_list);
    hp->mutex = wind_mutex_create(name);
    WIND_ASSERT_RETURN(hp->mutex != NULL,NULL);
    item = hp->addr;
    heapitem_init(item,hp,hpsize - sizeof(heap_s),0);
    wind_disable_switch();
    dlist_insert_head(&hp->free_list,&item->itemnode.node);
    dlist_insert_tail(&g_core.heaplist,&hp->heapnode);
    wind_enable_switch();

    return hp;
}

w_err_t wind_heap_destroy(w_addr_t base)
{
    heap_s *heap;
    heap = (heap_s*)(__ALIGN_R((w_addr_t)base));
    WIND_ASSERT_RETURN(heap != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,ERR_INVALID_PARAM);
    wind_disable_switch();
    dlist_remove(&g_core.heaplist,&heap->heapnode);
    heap->magic = 0;
    wind_enable_switch();
    wind_mutex_destroy(heap->mutex);
    heap->mutex = NULL;
    return ERR_OK;
}



static void *alloc_from_freeitem(heap_s* heap,heapitem_s* freeitem,w_uint32_t size)
{
    void *p = NULL;
    heapitem_s* item = freeitem;
    w_uint32_t rest;
    heap_s* hp = heap;
    WIND_ASSERT_RETURN(freeitem->size >= size,NULL);
    item = freeitem;
    rest = freeitem->size - size;
    if(rest > sizeof(heapitem_s))
    {
        item->size = size;
        freeitem = OFFSET_ADDR(item,size);
        freeitem->size = rest;
    }
    else
    {
        item->size = freeitem->size;
        freeitem = NULL;
    }
    WIND_STATI_ADD(hp->stati,item->size);
    heapitem_init(item,heap,item->size,1);
    p = OFFSET_ADDR(item,sizeof(heapitem_s));
    dlist_insert_prio(&hp->used_list,&item->itemnode,(w_uint32_t)&item);

    if(freeitem)
    {
        heapitem_init(freeitem,heap,rest,0);
        dlist_insert_prio(&hp->free_list,&freeitem->itemnode,(w_uint32_t)freeitem);
    }
    return p;
}


static w_err_t combine_heapitem(heapitem_s* item1,heapitem_s* item2)
{
    WIND_ASSERT_RETURN(item1 != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(item2 != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(item1->heap == item2->heap,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item1->magic == WIND_HEAPITEM_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item2->magic == WIND_HEAPITEM_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item1->used == 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item2->used == 0,ERR_INVALID_PARAM);
    if(item1->size + (w_uint32_t)item1 == (w_uint32_t)item2)
    {
        //wind_heapitem_print(&g_core.heaplist);
        //wind_debug("combine_heapitem 0x%0x,0x%0x,0x%x",item1,item1->size,item2);
        dlist_remove(&item1->heap->free_list,&item2->itemnode.node);
        item1->size += item2->size;
        item2->magic = 0;
        item2->heap = NULL;
        PRIO_DNODE_INIT(item2->itemnode);
        //wind_heapitem_print(&g_core.heaplist);
    }
    return ERR_OK;
}

//从内存堆中分出一块空间
void *wind_heap_malloc(heap_s* heap,w_uint32_t size)
{
    void *p = NULL;
    heap_s* hp = heap;
    dnode_s *pdnode;
    heapitem_s* freeitem;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    WIND_ASSERT_RETURN(size > 0,NULL);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,NULL);
    //wind_debug("heap_malloc %d",size);
    //if (size < WIND_HEAP_MINIALLOC)
    //    size = WIND_HEAP_MINIALLOC;
    size = __ALIGN_R(size) + sizeof(heapitem_s);
    
    wind_mutex_lock(hp->mutex);
    foreach_node(pdnode,&hp->free_list)
    {
        freeitem = PRI_DLIST_OBJ(pdnode,heapitem_s,itemnode);
        if(freeitem->size > 0xffff)
            freeitem->size = freeitem->size;
        if(size <= freeitem->size)
        {
            dlist_remove(&hp->free_list,&freeitem->itemnode.node);
            p = alloc_from_freeitem(heap,freeitem,size);
            break;
        }
    }
    wind_mutex_unlock(hp->mutex);
    //wind_debug("base:0x%0x,addr:0x%0x,size:%d",freeitem,p,size);
    return p;
}



void *wind_heap_realloc(heap_s* heap, void* ptr, w_uint32_t newsize)
{
    void *p = NULL;
    heapitem_s* old = NULL;
    w_uint32_t size;
    WIND_ASSERT_RETURN(heap != NULL,NULL);
    WIND_ASSERT_RETURN(newsize > 0,NULL);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,NULL);
    //wind_debug("\r\nheap_realloc 0x%08x,%d",ptr,newsize);
    newsize = __ALIGN_R(newsize);
    if(ptr == NULL)
        return wind_heap_malloc(heap,newsize);
    old = ITEM_FROM_PTR(ptr);
    WIND_ASSERT_RETURN(old->magic == WIND_HEAPITEM_MAGIC,NULL);
    WIND_ASSERT_RETURN(old->used == 1,NULL);
    WIND_ASSERT_RETURN(old->heap == heap,NULL);       
    size = newsize + sizeof(heapitem_s);
    wind_mutex_lock(heap->mutex);
    p = wind_heap_malloc(heap,size);
    if(p != NULL)
    {
        //WIND_STATI_ADD(heap->stati,size);
        size = old->size - sizeof(heapitem_s) > newsize?newsize:old->size - sizeof(heapitem_s);
        wind_memcpy(p,ptr,size);
    }
	else
	{
	    wind_error("error realloc.");
		wind_heapitem_print(&g_core.heaplist);
	}
    dlist_remove(&heap->used_list,&old->itemnode.node);
    wind_free(ptr);
    wind_mutex_unlock(heap->mutex);
    return p;
}



w_err_t wind_heap_free(heap_s* heap,void *ptr)
{
    
    heapitem_s* item,*tmpitem;
    dnode_s *pdnode;
    //wind_debug("heap_free 0x%08x",ptr);
    WIND_ASSERT_RETURN(ptr != NULL,ERR_NULL_POINTER);
    item = ITEM_FROM_PTR(ptr);
    WIND_ASSERT_RETURN(item->magic == WIND_HEAPITEM_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item->used == 1,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item->heap != NULL,ERR_INVALID_PARAM);
    heap = item->heap;
    WIND_ASSERT_RETURN(heap != NULL,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(heap->magic == WIND_HEAP_MAGIC,ERR_INVALID_PARAM);
    wind_mutex_lock(heap->mutex);
    dlist_remove(&heap->used_list,&item->itemnode.node);
    WIND_STATI_MINUS(heap->stati,item->size);
    heapitem_init(item,heap,item->size,0);
    //wind_debug("insert3 0x%x",&item->itemnode);
    dlist_insert_prio(&heap->free_list,&item->itemnode,(w_uint32_t)item);
    //printf_list_addr(&heap->free_list);
    pdnode = dnode_next(&item->itemnode.node);
    if(pdnode != NULL)
    {
        tmpitem = DLIST_OBJ(pdnode,heapitem_s,itemnode.node);
        if(tmpitem != NULL)
            combine_heapitem(item,tmpitem);
    }
    pdnode = dnode_prev(&item->itemnode.node);
    if(pdnode != NULL)
    {
        tmpitem = DLIST_OBJ(pdnode,heapitem_s,itemnode.node);
        if(tmpitem != NULL)
            combine_heapitem(tmpitem,item);
    }
    wind_mutex_unlock(heap->mutex);
    return ERR_OK;
}

w_err_t wind_heap_print(dlist_s *list)
{
    dnode_s *pnode;
    heap_s* heap;
    wind_printf("heap list:\r\n");
    foreach_node(pnode,list)
    {
        heap = DLIST_OBJ(pnode,heap_s,heapnode);
        wind_printf("magic :%x\r\n",heap->magic);
        wind_printf("name  :%s\r\n",heap->name);
        wind_printf("addr  :%x\r\n",heap->addr);
        wind_printf("size  :%d\r\n",heap->stati.tot);
        wind_printf("used  :%d\r\n",heap->stati.used);
        wind_printf("maxused  :%d\r\n",heap->stati.max);
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
                heapitem,heapitem->size,(heapitem->used)?"uesd":"free");
        }
        foreach_node(dnode1,&heap->used_list)
        {
            heapitem = (heapitem_s *)DLIST_OBJ(dnode1,heapitem_s,itemnode.node);
            wind_printf("0x%-14x %-8d %-10s\r\n",
                heapitem,heapitem->size,(heapitem->used)?"uesd":"free");
        }
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}

void *wind_malloc(w_uint32_t size)
{
    void *ptr;
    heap_s* heap;
    dnode_s *pnode;
    //wind_debug("wind_malloc %d",size);
    size = __ALIGN_R(size);
    foreach_node(pnode,&g_core.heaplist)
    {
        heap = DLIST_OBJ(pnode,heap_s,heapnode);
        //wind_debug("malloc in heap:0x%x\r\n",heap);
        if(!heap->is_private)
        {
            ptr = wind_heap_malloc(heap, size);
            if(ptr)
                return ptr;
        }
    }
    return NULL;
}


w_err_t wind_free(void *ptr)
{
    heapitem_s *item;
    //wind_debug("wind_free 0x%0x",ptr);
    if(ptr == NULL)
        ptr = NULL;
    WIND_ASSERT_RETURN(ptr != NULL,ERR_NULL_POINTER);
    item = ITEM_FROM_PTR(ptr);
    if(item->used != 1)
        item->used = item->used;
    WIND_ASSERT_RETURN(item->magic == WIND_HEAPITEM_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(item->used == 1,ERR_INVALID_PARAM);
    return wind_heap_free(item->heap,ptr);
}


void *wind_realloc(void *ptr, w_uint32_t newsize)
{
    void *pnew;
    heap_s* heap;
    dnode_s *pnode;
    //wind_debug("wind_realloc 0x%0x,%d",ptr,newsize);
    newsize = __ALIGN_R(newsize);
    if (ptr == NULL)
        return wind_malloc(newsize);
    foreach_node(pnode,&g_core.heaplist)
    {
        heap = DLIST_OBJ(pnode,heap_s,heapnode);
        if(!heap->is_private)
        {
            pnew = wind_heap_realloc(heap,ptr,newsize);
            if(pnew)
                return pnew;
        }
    }
    return NULL;
}


void *wind_calloc(w_uint32_t count, w_uint32_t size)
{
    void *ptr;
    w_uint32_t tot_size;
    WIND_ASSERT_RETURN(count > 0,NULL);
    WIND_ASSERT_RETURN(size > 0,NULL);
    size = __ALIGN_R(size);
    wind_debug("wind_calloc %d,%d",count,size);
    tot_size = count *size;
    ptr = wind_malloc(tot_size);
    WIND_ASSERT_RETURN(ptr != NULL,NULL);
    wind_memset(ptr, 0, tot_size);
    return ptr;
}




#endif

