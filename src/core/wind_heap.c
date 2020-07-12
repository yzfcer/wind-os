/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_heap.c
** Author      : Jason Zhou
** Last Date   : 2019.12.19
** Description : wind-os memory heap function set
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_heap.h"
#include "wind_mutex.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_core.h"
#include "wind_diagnose.h"
#include "wind_board_port.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_HEAP_SUPPORT
#define NODE_TO_HEAP(node) NODEPTR_TO_ITEMPTR(node,w_heap_s,obj.objnode)
#define NODE_TO_HEAPITEM(node) NODEPTR_TO_ITEMPTR(node,w_heapitem_s,itemnode)
static w_dlist_s heaplist;

#define OFFSET_ADDR(base,offset) (((w_addr_t)(base))+(offset))
#define ITEM_FROM_PTR(ptr) (w_heapitem_s*)(void*)((w_addr_t)ptr - WIND_HEAP_ITEM_SIZE)

#define NEXT_HEAPITEM(item) NODE_TO_HEAPITEM(item->itemnode.dnode.next)
#define PREV_HEAPITEM(item) NODE_TO_HEAPITEM(item->itemnode.dnode.prev)

#if WIND_DIAGNOSE_SUPPORT

static void print_heapitem_info(w_heapitem_s *item)
{
    wind_printf("item=%p\n",item);
    wind_printf("item->magic=%d\n",item->magic);
    wind_printf("item->flag=0x%x\n",item->flag);
    wind_printf("item->allocid=%d\n",item->allocid);
    wind_printf("item->size=%d\n",item->size);
    wind_printf("item->itemnode.dnode.prev=%p\n",PREV_HEAPITEM(item));
    wind_printf("item->itemnode.dnode.next=%p\n",NEXT_HEAPITEM(item));
}


