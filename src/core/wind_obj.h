/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_obj.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.01.28
**描        述: 系统用户管理模块
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.01.28
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.01.28
** 描　述: 
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

