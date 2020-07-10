/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_pool.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os functions related to memory pool management
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.11.27
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.11.27
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_diagnose.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define NODE_TO_POOL(node) NODEPTR_TO_ITEMPTR(node,w_pool_s,obj.objnode)
#define WIND_MPOOL_ALIGN_R(x) (((x)+7) & (~0x07))
#define WIND_MPOOL_ALIGN_L(x) ((x) & (~0x07))

#define WIND_MPOOL_DEBUG(...) 
static w_dlist_s poollist;



#if WIND_DIAGNOSE_SUPPORT

static w_int32_t poolitem_diagnose(w_pool_s *pool)
{
    w_int32_t i;
    w_poolitem_s *poolitem;
    w_uint16_t magic = 0;


    for(i = 0;i < pool->itemnum;i ++)
    {
        poolitem = (w_poolitem_s *)((w_addr_t)pool->head + i * pool->itemsize);
        if(magic == 0)
            magic = poolitem->head.magic;
        if((poolitem->head.magic != (w_uint16_t)(~WIND_POOLITEM_MAGIC)) &&
            (poolitem->head.magic != (w_uint16_t)WIND_POOLITEM_MAGIC))
        {
            wind_error("poolitem magic error,ptr=0x%x",poolitem);
            return DIAG_RES_OBJ_MAGIC_ERROR;
        }
    }
    return DIAG_RES_OK;
}

static w_int32_t pool_diagnose(void)
{
    w_int32_t res;
    w_dnode_s *dnode;
    w_pool_s *pool;
    w_dlist_s *list = &poollist;
    wind_disable_switch();
    //foreach_node(dnode,list)
    //{
    //    pool = NODE_TO_POOL(dnode);
    //    wind_notice("pool:%-16s start:0x%08x,size:0x%08x",wind_obj_name(pool),
    //                pool,pool->size+sizeof(w_pool_s));
    //}
        
    foreach_node(dnode,list)
    {
        pool = NODE_TO_POOL(dnode);
        if(pool->obj.magic != WIND_POOL_MAGIC)
        {
            wind_enable_switch();
            wind_error("pool magic error,ptr=0x%x",pool);
            return DIAG_RES_OBJ_MAGIC_ERROR;
        }
        res = poolitem_diagnose(pool);
        if(res != DIAG_RES_OK)
        {
            wind_enable_switch();
            wind_notice("check pool %-16s [ERROR]",pool->obj.name);
            return res;
        }
        else
        {
            wind_notice("check pool %-16s [OK]",pool->obj.name);
        }
    }
    wind_enable_switch();
    return DIAG_RES_OK;
}

w_err_t _wind_pool_diagnose_init(void)
{
    w_err_t err;
    DIAGNOSENOSE_DEF(pool,pool_diagnose);
    err = wind_diagnose_register(DIAGNOSENOSE(pool));
    return err;
}
#else
w_err_t _wind_pool_diagnose_init(void)
{
    return W_ERR_OK;
}
#endif

w_err_t _wind_pool_mod_init(void)
{
    DLIST_INIT(poollist);
    return W_ERR_OK;
}

w_pool_s *wind_pool_get_by_mem(void *mem)
{
    w_pool_s *pm;
    WIND_ASSERT_RETURN(mem != W_NULL,W_NULL);
    pm = (w_pool_s *)WIND_MPOOL_ALIGN_R((w_addr_t)mem);
    return pm;
}

