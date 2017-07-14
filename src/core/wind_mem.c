/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mem.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 内存管理相关的代码（未完待续）
**备        注: 嵌入式系统的内存空间容量本身就有限，并且都会对时效性有要求，因此建议除非是特别必要，
**              否则尽量不要用动态分配的方式，同时，使用完成后要尽快释放，否则会产生较多的碎片，会
**              影响系统的性能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_types.h"
#include "wind_mem.h"
#include "wind_debug.h"
#include "wind_os_hwif.h"
#include "wind_err.h"
#include "wind_heap.h"
#include "wind_assert.h"
#if WIND_MEM_SUPPORT > 0





meminfo_s g_mem_for_allcoc[] = 
{
    {
        0x30100000,
        0x4000000,
        0,
        NULL,
        NULL
    }
};

w_err_t wind_mem_init(w_uint8_t ix,w_uint32_t base,w_uint32_t lenth)
{
    WIND_ASSERT_RETURN((lenth > 0),ERR_COMMAN);
    g_mem_for_allcoc[ix].base = base;
    g_mem_for_allcoc[ix].lenth = lenth;
    g_mem_for_allcoc[ix].offset = 0;
    g_mem_for_allcoc[ix].plast = NULL;
    g_mem_for_allcoc[ix].pfree = NULL;
    return ERR_OK;
    
}

//从已经回收的空间中找到一块
static void *core_get_free_space(w_uint32_t size)
{
    return NULL;
}

//分配一个连续的空间，如果分配成功，就会返回空间指针，失败返回NULL
void *wind_malloc(w_uint32_t size)
{
    w_uint32_t si;
    void *p;
    pmemhead_s phead;
#if MEM_ALIGN_4
    si = (size + 3) & 0xfffffffc;
#else
    si = size;
#endif
    wind_close_interrupt();
    //这里如果有多个不连续的自由空间，可能还需要改进
    //phead = (pmemhead_s)(g_mem_for_allcoc[0].base + g_mem_for_allcoc[0].offset);
    phead = (pmemhead_s)wind_heap_alloc_default(si + sizeof(memhead_s));
    //因为前面的初始空间已经分配完了，因此，可以开始考虑从回收的空间中分配新的空间
    //如果从回收的空间中分配了新的空间，那么分配可以成功，否则分配将失败
    //phead = core_get_free_space(si + sizeof(memhead_s));
    if(!phead)
    {
        wind_open_interrupt();
        return NULL;
    }
    
    p = (void*)((phead) + sizeof(memhead_s));
    phead->pre = g_mem_for_allcoc[0].plast;
    phead->next = NULL;
    phead->lenth = si + sizeof(memhead_s);
    if(g_mem_for_allcoc[0].plast)
        g_mem_for_allcoc[0].plast->next = phead;
    g_mem_for_allcoc[0].plast = phead;
    wind_open_interrupt();
    return p;
}

//分配一个二维数组指针，但分配的空间不能直接作为一维指针使用
void **wind_calloc(w_uint32_t block,w_uint32_t size)
{
    w_uint32_t si,i,base;
    void **p;
#if MEM_ALIGN_4
    si = (size + 3) & 0xfffffffc;
#else
    si = size;
#endif
    p = (void **)wind_malloc(block * sizeof(void *) + block * si);
    if(p == NULL)
        return NULL;
    base = block * sizeof(void *) + (w_uint32_t)p;
    for(i = 0;i < block;i ++)
    {
        p[i] = (void *)(base + si * i);
    }
    return p;
}


//分配一个三维数组指针，但分配的空间不能直接作为一维指针使用
void ***wind_talloc(w_uint32_t num,w_uint32_t block,w_uint32_t size)
{
    w_uint32_t si,i,j,base;
    
    void ***p;
#if MEM_ALIGN_4
    si = (size + 3) & 0xfffffffc;
#else
    si = size;
#endif
    p = (void ***)wind_malloc(num * sizeof(void **) + num * block * sizeof(void *) + num * block * si);
    if(p == NULL)
        return NULL;
    base = num * sizeof(void **) + num * block * sizeof(void *) + (w_uint32_t)p;
    for(i = 0;i < num;i ++)
    {
        p[i] = (void **)(base + num * sizeof(void **) + i * block * sizeof(void *));
        for(j = 0;j < block;j ++)
        {
            p[i][j] = (void *)(base + i * block * si + j *si);
        }
    }
    return p;
}

//为系统回收空间，并把相邻的空间连接成连续整片的空间,这个函数有些困难啊，唉。。。
void wind_free(void *p)
{
    pmemhead_s phead,ph1,ph2;
    if(p == NULL || (w_uint32_t)p < g_mem_for_allcoc[0].base 
        || (w_uint32_t)p > (g_mem_for_allcoc[0].base + g_mem_for_allcoc[0].lenth))
        return;
    phead = (pmemhead_s)(((w_uint32_t)p) - sizeof(memhead_s));
    wind_close_interrupt();
    if(g_mem_for_allcoc[0].pfree == NULL)
    {
        g_mem_for_allcoc[0].pfree = phead;
        phead->next = NULL;
        phead->pre = NULL;
        wind_open_interrupt();
        return;        
    }
    else 
    {
        ph1 = NULL;
        ph2 = g_mem_for_allcoc[0].pfree;
        //找到返回的块合适的位置
        while(ph2 != NULL)
        {
            if(phead < ph2)
            {
                ph1 = ph2;
                ph2 = ph2->next;
            }
            else
                break;
        }
        //将内存块连接到链表中
        if(ph1 == NULL)
        {
            g_mem_for_allcoc[0].pfree = phead;
            phead->pre = NULL;
        }
        else
        {
            ph1->next = phead;
            phead->pre = ph1;
        }
        
        
        phead->next = ph2;
        if(ph2 != NULL)
            ph2->pre = phead;
        //合并相邻的内存块
        if(ph1->lenth + sizeof(memhead_s) + (w_uint32_t)ph1 == (w_uint32_t)phead)
        {
            ph1->lenth += (phead->lenth + sizeof(memhead_s));
            ph1->next = ph2;
            ph2->pre = ph1;
            phead = ph1;
        }
        if(ph2 && (phead->lenth + sizeof(memhead_s) + (w_uint32_t)phead == (w_uint32_t)ph2))
        {
            phead->lenth += (ph2->lenth + sizeof(memhead_s));
            phead->next = ph2->next;
            ph2->next->pre = phead;
        }
    }
    wind_open_interrupt();
    return;
}

#endif //WIND_MEM_SUPPORT
