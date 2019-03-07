/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_mpool.h / wind_mpool.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os���ڴ�ع�����صĹ��ܺ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.11.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2013.11.27
** �衡��: 
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
#endif
#define WIND_POOL_MAGIC 0x5d9c843e
#define WIND_POOLITEM_MAGIC 0x507c

#define F_POOLITEM_USED (0x01 << 0) //���sem�����Ƿ�ͨ���ڴ�ط���
#define IS_F_POOLITEM_USED(item) ((item->head.flag & F_POOLITEM_USED) == F_POOLITEM_USED)
#define SET_F_POOLITEM_USED(item) (item->head.flag |= F_POOLITEM_USED)
#define CLR_F_POOLITEM_USED(item) (item->head.flag &= (~F_POOLITEM_USED))
#define POOL_ITEM(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
typedef struct __w_poolitem_s w_poolitem_s;
typedef struct __w_pool_s w_pool_s;
#define WIND_POOL_ALIGN(size) (((size)+7) & (~0x07))

typedef struct
{
    w_uint16_t magic;
    w_uint16_t flag;
    w_poolitem_s* next;
}w_pihead_s;

//һ���������ڴ�ض���ṹ��
struct __w_poolitem_s
{
    w_pihead_s head;
    w_uint8_t buff[8];
};

//�ڴ�صĻ���������Ϣ�ṹ��
struct __w_pool_s
{
    w_obj_s obj;
    w_stati_s stati;
    void *head;//�ڴ�ص�ͷ��λ��
    w_uint32_t size;//�ڴ�ص�ʵ�ʿ��ÿռ��С
    w_uint32_t itemsize;//ÿ����Ĵ�С
    w_int32_t itemnum;//�ֳɵ��ڴ�������
    w_poolitem_s* free_head;//���п��ָ��
    w_poolitem_s* free_end;//���һ�����п��ָ��
    w_int32_t pad;
};

//�����ڴ�صķ���
#define WIND_POOL(pool,itemnum,itemsize) w_uint8_t pool[sizeof(w_pool_s) + itemnum *(WIND_POOL_ALIGN(itemsize) + sizeof(w_pihead_s))+8]

#define NODE_TO_POOL(node) (w_pool_s*)(((char*)(node))-((w_uint32_t)&(((w_pool_s*)0)->obj.objnode)))

w_err_t _wind_pool_mod_init(void);
w_err_t _wind_pool_diagnose_init(void);

w_pool_s *wind_pool_get_by_mem(void *mem);

w_pool_s *wind_pool_get(const char *name);

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t obj_size);

w_err_t wind_pool_destroy(void *mem);

void   *wind_pool_malloc(void *mem);

w_err_t wind_pool_free(void *mem,void *block);

void wind_pool_print_list(void);

void wind_pool_stati_print(void);

#ifdef __cplusplus
}
#endif


#endif

