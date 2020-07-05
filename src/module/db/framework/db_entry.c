/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : db_entry.c
** Author      : Jason Zhou
** Last Date   : 2017.12.19
** Description : DB basic operation function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2017.12.19
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2017.12.19
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "db_def.h"
#include "db_entry.h"
#include "tb_entry.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_debug.h"
#if WIND_MODULE_DB_SUPPORT

#define NODE_TO_DBENTRY(node) NODEPTR_TO_ITEMPTR(node,w_db_s,dbnode)

static w_dlist_s db_list = {W_NULL,W_NULL};



static w_int32_t get_db_hash(char *dbname)
{
    w_int32_t i,len;
    w_int32_t hash = 0;
    len = wind_strlen(dbname);
    WIND_ASSERT_RETURN(len < DB_NAME_LEN,-1);
    for(i = 0;i < len;i ++)
        hash += dbname[i];
    return hash;       
}

static w_dlist_s *get_db_list(void)
{
    return &db_list;
}


w_err_t db_entry_create(char *dbname,w_uint16_t attr)
{
    w_dlist_s *dblist;
    w_db_s *db;
    db = db_get_byname(dbname);
    WIND_ASSERT_RETURN(db == W_NULL,W_ERR_INVALID);
    db = (w_db_s *)db_malloc(sizeof(w_db_s));
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_MEM);
    db->magic = DB_MAGIC;
    wind_strncpy(db->name,dbname,TB_NAME_LEN);
    DNODE_INIT(db->dbnode);
    DLIST_INIT(db->tblist);
    db->base = (w_addr_t)db;
    db->attr = attr;
    db->hash = get_db_hash(db->name);
    WIND_ASSERT_RETURN(db->hash > 0,W_ERR_INVALID);
    db->tb_count = 0;
    dblist = get_db_list();
    dlist_insert_tail(dblist,&db->dbnode);
    return W_ERR_OK;
}

w_err_t db_entry_destroy(w_db_s *db)
{
    w_dlist_s *dblist;
    w_dnode_s *node;
    w_tb_s *tentry;
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    dblist = get_db_list();
    dlist_remove(dblist,&db->dbnode);
    node = dlist_head(&db->tblist);
    if(node != W_NULL)
    {
        foreach_node(node,&db->tblist)
        {
            node = dlist_remove_head(&db->tblist);
            tentry = NODE_TO_TBENTRY(node);
            tb_entry_destroy(tentry);
        }
    }
    wind_memset(db,0,sizeof(w_db_s));
    db_free(db);
    return W_ERR_OK;
}

w_err_t db_entry_init(void)
{
    DLIST_INIT(db_list);
    return W_ERR_OK;
}

w_db_s *db_get_byname(char *dbname)
{
    w_dlist_s *dblist;
    w_db_s *db;
    w_dnode_s *dnode;
    w_int32_t hash;
    WIND_ASSERT_RETURN(dbname != W_NULL,W_NULL);
    hash = get_db_hash(dbname);
    WIND_ASSERT_RETURN(hash > 0,W_NULL);
    dblist = get_db_list();
    foreach_node(dnode,dblist)
    {
        db = NODE_TO_DBENTRY(dnode);
        if(hash != db->hash)
            continue;
        if(wind_strcmp(db->name,dbname) == 0)
            return db;
    }
    return W_NULL;
}

w_bool_t db_entry_exist(char *dbname)
{
    w_db_s *db;
    WIND_ASSERT_RETURN(dbname != W_NULL,W_FALSE);
    db = db_get_byname(dbname);
    if(db != W_NULL)
        return W_TRUE;
    return W_FALSE;
}

w_err_t db_entry_setattr(w_db_s *db,w_uint16_t attr)
{
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    db->attr = attr;
    return W_ERR_OK;
}

w_err_t db_entry_getattr(w_db_s *db,w_uint16_t *attr)
{
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    *attr = db->attr;
    return W_ERR_OK;
}

w_err_t db_entry_insert_tb(w_db_s *db,w_tb_s *tb)
{
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    dlist_insert_tail(&db->tblist,&tb->tbnode);
    db->tb_count ++;
    return W_ERR_OK;
}

w_err_t db_entry_remove_tb(w_db_s *db,w_tb_s *tb)
{
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_NULL_PTR);
    dlist_remove(&db->tblist,&tb->tbnode);
    db->tb_count --;
    return W_ERR_OK;
}


w_err_t db_entry_print_info(w_db_s *db)
{
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    wind_printf("\r\ndb info:\r\n");
    wind_printf("db name:%s\r\n",db->name);
    wind_printf("table count:%d\r\n",db->tb_count);
    return W_ERR_OK;
}


w_err_t db_entry_print_data(w_db_s *db)
{
    w_tb_s *tb;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(db->magic == DB_MAGIC,W_ERR_INVALID);
    wind_printf("|---<DB name=%s>\r\n",db->name);
    foreach_node(dnode,&db->tblist)
    {
        tb = NODE_TO_TBENTRY(dnode);
        tb_entry_print_data(tb);
    }
    wind_printf("|---</DB name=%s>\r\n",db->name);
    return W_ERR_OK;
    return W_ERR_OK;
}

w_err_t db_entry_print_all(void)
{
    w_db_s *db;
    w_dnode_s *dnode;
    w_dlist_s *dblist;
    dblist = get_db_list();
    foreach_node(dnode,dblist)
    {
        db = NODE_TO_DBENTRY(dnode);
        db_entry_print_data(db);
    }
    return W_ERR_OK;
}

#endif //#if WIND_MODULE_DB_SUPPORT


