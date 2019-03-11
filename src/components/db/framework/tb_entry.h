#ifndef DB_TABLE__ENTRY_H_
#define DB_TABLE__ENTRY_H_
#include "db.h"
#include "db_if.h"
#include "wind_type.h"
//#include "db_adapter.h"
#include "db_entry.h"

w_err_t tb_entry_create(char *tbname,tb_item_info_s *info,w_int32_t item_cnt);
w_bool_t tb_entry_exist(char *tbname);
w_err_t tb_entry_destroy(char *tbname);

w_err_t tb_entry_insert(char *tbname,void *data,w_int32_t datasize);
w_err_t tb_entry_delete(char *tbname,w_int32_t row_idx);
w_err_t tb_entry_get_data(char *tbname,w_int32_t row_idx,void *data,w_int32_t data_size);

w_err_t tb_entry_modify(char *tbname,w_int32_t row_idx,void *data,w_int32_t data_size);
w_err_t tb_entry_modify_value(char *tbname,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size);
w_err_t tb_entry_query_count(char *tbname,w_int32_t *count);

//cond的格式为"mbrname1=value1&&mbr2=value2"类似的格式
w_err_t tb_entry_query_cond_count(char *tbname,char *cond,w_int32_t *idxlist,w_int32_t cnt);

w_err_t tb_entry_print_info(char *tbname);
w_err_t tb_entry_print_data(tb_entry_s *entry);
w_err_t tb_entry_print_table(char *tbname);
void tb_print_data(tb_entry_s *entry,w_uint8_t *data,w_int32_t idx);

#endif


