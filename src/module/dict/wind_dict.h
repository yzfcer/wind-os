/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: module_shell.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2019/11/04 20:24:37
** 描        述: 参数字典功能API
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019/11/04 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
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
#define WIND_DICT_MAGIC 0x34782A5C

typedef struct
{
    w_dnode_s dictnode;
    char *name;
    char *value;
    w_uint16_t key;
    w_uint16_t count;
}w_dict_s;


//一个字典集包含多个词条
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


w_dict_s *wind_dict_get(w_dictset_s *dictset,char *name);
w_dict_s *wind_dict_create(char *name,char *value);
w_err_t wind_dict_destroy(w_dict_s *dict);
w_err_t wind_dict_print(w_dict_s *dict);


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__
