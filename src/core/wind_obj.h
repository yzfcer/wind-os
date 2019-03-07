/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_obj.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.01.28
**��        ��: ϵͳ�û�����ģ��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.01.28
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.01.28
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_OBJ_H__
#define WIND_OBJ_H__
#include "wind_type.h"
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _wind_obj
{
    w_uint32_t magic;
    const char *name;
    w_dnode_s objnode;
    w_uint16_t key;
    w_uint16_t flag;
}w_obj_s;

#define NODE_TO_OBJ(node) (w_obj_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_obj_s*)0)->objnode)))

w_obj_s *wind_obj_get(const char *name,w_dlist_s *list);
w_err_t wind_obj_init(w_obj_s *obj,w_uint32_t magic,const char *name,w_dlist_s *list);
w_err_t wind_obj_deinit(w_obj_s *obj,w_uint32_t magic,w_dlist_s *list);


#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_OBJ_H__

