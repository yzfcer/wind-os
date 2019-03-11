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
#include "db.h"
#include "wind_heap.h"

typedef struct
{
    char *name;
    w_uint8_t type;
    w_uint8_t count;
    w_uint16_t size;
    w_uint16_t offset;
    w_uint16_t attr;
}tb_item_info_s;

typedef struct
{
    char *name;
    w_uint16_t attr;
    tb_item_info_s *tb_item;
    w_int32_t item_cnt;
}tb_param_s;

//�ֶ����Զ���
#define DB_ATTR_KEY (0x0001<<0)//�ɶ�
#define DB_ATTR_RD (0x0001<<1)//�ɶ�
#define DB_ATTR_WR (0x0001<<2) //��д
#define DB_ATTR_CRYPT (0x0001<<3) //����
#define DB_ATTR_VISIBLE (0x0001<<4) //�ɼ�
#define DB_ATTR_NOT_W_NULL (0x0001<<5) //���ܿ�
#define DB_ATTR_FLUSH (0x0001<<6) //Ҫˢ��
#define DB_ATTR_SINGLE (0x0001<<7) //����Ŀ

#define DB_ATTR_KEY_ITEM (DB_ATTR_KEY  | DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_W_NULL)//��ֵ
#define DB_ATTR_DEFAULT_ITEM (DB_ATTR_RD | DB_ATTR_WR | DB_ATTR_VISIBLE)//Ĭ���ֶ�����
#define DB_ATTR_READONLY (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_W_NULL)
#define DB_ATTR_RAMONLY (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_W_NULL)



//�ж������Ƿ�����
#define DB_ATTR_EQ(attr,va) (((attr)&(va))==(va))
#define DB_IS_KEY(attr) DB_ATTR_EQ((attr),DB_ATTR_KEY)
#define DB_IS_RD(attr) DB_ATTR_EQ((attr),DB_ATTR_RD)
#define DB_IS_WR(attr) DB_ATTR_EQ((attr),DB_ATTR_WR)
#define DB_IS_CRYPT(attr) DB_ATTR_EQ((attr),DB_ATTR_CRYPT)
#define DB_IS_VISIBLE(attr) DB_ATTR_EQ((attr),DB_ATTR_VISIBLE)
#define DB_IS_NOT_W_NULL(attr) DB_ATTR_EQ((attr),DB_ATTR_NOT_W_NULL)


//���ݱ���
#define TB_OFFSET(tb_type,mbr) (w_int32_t)(&(((tb_type*)0)->mbr))
#define TB_MBRSIZE(tb_type,mbr) (sizeof(((tb_type*)0)->mbr))
#define TB_MBRCNT(tb_type,mbr_type,mbr) (sizeof(((tb_type*)0)->mbr)/sizeof(mbr_type))

#define TABLE_START(tb_type) static tb_item_info_s db_info_list_##tb_type[] = {
#define TABLE_ITEM(tb_type,mbr_type,mbr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(tb_type,mbr_type,mbr),TB_MBRSIZE(tb_type,mbr),TB_OFFSET(tb_type,mbr),DB_ATTR_DEFAULT_ITEM},
#define TABLE_ITEM_A(tb_type,mbr_type,mbr,tb_attr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(tb_type,mbr_type,mbr),TB_MBRSIZE(tb_type,mbr),TB_OFFSET(tb_type,mbr),tb_attr},
#define TABLE_END };

#define TABLE_DEF(tb_type,attr) \
    tb_param_s tb_param_##tb_type = \
    {#tb_type,attr,db_info_list_##tb_type,\
    sizeof(db_info_list_##tb_type)/sizeof(tb_item_info_s)}; 

#define TABLE_DECLARE(tb_type) extern tb_param_s tb_param_##tb_type;

#define TABLE_PARA(tb_type) tb_param_##tb_type.tb_item,tb_param_##tb_type.item_cnt

void *db_malloc(w_int32_t size);
w_err_t *db_free(void* ptr);


//���ݿ⺯��
w_err_t wind_db_create(char *dbname,w_uint16_t attr);
w_err_t wind_db_distroy(char *dbname);

//���ݱ���,���ݱ����Ƹ�ʽΪ dbname.tbname
w_err_t wind_tb_create(char *tbname,tb_item_info_s *item_info,w_int32_t item_cnt);
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

