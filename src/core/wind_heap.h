/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_heap.h / wind_heap.c
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ϵͳ���ڴ�ѵĶ�̬���亯��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
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
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_HEAP_SUPPORT > 0


#define WIND_HEAP_MIN_SIZE    256
#define WIND_HEAP_MINIALLOC    12
#define WIND_HEAP_ALIGN_SIZE 4
#define WIND_HEAP_MAGIC        0xa5e87490
#define WIND_HEAPITEM_MAGIC    0x1ea01ea0
#define WIND_HEAP_MASK         0xfffffffe

#define WIND_HEAP_USED         0x01
#define WIND_HEAP_PRIVATE      0x01

#define HEAP_IS_USED(h)   ((h)->magic & WIND_HEAP_USED)
#define HEAP_IS_PRIVATE(h)   ((h)->magic & WIND_HEAP_PRIVATE)

#define __ALIGN_R(size,N) ((size + N - 1) / N *N)
#define __ALIGN_L(size,N) ((size - N + 1) / N *N)

#define WIND_HEAP_SIZE         __ALIGN_R(sizeof(heapitem_s), WIND_HEAP_ALIGN_SIZE)


//�ڴ��Ŀ����Ķ��壬���ϵͳ���ڶ�����������ڴ�������Ӧ����������Ӷ���
//ͬʱ��Ӧ����wind_heap_mutex_init������ע���µ��ڴ��
typedef enum __HeapBmutex_e
{
    HEAP_BLOCK_1 = 0,//��һ��
    HEAP_BLOCK_CNT//����
}HeapBmutex_e;



typedef struct __heapitem_s heapitem_s;
typedef struct __heap_s heap_s;
struct __heapitem_s
{
    w_uint32_t magic;
    heap_s *heap;
    prionode_s itemnode;
    w_int32_t size;
};


struct __heap_s
{
    w_uint32_t magic;
    const char *name;
    void *addr; 
    w_uint32_t size;
    w_uint32_t rest;
    w_uint32_t max_used;
    dnode_s heapnode;
    dlist_s used_list;
    dlist_s free_list;
    void *pmutex; 
};

heap_s *wind_heap_get(const char *name);

heap_s *wind_heap_create(const char *name,
             w_addr_t base,w_uint32_t size,w_uint32_t flag);

w_err_t wind_heap_destroy(w_addr_t base);


void *wind_heap_malloc(heap_s* heap, w_uint32_t size);

void *wind_heap_realloc(heap_s* heap, void* ptr, w_uint32_t newsize);

w_err_t wind_heap_free(heap_s* heap,void *ptr);

w_err_t wind_heap_print(void);

w_err_t wind_heapitem_print(dlist_s *list);


void *wind_malloc(w_uint32_t size);

void *wind_calloc(w_uint32_t count, w_uint32_t size);

void *wind_realloc(void *rmem, w_uint32_t newsize);

w_err_t wind_free(void *rmem);



#endif

#ifdef __cplusplus
}
#endif


#endif
