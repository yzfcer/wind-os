/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_heap.h / wind_heap.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 系统的内存堆的动态分配函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
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
#include "wind_core.h"
#include "wind_diagnose.h"
#include "wind_board_port.h"

#if WIND_HEAP_SUPPORT
#define NODE_TO_HEAP(node) (w_heap_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_heap_s*)0)->obj.objnode)))
#define NODE_TO_HEAPITEM(node) (w_heapitem_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_heapitem_s*)0)->itemnode)))
static w_dlist_s heaplist;

#define OFFSET_ADDR(base,offset) (void*)(((char*)(base))+(offset))
#define ITEM_FROM_PTR(ptr) (void*)((w_addr_t)ptr - sizeof(w_heapitem_s))


#if WIND_DIAGNOSE_SUPPORT
static w_int32_t heapitem_diagnose(w_heap_s *heap)
{
    w_dnode_s *dnode;
    w_heapitem_s *heapitem;
    foreach_node(dnode,&heap->free_list)
    {
        heapitem = NODE_TO_HEAPITEM(dnode);
        if(heapitem->magic != (w_uint16_t)(~WIND_HEAPITEM_MAGIC))
        {
            wind_error("heapitem magic error,ptr=0x%x",heapitem);
            return DIAG_RES_OBJ_MAGIC_ERROR;
        }
    }
    foreach_node(dnode,&heap->used_list)
    {
        heapitem = NODE_TO_HEAPITEM(dnode);
        if(heapitem->magic != (w_uint16_t)WIND_HEAPITEM_MAGIC)
        {
            wind_error("heapitem magic error,ptr=0x%x",heapitem);
            return DIAG_RES_OBJ_MAGIC_ERROR;
        }
    }
    return DIAG_RES_OK;
}

static w_int32_t heap_diagnose(void)
{
    w_int32_t res;
    w_dnode_s *dnode;
    w_heap_s *heap;
    w_dlist_s *list = &heaplist;
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        heap = NODE_TO_HEAP(dnode);
        if(heap->obj.magic != WIND_HEAP_MAGIC)
        {
            wind_enable_switch();
            wind_error("heap magic error,ptr=0x%x",heap);
            return DIAG_RES_OBJ_MAGIC_ERROR;
        }
        res = heapitem_diagnose(heap);
        if(res != DIAG_RES_OK)
        {
            wind_enable_switch();
            wind_notice("check heap %-16s [ERROR]",heap->obj.name);
            return res;
        }
        else
        {
            wind_notice("check heap %-16s [OK]",heap->obj.name);
        }
    }
    wind_enable_switch();
    return DIAG_RES_OK;
}

static w_err_t heap_diagnose_init(void)
{
    w_err_t err;
    DIAGNOSENOSE_DEF(heap,heap_diagnose);
    err = wind_diagnose_register(DIAGNOSENOSE(heap));
    return err;
}
#else
#define heap_diagnose_init() W_ERR_OK
#endif

w_err_t _wind_heap_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(heaplist);
    _wind_heaps_create();
    err = heap_diagnose_init();
    return err;
}

static void heapitem_init(w_heapitem_s *item,w_heap_s *hp,w_uint16_t magic,w_int32_t size,w_uint8_t flag)
{
    item->magic = magic;
    item->flag = flag;
    item->allocid = 0;
    item->heap = hp;
    PRIO_DNODE_INIT(item->itemnode);
    item->size = size;
}

w_heap_s *wind_heap_get(const char *name)
{
    return (w_heap_s*)wind_obj_get(name,&heaplist);
}


