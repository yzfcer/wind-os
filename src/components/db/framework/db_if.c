/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**文   件   名: db_if.c
**创   建   人: Jason Zhou
**最后修改日期: 2017/11/23
**描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2017/11/23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "db_def.h"
#include "db_if.h"
#include "db_entry.h"

void *db_malloc(w_int32_t size)
{
    return wind_malloc(size);
}
w_err_t db_free(void* ptr)
{
    return wind_free(ptr);
}

w_err_t wind_db_create(char *dbname,w_uint16_t attr)
{
    return db_entry_create(dbname,attr);
}

w_err_t wind_db_distroy(char *dbname)
{
    return db_entry_destroy(dbname);
}


w_err_t wind_tb_create(char *tbname,tb_item_info_s *item_info,w_int32_t item_cnt)
{
    return tb_entry_create(tbname,item_info,item_cnt);
}

w_err_t wind_tb_distroy(char *tbname)
{
    return tb_entry_destroy(tbname);
}


w_err_t wind_tb_insert(char *tbname,void *row_data,w_int32_t row_size)
{
    return tb_entry_insert(tbname,row_data,row_size);
}

w_err_t wind_tb_delete(char *tbname,w_int32_t row_idx)
{
    return tb_entry_delete(tbname,row_idx);
}

w_int32_t wind_tb_get_row_index(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size)
{
    return tb_entry_get_data(tbname,row_idx,data,data_size);
}

w_err_t wind_tb_modify(char *tbname,w_int32_t tbindex,void *row_data,w_int32_t row_size)
{
    return tb_entry_modify(tbname,tbindex,row_data,row_size);
}

w_err_t wind_tb_modify_value(char *tbname,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size)
{
    return tb_entry_modify_value(tbname,mbrname,row_idx,data,data_size);
}

w_err_t wind_tb_query_cond_count(char *tbname,char *cond,w_int32_t *idxlist,w_int32_t cnt)
{
    return tb_entry_query_cond_count(tbname,cond,idxlist,cnt);
}

w_err_t wind_tb_query_count(char *tbname,w_int32_t *count)
{
    return tb_entry_query_count(tbname,count);
}

w_err_t wind_tb_getdata(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size)
{
    return tb_entry_get_data(tbname,row_idx,data,data_size);
}

