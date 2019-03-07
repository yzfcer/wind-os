/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_heap.h / wind_heap.c
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: ϵͳ���ڴ�ѵĶ�̬���亯��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 2013.11.03,��������ͨ��
** �衡��: 
**
** ��  ע����Ҫ���жϷ��������з�����ͷŶ�̬�ڴ��
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

#define __ALIGN_R(size) (((size + 7) >> 3) << 3)
#define __ALIGN_L(size) (((size) >> 3) << 3)
#define WIND_HEAP_SIZE         __ALIGN_R(sizeof(w_heapitem_s), WIND_HEAP_ALIGN_SIZE)



#define F_HEAP_PRIVATE (0x01 << 0) //���heap�����Ƿ�ͨ���ڴ�ط���
#define IS_F_HEAP_PRIVATE(heap) ((heap->obj.flag & F_HEAP_PRIVATE) == F_HEAP_PRIVATE)
#define SET_F_HEAP_PRIVATE(heap) (heap->obj.flag |= F_HEAP_PRIVATE)
#define CLR_F_HEAP_PRIVATE(heap) (heap->obj.flag &= (~F_HEAP_PRIVATE))


#define F_HEAPITEM_USED (0x01 << 0) //���heapitem�����Ƿ�ͨ���ڴ�ط���
#define IS_F_HEAPITEM_USED(heapitem) ((heapitem->flag & F_HEAPITEM_USED) == F_HEAPITEM_USED)
#define SET_F_HEAPITEM_USED(heapitem) (heapitem->flag |= F_HEAPITEM_USED)
#define CLR_F_HEAPITEM_USED(heapitem) (heapitem->flag &= (~F_HEAPITEM_USED))



typedef struct __w_heapitem_s w_heapitem_s;
typedef struct __w_heap_s w_heap_s;
struct __w_heapitem_s
{
    w_uint16_t magic;
    w_uint16_t flag;
    w_heap_s *heap;
    w_prinode_s itemnode;
    w_int32_t size;
};


struct __w_heap_s
{
    w_obj_s obj;
    void *addr;
    w_stati_s stati;
    w_dlist_s used_list;
    w_dlist_s free_list;
    void *mutex; 
    w_uint32_t pad; 
};

#define NODE_TO_HEAP(node) (w_heap_s*)(((char*)(node))-((w_uint32_t)&(((w_heap_s*)0)->obj.objnode)))
#define NODE_TO_HEAPITEM(node) (w_heapitem_s*)(((char*)(node))-((w_uint32_t)&(((w_heapitem_s*)0)->itemnode)))

w_err_t _wind_heap_mod_init(void);

w_heap_s *wind_heap_get(const char *name);

w_heap_s *wind_heap_create(const char *name,w_addr_t base,w_uint32_t size,w_uint16_t flag);

w_err_t wind_heap_destroy(w_addr_t base);

w_err_t wind_heap_setflag(w_heap_s *heap,w_int16_t flag);
w_err_t wind_heap_clrflag(w_heap_s *heap,w_int16_t flag);


void *wind_heap_malloc(w_heap_s* heap, w_uint32_t size);

void *wind_heap_realloc(w_heap_s* heap, void* ptr, w_uint32_t newsize);

w_err_t wind_heap_free(w_heap_s* heap,void *ptr);

w_err_t wind_heap_print(void);

w_err_t wind_heapitem_print(void);

w_err_t wind_heap_stati_print(void);


void *wind_malloc(w_uint32_t size);

void *wind_calloc(w_uint32_t count, w_uint32_t size);

void *wind_realloc(void *rmem, w_uint32_t newsize);

w_err_t wind_free(void *rmem);



#endif

#ifdef __cplusplus
}
#endif


#endif
