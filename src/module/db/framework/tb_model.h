/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: db_entry.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.3.13
**��        ��: ���ݱ�ģ�Ϳ��,���ݱ�ģ�Ͷ�����һ�����Ļ����ṹ����ص����ԣ�ϵͳ���Ը��ݸ�ģ���ڲ�ͬ��
**              ��DB�д�����ͬ�����ݱ������ᵼ�·��ʳ�ͻ
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 2019.3.13
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 2019.3.13
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef TB_MODEL_H__
#define TB_MODEL_H__

#include "wind_type.h"
#include "wind_obj.h"

#define TB_MODEL_MAGIC 0x236F79AC

//�ֶ����Զ���
#define DB_ATTR_KEY          (0x0001<<0) //�ɶ�
#define DB_ATTR_RD           (0x0001<<1) //�ɶ�
#define DB_ATTR_WR           (0x0001<<2) //��д
#define DB_ATTR_CRYPT        (0x0001<<3) //����
#define DB_ATTR_VISIBLE      (0x0001<<4) //�ɼ�
#define DB_ATTR_NOT_NULL     (0x0001<<5) //���ܿ�
#define DB_ATTR_FLUSH        (0x0001<<6) //Ҫˢ��
#define DB_ATTR_SINGLE       (0x0001<<7) //����Ŀ

#define DB_ATTR_KEY_ITEM     (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_NULL | DB_ATTR_KEY)//��ֵ
#define DB_ATTR_DEFAULT_ITEM (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_WR )//Ĭ���ֶ�����
#define DB_ATTR_READONLY     (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_NULL)
#define DB_ATTR_RAMONLY      (DB_ATTR_RD | DB_ATTR_VISIBLE | DB_ATTR_NOT_NULL)



//�ж������Ƿ�����
#define DB_ATTR_EQ(attr,va)    (((attr)&(va))==(va))
#define DB_IS_KEY(attr)        DB_ATTR_EQ((attr),DB_ATTR_KEY)
#define DB_IS_RD(attr)         DB_ATTR_EQ((attr),DB_ATTR_RD)
#define DB_IS_WR(attr)         DB_ATTR_EQ((attr),DB_ATTR_WR)
#define DB_IS_CRYPT(attr)      DB_ATTR_EQ((attr),DB_ATTR_CRYPT)
#define DB_IS_VISIBLE(attr)    DB_ATTR_EQ((attr),DB_ATTR_VISIBLE)
#define DB_IS_NOT_W_NULL(attr) DB_ATTR_EQ((attr),DB_ATTR_NOT_NULL)


//���ݱ�����
#define TB_OFFSET(name,mbr) (w_uint16_t)(w_addr_t)(&(((name*)0)->mbr))
#define TB_MBRSIZE(name,mbr) (sizeof(((name*)0)->mbr))
#define TB_MBRCNT(name,mbr_type,mbr) (sizeof(((name*)0)->mbr)/sizeof(mbr_type))



#define TBMODEL_START(name) static tbmodel_item_s db_info_list_##name[] = {
#define TBMODEL_ITEM(name,mbr_type,mbr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(name,mbr_type,mbr),TB_MBRSIZE(name,mbr),TB_OFFSET(name,mbr),DB_ATTR_DEFAULT_ITEM},
#define TBMODEL_ITEM_A(name,mbr_type,mbr,tb_attr) {#mbr,TYPE_##mbr_type,TB_MBRCNT(name,mbr_type,mbr),TB_MBRSIZE(name,mbr),TB_OFFSET(name,mbr),tb_attr},
#define TBMODEL_END };

#define TBMODEL_DEF(name,attr) \
    tb_model_s tb_param_##name = \
    {{~TB_MODEL_MAGIC,0,attr,#name,{W_NULL,W_NULL}},\
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

w_err_t wind_tbmodel_init(void);
w_dlist_s *wind_tbmodel_get_list(void);
tb_model_s *wind_tbmodel_get(const char *name);
w_err_t wind_tbmodel_register(tb_model_s *tbmodel);
w_err_t wind_tbmodel_unregister(tb_model_s *tbmodel);
w_err_t wind_tbmodels_register(void);
w_err_t wind_tbmodels_unregister(void);
w_err_t wind_tbmodel_print_list(void);
w_err_t wind_tbmodel_print(tb_model_s *tbmodel);
w_err_t wind_tbmodel_print_all(void);


#endif
