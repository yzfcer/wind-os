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
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"

#define WIND_MPOOL_ALIGN_R(x) (((x)+7) & (~0x07))
#define WIND_MPOOL_ALIGN_L(x) ((x) & (~0x07))

#define WIND_MPOOL_DEBUG(...) 
static w_dlist_s poollist;


w_pool_s *wind_pool_get_by_mem(void *mem)
{
    w_pool_s *pm;
    WIND_ASSERT_RETURN(mem != W_NULL,W_NULL);
    pm = (w_pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    return pm;
}

w_err_t _wind_pool_mod_init(void)
{
    DLIST_INIT(poollist);
    return W_ERR_OK;
}

w_pool_s *wind_pool_get(const char *name)
{
    w_pool_s *pool;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&poollist)
    {
        pool = DLIST_OBJ(dnode,w_pool_s,poolnode);
        if(pool->name && (wind_strcmp(name,pool->name) == 0))
        {
            wind_enable_switch();
            return pool;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t obj_size)
{
    w_uint32_t i;
    w_poolitem_s* item;
    w_pool_s *pm;
    wind_notice("create pool:%s",name?name:"null");
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(obj_size > 0,W_ERR_INVALID);
    obj_size = WIND_MPOOL_ALIGN_R(obj_size);
    WIND_ASSERT_RETURN(memsize >= sizeof(w_pool_s)+obj_size+sizeof(w_pihead_s),W_ERR_INVALID);
    memsize = WIND_MPOOL_ALIGN_L(memsize - 8);

    item = (w_poolitem_s*)((w_uint32_t)pm + sizeof(w_pool_s));
    pm->magic = 0;
    pm->head = item;
    pm->name = name;
    DNODE_INIT(pm->poolnode);
    pm->size = memsize - sizeof(w_pool_s);
    pm->itemsize = obj_size + sizeof(w_pihead_s);
    pm->itemnum = pm->size / pm->itemsize;
    pm->free_head = (w_poolitem_s*)pm->head;
    item = (w_poolitem_s*)pm->head;
    for(i = 0;i < pm->itemnum;i ++)
    {
        pm->free_end = item;
        item->head.magic = 0;
        item->head.flag = 0;
        CLR_F_POOLITEM_USED(item);
        item->head.next = (w_poolitem_s*)((w_addr_t)item + pm->itemsize);
        if(i < pm->itemnum - 1)
            item = item->head.next;
        else
            item->head.next = W_NULL;
    }
    WIND_STATI_INIT(pm->stati,pm->itemnum);
    wind_disable_interrupt();
    dlist_insert_tail(&poollist,&pm->poolnode);
    wind_enable_interrupt();
    pm->magic = WIND_POOL_MAGIC;
    return W_ERR_OK;
}

w_err_t wind_pool_destroy(void *mem)
{
    w_pool_s *pm;
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pm->magic == WIND_POOL_MAGIC,W_ERR_INVALID);
    wind_notice("destroy pool:%s",pm->name?pm->name:"null");
    wind_disable_interrupt();
    dlist_remove(&poollist,&pm->poolnode);
    wind_enable_interrupt();
    pm->magic = 0;
    return W_ERR_OK;
}

void *wind_pool_malloc(void *mem)
{
    void *p;
    w_pool_s *pm;
    w_poolitem_s* item;
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(pm->magic == WIND_POOL_MAGIC,W_NULL);

    wind_disable_interrupt();
    if(pm->free_head == W_NULL)
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_enable_interrupt();
        wind_error("mpool empty");
        return W_NULL;
    }
    item = pm->free_head;
    pm->free_head = pm->free_head->head.next;
    if(pm->free_head == W_NULL)
        pm->free_end = W_NULL;
    
    if(IS_F_POOLITEM_USED(item))
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_enable_interrupt();
        wind_critical("mpool is NOT free.");
        return W_NULL;
    }
    WIND_STATI_INC(pm->stati);
    item->head.magic = WIND_POOLITM_MAGIC;
    SET_F_POOLITEM_USED(item);
    item->head.next = W_NULL;
    p = (void*)(item->buff);
    wind_enable_interrupt();
    return p;
}

w_err_t wind_pool_free(void *mem,void *block)
{
    w_pool_s *pm;
    w_poolitem_s* item;
    
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(block != W_NULL,W_ERR_PTR_NULL);    
    item = (w_poolitem_s*)POOL_ITEM(block,w_poolitem_s,buff);
    WIND_ASSERT_RETURN(item->head.magic == WIND_POOLITM_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_POOLITEM_USED(item),W_ERR_INVALID);
    wind_disable_interrupt();
    item->head.magic = 0;
    CLR_F_POOLITEM_USED(item);
    item->head.next = W_NULL;
    if(pm->free_end == W_NULL)
    {
        pm->free_head = (w_poolitem_s*)item;
        pm->free_end = (w_poolitem_s*)item;
    }
    else
    {
        pm->free_end->head.next = (w_poolitem_s*)item;
        pm->free_end = (w_poolitem_s*)item;
    }
    WIND_STATI_DEC(pm->stati);
    wind_enable_interrupt();
    return W_ERR_OK;
}


void wind_pool_print_list(void)
{
    w_dnode_s *pdnode;
    w_pool_s *pm;
    w_dlist_s *list = &poollist;
    wind_printf("\r\n\r\nmpool list as following:\r\n");
    wind_print_space(7);
    wind_printf("%-12s %-12s %-8s %-8s %-8s\r\n","name","head","size","itemnum","itemsize");
    wind_print_space(7);
    foreach_node(pdnode,list)
    {
        pm = DLIST_OBJ(pdnode,w_pool_s,poolnode);
        wind_printf("%-12s 0x%-10x %-8d %-8d %-8d\r\n",
            pm->name?pm->name:"null",pm->head,pm->size,pm->itemnum,pm->itemsize);
    }
    wind_print_space(7);
}

void wind_pool_stati_print(void)
{
    w_dnode_s *dnode;
    w_pool_s *pool;
    w_dlist_s *list;
    wind_print_space(7);
    wind_printf("%-16s %-8s %-8s %-8s %-8s\r\n","pool","tot","used","maxused","err");
    wind_print_space(7);
    list = &poollist;
    foreach_node(dnode,list)
    {
        pool = (w_pool_s*)DLIST_OBJ(dnode,w_pool_s,poolnode);
        wind_printf("%-16s %-8d %-8d %-8d %-8d\r\n",pool->name,pool->stati.tot,
            pool->stati.used,pool->stati.max,pool->stati.err);
    }
    wind_print_space(7);
}


