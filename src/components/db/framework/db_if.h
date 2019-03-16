/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**��   ��   ��: db_if.h
**��   ��   ��: Jason Zhou
**����޸�����: 2017/11/23
**��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2017/11/23
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
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

//���ݿ⺯��
w_err_t wind_db_create(char *dbname,w_uint16_t attr);
w_err_t wind_db_distroy(char *dbname);

//���ݱ���,���ݱ����Ƹ�ʽΪ dbname.tbname
w_err_t wind_tb_create(char *tbname,tbmodel_item_s *item_info,w_int32_t item_cnt);
w_err_t wind_tb_distroy(char *tbname);

//���ݲ�������,
w_err_t wind_tb_insert(char *tbname,void *row_data,w_int32_t row_size);
w_err_t wind_tb_delete(char *tbname,w_int32_t row_idx);
w_err_t wind_tb_get_row_index(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size);
w_err_t wind_tb_modify(char *tbname,w_int32_t tbindex,void *row_data,w_int32_t row_size);
//�����ֶ����Ƹ�ʽΪ dbname.tbname.mbrname
w_err_t wind_tb_modify_value(char *tbname,char *mbrname,w_int32_t row_idx,void *data,w_int32_t data_size);
//cond�ĸ�ʽΪ"mbrname1=value1&&mbr2=value2"���Ƶĸ�ʽ
w_err_t wind_tb_query_cond_count(char *tbname,char *cond,w_int32_t *idxlist,w_int32_t cnt);
w_err_t wind_tb_query_count(char *tbname,w_int32_t *count);
w_err_t wind_tb_getdata(char * tbname,w_int32_t row_idx,void * data,w_int32_t data_size);

#endif