w_heap_s *wind_heap_create(const char *name,w_addr_t base,w_uint32_t size,w_uint16_t flag)
{
    w_heapitem_s* item;
    w_heap_s* hp;
    w_uint32_t hpsize;
    
    wind_notice("create heap:%s",name?name:"null");
    wind_debug("base=0x%08x,size=0x%08x",base,size);
    WIND_ASSERT_RETURN(size > WIND_HEAP_MIN_SIZE,W_NULL);
    
    hp = (w_heap_s*)(__ALIGN_R((w_addr_t)base));
    hpsize = size;
    hpsize += (w_uint32_t)(((w_addr_t)hp) - base);
    hpsize = __ALIGN_L(hpsize);
    
    
    hp->addr = OFFSET_ADDR(hp,sizeof(w_heap_s));
    WIND_STATI_INIT(hp->stati,hpsize);
    WIND_STATI_ADD(hp->stati,sizeof(w_heap_s));
    DLIST_INIT(hp->used_list);
    DLIST_INIT(hp->free_list);
    hp->mutex = wind_mutex_create(name);
    WIND_ASSERT_RETURN(hp->mutex != W_NULL,W_NULL);
    item = hp->addr;
    heapitem_init(item,hp,(w_uint16_t)(~WIND_HEAPITEM_MAGIC),hpsize - sizeof(w_heap_s),0);

    
    wind_disable_switch();
    dlist_insert_head(&hp->free_list,&item->itemnode.dnode);
    wind_enable_switch();
    wind_obj_init(&hp->obj,WIND_HEAP_MAGIC,name,&heaplist);
    wind_heap_setflag(hp, flag);

    return hp;
}

w_err_t wind_heap_destroy(w_heap_s *heap)
{
    w_err_t err;
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID);
    wind_notice("destroy heap:%s",heap->obj.name?heap->obj.name:"null");
    wind_mutex_lock(heap->mutex);
    err = wind_obj_deinit(&heap->obj,WIND_HEAP_MAGIC,&heaplist);
    wind_mutex_unlock(heap->mutex);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    wind_mutex_destroy(heap->mutex);
    heap->mutex = W_NULL;
    return W_ERR_OK;
}

w_err_t wind_heap_setflag(w_heap_s *heap,w_int16_t flag)
{
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(heap->mutex);
    if(flag & F_HEAP_PRIVATE)
        SET_F_HEAP_PRIVATE(heap);
    wind_mutex_unlock(heap->mutex);
    return W_ERR_OK;

}
w_err_t wind_heap_clrflag(w_heap_s *heap,w_int16_t flag)
{
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(heap->mutex);
    if(flag & F_HEAP_PRIVATE)
        CLR_F_HEAP_PRIVATE(heap);
    wind_mutex_unlock(heap->mutex);
    return W_ERR_OK;
}


static void *alloc_from_freeitem(w_heap_s* heap,w_heapitem_s* freeitem,w_uint32_t size)
{
    void *p = W_NULL;
    w_heapitem_s* item = freeitem;
    w_uint32_t rest;
    w_heap_s* hp = heap;
    WIND_ASSERT_RETURN(freeitem->size >= size,W_NULL);
    item = freeitem;
    rest = freeitem->size - size;
    if(rest > sizeof(w_heapitem_s))
    {
        item->size = size;
        freeitem = OFFSET_ADDR(item,size);
        freeitem->size = rest;
    }
    else
    {
        item->size = freeitem->size;
        freeitem = W_NULL;
    }
    WIND_STATI_ADD(hp->stati,item->size);
    heapitem_init(item,heap,WIND_HEAPITEM_MAGIC,item->size,1);
    p = OFFSET_ADDR(item,sizeof(w_heapitem_s));
    dlist_insert_prio(&hp->used_list,&item->itemnode,(w_uint32_t)item);
    item->magic = WIND_HEAPITEM_MAGIC;

    if(freeitem)
    {
        heapitem_init(freeitem,heap,(w_uint16_t)(~WIND_HEAPITEM_MAGIC),rest,0);
        dlist_insert_prio(&hp->free_list,&freeitem->itemnode,(w_uint32_t)freeitem);
    }
    return p;
}


