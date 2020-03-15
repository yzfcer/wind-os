/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: module_shell.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2019/11/04 20:24:37
** ��        ��: �����ֵ书��API
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019/11/04 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ���������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __SHELL_FRAMEWORK_H__
#define __SHELL_FRAMEWORK_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_mutex.h"
#include "wind_obj.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_MODULE_DICT_SUPPORT
//#define WIND_DICT_MAGIC 0x34782A5C
#define WIND_DICTSET_MAGIC 0x38792A5D

typedef struct
{
    w_dnode_s dictnode;
    char *name;
    char *value;
    w_uint16_t key;
    w_uint16_t count;
}w_dict_s;


//һ���ֵ伯�����������
typedef struct
{
    w_obj_s obj;
    w_dlist_s list;
    w_mutex_s *mutex;
}w_dictset_s;


w_err_t _wind_dict_mod_init(void);

w_dictset_s *wind_dictset_get(char *name);
w_dictset_s *wind_dictset_create(char *name);
w_err_t wind_dictset_destroy(w_dictset_s *dictset);
w_err_t wind_dictset_insert(w_dictset_s *dictset,w_dict_s *dict);
w_err_t wind_dictset_remove(w_dictset_s *dictset,w_dict_s *dict);
w_err_t wind_dictset_print(w_dictset_s *dictset);
w_err_t wind_dictset_print_all(void);


w_dict_s *wind_dict_get(w_dictset_s *dictset,char *name);
w_dict_s *wind_dict_create(char *name,char *value);
w_err_t wind_dict_destroy(w_dict_s *dict);
w_err_t wind_dict_print(w_dict_s *dict);


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__
