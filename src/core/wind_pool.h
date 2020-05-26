/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_mpool.h / wind_mpool.c
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

#ifndef WIND_POOL_H__
#define WIND_POOL_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_stati.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define WIND_POOL_MAGIC 0x5d9c843e
#define WIND_POOLITEM_MAGIC 0x507c

#define F_POOLITEM_USED (0x01 << 0) //Mark whether objects are allocated through the memory pool
#define IS_F_POOLITEM_USED(item) ((item->head.flag & F_POOLITEM_USED) == F_POOLITEM_USED)
#define SET_F_POOLITEM_USED(item) (item->head.flag |= F_POOLITEM_USED)
#define CLR_F_POOLITEM_USED(item) (item->head.flag &= (~F_POOLITEM_USED))
#define POOL_ITEM(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_addr_t)&(((type*)0)->mbrnode)))
typedef struct __w_poolitem_s w_poolitem_s;
typedef struct __w_pool_s w_pool_s;
#define WIND_POOL_ALIGN(size) (((size)+7) & (~0x07))

//Memory pool item header structure
typedef struct
{
    w_uint16_t magic;
    w_uint16_t flag;
    w_poolitem_s* next;
}w_pihead_s;

//Memory pool item object structure
struct __w_poolitem_s
{
    w_pihead_s head;   //Memory pool item header
    w_uint8_t buff[8]; //Element buffer(at least 8 bytes)
};

//Basic description information structure of memory pool
struct __w_pool_s
{
    w_obj_s obj;                //Basic object information
    w_stati_s stati;            //Memory pool statistics
    void *head;                 //Head location of memory pool
    w_uint32_t size;            //Actual free space size of memory pool
    w_uint32_t itemsize;        //Element size
    w_int32_t itemnum;          //Element number
    w_poolitem_s* free_head;    //Free element list head
    w_poolitem_s* free_end;     //Last free element tail
    w_int32_t pad;              //Memory fill alignment
};

//To define a memory pool
#define WIND_POOL(pool,itemnum,itemsize) \
    w_uint8_t pool[sizeof(w_pool_s) + itemnum *(WIND_POOL_ALIGN(itemsize) + sizeof(w_pihead_s))+8]


w_err_t _wind_pool_mod_init(void);
w_err_t _wind_pool_diagnose_init(void);

w_pool_s *wind_pool_get_by_mem(void *mem);

w_pool_s *wind_pool_get(const char *name);

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t obj_size);

w_err_t wind_pool_destroy(void *mem);

void   *wind_pool_malloc(void *mem);

w_err_t wind_pool_free(void *mem,void *block);

w_err_t wind_pool_print_list(void);

void wind_pool_print_detail(void);

void wind_pool_stati_print(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_POOL_H__

