/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : db_if.c
** Author      : Jason Zhou
** Last Date   : 2017.12.19
** Description : DB API interface
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
#include "db_if.h"
#include "db_entry.h"
#include "tb_model.h"
#include "wind_debug.h"
#if WIND_MODULE_DB_SUPPORT

void *db_malloc(w_int32_t size)
{
    return wind_malloc(size);
}
w_err_t db_free(void* ptr)
{
    return wind_free(ptr);
}

w_err_t _wind_db_mod_init(void)
{
    wind_tbmodel_init();
    db_entry_init();
    return W_ERR_OK;
}

w_err_t _wind_db_mod_deinit(void)
{
    wind_tbmodels_unregister();
    return W_ERR_OK;
}

w_err_t wind_db_create(char *dbname,w_uint16_t attr)
{
    return db_entry_create(dbname,attr);
}

w_err_t wind_db_distroy(char *dbname)
{
    w_db_s *entry = db_get_byname(dbname);
    return db_entry_destroy(entry);
}


w_err_t wind_tb_create(char *tbname,tbmodel_item_s *item_info,w_uint16_t item_cnt)
{
    return tb_entry_create(tbname,item_info,item_cnt);
}

w_err_t wind_tb_distroy(char *tbname)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_destroy(tb);
}


w_err_t wind_tb_insert(char *tbname,void *row_data,w_int32_t row_size)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    WIND_ASSERT_RETURN(tb != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tb->data_cnt < 65535,W_ERR_FAIL);
    return tb_entry_insert(tb,row_data,row_size);
}

w_err_t wind_tb_delete(char *tbname,w_int32_t row_idx)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_delete(tb,row_idx);
}

w_err_t wind_tb_get_row_index(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_get_data(tb,row_idx,data,data_size);
}

w_err_t wind_tb_modify(char *tbname,w_int32_t tbindex,void *row_data,w_int32_t row_size)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_modify(tb,tbindex,row_data,row_size);
}

w_err_t wind_tb_modify_value(char *tbname,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_modify_value(tb,mbrname,row_idx,data,data_size);
}

w_err_t wind_tb_query_cond_count(char *tbname,char *cond,w_int32_t *idxlist,w_int32_t cnt)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_query_cond_count(tb,cond,idxlist,cnt);
}

w_err_t wind_tb_query_count(char *tbname,w_int32_t *count)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_query_count(tb,count);
}

w_err_t wind_tb_getdata(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size)
{
    w_tb_s *tb;
    tb = tb_entry_get_byname(tbname);
    return tb_entry_get_data(tb,row_idx,data,data_size);
}

#endif //#if WIND_MODULE_DB_SUPPORT
