/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : wind_dict.c
** Author      : Jason Zhou
** Last Date   : 2019/11/04 20:24:37
** Description : Parameter dictionary function set
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019/11/04 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_dict.h"
#include "wind_heap.h"
#include "wind_string.h"
#include "wind_core.h"
#if WIND_MODULE_DICT_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

/********************************************internal variables**********************************************/
#define NODE_TO_DICT(node) NODEPTR_TO_ITEMPTR(node,w_dict_s,dictnode)
#define NODE_TO_DICTSET(node) NODEPTR_TO_ITEMPTR(node,w_dictset_s,obj.objnode)
w_dlist_s g_dictlist;



/********************************************internal functions**********************************************/
static w_uint16_t calc_dict_key(const char *name)
{
    w_uint16_t i,key = 0;
    if(name == W_NULL)
        return 0;
    for(i = 0;name[i] != 0;i ++)
        key += name[i];
    key = (key&0xff00) + name[0];
    return key;
}

/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
w_err_t _wind_dict_mod_init(void)
{
    DLIST_INIT(g_dictlist);
    return W_ERR_OK;
}

w_dictset_s *wind_dictset_get(char *name)
{
    return (w_dictset_s *)wind_obj_get(name,&g_dictlist);
}

w_dictset_s *wind_dictset_create(char *name)
{
    w_err_t err;
    w_dictset_s *dictset = (w_dictset_s*)W_NULL;
    w_mutex_s *mutex = (w_mutex_s *)W_NULL; 
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    do
    {
        err = W_ERR_OK;
        dictset = (w_dictset_s*)wind_malloc(sizeof(w_dictset_s));
        WIND_ASSERT_BREAK(dictset != W_NULL, W_ERR_MEM,"alloc dictset failed");
        DLIST_INIT(dictset->list);
        mutex = wind_mutex_create(name);
        WIND_ASSERT_BREAK(mutex != W_NULL, W_ERR_MEM,"create mutex failed");
        dictset->mutex = mutex;
        wind_disable_switch();
        wind_obj_init(&dictset->obj,WIND_DICTSET_MAGIC,name,&g_dictlist);
        wind_enable_switch();    
    }while(0);
    if(err != W_ERR_OK)
    {
        if(mutex != W_NULL)
            wind_free(mutex);
        if(dictset != W_NULL)
            wind_free(dictset);
        dictset = (w_dictset_s*)W_NULL;
    }
    return dictset;
}

w_err_t wind_dictset_destroy(w_dictset_s *dictset)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_dict_s *dict;
    WIND_ASSERT_RETURN(dictset != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dictset->obj.magic == WIND_DICTSET_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        err = wind_obj_deinit(&dictset->obj,WIND_DICTSET_MAGIC,&g_dictlist);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"");
        foreach_node(dnode, &dictset->list)
        {
            wind_disable_switch();
            dlist_remove(&dictset->list,dnode);
            wind_enable_switch();
            dict = NODE_TO_DICT(dnode);
            err = wind_dict_destroy(dict);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"");
        }
        if(dictset->mutex != W_NULL)
            wind_mutex_destroy(dictset->mutex);
        if(dictset != W_NULL)
            wind_free(dictset);
    }while(0);
    return err;
}

w_err_t wind_dictset_insert(w_dictset_s *dictset,w_dict_s *dict)
{
    WIND_ASSERT_RETURN(dictset != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dict != W_NULL,W_ERR_NULL_PTR);
    wind_mutex_lock(dictset->mutex);
    dlist_insert_tail(&dictset->list,&dict->dictnode);
    wind_mutex_unlock(dictset->mutex);
    return W_ERR_OK;
}

w_err_t wind_dictset_remove(w_dictset_s *dictset,w_dict_s *dict)
{
    WIND_ASSERT_RETURN(dictset != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dict != W_NULL,W_ERR_NULL_PTR);
    wind_mutex_lock(dictset->mutex);
    dlist_remove(&dictset->list,&dict->dictnode);
    wind_mutex_unlock(dictset->mutex);
    return W_ERR_OK;
}