w_pool_s *wind_pool_get(const char *name)
{
    return (w_pool_s *)wind_obj_get(name,&poollist);
}

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t obj_size)
{
    w_int32_t i;
    w_poolitem_s* item;
    w_pool_s *pm;
    wind_notice("create pool:%s",name?name:"null");
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(obj_size > 0,W_ERR_INVALID);
    obj_size = WIND_MPOOL_ALIGN_R(obj_size);
    WIND_ASSERT_RETURN(memsize >= sizeof(w_pool_s)+obj_size+sizeof(w_pihead_s),W_ERR_INVALID);
    memsize = WIND_MPOOL_ALIGN_L(memsize - 8);

    item = (w_poolitem_s*)((w_addr_t)pm + sizeof(w_pool_s));
    pm->head = item;
    pm->size = memsize - sizeof(w_pool_s);
    pm->itemsize = obj_size + sizeof(w_pihead_s);
    pm->itemnum = pm->size / pm->itemsize;
    pm->free_head = (w_poolitem_s*)pm->head;
    item = (w_poolitem_s*)pm->head;
    for(i = 0;i < pm->itemnum;i ++)
    {
        pm->free_end = item;
        item->head.magic = (~WIND_POOLITEM_MAGIC);
        item->head.flag = 0;
        CLR_F_POOLITEM_USED(item);
        item->head.next = (w_poolitem_s*)((w_addr_t)item + pm->itemsize);
        if(i < pm->itemnum - 1)
            item = item->head.next;
        else
            item->head.next = (w_poolitem_s*)W_NULL;
    }
    WIND_STATI_INIT(pm->stati,pm->itemnum);

    wind_obj_init(&pm->obj,WIND_POOL_MAGIC,name,&poollist);
    return W_ERR_OK;
}

w_err_t wind_pool_destroy(void *mem)
{
    w_err_t err;
    w_pool_s *pm;
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_ERR_NULL_PTR);
    wind_notice("destroy pool:%s",wind_obj_name(&pm->obj));
    err = wind_obj_deinit(&pm->obj,WIND_POOL_MAGIC,&poollist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    return W_ERR_OK;
}

void *wind_pool_malloc(void *mem)
{
    void *p;
    w_pool_s *pm;
    w_poolitem_s* item;
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(pm->obj.magic == WIND_POOL_MAGIC,W_NULL);

    wind_disable_interrupt();
    if(pm->free_head == W_NULL)
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_enable_interrupt();
        wind_warn("pool empty");
        return W_NULL;
    }
    item = pm->free_head;
    pm->free_head = pm->free_head->head.next;
    if(pm->free_head == W_NULL)
        pm->free_end = (w_poolitem_s*)W_NULL;
    
    if(IS_F_POOLITEM_USED(item))
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_enable_interrupt();
        wind_critical("pool is NOT free.");
        return W_NULL;
    }
    WIND_STATI_INC(pm->stati);
    item->head.magic = WIND_POOLITEM_MAGIC;
    SET_F_POOLITEM_USED(item);
    item->head.next = (w_poolitem_s*)W_NULL;
    p = (void*)(item->buff);
    wind_enable_interrupt();
    return p;
}

w_err_t wind_pool_free(void *mem,void *block)
{
    w_pool_s *pm;
    w_poolitem_s* item;
    
    pm = wind_pool_get_by_mem(mem);
    WIND_ASSERT_RETURN(pm != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(block != W_NULL,W_ERR_NULL_PTR);    
    item = (w_poolitem_s*)POOL_ITEM(block,w_poolitem_s,buff);
    WIND_ASSERT_RETURN(item->head.magic == WIND_POOLITEM_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(IS_F_POOLITEM_USED(item),W_ERR_INVALID);
    wind_disable_interrupt();
    item->head.magic = (w_uint16_t)(~WIND_POOLITEM_MAGIC);
    CLR_F_POOLITEM_USED(item);
    item->head.next = (w_poolitem_s*)W_NULL;
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

w_err_t wind_pool_print_list(void)
{
    return wind_obj_print_list(&poollist);
}

void wind_pool_print_detail(void)
{
    w_dnode_s *dnode;
    w_pool_s *pm;
    w_dlist_s *list = &poollist;
    wind_printf("\r\n\r\nmemory pool list:\r\n");
    wind_print_space(7);
    wind_printf("%-12s %-12s %-8s %-8s %-8s\r\n","name","head","size","itemnum","itemsize");
    wind_print_space(7);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        pm = NODE_TO_POOL(dnode);
        wind_printf("%-12s 0x%-10x %-8d %-8d %-8d\r\n",
            wind_obj_name(&pm->obj),pm->head,pm->size,pm->itemnum,pm->itemsize);
    }
    wind_enable_switch();
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
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        pool = NODE_TO_POOL(dnode);
        wind_printf("%-16s %-8d %-8d %-8d %-8d\r\n",pool->obj.name,pool->stati.tot,
            pool->stati.used,pool->stati.max,pool->stati.err);
    }
    wind_enable_switch();
    wind_print_space(7);
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus


