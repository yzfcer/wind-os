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
#include "wind_var.h"
#include "wind_core.h"
#include "wind_string.h"

#define WIND_MPOOL_ALIGN_R(x) (((x)+3) & (~0x03))
#define WIND_MPOOL_ALIGN_L(x) ((x) & (~0x03))

#define WIND_MPOOL_DEBUG(...) 
static w_dlist_s poollist;

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

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t itemsize)
{
    w_uint32_t i,si;
    w_pool_item_s* item;
    w_pool_s *pm;
    wind_notice("create pool:%s",name?name:"null");
    WIND_ASSERT_RETURN(mem != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(memsize > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(itemsize > 0,W_ERR_INVALID);
    
    pm = (w_pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    if(pm != (w_pool_s *)mem)
        memsize -= (w_uint32_t)((w_uint32_t)pm - (w_uint32_t)mem);
    memsize = WIND_MPOOL_ALIGN_L(memsize);

    si = itemsize < sizeof(w_pool_item_s*)?sizeof(w_pool_item_s*):itemsize;
    si = WIND_MPOOL_ALIGN_R(si);
    if(memsize < sizeof(w_pool_s) + si)
        return -1;
    item = (w_pool_item_s*)(sizeof(w_pool_s) + (w_uint32_t)pm);
    pm->head = item;
    pm->name = name;
    DNODE_INIT(pm->poolnode);
    pm->size = memsize - sizeof(w_pool_s);
    pm->itemsize = si + sizeof(w_pool_item_s);
    pm->itemnum = pm->size / pm->itemsize;
    pm->free_head = (w_pool_item_s*)pm->head;
    item = (w_pool_item_s*)pm->head;
    for(i = 0;i < pm->itemnum;i ++)
    {
        pm->free_end = item;
        item->flag = POOL_BLK_FREE;
        item->next = (w_pool_item_s*)(pm->itemsize + (w_uint32_t)item);
        item = item->next;
    }
    item->next = W_NULL;
    WIND_STATI_INIT(pm->stati,pm->itemnum);
    wind_disable_interrupt();
    dlist_insert_tail(&poollist,&pm->poolnode);
    wind_enable_interrupt();
    pm->magic = WIND_POOL_MAGIC;
    //_wind_pool_print_list(&g_core.poollist);
    return W_ERR_OK;
}

w_err_t wind_pool_destroy(void *mem)
{
    w_pool_s *pm;
    WIND_ASSERT_RETURN(mem != W_NULL,W_ERR_PTR_NULL);
    pm = (w_pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
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
    w_pool_item_s* item;
    WIND_ASSERT_RETURN(mem != W_NULL,W_NULL);
    pm = (w_pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
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
    WIND_ASSERT_RETURN(item != W_NULL,W_NULL);
    if(item->flag != POOL_BLK_FREE)
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_enable_interrupt();
        wind_error("mpool is NOT free.");
        return W_NULL;
    }
    pm->free_head = item->next;
    WIND_STATI_INC(pm->stati);
    if(pm->free_head == W_NULL)
        pm->free_end = W_NULL;
    
    item->flag = POOL_BLK_USED;
    item->next = W_NULL;
    p = (void*)(sizeof(w_pool_item_s*) + (w_uint32_t)item);
    wind_enable_interrupt();
    return p;
}

w_err_t wind_pool_free(void *mem,void *block)
{
    w_pool_s *pm;
    w_pool_item_s* item;
    
    WIND_ASSERT_RETURN(mem != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(block != W_NULL,W_ERR_PTR_NULL);    
    pm = (w_pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    WIND_ASSERT_RETURN(pm->magic == WIND_POOL_MAGIC,W_ERR_INVALID);
    item = (w_pool_item_s*)(((w_uint32_t)block) - sizeof(w_pool_item_s*));
    WIND_ASSERT_RETURN(item->flag == POOL_BLK_USED,W_ERR_INVALID);
    wind_disable_interrupt();
    item->flag = POOL_BLK_FREE;
    item->next = W_NULL;
    if(pm->free_end == W_NULL)
    {
        pm->free_head = (w_pool_item_s*)item;
        pm->free_end = (w_pool_item_s*)item;
    }
    else
    {
        pm->free_end->next = (w_pool_item_s*)item;
        pm->free_end = (w_pool_item_s*)item;
    }
    WIND_STATI_DEC(pm->stati);
    wind_enable_interrupt();
    return W_ERR_OK;
}


void _wind_pool_print_list(void)
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