static w_err_t combine_heapitem(w_heapitem_s* item1,w_heapitem_s* item2)
{
    WIND_ASSERT_RETURN(item1 != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(item2 != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(item1->heap == item2->heap,W_ERR_INVALID);
    WIND_ASSERT_RETURN(item1->magic == (w_uint16_t)(~WIND_HEAPITEM_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(item2->magic == (w_uint16_t)(~WIND_HEAPITEM_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(!IS_F_HEAPITEM_USED(item1),W_ERR_INVALID);
    WIND_ASSERT_RETURN(!IS_F_HEAPITEM_USED(item2),W_ERR_INVALID);
    if(item1->size + (w_addr_t)item1 == (w_addr_t)item2)
    {
        dlist_remove(&item1->heap->free_list,&item2->itemnode.dnode);
        item1->size += item2->size;
        item2->magic = (w_uint16_t)(~WIND_HEAPITEM_MAGIC);
        item2->heap = W_NULL;
        PRIO_DNODE_INIT(item2->itemnode);
    }
    return W_ERR_OK;
}

//从内存堆中分出一块空间
void *wind_heap_malloc(w_heap_s* heap,w_uint32_t size)
{
    void *p = W_NULL;
    w_heap_s* hp = heap;
    w_dnode_s *dnode;
    w_heapitem_s* freeitem;
    WIND_ASSERT_RETURN(heap != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(size > 0,W_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_NULL);
    wind_debug("heap_malloc %d",size);
    size = __ALIGN_R(size) + sizeof(w_heapitem_s);
    
    wind_mutex_lock(hp->mutex);
    foreach_node(dnode,&hp->free_list)
    {
        freeitem = NODE_TO_HEAPITEM(dnode);
        if(freeitem->size > 0xffff)
            freeitem->size = freeitem->size;
        if(size <= freeitem->size)
        {
            dlist_remove(&hp->free_list,&freeitem->itemnode.dnode);
            p = alloc_from_freeitem(heap,freeitem,size);
            break;
        }
    }
    wind_mutex_unlock(hp->mutex);
    wind_debug("base:0x%0x,addr:0x%0x,size:%d",freeitem,p,size);
    return p;
}



void *wind_heap_realloc(w_heap_s* heap, void* ptr, w_uint32_t newsize)
{
    w_err_t err;
    void *p = W_NULL;
    w_heapitem_s* old = W_NULL;
    w_uint32_t size;
    WIND_ASSERT_RETURN(heap != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(newsize > 0,W_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_NULL);
    wind_debug("\r\nheap_realloc 0x%08x,%d",ptr,newsize);
    wind_mutex_lock(heap->mutex);
    do
    {
        err = W_ERR_OK;
        newsize = __ALIGN_R(newsize);
        if(ptr == W_NULL)
        {
            p = wind_heap_malloc(heap,newsize);
            break;
        }
        old = ITEM_FROM_PTR(ptr);
        WIND_ASSERT_BREAK(old->magic == WIND_HEAPITEM_MAGIC,W_ERR_INVALID,"invalid pointer");
        WIND_ASSERT_BREAK(IS_F_HEAPITEM_USED(old),W_ERR_INVALID,"memory is NOT alloced");
        WIND_ASSERT_BREAK(old->heap == heap,W_ERR_INVALID,"pointer is NOT in this heap");       
        size = newsize + sizeof(w_heapitem_s);
        p = wind_heap_malloc(heap,size);
        if(p != W_NULL)
        {
            size = old->size - sizeof(w_heapitem_s) > newsize?newsize:old->size - sizeof(w_heapitem_s);
            wind_memcpy(p,ptr,size);
            wind_free(ptr);
            ptr = W_NULL;
        }
        else
        {
            wind_error("error realloc.");
            wind_heapitem_print(HP_ALLOCID_ALL);
        }
        dlist_remove(&heap->used_list,&old->itemnode.dnode);
        
    }while(0);
    if(ptr != W_NULL)
        wind_free(ptr);
    wind_mutex_unlock(heap->mutex);
        
    return p;
}



w_err_t wind_heap_free(w_heap_s* heap,void *ptr)
{
    w_err_t err;
    w_heapitem_s* item,*tmpitem;
    w_dnode_s *dnode;
    if(ptr == W_NULL)
    {
        wind_debug("wind_free:ptr=null");
        return W_ERR_OK;
    }
    wind_mutex_lock(heap->mutex);
    do
    {
        err = W_ERR_OK;
        item = ITEM_FROM_PTR(ptr);
        WIND_ASSERT_BREAK(item->magic == WIND_HEAPITEM_MAGIC,W_ERR_INVALID,"invalid pointer");
        WIND_ASSERT_BREAK(IS_F_HEAPITEM_USED(item),W_ERR_INVALID,"memory is NOT alloced");
        WIND_ASSERT_BREAK(item->heap == heap,W_ERR_INVALID,"pointer is NOT in this heap");       
        wind_debug("heap_free %s,0x%08x",heap->obj.name?heap->obj.name:"null",ptr);
        heap = item->heap;
        WIND_ASSERT_BREAK(heap != W_NULL,W_ERR_INVALID,"null heap");
        WIND_ASSERT_BREAK(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID,"nagic error");
        dlist_remove(&heap->used_list,&item->itemnode.dnode);
        WIND_STATI_MINUS(heap->stati,item->size);
        heapitem_init(item,heap,(w_uint16_t)(~WIND_HEAPITEM_MAGIC),item->size,0);
        wind_debug("insert3 0x%x",&item->itemnode);
        dlist_insert_prio(&heap->free_list,&item->itemnode,(w_uint32_t)item);
        dnode = dnode_next(&item->itemnode.dnode);
        if(dnode != W_NULL)
        {
            tmpitem = NODE_TO_HEAPITEM(dnode);
            if(tmpitem != W_NULL)
                combine_heapitem(item,tmpitem);
        }
        dnode = dnode_prev(&item->itemnode.dnode);
        if(dnode != W_NULL)
        {
            tmpitem = NODE_TO_HEAPITEM(dnode);
            if(tmpitem != W_NULL)
                combine_heapitem(tmpitem,item);
        }       
    }while(0);

    wind_mutex_unlock(heap->mutex);
    return W_ERR_OK;
}

w_err_t wind_heap_print(void)
{
    w_dnode_s *dnode;
    w_heap_s* heap;
    w_dlist_s *list = &heaplist;
    wind_printf("\r\n\r\nheap list:\r\n");
    wind_print_space(7);
    wind_printf("%-16s %-10s %-12s %-10s \r\n","heap","base","size","isprivate");
    wind_print_space(7);

    foreach_node(dnode,list)
    {
        heap = NODE_TO_HEAP(dnode);
        wind_mutex_lock(heap->mutex);
        wind_printf("%-16s 0x%08x %-12d %-10s\r\n",
            heap->obj.name?heap->obj.name:"null",heap->addr,heap->stati.tot,
            IS_F_HEAP_PRIVATE(heap)?"YES":"NO");
        wind_mutex_unlock(heap->mutex);
    }
    wind_print_space(7);
    return W_ERR_OK;
}

w_err_t wind_heapitem_print(w_allocid_e allocid)
{   
    w_err_t err;
    w_dnode_s *dnode,*dnode1;
    w_heap_s *heap;
    w_heapitem_s *heapitem;
    w_dlist_s *list = &heaplist;
    WIND_ASSERT_RETURN(list->head != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nheapitem list:\r\n");
    wind_print_space(6);
    wind_printf("%-12s %-10s %-10s %-8s\r\n","addr","size","state","allocid");
    wind_print_space(6);
    
    foreach_node(dnode,list)
    {
        err = W_ERR_OK;
        heap = NODE_TO_HEAP(dnode);
        wind_mutex_lock(heap->mutex);
        foreach_node(dnode1,&heap->free_list)
        {
            heapitem = NODE_TO_HEAPITEM(dnode1);
            WIND_ASSERT_BREAK(heapitem->magic = (w_uint16_t)(~WIND_HEAPITEM_MAGIC),
                            W_ERR_MEM,"heap memory has been illegally accessed .");
            if((allocid == 255)||(allocid == heapitem->allocid))
            {
                wind_printf("0x%-10x %-10d %-10s %-8d\r\n",heapitem,heapitem->size,
                    IS_F_HEAPITEM_USED(heapitem)?"used":"free",heapitem->allocid);
            }
        }
        foreach_node(dnode1,&heap->used_list)
        {
            heapitem = NODE_TO_HEAPITEM(dnode1);
            WIND_ASSERT_BREAK(heapitem->magic = (w_uint16_t)(~WIND_HEAPITEM_MAGIC),
                            W_ERR_MEM,"heap memory has been illegally accessed .");
            if((allocid == 255)||(allocid == heapitem->allocid))
            {
                wind_printf("0x%-10x %-10d %-10s %-8d\r\n",heapitem,heapitem->size,
                    IS_F_HEAPITEM_USED(heapitem)?"used":"free",heapitem->allocid);
            }
        }
        wind_mutex_unlock(heap->mutex);
    }
    wind_print_space(6);
    return W_ERR_OK;
}

w_err_t wind_heap_stati_print(void)
{
    w_dnode_s *dnode;
    w_heap_s *heap;
    w_dlist_s *list = &heaplist;
    wind_print_space(7);
    wind_printf("%-16s %-10s %-10s %-8s %-8s\r\n","heap","tot","used","maxused","err");
    wind_print_space(7);
    
    foreach_node(dnode,list)
    {
        heap = NODE_TO_HEAP(dnode);
        wind_mutex_lock(heap->mutex);
        wind_printf("%-16s %-10d %-10d %-8d %-8d\r\n",heap->obj.name,heap->stati.tot,
            heap->stati.used,heap->stati.max,heap->stati.err);
        wind_mutex_unlock(heap->mutex);
    }
   
    wind_print_space(7);
    return W_ERR_OK;
}

void *wind_malloc(w_uint32_t size)
{
    void *ptr;
    w_heap_s* heap;
    w_dnode_s *dnode;
    wind_debug("wind_malloc:%d",size);
    size = __ALIGN_R(size);
    foreach_node(dnode,&heaplist)
    {
        heap = NODE_TO_HEAP(dnode);
        wind_debug("malloc in heap:0x%x",heap);
        if(!IS_F_HEAP_PRIVATE(heap))
        {
            ptr = wind_heap_malloc(heap, size);
            if(ptr)
            {
                wind_debug("wind_malloc:ptr=0x%x",ptr);
                return ptr;
            }
                
        }
    }
    wind_error("memory is W_NULL");
    return W_NULL;
}

void *wind_zalloc(w_uint32_t size,w_uint8_t allocid)
{
    void *ptr;
    ptr = wind_alloc(size,allocid);
    if(ptr != W_NULL)
        wind_memset(ptr,0,size);
    return ptr;
}

void *wind_alloc(w_uint32_t size,w_uint8_t allocid)
{
    void *ptr;
    w_heapitem_s *item;
    ptr = wind_malloc(size);
    if(ptr != W_NULL)
    {
        item = ITEM_FROM_PTR(ptr);
        item->allocid = allocid;
    }
    return ptr;
}

void *wind_salloc(char *str,w_uint8_t allocid)
{
    char *ptr;
    w_int32_t len;
    if(str == W_NULL)
        return W_NULL;
    len = wind_strlen(str);
    ptr = wind_alloc(len+1,allocid);
    if(ptr == W_NULL)
        return W_NULL;
    wind_strcpy(ptr,str);
    return ptr;
}

void *wind_clone(void *object,w_uint32_t size)
{
    void *obj;
    if(object == W_NULL)
        return W_NULL;
    obj = wind_malloc(size);
    WIND_ASSERT_RETURN(obj != W_NULL,W_NULL);
    wind_memcpy(obj,object,(w_int32_t)size);
    return obj;
}


w_err_t wind_free(void *ptr)
{
    w_heapitem_s *item;
    WIND_CHECK_RETURN(ptr != W_NULL,W_ERR_OK);
    wind_debug("wind_free:ptr=0x%x",ptr);
    item = ITEM_FROM_PTR(ptr);
    if(item->magic != WIND_HEAPITEM_MAGIC)
        item->magic = item->magic;
    WIND_ASSERT_RETURN(item->magic == WIND_HEAPITEM_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_HEAPITEM_USED(item),W_ERR_INVALID);
    return wind_heap_free(item->heap,ptr);
}


void *wind_realloc(void *ptr, w_uint32_t newsize)
{
    void *pnew;
    w_heap_s* heap;
    w_dnode_s *dnode;
    wind_debug("wind_realloc:0x%0x,%d",ptr,newsize);
    newsize = __ALIGN_R(newsize);
    if (ptr == W_NULL)
        return wind_malloc(newsize);
    foreach_node(dnode,&heaplist)
    {
        heap = NODE_TO_HEAP(dnode);
        if(!IS_F_HEAP_PRIVATE(heap))
        {
            pnew = wind_heap_realloc(heap,ptr,newsize);
            if(pnew)
                return pnew;
        }
    }
    return W_NULL;
}


void *wind_calloc(w_uint32_t count, w_uint32_t size)
{
    void *ptr;
    w_uint32_t tot_size;
    WIND_ASSERT_RETURN(count > 0,W_NULL);
    WIND_ASSERT_RETURN(size > 0,W_NULL);
    size = __ALIGN_R(size);
    wind_debug("wind_calloc:%d,%d",count,size);
    tot_size = count *size;
    ptr = wind_malloc(tot_size);
    WIND_ASSERT_RETURN(ptr != W_NULL,W_NULL);
    wind_memset(ptr, 0, tot_size);
    return ptr;
}




#endif

