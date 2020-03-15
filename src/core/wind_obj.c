/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_obj.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.01.28
**��        ��: ����ͨ��ģ��
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
#include "wind_obj.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_core.h"
#define OBJ_MAX_LEN 32
#define NODE_TO_OBJ(node) (w_obj_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_obj_s*)0)->objnode)))

static w_uint16_t calc_obj_key(const char *name)
{
    w_uint16_t i,key = 0;
    if(name == W_NULL)
        return 0;
    for(i = 0;(name[i] != 0)&&(i < OBJ_MAX_LEN);i ++)
        key += name[i];
    key = (key&0xff00) + name[0];
    return key;
}

static w_err_t insert_obj(w_dlist_s *list,w_obj_s *obj)
{
    w_dnode_s *dnode;
    w_obj_s *obj1;
    w_int32_t res;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(obj != W_NULL,W_ERR_PTR_NULL);
    wind_disable_switch();
    if(list->head == W_NULL)
    {
        dlist_insert_head(list,&obj->objnode);
        wind_enable_switch();
        return W_ERR_OK;
    }
    if(obj->name == W_NULL)
    {
        dlist_insert_tail(list,&obj->objnode);
        wind_enable_switch();
        return W_ERR_OK;
    }
    
    foreach_node(dnode,list)
    {
        obj1 = NODE_TO_OBJ(dnode);
        res = wind_strcmp(obj->name,obj1->name);
        if(res == 0)
        {
            wind_error("object \"%s\" has been in the list",obj->name);
            wind_enable_switch();
			return W_ERR_FAIL;
        }
        else if (res < 0)
        {
            dlist_insert(list,dnode->prev,&obj->objnode);
            wind_enable_switch();
            return W_ERR_OK;
        }
    }
    dlist_insert_tail(list,&obj->objnode);
    wind_enable_switch();
    return W_ERR_OK;
}

const char *wind_obj_name(void *obj)
{
    w_obj_s *obj1 = (w_obj_s*)obj;
    return obj1->name != W_NULL?obj1->name:"null";
}

w_obj_s *wind_obj_get(const char *name,w_dlist_s *list)
{
    w_int16_t key;
    w_obj_s *obj;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(list != W_NULL,W_NULL);
    key = calc_obj_key(name);
    wind_disable_switch();
    if(list->head == W_NULL)
    {
        wind_enable_switch();
        return W_NULL;
    }
    foreach_node(dnode,list)
    {
        obj = NODE_TO_OBJ(dnode);
        if(obj->key != key)
            continue;
        if(obj->name && (wind_strcmp(name,obj->name) == 0))
        {
            wind_enable_switch();
            return obj;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_obj_init(w_obj_s *obj,w_uint32_t magic,const char *name,w_dlist_s *list)
{
    WIND_ASSERT_RETURN(obj != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    obj->name = (char*)name;
    DNODE_INIT(obj->objnode);
    obj->key = name == W_NULL?0:calc_obj_key(name);
    obj->flag = 0;
    insert_obj(list,obj);
    obj->magic = magic;
    return W_ERR_OK;
}

w_err_t wind_obj_deinit(w_obj_s *obj,w_uint32_t magic,w_dlist_s *list)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(obj != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(obj->magic == magic,W_ERR_INVALID);
    wind_disable_switch();
    dnode = dlist_remove(list,&obj->objnode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    obj->magic = (~magic);
    return W_ERR_OK;  
}




