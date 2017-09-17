/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mem.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 内存管理相关的代码头文件
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
#ifndef WIND_MEM_H__
#define WIND_MEM_H__



#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
#if WIND_MEM_SUPPORT > 0


typedef struct __memhaed_s
{
    struct __memhaed_s *pre;//前一块分配区的head指针位置
    struct __memhaed_s *next;//后一块分配区的head指针的位置
    w_uint32_t lenth;//当前分配区的长度
} memhead_s,*pmemhead_s;

typedef struct __meminfo_s
{
    w_uint32_t base;//自由空间的基地址
    w_uint32_t lenth;//自由空间的总长度    
    w_uint32_t offset;//当前的空间偏移量
    //w_uint32_t used;//已经分配的空间的总和
    pmemhead_s plast;//最后的一次分配的指针的位置
    pmemhead_s pfree;
}meminfo_s,*pmeminfo_s;

#define MEM_ALIGN_4 1 //定义内存空间以4字节对齐

w_err_t wind_mem_init(w_uint8_t num,w_uint32_t base,w_uint32_t lenth);

//w_err_t wind_memset(void *p,w_uint32_t value,w_uint32_t len);

//w_err_t wind_memcpy(void *dest,const void *src,w_uint32_t len);


//以下三个函数是关于内存分配的函数，但鉴于内存管理的代码还没有建立
//因此，内存分配只是作为将来的一种方案

void *wind_malloc(w_uint32_t size);

void **wind_calloc(w_uint32_t block,w_uint32_t size);

void ***wind_talloc(w_uint32_t num,w_uint32_t block,w_uint32_t size);

void wind_free(void *p);
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //WIND_MEM_SUPPORT
#endif //WIND_MEM_H__

