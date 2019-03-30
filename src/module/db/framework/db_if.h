/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**文   件   名: db_if.h
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
#ifndef DB_IF_H__
#define DB_IF_H__
#include "wind_type.h"
#include "db_def.h"
#include "tb_model.h"
#include "wind_heap.h"

void *db_malloc(w_int32_t size);
w_err_t db_free(void* ptr);

w_err_t _wind_db_mod_init(void);

//数据库函数
w_err_t wind_db_create(char *dbname,w_uint16_t attr);
w_err_t wind_db_distroy(char *dbname);

//数据表函数,数据表名称格式为 dbname.tbname
w_err_t wind_tb_create(char *tbname,tbmodel_item_s *item_info,w_int32_t item_cnt);
w_err_t wind_tb_distroy(char *tbname);

//数据操作函数,
w_err_t wind_tb_insert(char *tbname,void *row_data,w_int32_t row_size);
w_err_t wind_tb_delete(char *tbname,w_int32_t row_idx);
w_err_t wind_tb_get_row_index(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size);
w_err_t wind_tb_modify(char *tbname,w_int32_t tbindex,void *row_data,w_int32_t row_size);
//数据字段名称格式为 dbname.tbname.mbrname
w_err_t wind_tb_modify_value(char *tbname,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size);
//cond的格式为"mbrname1=value1&&mbr2=value2"类似的格式
w_err_t wind_tb_query_cond_count(char *tbname,char *cond,w_int32_t *idxlist,w_int32_t cnt);
w_err_t wind_tb_query_count(char *tbname,w_int32_t *count);
w_err_t wind_tb_getdata(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size);

#endif

