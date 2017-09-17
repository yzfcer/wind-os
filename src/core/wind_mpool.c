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

w_err_t wind_mpool_print(w_int8_t *name,void *pool)
{
    pmpoolHead_s pm = pool;
    WIND_ASSERT_RETURN(name != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pool != NULL,ERR_NULL_POINTER);    
    wind_printf("pool name:%s\r\n",name);
    wind_printf("pool head:%d\r\n",pm->head);
    wind_printf("pool lenth:%d\r\n",pm->len);
    wind_printf("pool num:%d\r\n",pm->num);
    wind_printf("pool itemsize:%d\r\n\r\n",pm->itemsize);
    return ERR_OK;
}
w_err_t wind_mpool_create(const char *name,void *mem,w_uint32_t msize,w_uint32_t itemsize)
{
    w_uint32_t i,si;
    ppool_s head;
    pmpoolHead_s pm;
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    si = itemsize < sizeof(ppool_s)?sizeof(ppool_s):itemsize;
    if((si & 0x03) != 0)
    {
        si = (((si + 3) >> 2) << 2);
    }
    if(msize < sizeof(mpoolHead_s) + si)
        return -1;
    WIND_MPOOL_DEBUG("mpool:%d,%d,%d,%d\r\n",mem,msize,itemsize,sizeof(mpoolHead_s));
    pm = (pmpoolHead_s)mem;
    head = (ppool_s)(sizeof(mpoolHead_s) + (w_uint32_t)mem);
    pm->head = head;
    pm->name = name;
    //WIND_MPOOL_DEBUG("pm:0x%x,pm->head:0x%x\r\n",pm,pm->head);
    pm->len = msize - sizeof(mpoolHead_s);
    pm->itemsize = si + sizeof(w_uint32_t);
    pm->num = pm->len / pm->itemsize;
    pm->used = 0;
    pm->free = (ppool_s)pm->head;
    head = (ppool_s)pm->head;
    for(i = 0;i < pm->num;i ++)
    {
        //WIND_MPOOL_DEBUG("head:0x%x\r\n",head);
        pm->last = head;
        head->next = (ppool_s)(pm->itemsize + (w_uint32_t)head);
        head = head->next;
    }
    head->next = NULL;
    
    pm->magic = WIND_MPOOL_MAGIC;
    //WIND_MPOOL_DEBUG("pm->itemsize:%d\r\n",pm->itemsize);
    //WIND_MPOOL_DEBUG("pm->num:%d\r\n",pm->num);
    //WIND_MPOOL_DEBUG("pm->len:%d\r\n",pm->len);
    WIND_MPOOL_DEBUG("pm->head:%d\r\n",pm->head);
    return ERR_OK;
}


void *wind_mpool_alloc(void *mem)
{
    pmpoolHead_s pm;
    void *p;
    ppool_s head;
    WIND_ASSERT_RETURN(mem != NULL,NULL);
    pm = (pmpoolHead_s)mem;
    if(pm->magic != WIND_MPOOL_MAGIC)
    {
        WIND_ERROR("mpool alloc magic err\r\n");
        return NULL;
    }
    wind_close_interrupt();
    if(pm->free == NULL)
    {
        wind_open_interrupt();
        WIND_ERROR("mpool empty\r\n");
        return NULL;
    }
    //WIND_MPOOL_DEBUG("pm->head:%d\r\n",pm->head);
    head = pm->free;
    p = head;
    pm->free = head->next;
    pm->used ++;
    if(pm->free == NULL)
        pm->last = NULL;
    *((w_uint32_t*)p) = WIND_MPOOL_BLOCK_MAGIC;
    p = (void*)(sizeof(w_uint32_t) + (w_uint32_t)p);    
    wind_open_interrupt();
    //WIND_MPOOL_DEBUG("mpool alloc-size:%d,addr:0x%x\r\n",pm->itemsize,p);
    return p;    
}

w_err_t wind_mpool_free(void *mem,void *block)
{
    pmpoolHead_s pm;
    //void *p;
    ppool_s pp;
    
    WIND_ASSERT_RETURN(mem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(block != NULL,ERR_NULL_POINTER);    
    pm = (pmpoolHead_s)mem;
    WIND_ASSERT_RETURN(pm->magic == WIND_MPOOL_MAGIC,pm->magic != WIND_MPOOL_MAGIC);
    pp = (ppool_s)(((w_uint32_t)block) - sizeof(w_uint32_t));
    //WIND_MPOOL_DEBUG("pp:%d\r\n",pp);
    if((((w_uint32_t)pp) - ((w_uint32_t)pm->head))%pm->itemsize != 0)
    {
        WIND_ERROR("mpool free pos err\r\n");
        return ERR_COMMAN;
    }
    //防止重复释放
    if((w_uint32_t)pp->next != WIND_MPOOL_BLOCK_MAGIC)
    {
        WIND_ERROR("mpool item magic err\r\n");
        return ERR_COMMAN;
    }
    wind_close_interrupt();
    if(pm->last == NULL)
        pm->last = (ppool_s)pp;
    else
    {
        pm->last->next = (ppool_s)pp;
        pm->last = (ppool_s)pp;
    }
    pm->name = NULL;
    *((w_uint32_t*)((w_uint32_t)block - sizeof(w_uint32_t))) = 0;
    wind_open_interrupt();
    return ERR_OK;
}

WIND_MPOOL(mpooltest,10,8);
void wind_mpool_test(void)
{
    void *p1,*p2;
    int i;
    wind_mpool_create("testpool",mpooltest,sizeof(mpooltest),8);
    wind_mpool_print("testpool",mpooltest);
    for(i = 0;i < 20;i ++)
    {
        p1 = wind_mpool_alloc(mpooltest);
        WIND_MPOOL_DEBUG("mpool alloc p1:%d\r\n",p1);
        p2 = wind_mpool_alloc(mpooltest);
        WIND_MPOOL_DEBUG("mpool alloc p2:%d\r\n",p2);
        wind_mpool_free(mpooltest,p1);
        WIND_MPOOL_DEBUG("mpool free p1\r\n",p1);
        wind_mpool_free(mpooltest,p2);
        WIND_MPOOL_DEBUG("mpool free p2\r\n",p2);
        wind_mpool_free(mpooltest,p2);
    }
}

