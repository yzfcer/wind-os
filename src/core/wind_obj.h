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
#endif // #ifdef __cplusplus

//Basic object information structure
typedef struct _wind_obj
{
    w_uint32_t magic;  //magic code
    char *name;        //Object name
    w_dnode_s objnode; //Object list node
    w_uint16_t key;    //Key value, used to speed up object search
    w_uint16_t flag;   //Object attribute tag
}w_obj_s;


const char *wind_obj_name(void *obj);
w_obj_s *wind_obj_get(const char *name,w_dlist_s *list);
w_err_t wind_obj_init(w_obj_s *obj,w_uint32_t magic,const char *name,w_dlist_s *list);
w_err_t wind_obj_deinit(w_obj_s *obj,w_uint32_t magic,w_dlist_s *list);


#ifdef __cplusplus
}
#endif
#endif// #ifndef WIND_OBJ_H__

