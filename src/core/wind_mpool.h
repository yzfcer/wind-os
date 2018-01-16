/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_mpool.h / wind_mpool.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os���ڴ�ع�����صĹ��ܺ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.11.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2013.11.27
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_MPOOL_H__
#define WIND_MPOOL_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_dlist.h"
#include "wind_stati.h"
#ifdef __cplusplus
extern "C" {
#endif
#define WIND_MPOOL_MAGIC 0x5d9c843e
#define POOL_BLK_FREE 0x52d6e300
#define POOL_BLK_USED (POOL_BLK_FREE | 0xA5)

typedef struct __pool_item_s pool_item_s;
typedef struct __mpool_s pool_s;

//һ���������ڴ�����ṹ��
struct __pool_item_s
{
    w_uint32_t flag;
    pool_item_s* next;
};

//�ڴ�ص�ͷ����Ϣ�ṹ��
struct __mpool_s
{
    w_uint32_t magic;//�ڴ�ر��ɹ������ı�־
    const char *name;
    dnode_s poolnode;
    stati_s stati;
    void *head;//�ڴ�ص�ͷ��λ��
    w_uint32_t size;//�ڴ�ص�ʵ�ʿ��ÿռ��С
    w_uint32_t itemsize;//ÿ����Ĵ�С
    w_uint32_t itemnum;//�ֳɵ��ڴ�������
    w_uint32_t used;//�Ѿ�ʹ�õ��ڴ�������
    pool_item_s* free_head;//���п��ָ��
    pool_item_s* free_end;//���һ�����п��ָ��
};

//�����ڴ�صķ���
#define WIND_MPOOL_ALIGN(size) (((size)+3) & (~0x03))
#define WIND_MPOOL(pool,itemnum,itemsize) w_uint8_t pool[sizeof(pool_s) + itemnum *(WIND_MPOOL_ALIGN(itemsize) + sizeof(pool_item_s))+8]

w_err_t wind_pool_create(const char *name,void *mem,w_uint32_t memsize,w_uint32_t itemsize);

void   *wind_pool_alloc(void *mem);

w_err_t wind_pool_free(void *mem,void *block);

w_err_t wind_pool_print(void *pool);

void wind_pool_print_list(dlist_s *list);

#ifdef __cplusplus
}
#endif


#endif