static w_int32_t heapitem_diagnose(w_heap_s *heap)
{
    w_dnode_s *dnode;
    w_heapitem_s *heapitem;
    foreach_node(dnode,&heap->free_list)
    {
        heapitem = NODE_TO_HEAPITEM(dnode);
        if(heapitem->magic != (w_uint16_t)(~WIND_HEAPITEM_MAGIC))
        {
            wind_error("heap %s heapitem magic error,ptr=%p",heap->obj.name,heapitem);
            wind_error("magic is expected 0x%4x,but in fact 0x%4x",(w_uint16_t)(~WIND_HEAPITEM_MAGIC),heapitem->magic);
            return DIAG_RES_OBJ_MAGIC_ERROR;
        }
    }
    foreach_node(dnode,&heap->used_list)
    {
        heapitem = NODE_TO_HEAPITEM(dnode);
        if(heapitem->magic != (w_uint16_t)WIND_HEAPITEM_MAGIC)
        {
            wind_error("heap %s heapitem magic error,ptr=%p",heap->obj.name,heapitem);
            wind_error("magic is expected 0x%4x,but in fact 0x%4x",WIND_HEAPITEM_MAGIC,heapitem->magic);
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
#define print_heapitem_info(tmpitem)
#define wind_diagnose_check()

#endif

w_err_t _wind_heap_mod_init(void)
{
    w_err_t err;
    //wind_printf("sizeof(w_obj_s)=%d\n",sizeof(w_obj_s));
    //wind_printf("WIND_HEAP_ITEM_SIZE=%d\n",WIND_HEAP_ITEM_SIZE);
    //wind_printf("WIND_HEAP_ITEM_SIZE=%d\n",WIND_HEAP_ITEM_SIZE);
    //wind_printf("sizeof(w_heap_s)=%d\n",sizeof(w_heap_s));
    //wind_printf("sizeof(w_heapitem_s)=%d\n",sizeof(w_heapitem_s));
    DLIST_INIT(heaplist);
    _wind_heaps_create();
    err = heap_diagnose_init();
    return err;
}

static void heapitem_init(w_heapitem_s *item,w_heap_s *hp,w_uint16_t magic,w_int32_t size,w_uint8_t flag)
{
    wind_debug("item base=%p",item);
    wind_debug("item size=%p",size);
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
    WIND_ASSERT_RETURN(size > WIND_HEAP_MIN_SIZE,W_NULL);
    
    hp = (w_heap_s*)(HEAP_ALIGN_R((w_addr_t)base));
    hpsize = size;
    hpsize += (w_uint32_t)(((w_addr_t)hp) - base);
    hpsize = HEAP_ALIGN_L(hpsize);
    
    
    hp->addr = OFFSET_ADDR(hp,WIND_HEAP_HEAD_SIZE);
    //hp->addr = (void*)(HEAP_ALIGN_R((w_addr_t)base));
    
    WIND_STATI_INIT(hp->stati,hpsize);
    WIND_STATI_ADD(hp->stati,WIND_HEAP_HEAD_SIZE);
    DLIST_INIT(hp->used_list);
    DLIST_INIT(hp->free_list);
    hp->mutex = wind_mutex_create(name);
    WIND_ASSERT_RETURN(hp->mutex != W_NULL,W_NULL);
    item = (w_heapitem_s*)hp->addr;
    heapitem_init(item,hp,(w_uint16_t)(~WIND_HEAPITEM_MAGIC),hpsize - WIND_HEAP_HEAD_SIZE,0);
    
    wind_disable_switch();
    dlist_insert_head(&hp->free_list,&item->itemnode.dnode);
    wind_enable_switch();
    wind_obj_init(&hp->obj,WIND_HEAP_MAGIC,name,&heaplist);
    wind_heap_setflag(hp, flag);
    
    wind_debug("mem base=%p",base);
    wind_debug("mem size=%d",size);
    wind_debug("hp  base=%p",hp);
    wind_debug("itm base=%p",item);
    wind_debug("hp  addr=%p",hp->addr);
    wind_debug("hp  hsiz=%p",WIND_HEAP_HEAD_SIZE);
    wind_debug("hp  size=%d",hp->stati.tot);
    wind_debug("hp__size=%d",hpsize);
    //WIND_TRAP();

    return hp;
}

w_err_t wind_heap_destroy(w_heap_s *heap)
{
    w_err_t err;
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID);
    wind_notice("destroy heap:%s",wind_obj_name(&heap->obj));
    wind_mutex_lock(heap->mutex);
    err = wind_obj_deinit(&heap->obj,WIND_HEAP_MAGIC,&heaplist);
    wind_mutex_unlock(heap->mutex);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    wind_mutex_destroy(heap->mutex);
    heap->mutex = (w_mutex_s*)W_NULL;
    return W_ERR_OK;
}

w_err_t wind_heap_setflag(w_heap_s *heap,w_int16_t flag)
{
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(heap->mutex);
    if(flag & F_HEAP_PRIVATE)
        SET_F_HEAP_PRIVATE(heap);
    wind_mutex_unlock(heap->mutex);
    return W_ERR_OK;

}
w_err_t wind_heap_clrflag(w_heap_s *heap,w_int16_t flag)
{
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_NULL_PTR);
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
    WIND_ASSERT_RETURN(freeitem->size >= size,W_NULL);
    item = freeitem;
    rest = freeitem->size - size;
    if(rest > WIND_HEAP_ITEM_SIZE)
    {
        item->size = size;
        freeitem = (w_heapitem_s*)OFFSET_ADDR(item,size);
        freeitem->size = rest;
    }
    else
    {
        item->size = freeitem->size;
        freeitem = (w_heapitem_s*)W_NULL;
    }
    WIND_STATI_ADD(heap->stati,item->size);
    heapitem_init(item,heap,WIND_HEAPITEM_MAGIC,item->size,1);
    p = (void*)OFFSET_ADDR(item,WIND_HEAP_ITEM_SIZE);
    dlist_insert_prio(&heap->used_list,&item->itemnode,(w_uint32_t)(w_addr_t)item);
    item->magic = WIND_HEAPITEM_MAGIC;

    if(freeitem)
    {
        heapitem_init(freeitem,heap,(w_uint16_t)(~WIND_HEAPITEM_MAGIC),rest,0);
        dlist_insert_prio(&heap->free_list,&freeitem->itemnode,(w_uint32_t)(w_addr_t)freeitem);
    }
    return p;
}


static w_err_t combine_heapitem(w_heapitem_s* item1,w_heapitem_s* item2)
{
    WIND_ASSERT_RETURN(item1 != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(item2 != W_NULL,W_ERR_NULL_PTR);
    wind_debug("heap1=%p,heap2=%p\r\n",item1->heap,item2->heap);
    wind_debug("item1->magic=%d,item2->magic=%d\r\n",item1->magic,item2->magic);
    WIND_ASSERT_RETURN(item1->magic == (w_uint16_t)(~WIND_HEAPITEM_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(item2->magic == (w_uint16_t)(~WIND_HEAPITEM_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(!IS_F_HEAPITEM_USED(item1),W_ERR_INVALID);
    WIND_ASSERT_RETURN(!IS_F_HEAPITEM_USED(item2),W_ERR_INVALID);
    WIND_ASSERT_RETURN(item1->heap == item2->heap,W_ERR_INVALID);
    if(item1->size + (w_addr_t)item1 == (w_addr_t)item2)
    {
        dlist_remove(&item1->heap->free_list,&item2->itemnode.dnode);
        item1->size += item2->size;
        item2->magic = (w_uint16_t)(~WIND_HEAPITEM_MAGIC);
        item2->heap = (w_heap_s*)W_NULL;
        PRIO_DNODE_INIT(item2->itemnode);
    }
    return W_ERR_OK;
}


void *wind_heap_malloc(w_heap_s* heap,w_uint32_t size)
{
    void *p = W_NULL;
    w_heap_s* hp = heap;
    w_dnode_s *dnode;
    w_err_t err = W_ERR_OK;
    w_heapitem_s* freeitem;
    WIND_ASSERT_RETURN(heap != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(size > 0,W_NULL);
    wind_debug("heap_malloc %d",size);
    size = HEAP_ALIGN_R(size) + WIND_HEAP_ITEM_SIZE;
    
    wind_mutex_lock(hp->mutex);
    foreach_node(dnode,&hp->free_list)
    {
        freeitem = NODE_TO_HEAPITEM(dnode);
        //if(freeitem->size > 0xffff)
        //    freeitem->size = freeitem->size;
        if(size <= freeitem->size)
        {
            dlist_remove(&hp->free_list,&freeitem->itemnode.dnode);
            p = alloc_from_freeitem(heap,freeitem,size);
            WIND_ASSERT_BREAK(p != W_NULL,W_ERR_MEM,"alloc from freeitem failed");
            break;
        }
    }
    wind_mutex_unlock(hp->mutex);
    wind_debug("base:0x%0x,addr:0x%0x,size:%d",freeitem,p,size);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    return p;
}



void *wind_heap_realloc(w_heap_s* heap, void* ptr, w_uint32_t newsize)
{
    w_err_t err;
    void *p = W_NULL;
    w_heapitem_s* old = (w_heapitem_s*)W_NULL;
    w_uint32_t size;
    WIND_ASSERT_RETURN(heap != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(newsize > 0,W_NULL);
    wind_debug("\r\nheap_realloc 0x%08x,%d",ptr,newsize);
    wind_mutex_lock(heap->mutex);
    do
    {
        err = W_ERR_OK;
        newsize = HEAP_ALIGN_R(newsize);
        if(ptr == W_NULL)
        {
            p = wind_heap_malloc(heap,newsize);
            break;
        }
        old = ITEM_FROM_PTR(ptr);
        WIND_ASSERT_BREAK(old->magic == WIND_HEAPITEM_MAGIC,W_ERR_INVALID,"invalid pointer");
        WIND_ASSERT_BREAK(IS_F_HEAPITEM_USED(old),W_ERR_INVALID,"memory is NOT alloced");
        WIND_ASSERT_BREAK(old->heap == heap,W_ERR_INVALID,"pointer is NOT in this heap");       
        size = newsize + WIND_HEAP_ITEM_SIZE;
        p = wind_heap_malloc(heap,size);
        if(p != W_NULL)
        {
            size = old->size - WIND_HEAP_ITEM_SIZE > newsize?newsize:old->size - WIND_HEAP_ITEM_SIZE;
            wind_memcpy(p,ptr,size);
        }
        else
        {
            wind_error("error realloc,need size:%d.",newsize);
            wind_heap_stati_print();
            wind_heapitem_print_detail(HP_ALLOCID_ALL);
        }
        //dlist_remove(&heap->used_list,&old->itemnode.dnode);
        wind_free(ptr);
        ptr = W_NULL;
        
    }while(0);
    //if(ptr != W_NULL)
    //    wind_free(ptr);
    wind_mutex_unlock(heap->mutex);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    return p;
}



w_err_t wind_heap_free(w_heap_s* heap,void *ptr)
{
    w_err_t err;
    w_heapitem_s* item,*tmpitem;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(heap != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_NULL_PTR);
    WIND_CHECK_RETURN(ptr != W_NULL,W_ERR_OK);
    wind_mutex_lock(heap->mutex);
    do
    {
        err = W_ERR_OK;
        item = ITEM_FROM_PTR(ptr);
        WIND_ASSERT_BREAK(item->magic == WIND_HEAPITEM_MAGIC,W_ERR_INVALID,"invalid pointer");
        WIND_ASSERT_BREAK(IS_F_HEAPITEM_USED(item),W_ERR_INVALID,"memory is NOT alloced");
        WIND_ASSERT_BREAK(item->heap == heap,W_ERR_INVALID,"pointer is NOT in this heap");       
        wind_debug("heap_free %s,%p",wind_obj_name(&heap->obj),ptr);
        heap = item->heap;
        WIND_ASSERT_BREAK(heap != W_NULL,W_ERR_INVALID,"null heap");
        WIND_ASSERT_BREAK(heap->obj.magic == WIND_HEAP_MAGIC,W_ERR_INVALID,"magic error");
        dlist_remove(&heap->used_list,&item->itemnode.dnode);
        WIND_STATI_MINUS(heap->stati,item->size);
        heapitem_init(item,heap,(w_uint16_t)(~WIND_HEAPITEM_MAGIC),item->size,0);
        wind_debug("insert3 0x%x",&item->itemnode);
        dlist_insert_prio(&heap->free_list,&item->itemnode,(w_uint32_t)(w_addr_t)item);
        dnode = dnode_next(&item->itemnode.dnode);
        if(dnode != W_NULL)
        {
            tmpitem = NODE_TO_HEAPITEM(dnode);
            if(tmpitem != W_NULL)
            {
                wind_debug("item=%p,tmpitem=%p\r\n",item,tmpitem);
                combine_heapitem(item,tmpitem);
            }
        }
        dnode = dnode_prev(&item->itemnode.dnode);
        if(dnode != W_NULL)
        {
            tmpitem = NODE_TO_HEAPITEM(dnode);
            if(tmpitem != W_NULL)
            {
                wind_debug("item=%p,tmpitem=%p\r\n",item,tmpitem);
                if(tmpitem->magic == 0)
                {
                    print_heapitem_info(tmpitem);
                    wind_diagnose_check();
                    WIND_TRAP();
                }
                combine_heapitem(tmpitem,item);
            }
        }       
    }while(0);

    wind_mutex_unlock(heap->mutex);
    return err;
}

w_err_t wind_heap_print_list(void)
{
    return wind_obj_print_list(&heaplist);
}

w_err_t wind_heap_print_detail(void)
{
    w_dnode_s *dnode;
    w_heap_s* heap;
    char * hdr_fmt,*dat_fmt;
    w_dlist_s *list = &heaplist;
    if(sizeof(w_addr_t) == 8)
    {
        hdr_fmt = "%-16s %-18s %-12s %-10s \r\n";
        dat_fmt = "%-16s %-16p %-12d %-10s\r\n";
    }
    else
    {
        hdr_fmt = "%-16s %-10s %-12s %-10s \r\n";
        dat_fmt = "%-16s %-8p %-12d %-10s\r\n";
    }
    wind_printf("\r\n\r\nheap list:\r\n");
    wind_print_space(7);
    wind_printf(hdr_fmt,"heap","base","size","isprivate");
    wind_print_space(7);

    foreach_node(dnode,list)
    {
        heap = NODE_TO_HEAP(dnode);
        wind_mutex_lock(heap->mutex);
        wind_printf(dat_fmt,wind_obj_name(&heap->obj),heap->addr,
            heap->stati.tot,IS_F_HEAP_PRIVATE(heap)?"YES":"NO");
        wind_mutex_unlock(heap->mutex);
    }
    wind_print_space(7);
    return W_ERR_OK;
}


w_err_t wind_heapitem_print_detail(w_allocid_e allocid)
{   
    w_err_t err;
    w_dnode_s *dnode,*dnode1;
    w_heap_s *heap;
    char * hdr_fmt,*dat_fmt;
    w_heapitem_s *heapitem;
    w_dlist_s *list = &heaplist;
    WIND_ASSERT_RETURN(list->head != W_NULL,W_ERR_NULL_PTR);
    if(sizeof(w_addr_t) == 8)
    {
        hdr_fmt = "%-18s   %-10s %-10s %-8s\r\n";
        dat_fmt = "%-16p   %-10d %-10s %-8d\r\n";
    }
    else
    {
        hdr_fmt = "%-10s   %-10s %-10s %-8s\r\n";
        dat_fmt = "%-8p   %-10d %-10s %-8d\r\n";
    }

    wind_printf("\r\n\r\nheapitem list:\r\n");
    wind_print_space(6);
    wind_printf(hdr_fmt,"addr","size","state","allocid");
    wind_print_space(6);
    
    foreach_node(dnode,list)
    {
        err = W_ERR_OK;
        heap = NODE_TO_HEAP(dnode);
        wind_mutex_lock(heap->mutex);
        foreach_node(dnode1,&heap->free_list)
        {
            heapitem = NODE_TO_HEAPITEM(dnode1);
            WIND_ASSERT_BREAK(heapitem->magic == (w_uint16_t)(~WIND_HEAPITEM_MAGIC),
                            W_ERR_MEM,"heap memory has been illegally accessed .");
            if((allocid == 255)||(allocid == heapitem->allocid))
            {
                wind_printf(dat_fmt,heapitem,heapitem->size,
                    IS_F_HEAPITEM_USED(heapitem)?"used":"free",heapitem->allocid);
            }
        }
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"heap memory error.");
        foreach_node(dnode1,&heap->used_list)
        {
            heapitem = NODE_TO_HEAPITEM(dnode1);
            WIND_ASSERT_BREAK(heapitem->magic == (w_uint16_t)(WIND_HEAPITEM_MAGIC),
                            W_ERR_MEM,"%d != %d.",heapitem->magic,WIND_HEAPITEM_MAGIC);
            if((allocid == 255)||(allocid == heapitem->allocid))
            {
                wind_printf(dat_fmt,heapitem,heapitem->size,
                    IS_F_HEAPITEM_USED(heapitem)?"used":"free",heapitem->allocid);
            }
        }
        wind_mutex_unlock(heap->mutex);
    }
    wind_print_space(6);
    return err;
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
    size = HEAP_ALIGN_R(size);
    foreach_node(dnode,&heaplist)
    {
        heap = NODE_TO_HEAP(dnode);
        wind_debug("malloc in heap:0x%x",heap);
        if(!IS_F_HEAP_PRIVATE(heap))
        {
            ptr = wind_heap_malloc(heap, size);
            if(ptr)
            {
                wind_debug("wind_malloc:ptr=%p,size:%d",ptr,size);
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
        item = (w_heapitem_s*)ITEM_FROM_PTR(ptr);
        item->allocid = allocid;
    }
    return ptr;
}

void *wind_salloc(const char *str,w_uint8_t allocid)
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
    wind_debug("wind_free:ptr=%p",ptr);
    item = ITEM_FROM_PTR(ptr);
    //if(item->magic != WIND_HEAPITEM_MAGIC)
    //    item->magic = item->magic;
    WIND_ASSERT_RETURN(item->magic == WIND_HEAPITEM_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_HEAPITEM_USED(item),W_ERR_INVALID);
    return wind_heap_free(item->heap,ptr);
}


void *wind_realloc(void *ptr, w_uint32_t newsize)
{
    void *pnew;
	w_heapitem_s *olditm,*newitm;
	w_uint32_t cpsize;
    //w_heap_s* heap;
    //w/_dnode_s *dnode;
    do
    {
        pnew = W_NULL;
        wind_debug("wind_realloc:0x%0x,%d",ptr,newsize);
        newsize = HEAP_ALIGN_R(newsize);
		pnew = wind_malloc(newsize);
        if ((ptr == W_NULL) || (pnew == W_NULL))
		{
			break;
		}
		olditm = (w_heapitem_s*)ITEM_FROM_PTR(ptr);
		newitm = (w_heapitem_s*)ITEM_FROM_PTR(pnew);
		cpsize = olditm->size < newitm->size?olditm->size : newitm->size;
		cpsize -= WIND_HEAP_ITEM_SIZE;
		wind_memcpy(pnew,ptr,cpsize);

	}while(0);
	if(ptr != W_NULL)
		wind_free(ptr);

    return pnew;
}


void *wind_calloc(w_uint32_t count, w_uint32_t size)
{
    void *ptr;
    w_uint32_t tot_size;
    WIND_ASSERT_RETURN(count > 0,W_NULL);
    WIND_ASSERT_RETURN(size > 0,W_NULL);
    size = HEAP_ALIGN_R(size);
    wind_debug("wind_calloc:%d,%d",count,size);
    tot_size = count *size;
    ptr = wind_malloc(tot_size);
    WIND_ASSERT_RETURN(ptr != W_NULL,W_NULL);
    wind_memset(ptr, 0, tot_size);
    return ptr;
}




#endif // #if WIND_HEAP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
