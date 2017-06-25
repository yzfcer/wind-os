/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_list.h / wind_list.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的内核链表方法
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_types.h"
#include "wind_list.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
#include "wind_err.h"
#include "wind_stat.h"
#include "wind_var.h"
#include "wind_assert.h"


//malloc a node from the node pool
pnode_s wind_node_malloc(core_type type)
{
    pnode_s pnode;
    pnode = wind_core_alloc(STAT_NODE);
    WIND_ASSERT_RETURN(pnode != NULL,NULL);
    pnode->used = B_TRUE;
    pnode->type = type;
    pnode->next = NULL;
    return pnode;
}

//free a node space
err_t wind_node_free(pnode_s node)
{
    err_t err;
    WIND_ASSERT_RETURN(node != NULL,ERR_NULL_POINTER);
    node->next = NULL;
    node->obj = NULL;
    node->used = B_FALSE;    
    err = wind_core_free(STAT_NODE,node);
    return err;
}

static err_t core_list_insert(pnode_s *list,pnode_s node,bool_t minus)
{
    pnode_s pnode = NULL,pnode1 = NULL;
    wind_close_interrupt();
    if(list == NULL || node == NULL)
    {
        WIND_ERROR("wind_list_insert err\r\n");
        wind_open_interrupt();
        return ERR_INVALID_PARAM;
    }
    /*if(minus)
    {
        pnode = *list;
        while(pnode)
        {
            WIND_DEBUG("pnode->key1 = %d\r\n",pnode->key);
            pnode = pnode->next;
        }
    }*/

    node->minus = minus;
    if((*list) == NULL)
    {
        *list = node;
        node->next = NULL;
        wind_open_interrupt();
        return ERR_OK;
    }
    pnode = NULL;    
    pnode1 = *list;
    while (pnode1)
    {
        if(pnode1->key <= node->key)
        {
            if(minus)
            {
                //WIND_DEBUG("pnode1->key=%d,node->key=%d\r\n",pnode1->key,node->key);
                node->key -= pnode1->key;//这里对键值做减法处理，主要是方便一些延时操作
            }
            pnode = pnode1;
            pnode1 = pnode1->next;
        }
        else
        {
            break;
        }
    }
    if(pnode == NULL)
    {
        node->next = (*list);
        (*list) = node;
        pnode1 = node->next;
    }
    else
    {
        node->next = pnode1;
        pnode->next = node;
    }
    if(minus)
    {
        
        //while(pnode1)
        //{
            pnode1->key -= node->key;
        //    pnode1 = pnode1->next;
        //}

    }
    wind_open_interrupt();
    return ERR_OK;
}



//insert a node to the list and sorted by its key
err_t wind_list_insert(pnode_s *list,pnode_s node)
{
    return core_list_insert(list,node,B_FALSE);
}

//insert a node to the end of a list
err_t wind_list_inserttoend(pnode_s *list,pnode_s node)
{
    pnode_s pnode;
    wind_close_interrupt();
    if(list == NULL || node == NULL)
    {
        wind_open_interrupt();
        return ERR_INVALID_PARAM;
    }
    if(*list == NULL)
    {
        *list = node;
        wind_open_interrupt();
        return ERR_OK;
    }
    pnode = *list;
    while(pnode->next)
        pnode = pnode->next;
    pnode->next = node;
    node->next = NULL;
    wind_open_interrupt();
    return ERR_OK;
}

//insert a node to the head of a list
err_t wind_list_inserttohead(pnode_s *list,pnode_s node)
{
    pnode_s pnode = NULL;
    wind_close_interrupt();
    if(list == NULL || node == NULL)
    {
        wind_open_interrupt();
        return ERR_INVALID_PARAM;
    }
    //pnode->next = *list;
    node->next = *list;
    *list = node;
    wind_open_interrupt();
    return ERR_OK;
}

//插入一个元素，但插入的位置每向后移动一个元素，就需要减去它前面的元素的键值
err_t wind_list_insert_with_minus(pnode_s *list,pnode_s node)
{
    return core_list_insert(list,node,B_TRUE);
}
//find a node from the list by its obj
pnode_s wind_list_search(pnode_s *list,void *obj)
{
    pnode_s pnode;
    wind_close_interrupt();    
    if(list == NULL || *list == NULL || obj == NULL)
    {
        wind_open_interrupt();
        return NULL;
    }
    pnode = *list;
    while(pnode)
    {

        if((void*)pnode->obj == obj)
        {
            wind_open_interrupt();
            return pnode;
        }
        pnode = pnode->next;
        
    }
    wind_open_interrupt();
    return NULL;
}

//remove a node from the list
pnode_s wind_list_remove(pnode_s *list,pnode_s node)
{
    pnode_s pnode = NULL,pnode1 = NULL;
    wind_close_interrupt();
    if(list == NULL || node == NULL || *list == NULL)
    {
        WIND_WARN("null pointer in wind_list_remove\r\n");
        wind_open_interrupt();
        return NULL;
    }
    if(wind_list_search(list,node->obj) == NULL)
    {
        WIND_WARN("has no node in list for removing\r\n");
        wind_open_interrupt();
        return NULL;
    }
    if(*list == node)
    {
        if(node->minus)
            node->next->key += node->key;
        *list = (*list)->next;
        //wind_node_free(node);
        wind_open_interrupt();
        return node;
    }
    pnode1 = *list;
    while(pnode1->next != node)
    {
        pnode1 = pnode1->next;
    }
    pnode = pnode1->next;
    pnode1->next = pnode1->next->next;
    wind_open_interrupt();
    return pnode;
}

void printlist(pnode_s node)
{
    int cnt = 0;
    pnode_s pnode = node;
    while(pnode)
    {
        WIND_DEBUG("node %d type %d\r\n",cnt,pnode->type);
        WIND_DEBUG("node %d key %d\r\n",cnt,pnode->key);
        WIND_DEBUG("node %d obj %d\r\n",cnt,pnode->obj);
        pnode = pnode->next;
        cnt ++;
    }
}
/*
pnode_s list;
void listtest()
{
    pnode_s node;
    int cnt;
    for(cnt = 0;cnt < 4;cnt ++)
    {
        node = wind_node_malloc(2);
        node->key = cnt;
        node->obj = cnt;
        wind_list_insert(&list,node);
    }
    printlist(list);
    for(cnt = 0;cnt < 4;cnt ++)
    {
        wind_list_remove(&list,list);
    }
    
    printlist(list);
    
}
*/

