/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: db_entry.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.3.13
**描        述: 数据表模型框架
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2019.3.13
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2019.3.13
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef TB_MODEL_H__
#define TB_MODEL_H__

#include "wind_type.h"
#include "wind_obj.h"

#define TB_MODEL_MAGIC 0x236F79AC

//字段属性定义
#define DB_ATTR_KEY          (0x0001<<0) //可读
#define DB_ATTR_RD           (0x0001<<1) //可读
#define DB_ATTR_WR           (0x0001<<2) //可写
#define DB_ATTR_CRYPT        (0x0001<<3) //加密
#define DB_ATTR_VISIBLE      (0x0001<<4) //可见
#define DB_ATTR_NOT_NULL     (0x0001<<5) //不能空
#define DB_ATTR_FLUSH        (0x0001<<6) //要刷盘
#define DB_ATTR_SINGLE       (0x0001<<7) //单条目

#define DB_ATTR_KEY_ITEM     (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_NULL | DB_ATTR_KEY)//键值
#define DB_ATTR_DEFAULT_ITEM (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_WR )//默认字段属性
#define DB_ATTR_READONLY     (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_NULL)
#define DB_ATTR_RAMONLY      (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_NULL)



//判断属性是否满足
#define DB_ATTR_EQ(attr,va)    (((attr)&(va))==(va))
#define DB_IS_KEY(attr)        DB_ATTR_EQ((attr),DB_ATTR_KEY)
#define DB_IS_RD(attr)         DB_ATTR_EQ((attr),DB_ATTR_RD)
#define DB_IS_WR(attr)         DB_ATTR_EQ((attr),DB_ATTR_WR)
#define DB_IS_CRYPT(attr)      DB_ATTR_EQ((attr),DB_ATTR_CRYPT)
#define DB_IS_VISIBLE(attr)    DB_ATTR_EQ((attr),DB_ATTR_VISIBLE)
#define DB_IS_NOT_W_NULL(attr) DB_ATTR_EQ((attr),DB_ATTR_NOT_NULL)


//数据表定义
#define TB_OFFSET(name,mbr) (w_int32_t)(&(((name*)0)->mbr))
#define TB_MBRSIZE(name,mbr) (sizeof(((name*)0)->mbr))
#define TB_MBRCNT(name,mbr_type,mbr) (sizeof(((name*)0)->mbr)/sizeof(mbr_type))



#define TBMODEL_START(name) static tbmodel_item_s db_info_list_##name[] = {
#define TBMODEL_ITEM(name,mbr_type,mbr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(name,mbr_type,mbr),TB_MBRSIZE(name,mbr),TB_OFFSET(name,mbr),DB_ATTR_DEFAULT_ITEM},
#define TBMODEL_ITEM_A(name,mbr_type,mbr,tb_attr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(name,mbr_type,mbr),TB_MBRSIZE(name,mbr),TB_OFFSET(name,mbr),tb_attr},
#define TBMODEL_END };

#define TBMODEL_DEF(name,attr) \
    tb_model_s tb_param_##name = \
    {{~TB_MODEL_MAGIC,#name,{W_NULL,W_NULL},0,attr},\
    sizeof(db_info_list_##name)/sizeof(tbmodel_item_s),(tbmodel_item_s*)&db_info_list_##name,\
    }; 

#define TBMODEL_DECLARE(name) extern tb_model_s tb_param_##name;

#define TBMODEL(name) &tb_param_##name

typedef struct
{
    char *name;
    w_uint8_t type;
    w_uint8_t count;
    w_uint16_t size;
    w_uint16_t offset;
    w_uint16_t attr;
}tbmodel_item_s;

typedef struct
{
    w_obj_s obj;
    w_int32_t item_cnt;
    tbmodel_item_s *tb_item;
}tb_model_s;

tb_model_s *wind_tbmodel_get(const char *name);
w_err_t wind_tbmodel_register(tb_model_s *tbmodel);
w_err_t wind_tbmodel_unregister(tb_model_s *tbmodel);
w_err_t wind_tbmodels_register(void);
w_err_t wind_tbmodel_print(void);


#endif
