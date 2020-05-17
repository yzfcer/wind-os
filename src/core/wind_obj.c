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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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
    w_err_t err;
    w_dnode_s *dnode;
    w_obj_s *tmpobj;
    w_int32_t res;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(obj != W_NULL,W_ERR_PTR_NULL);
    wind_disable_switch();
    do
    {
        err = W_ERR_OK;
        if(list->head == W_NULL)
        {
            dlist_insert_head(list,&obj->objnode);
            err = W_ERR_OK;
            break;
        }
        if(obj->name == W_NULL)
        {
            dlist_insert_tail(list,&obj->objnode);
            err = W_ERR_OK;
            break;
        }
        foreach_node(dnode,list)
        {
            tmpobj = NODE_TO_OBJ(dnode);
            res = wind_strcmp(obj->name,tmpobj->name);
            if(res == 0)
            {
                wind_error("object \"%s\" has been in the list",obj->name);
                err = W_ERR_FAIL;
                break;
            }
            else if (res < 0)
            {
                dlist_insert(list,dnode->prev,&obj->objnode);
                err = W_ERR_OK;
                break;
            }
        }
        if(dnode == W_NULL)
            dlist_insert_tail(list,&obj->objnode);
    }while(0);
    wind_enable_switch();
    return err;
}

const char *wind_obj_name(void *obj)
{
    w_obj_s *obj1 = (w_obj_s*)obj;
    return obj1->name != W_NULL?obj1->name:"null";
}

w_obj_s *wind_obj_get(const char *name,w_dlist_s *list)
{
    w_err_t err;
    w_int16_t key;
    w_obj_s *obj;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(list != W_NULL,W_NULL);
    key = calc_obj_key(name);
    wind_disable_switch();
    do
    {
        err = W_ERR_FAIL;
        WIND_CHECK_BREAK(list->head != W_NULL,W_ERR_FAIL);
        foreach_node(dnode,list)
        {
            obj = NODE_TO_OBJ(dnode);
            if(obj->key != key)
                continue;
            if(obj->name && (wind_strcmp(name,obj->name) == 0))
            {
                err = W_ERR_OK;
                break;
            }
        }
    }while(0);
    wind_enable_switch();
    if(err != W_ERR_OK)
        return W_NULL;
    return obj;
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

w_err_t wind_obj_register(w_obj_s *obj,w_dlist_s *dlist)
{
    w_obj_s *tmpobj;
    WIND_ASSERT_RETURN(obj != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dlist != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(obj->name != W_NULL,W_ERR_PTR_NULL);
    tmpobj = wind_obj_get(obj->name,dlist);
    WIND_ASSERT_RETURN(tmpobj == W_NULL,W_ERR_REPEAT);

    DNODE_INIT(obj->objnode);
    obj->key = calc_obj_key(obj->name);
    insert_obj(dlist,obj);
    obj->magic = (!obj->magic);
    return W_ERR_OK;
    
}
w_err_t wind_obj_unregister(w_obj_s *obj,w_dlist_s *dlist)
{
    w_obj_s *tmpobj;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(obj != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dlist != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(obj->name != W_NULL,W_ERR_PTR_NULL);
    tmpobj = wind_obj_get(obj->name,dlist);
    WIND_ASSERT_RETURN(tmpobj != W_NULL,W_ERR_NO_OBJ);

    wind_disable_switch();
    dnode = dlist_remove(dlist,&obj->objnode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    obj->magic = (~obj->magic);
    return W_ERR_OK;  
}


w_err_t wind_obj_print_list(w_dlist_s *dlist)
{
    w_dnode_s *dnode;
    w_obj_s *obj;
    int cnt = 0;
    w_dlist_s *list = dlist;
    WIND_ASSERT_RETURN(dlist != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\nobject list:\r\n");

    foreach_node(dnode,list)
    {
        obj = NODE_TO_OBJ(dnode);
        wind_printf("%-12s ",obj->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus



