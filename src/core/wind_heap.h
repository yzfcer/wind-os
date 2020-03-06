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
** 描　述: 
**
** 备  注：不要在中断服务例程中分配或释放动态内存块
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_HEAP_H__
#define WIND_HEAP_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_stati.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_HEAP_SUPPORT > 0


#define WIND_HEAP_MIN_SIZE    256
#define WIND_HEAP_MAGIC        0x0a5e8749
#define WIND_HEAPITEM_MAGIC    0xA1ea

#define HEAP_ALIGN_R(size) ((((size) + 7) >> 3) << 3)
#define HEAP_ALIGN_L(size) (((size) >> 3) << 3)
#define WIND_HEAP_HEAD_SIZE  HEAP_ALIGN_R(sizeof(w_heap_s))
#define WIND_HEAP_ITEM_SIZE  HEAP_ALIGN_R(sizeof(w_heapitem_s))



#define F_HEAP_PRIVATE (0x01 << 0) //标记heap对象是否通过内存池分配
#define IS_F_HEAP_PRIVATE(heap) ((heap->obj.flag & F_HEAP_PRIVATE) == F_HEAP_PRIVATE)
#define SET_F_HEAP_PRIVATE(heap) (heap->obj.flag |= F_HEAP_PRIVATE)
#define CLR_F_HEAP_PRIVATE(heap) (heap->obj.flag &= (~F_HEAP_PRIVATE))


#define F_HEAPITEM_USED (0x01 << 0) //标记heapitem对象是否已经被分配
#define IS_F_HEAPITEM_USED(heapitem) ((heapitem->flag & F_HEAPITEM_USED) == F_HEAPITEM_USED)
#define SET_F_HEAPITEM_USED(heapitem) (heapitem->flag |= F_HEAPITEM_USED)
#define CLR_F_HEAPITEM_USED(heapitem) (heapitem->flag &= (~F_HEAPITEM_USED))

typedef enum
{
    HP_ALLOCID_COMMON = 0,
    HP_ALLOCID_VFS    = 1,
    HP_ALLOCID_LISTFS = 2,
    HP_ALLOCID_TREEFS = 3,
    HP_ALLOCID_HOSTFS = 4,
    HP_ALLOCID_DB     = 5,
    HP_ALLOCID_CJSON  = 6,
    HP_ALLOCID_ALL    = 255,
}w_allocid_e;

typedef struct __w_heapitem_s w_heapitem_s;
typedef struct __w_heap_s w_heap_s;
struct __w_heapitem_s
{
    w_uint16_t magic;
    w_uint8_t  flag;
    w_uint8_t  allocid;
    w_uint32_t size;
    w_heap_s *heap;
    w_prinode_s itemnode;
};


struct __w_heap_s
{
    w_obj_s   obj;
    w_addr_t  addr;
    w_stati_s stati;
    w_dlist_s used_list;
    w_dlist_s free_list;
    void *mutex; 
    //w_uint32_t pad; 
};


w_err_t _wind_heap_mod_init(void);

w_heap_s *wind_heap_get(const char *name);

w_heap_s *wind_heap_create(const char *name,w_addr_t base,w_uint32_t size,w_uint16_t flag);

w_err_t wind_heap_destroy(w_heap_s *heap);

w_err_t wind_heap_setflag(w_heap_s *heap,w_int16_t flag);
w_err_t wind_heap_clrflag(w_heap_s *heap,w_int16_t flag);


void *wind_heap_malloc(w_heap_s* heap, w_uint32_t size);

void *wind_heap_realloc(w_heap_s* heap, void* ptr, w_uint32_t newsize);

w_err_t wind_heap_free(w_heap_s* heap,void *ptr);

w_err_t wind_heap_print(void);

w_err_t wind_heapitem_print(w_allocid_e allocid);

w_err_t wind_heap_stati_print(void);


void *wind_malloc(w_uint32_t size);

void *wind_zalloc(w_uint32_t size,w_uint8_t allocid);

void *wind_alloc(w_uint32_t size,w_uint8_t flag);

void *wind_salloc(char *str,w_uint8_t allocid);

void *wind_clone(void *object,w_uint32_t size);

void *wind_calloc(w_uint32_t count, w_uint32_t size);

void *wind_realloc(void *rmem, w_uint32_t newsize);

w_err_t wind_free(void *ptr);



#endif

#ifdef __cplusplus
}
#endif


#endif
