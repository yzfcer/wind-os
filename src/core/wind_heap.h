/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_heap.h
** Author      : Jason Zhou
** Last Date   : 2019.12.19
** Description : wind-os memory heap function set
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
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
#endif // #ifdef __cplusplus

#if WIND_HEAP_SUPPORT


#define WIND_HEAP_MIN_SIZE    256
#define WIND_HEAP_MAGIC        0x0a5e8749
#define WIND_HEAPITEM_MAGIC    0xA1ea

#define HEAP_ALIGN_R(size) ((((size) + 7) >> 3) << 3)
#define HEAP_ALIGN_L(size) (((size) >> 3) << 3)
#define WIND_HEAP_HEAD_SIZE  HEAP_ALIGN_R(sizeof(w_heap_s))
#define WIND_HEAP_ITEM_SIZE  HEAP_ALIGN_R(sizeof(w_heapitem_s))


#define F_HEAP_PRIVATE (0x01 << 0) //Mark whether the heap object is private
#define IS_F_HEAP_PRIVATE(heap) ((heap->obj.flag & F_HEAP_PRIVATE) == F_HEAP_PRIVATE)
#define SET_F_HEAP_PRIVATE(heap) (heap->obj.flag |= F_HEAP_PRIVATE)
#define CLR_F_HEAP_PRIVATE(heap) (heap->obj.flag &= (~F_HEAP_PRIVATE))


#define F_HEAPITEM_USED (0x01 << 0) //Mark whether the heap element object is used
#define IS_F_HEAPITEM_USED(heapitem) ((heapitem->flag & F_HEAPITEM_USED) == F_HEAPITEM_USED)
#define SET_F_HEAPITEM_USED(heapitem) (heapitem->flag |= F_HEAPITEM_USED)
#define CLR_F_HEAPITEM_USED(heapitem) (heapitem->flag &= (~F_HEAPITEM_USED))

//Heap allocation tag ID
typedef enum
{
    HP_ALLOCID_COMMON = 0,  //General assignment ID
    HP_ALLOCID_VFS    = 1,  //VFS module assignment ID
    HP_ALLOCID_LISTFS = 2,  //listfs module assignment ID
    HP_ALLOCID_TREEFS = 3,  //treefs module assignment ID
    HP_ALLOCID_HOSTFS = 4,  //hostfs module assignment ID
    HP_ALLOCID_DB     = 5,  //DB module assignment ID
    HP_ALLOCID_CJSON  = 6,  //cJSON module assignment ID
    HP_ALLOCID_ALL    = 255,//undefined module assignment ID
}w_allocid_e;

typedef struct __w_heapitem_s w_heapitem_s;
typedef struct __w_heap_s w_heap_s;

//Heap allocation block object information
struct __w_heapitem_s
{
    w_uint16_t magic;      //Magic code
    w_uint8_t  flag;       //Heapitem attribute tag
    w_uint8_t  allocid;    //Allocate ID,mark which module the heapitem belong to
    w_uint32_t size;       //The heapitem size,contains the allocated memory
    w_heap_s *heap;        //The heap which allocating heaitem
    w_prinode_s itemnode;  //Hepitem linked list node
};

//Heap object information
struct __w_heap_s
{
    w_obj_s   obj;       //Basic object information
    w_addr_t  addr;      //Allocatable memory address
    w_stati_s stati;     //Memory allocation statistics
    w_dlist_s used_list; //List of allocated memory blocks
    w_dlist_s free_list; //List of unallocated memory blocks
    void *mutex;         //Heap mutex
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

w_err_t wind_heap_print_list(void);

w_err_t wind_heap_print_detail(void);

w_err_t wind_heapitem_print_detail(w_allocid_e allocid);

w_err_t wind_heap_stati_print(void);


void *wind_malloc(w_uint32_t size);

void *wind_zalloc(w_uint32_t size,w_uint8_t allocid);

void *wind_alloc(w_uint32_t size,w_uint8_t flag);

void *wind_salloc(const char *str,w_uint8_t allocid);

void *wind_clone(void *object,w_uint32_t size);

void *wind_calloc(w_uint32_t count, w_uint32_t size);

void *wind_realloc(void *rmem, w_uint32_t newsize);

w_err_t wind_free(void *ptr);



#endif // #if WIND_HEAP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_HEAP_H__
