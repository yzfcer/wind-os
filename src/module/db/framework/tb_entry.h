/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : tb_entry.h
** Author      : Jason Zhou
** Last Date   : 2017.12.19
** Description : DB table basic operation function header
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
#ifndef DB_TABLE__ENTRY_H_
#define DB_TABLE__ENTRY_H_
#include "wind_type.h"
#include "db_def.h"
#include "db_if.h"
#include "db_entry.h"
#define NODE_TO_TBENTRY(node) (w_tb_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_tb_s*)0)->tbnode)))

w_err_t tb_entry_create(char *tbname,tbmodel_item_s *info,w_uint16_t item_cnt);
w_tb_s *tb_entry_get_byname(char *combine_name);
w_bool_t tb_entry_exist(char *tbname);
w_err_t tb_entry_destroy(w_tb_s *tb);

w_err_t tb_entry_insert(w_tb_s *tb,void *data,w_int32_t datasize);
w_err_t tb_entry_delete(w_tb_s *tb,w_int32_t row_idx);
w_err_t tb_entry_get_data(w_tb_s *tb,w_int32_t row_idx,void *data,w_int32_t data_size);

w_err_t tb_entry_modify(w_tb_s *tb,w_int32_t row_idx,void *data,w_int32_t data_size);
w_err_t tb_entry_modify_value(w_tb_s *tb,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size);
w_err_t tb_entry_query_count(w_tb_s *tb,w_int32_t *count);

//The format of cond is similar to "mbrname1 = value1 & & mbr2 = Value2"
w_err_t tb_entry_query_cond_count(w_tb_s *tb,char *cond,w_int32_t *idxlist,w_int32_t cnt);

w_err_t tb_entry_print_info(w_tb_s *tb);
w_err_t tb_entry_print_data(w_tb_s *tb);
w_err_t tb_entry_print_table(w_tb_s *tb);
void tb_print_data(w_tb_s *tb,w_uint8_t *data,w_int32_t idx);

#endif


