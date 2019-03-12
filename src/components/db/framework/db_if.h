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
#include "wind_heap.h"
#define TB_PARA_MAGIC 0x236F79AC
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
    w_obj_s obj;
    w_int32_t item_cnt;
    tb_item_info_s *tb_item;
}tb_param_s;

//字段属性定义
#define DB_ATTR_KEY (0x0001<<0)//可读
#define DB_ATTR_RD (0x0001<<1)//可读
#define DB_ATTR_WR (0x0001<<2) //可写
#define DB_ATTR_CRYPT (0x0001<<3) //加密
#define DB_ATTR_VISIBLE (0x0001<<4) //可见
#define DB_ATTR_NOT_W_NULL (0x0001<<5) //不能空
#define DB_ATTR_FLUSH (0x0001<<6) //要刷盘
#define DB_ATTR_SINGLE (0x0001<<7) //单条目

#define DB_ATTR_KEY_ITEM (DB_ATTR_KEY  | DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_W_NULL)//键值
#define DB_ATTR_DEFAULT_ITEM (DB_ATTR_RD | DB_ATTR_WR | DB_ATTR_VISIBLE)//默认字段属性
#define DB_ATTR_READONLY (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_W_NULL)
#define DB_ATTR_RAMONLY (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_W_NULL)



//判断属性是否满足
#define DB_ATTR_EQ(attr,va) (((attr)&(va))==(va))
#define DB_IS_KEY(attr) DB_ATTR_EQ((attr),DB_ATTR_KEY)
#define DB_IS_RD(attr) DB_ATTR_EQ((attr),DB_ATTR_RD)
#define DB_IS_WR(attr) DB_ATTR_EQ((attr),DB_ATTR_WR)
#define DB_IS_CRYPT(attr) DB_ATTR_EQ((attr),DB_ATTR_CRYPT)
#define DB_IS_VISIBLE(attr) DB_ATTR_EQ((attr),DB_ATTR_VISIBLE)
#define DB_IS_NOT_W_NULL(attr) DB_ATTR_EQ((attr),DB_ATTR_NOT_W_NULL)


//数据表定义
#define TB_OFFSET(name,mbr) (w_int32_t)(&(((name*)0)->mbr))
#define TB_MBRSIZE(name,mbr) (sizeof(((name*)0)->mbr))
#define TB_MBRCNT(name,mbr_type,mbr) (sizeof(((name*)0)->mbr)/sizeof(mbr_type))

#define TABLE_START(name) static tb_item_info_s db_info_list_##name[] = {
#define TABLE_ITEM(name,mbr_type,mbr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(name,mbr_type,mbr),TB_MBRSIZE(name,mbr),TB_OFFSET(name,mbr),DB_ATTR_DEFAULT_ITEM},
#define TABLE_ITEM_A(name,mbr_type,mbr,tb_attr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(name,mbr_type,mbr),TB_MBRSIZE(name,mbr),TB_OFFSET(name,mbr),tb_attr},
#define TABLE_END };

#define TABLE_DEF(name,attr) \
    tb_param_s tb_param_##name = \
    {{~TB_PARA_MAGIC,#name,{W_NULL,W_NULL},0,attr},\
    sizeof(db_info_list_##name)/sizeof(tb_item_info_s),&db_info_list_##name,\
    }; 

#define TABLE_DECLARE(name) extern tb_param_s tb_param_##name;

#define TABLE(name) &tb_param_##name

void *db_malloc(w_int32_t size);
w_err_t db_free(void* ptr);


//数据库函数
w_err_t wind_db_create(char *dbname,w_uint16_t attr);
w_err_t wind_db_distroy(char *dbname);

//数据表函数,数据表名称格式为 dbname.tbname
w_err_t wind_tb_create(char *tbname,tb_item_info_s *item_info,w_int32_t item_cnt);
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