w_err_t wind_dictset_print(w_dictset_s *dictset)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_dict_s *dict;
    WIND_ASSERT_RETURN(dictset != W_NULL,W_ERR_NULL_PTR);
    wind_printf("[dictset name:%s]\r\n",dictset->obj.name);
    wind_mutex_lock(dictset->mutex);
    do
    {
        err = W_ERR_OK;
        foreach_node(dnode,&dictset->list)
        {
            dict = NODE_TO_DICT(dnode);
            //WIND_ASSERT_BREAK(dict->obj.magic == WIND_DICT_MAGIC,W_ERR_INVALID,"invalid dict obj");
            wind_dict_print(dict);
        }
    }while(0);
    wind_mutex_unlock(dictset->mutex);
    return err;
}

w_err_t wind_dictset_print_all(void)
{
    w_err_t err;
    w_dictset_s *dictset;
    w_dnode_s *dnode;
    foreach_node(dnode,&g_dictlist)
    {
        err = W_ERR_OK;
        dictset = NODE_TO_DICTSET(dnode);
        WIND_ASSERT_BREAK(dictset->obj.magic == WIND_DICTSET_MAGIC,W_ERR_INVALID,"invalid dict obj");
        wind_dictset_print(dictset);
    }
    return err;
}

w_dict_s *wind_dict_get(w_dictset_s *dictset,char *name)
{
    w_uint16_t key = 0;
    w_err_t err;
    w_dnode_s *dnode;
    w_dict_s *dict,*retdict;
    WIND_ASSERT_RETURN(dictset != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_mutex_lock(dictset->mutex);
    do
    {
        err = W_ERR_OK;
        retdict = (w_dict_s *)W_NULL;
        key = calc_dict_key(name);
        WIND_ASSERT_BREAK(dictset->list.head != W_NULL, W_ERR_INVALID,"");
        foreach_node(dnode,&dictset->list)
        {
            dict = NODE_TO_DICT(dnode);
            //WIND_ASSERT_BREAK(dict->obj.magic == WIND_DICT_MAGIC,W_ERR_INVALID,"invalid dict obj");
            if(dict->key != key)
                continue;
            if(dict->name && (wind_strcmp(name,dict->name) == 0))
            {
                retdict = dict;
                break;
            }
                
        }
    }while(0);

    wind_mutex_unlock(dictset->mutex);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    return retdict;
}

w_dict_s *wind_dict_create(char *name,char *value)
{
    w_err_t err;
    w_dict_s *dict;
    WIND_ASSERT_RETURN(name != W_NULL,(w_dict_s *)W_NULL);
    WIND_ASSERT_RETURN(value != W_NULL,(w_dict_s *)W_NULL);
    do
    {
        err = W_ERR_OK;
        dict = (w_dict_s*)wind_malloc(sizeof(w_dict_s));
        WIND_ASSERT_BREAK(dict != W_NULL, W_ERR_MEM,"");
        wind_memset(dict,0,sizeof(w_dict_s));
        dict->name = wind_salloc(name,0);
        WIND_ASSERT_BREAK(dict->name != W_NULL, W_ERR_MEM,"");
        dict->value = wind_salloc(value,0);
        WIND_ASSERT_BREAK(dict->value != W_NULL, W_ERR_MEM,"");
        DNODE_INIT(dict->dictnode);
        dict->key = calc_dict_key(name);
        dict->count = 0;
    }while(0);
    if((err != W_ERR_OK) && (dict != W_NULL))
    {
        if(dict->name != W_NULL)
            wind_free(dict->name);
        if(dict->value != W_NULL)
            wind_free(dict->value);
        wind_free(dict);
        dict = (w_dict_s*)W_NULL;
    }
    return dict;
}

w_err_t wind_dict_destroy(w_dict_s *dict)
{
    WIND_ASSERT_RETURN(dict != W_NULL,W_ERR_NULL_PTR);
    if(dict->name != W_NULL)
        wind_free(dict->name);
    if(dict->value != W_NULL)
        wind_free(dict->value);
    wind_free(dict);
    return W_ERR_OK;
}

w_err_t wind_dict_print(w_dict_s *dict)
{
    WIND_ASSERT_RETURN(dict != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dict->name != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(dict->value != W_NULL,W_ERR_NULL_PTR);
    wind_printf("%s=%s\r\n",dict->name,dict->value);
    return W_ERR_OK;
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
