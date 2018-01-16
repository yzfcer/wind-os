/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mpool.h / wind_mpool.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的内存池管理相关的功能函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.11.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.11.27
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_mpool.h"
#include "wind_var.h"

#define WIND_MPOOL_ALIGN_R(x) (((x)+3) & (~0x03))
#define WIND_MPOOL_ALIGN_L(x) ((x) & (~0x03))

#define WIND_MPOOL_DEBUG(...) 


w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t itemsize)
{
    w_uint32_t i,si;
    pool_item_s* item;
    pool_s *pm;
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(memsize > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(itemsize > 0,ERR_INVALID_PARAM);
    pm = (pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    if(pm != (pool_s *)mem)
        memsize -= (w_uint32_t)((w_uint32_t)pm - (w_uint32_t)mem);
    memsize = WIND_MPOOL_ALIGN_L(memsize);

    si = itemsize < sizeof(pool_item_s*)?sizeof(pool_item_s*):itemsize;
    si = WIND_MPOOL_ALIGN_R(si);
    if(memsize < sizeof(pool_s) + si)
        return -1;
    item = (pool_item_s*)(sizeof(pool_s) + (w_uint32_t)pm);
    pm->head = item;
    pm->name = name;
    DNODE_INIT(pm->poolnode);
    pm->size = memsize - sizeof(pool_s);
    pm->itemsize = si + sizeof(pool_item_s);
    pm->itemnum = pm->size / pm->itemsize;
    pm->used = 0;
    pm->free_head = (pool_item_s*)pm->head;
    item = (pool_item_s*)pm->head;
    for(i = 0;i < pm->itemnum;i ++)
    {
        pm->free_end = item;
        item->flag = POOL_BLK_FREE;
        item->next = (pool_item_s*)(pm->itemsize + (w_uint32_t)item);
        item = item->next;
    }
    item->next = NULL;
    WIND_STATI_INIT(pm->stati,pm->itemnum);
    wind_close_interrupt();
    dlist_insert_tail(&g_core.poollist,&pm->poolnode);
    wind_open_interrupt();
    pm->magic = WIND_MPOOL_MAGIC;
    wind_pool_print_list(&g_core.poollist);
    return ERR_OK;
}


void *wind_pool_alloc(void *mem)
{
    void *p;
    pool_s *pm;
    pool_item_s* item;
    WIND_ASSERT_RETURN(mem != NULL,NULL);
    pm = (pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    WIND_ASSERT_RETURN(pm->magic == WIND_MPOOL_MAGIC,NULL);

    wind_close_interrupt();
    if(pm->free_head == NULL)
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_open_interrupt();
        wind_error("mpool empty");
        return NULL;
    }
    item = pm->free_head;
    if(item->flag != POOL_BLK_FREE)
    {
        WIND_STATI_ERR_INC(pm->stati);
        wind_open_interrupt();
        wind_error("mpool is NOT free.");
        return NULL;
    }
    pm->free_head = item->next;
    pm->used ++;
    WIND_STATI_INC(pm->stati);
    if(pm->free_head == NULL)
        pm->free_end = NULL;
    
    item->flag = POOL_BLK_USED;
    item->next = NULL;
    p = (void*)(sizeof(pool_item_s*) + (w_uint32_t)item);
    wind_open_interrupt();
    return p;
}

w_err_t wind_pool_free(void *mem,void *block)
{
    pool_s *pm;
    pool_item_s* item;
    
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(block != NULL,ERR_NULL_POINTER);    
    pm = (pool_s *)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    WIND_ASSERT_RETURN(pm->magic == WIND_MPOOL_MAGIC,ERR_INVALID_PARAM);
    item = (pool_item_s*)(((w_uint32_t)block) - sizeof(pool_item_s*));
    WIND_ASSERT_RETURN(item->flag == POOL_BLK_USED,ERR_INVALID_PARAM);
    wind_close_interrupt();
    item->flag = POOL_BLK_FREE;
    item->next = NULL;
    if(pm->free_end == NULL)
    {
        pm->free_head = (pool_item_s*)item;
        pm->free_end = (pool_item_s*)item;
    }
    else
    {
        pm->free_end->next = (pool_item_s*)item;
        pm->free_end = (pool_item_s*)item;
    }
    pm->used --;
    WIND_STATI_MINUS(pm->stati);
    wind_open_interrupt();
    return ERR_OK;
}


void wind_pool_print_list(dlist_s *list)
{
    dnode_s *pdnode;
    pool_s *pm;
    wind_printf("\r\n\r\nthread list as following:\r\n");
    wind_printf("-----------------------------------------------------------------\r\n");
    wind_printf("%-12s %-10s %-8s %-8s %-10s %-8s\r\n","name","head","size","itemnum","itemsize","used");
    wind_printf("-----------------------------------------------------------------\r\n");
    foreach_node(pdnode,list)
    {
        pm = DLIST_OBJ(pdnode,pool_s,poolnode);
        wind_printf("%-12s 0x%-8x %-8d %-10d %-8d %-8d\r\n",
            pm->name,pm->head,pm->size,pm->itemnum,pm->itemsize,pm->used);
     }
    wind_printf("-----------------------------------------------------------------\r\n");
}



