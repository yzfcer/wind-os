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



pnode_s wind_node_malloc(core_type type)
{
    pnode_s pnode;
    pnode = wind_core_alloc(STAT_NODE);
    WIND_ASSERT_RETURN(pnode != NULL,NULL);
    pnode->used = B_TRUE;
    pnode->minus = B_FALSE;
    pnode->type = type;
    pnode->next = NULL;
    pnode->prev = NULL;
    return pnode;
}


w_err_t wind_node_free(pnode_s node)
{
    w_err_t err;
    WIND_ASSERT_RETURN(node != NULL,ERR_NULL_POINTER);
    node->next = NULL;
    node->prev = NULL;
    node->obj = NULL;
    node->used = B_FALSE;
    node->minus = B_FALSE;
    node->type = CORE_TYPE_NONE;
    err = wind_core_free(STAT_NODE,node);
    return err;
}

void wind_node_bindobj(pnode_s node,core_type type,w_int32_t key,void *obj)
{
    node->type = type;
    node->key = key;
    node->obj = obj;
    node->used = B_TRUE;
}

static void insert_to_head(plist_s list,pnode_s node)
{
    node->prev = NULL;
    node->next = list->head;
    list->head = node;
    if(node->next != NULL)
        node->next->prev = node;
    else
        list->tail = node;
    list->cnt ++;
}

static void insert_to_tail(plist_s list,pnode_s node)
{
    node->prev = list->tail;
    node->next = NULL;
    list->tail = node;
    if(node->prev != NULL)
        node->prev->next = node;
    else
        list->head = node;
    list->cnt ++;
}

static void insert_behind(plist_s list,pnode_s node,pnode_s dest)
{
    node->prev = dest;
    node->next = dest->next;
    if(dest->next == NULL)
        list->tail = node;
    else
        dest->next->prev = node;
    dest->next = node;
    list->cnt ++;
}

static void remove_head(plist_s list,pnode_s node)
{
    list->cnt --;
    if(node->next != NULL)
        node->next->prev = NULL;
    else
        list->tail = NULL;
    list->head = node->next;
    node->prev = NULL;
    node->next = NULL;
}

static void remove_tail(plist_s list,pnode_s node)
{
    list->cnt --;
    if(node->prev != NULL)
        node->prev->next = NULL;
    else
        list->head = NULL;
    list->tail = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

static void remove_behind(plist_s list,pnode_s node,pnode_s dest)
{
    list->cnt --;
    if(node->next != NULL)
        node->next->prev = dest;
    else
        list->tail = dest;
    dest->next = node->next;
    node->prev = NULL;
    node->next = NULL;
}


static w_err_t list_insert(plist_s list,pnode_s node,w_bool_t minus)
{
    pnode_s pnode = NULL;
    wind_close_interrupt();
    if(list == NULL || node == NULL)
    {
        WIND_ERROR("wind_list_insert err\r\n");
        wind_open_interrupt();
        return ERR_INVALID_PARAM;
    }

    node->minus = minus;
    if((list->head == NULL) ||
        (list->head->key >= node->key))
    {
        insert_to_head(list,node);
        wind_open_interrupt();
        return ERR_OK;
    }
    
    pnode = list->head;
    while (pnode)
    {
        if(pnode->key <= node->key)
        {
            if(minus)
                node->key -= pnode->key;
            pnode = pnode->next;
        }
        else
            break;
    }
    if(pnode == NULL)
        insert_to_tail(list,node);
    else
    {
        insert_behind(list,node,pnode->prev);
    }
    wind_open_interrupt();
    return ERR_OK;
}



w_err_t wind_list_init(plist_s list)
{
    if(list == NULL)
        return ERR_INVALID_PARAM;
    list->head = NULL;
    list->tail = NULL;
    list->cnt = 0;
    return ERR_OK;
}

w_err_t wind_list_insert(plist_s list,pnode_s node)
{
    return list_insert(list,node,B_FALSE);
}


w_err_t wind_list_inserttoend(plist_s list,pnode_s node)
{
    wind_close_interrupt();
    if(list == NULL || node == NULL)
    {
        wind_open_interrupt();
        return ERR_INVALID_PARAM;
    }
    insert_to_tail(list,node);
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_list_inserttohead(plist_s list,pnode_s node)
{
    wind_close_interrupt();
    if(list == NULL || node == NULL)
    {
        wind_open_interrupt();
        return ERR_INVALID_PARAM;
    }
    insert_to_head(list,node);
    wind_open_interrupt();
    return ERR_OK;
}

//插入一个元素，但插入的位置每向后移动一个元素，就需要减去它前面的元素的键值
w_err_t wind_list_insert_with_minus(plist_s list,pnode_s node)
{
    return list_insert(list,node,B_TRUE);
}

pnode_s wind_list_search(plist_s list,void *obj)
{
    pnode_s pnode;
    wind_close_interrupt();    
    if(list == NULL || list->head == NULL || obj == NULL)
    {
        wind_open_interrupt();
        return NULL;
    }
    pnode = list->head;
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


pnode_s wind_list_remove(plist_s list,pnode_s node)
{
    pnode_s pnode = NULL;
    wind_close_interrupt();
    pnode = wind_list_search(list,node->obj);
    if(pnode == NULL)
    {
        WIND_WARN("has no node in list for removing\r\n");
        wind_open_interrupt();
        return NULL;
    }
    if((node->minus)&&(node->next != NULL))
        node->next->key += node->key;
    if(list->head == node)
    {
        remove_head(list,node);
    }
    else if(list->tail == node)
        remove_tail(list,node);
    else
        remove_behind(list,node,pnode->prev);
    wind_open_interrupt();
    return pnode;
}

void wind_list_print(plist_s list)
{
    pnode_s pnode = list->head;
    wind_printf("-------------------------------------------\r\n");
    wind_printf("%-10s %-8s %-10s\r\n","type","key","objaddr");
    wind_printf("-------------------------------------------\r\n");
    while(pnode)
    {
        wind_printf("%-10d %-8d 0x%08x\r\n",
            pnode->type,pnode->key,pnode->obj);
        pnode = pnode->next;
    }
    wind_printf("-------------------------------------------\r\n");
}


list_s test_list = {NULL,NULL,0};
void listtest(void)
{
    pnode_s node;
    int cnt;
    for(cnt = 0;cnt < 4;cnt ++)
    {
        node = wind_node_malloc(CORE_TYPE_NONE);
        if(node == NULL)
            return;
        wind_node_bindobj(node,CORE_TYPE_NONE,cnt,(void *)(cnt+20));
        wind_list_insert(&test_list,node);
    }
    wind_list_print(&test_list);
    wind_list_remove(&test_list,test_list.head);
    wind_list_remove(&test_list,test_list.tail);
    
    wind_list_print(&test_list);
    
}


