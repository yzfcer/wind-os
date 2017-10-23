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
#include "wind_err.h"
#include "wind_mpool.h"
#include "wind_assert.h"

#define WIND_MPOOL_ALIGN_R(x) (((x)+3) & (~0x03))
#define WIND_MPOOL_ALIGN_L(x) ((x) & (~0x03))

#define WIND_MPOOL_DEBUG(...) 


w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t itemsize)
{
    w_uint32_t i,si;
    ppool_item_s item;
    ppool_s pm;
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(memsize > 0,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(itemsize > 0,ERR_INVALID_PARAM);
    pm = (ppool_s)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    if(pm != (ppool_s)mem)
        memsize -= (w_uint32_t)((w_uint32_t)pm - (w_uint32_t)mem);
    memsize = WIND_MPOOL_ALIGN_L(memsize);

    si = itemsize < sizeof(ppool_item_s)?sizeof(ppool_item_s):itemsize;
    si = WIND_MPOOL_ALIGN_R(si);
    if(memsize < sizeof(pool_s) + si)
        return -1;
    
    item = (ppool_item_s)(sizeof(pool_s) + (w_uint32_t)pm);
    pm->head = item;
    pm->name = name;
    pm->size = memsize - sizeof(pool_s);
    pm->itemsize = si + sizeof(pool_item_s);
    pm->itemnum = pm->size / pm->itemsize;
    pm->used = 0;
    pm->free_head = (ppool_item_s)pm->head;
    item = (ppool_item_s)pm->head;
    for(i = 0;i < pm->itemnum;i ++)
    {
        pm->free_end = item;
        item->flag = POOL_BLK_FREE;
        item->next = (ppool_item_s)(pm->itemsize + (w_uint32_t)item);
        item = item->next;
    }
    item->next = NULL;
    pm->magic = WIND_MPOOL_MAGIC;
    return ERR_OK;
}


void *wind_pool_alloc(void *mem)
{
    void *p;
    ppool_s pm;
    ppool_item_s item;
    WIND_ASSERT_RETURN(mem != NULL,NULL);
    pm = (ppool_s)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    WIND_ASSERT_RETURN(pm->magic == WIND_MPOOL_MAGIC,NULL);

    wind_close_interrupt();
    if(pm->free_head == NULL)
    {
        wind_open_interrupt();
        WIND_ERROR("mpool empty\r\n");
        return NULL;
    }
    item = pm->free_head;
    if(item->flag != POOL_BLK_FREE)
    {
        wind_open_interrupt();
        WIND_ERROR("mpool ERROR\r\n");
        return NULL;
    }
    pm->free_head = item->next;
    pm->used ++;
    if(pm->free_head == NULL)
        pm->free_end = NULL;
    
    item->flag = POOL_BLK_USED;
    item->next = NULL;
    p = (void*)(sizeof(ppool_item_s) + (w_uint32_t)item);    
    wind_open_interrupt();
    return p;    
}

w_err_t wind_pool_free(void *mem,void *block)
{
    ppool_s pm;
    ppool_item_s item;
    
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(block != NULL,ERR_NULL_POINTER);    
    pm = (ppool_s)WIND_MPOOL_ALIGN_R((w_uint32_t)mem);
    WIND_ASSERT_RETURN(pm->magic == WIND_MPOOL_MAGIC,ERR_INVALID_PARAM);
    item = (ppool_item_s)(((w_uint32_t)block) - sizeof(ppool_item_s));
    WIND_ASSERT_RETURN(item->flag == POOL_BLK_USED,ERR_INVALID_PARAM);
    wind_close_interrupt();
    item->flag = POOL_BLK_FREE;
    item->next = NULL;
    if(pm->free_end == NULL)
    {
        pm->free_head = (ppool_item_s)item;
        pm->free_end = (ppool_item_s)item;
    }
    else
    {
        pm->free_end->next = (ppool_item_s)item;
        pm->free_end = (ppool_item_s)item;
    }
    pm->used --;
    wind_open_interrupt();
    return ERR_OK;
}

WIND_MPOOL(mpooltest,10,8);
void wind_mpool_test(void)
{
    void *p1,*p2;
    int i;
    wind_pool_create("testpool",mpooltest,sizeof(mpooltest),8);
    wind_pool_print("testpool",mpooltest);
    for(i = 0;i < 20;i ++)
    {
        p1 = wind_pool_alloc(mpooltest);
        WIND_MPOOL_DEBUG("mpool alloc p1:%d\r\n",p1);
        p2 = wind_pool_alloc(mpooltest);
        WIND_MPOOL_DEBUG("mpool alloc p2:%d\r\n",p2);
        wind_pool_free(mpooltest,p1);
        WIND_MPOOL_DEBUG("mpool free p1\r\n",p1);
        wind_pool_free(mpooltest,p2);
        WIND_MPOOL_DEBUG("mpool free p2\r\n",p2);
        wind_pool_free(mpooltest,p2);
    }
}

w_err_t wind_pool_print(w_int8_t *name,void *pool)
{
    ppool_s pm = pool;
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pool != NULL,ERR_NULL_POINTER);    
    wind_printf("pool name:%s\r\n",name);
    wind_printf("pool head:%d\r\n",pm->head);
    wind_printf("pool lenth:%d\r\n",pm->size);
    wind_printf("pool num:%d\r\n",pm->itemnum);
    wind_printf("pool itemsize:%d\r\n\r\n",pm->itemsize);
    return ERR_OK;
}


