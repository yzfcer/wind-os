/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: tb_model.c
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
#include "wind_type.h"
#include "db_if.h"
#include "wind_debug.h"
#define NODE_TO_TBMODEL(dnode) (tb_model_s*)(((w_uint8_t*)(dnode))-((w_uint32_t)&(((tb_model_s*)0)->obj.objnode)))
static w_dlist_s tbmodellist;
typedef struct
{
    w_int32_t type;
    char *typename;
}dbt_name_s;



tb_model_s *wind_tbmodel_get(const char *name)
{
    return (tb_model_s*)wind_obj_get(name,&tbmodellist);
}


w_err_t wind_tbmodel_register(tb_model_s *tbmodel)
{
    tb_model_s *tbm;    
    WIND_ASSERT_RETURN(tbmodel != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tbmodel->obj.magic == (~TB_MODEL_MAGIC),W_ERR_INVALID);


    WIND_ASSERT_RETURN(tbmodel->obj.magic == (~TB_MODEL_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(tbmodel->obj.name != W_NULL,W_ERR_PTR_NULL);
    wind_notice("register table:%s",tbmodel->obj.name);
    tbm = wind_tbmodel_get(tbmodel->obj.name);
    if(tbm != W_NULL)
    {
        wind_notice("table has been registered.\r\n");
        return W_ERR_OK;
    }
    wind_obj_init(&tbmodel->obj,TB_MODEL_MAGIC,tbmodel->obj.name,&tbmodellist);
    return W_ERR_OK;
}

w_err_t wind_tbmodel_unregister(tb_model_s *tbmodel)
{
    w_err_t err;
    WIND_ASSERT_RETURN(tbmodel != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tbmodel->obj.magic == TB_MODEL_MAGIC,W_ERR_INVALID);
    wind_notice("unregister table:%s",tbmodel->obj.name);
    err = wind_obj_deinit(&tbmodel->obj,TB_MODEL_MAGIC,&tbmodellist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return W_ERR_OK;
}

static char *get_type_name(w_uint8_t type)
{
    w_int32_t i;
    static dbt_name_s dbt_list[] =
    {
        {TYPE_w_int8_t,"int8"},
        {TYPE_w_int16_t,"int16"},
        {TYPE_w_int32_t,"int32"},
        {TYPE_w_int64_t,"int64"},
        {TYPE_w_uint8_t,"uint8"},
        {TYPE_w_uint16_t,"uint16"},
        {TYPE_w_uint32_t,"uint32"},
        {TYPE_w_uint64_t,"uint64"},
        {TYPE_w_bool_t,"bool"},
        {TYPE_w_str16_t,"str16"},
        {TYPE_w_str32_t,"str32"},
        {TYPE_w_str64_t,"str64"},
        {TYPE_w_str128_t,"str128"},
        {TYPE_w_str256_t,"str256"},
    };
    for(i = 0;i < sizeof(dbt_list)/sizeof(dbt_name_s);i ++)
        if(type == dbt_list[i].type)
            return dbt_list[i].typename;
    return "unknown";
    
}

static w_err_t wind_tbmodel_printitem(tb_model_s *tbmodel)
{
    w_int32_t i;
    tbmodel_item_s *item;
    WIND_ASSERT_RETURN(tbmodel->obj.magic == TB_MODEL_MAGIC,W_ERR_INVALID);
    wind_printf("\r\ntable %s member attribution:\r\n",tbmodel->obj.name);
    wind_print_space(7);
    wind_printf("%-16s %-8s %-8s %-8s %-8s\r\n","name","type","count","size","attr");
    wind_print_space(7);

    for(i = 0;i < tbmodel->item_cnt;i ++)
    {
        item = &tbmodel->tb_item[i];
        wind_printf("%-16s %-8s %-8d %-8d 0x%-x\r\n",
            item->name,get_type_name(item->type),item->count,
            item->size,item->attr);
    }
    wind_print_space(7);
    return W_ERR_OK;
}

w_err_t wind_tbmodel_print(void)
{
    
    w_dnode_s *dnode;
    tb_model_s *tbmodel;
    foreach_node(dnode,&tbmodellist)
    {
        tbmodel = NODE_TO_TBMODEL(dnode);
        wind_tbmodel_printitem(tbmodel);
    }
    return W_ERR_OK;
}


TBMODEL_DECLARE(tb_demo);
TBMODEL_DECLARE(tb_demo1);

w_err_t wind_tbmodels_register(void)
{
    wind_tbmodel_register(TBMODEL(tb_demo));
    wind_tbmodel_register(TBMODEL(tb_demo1));
    return W_ERR_OK;
}

w_err_t wind_tbmodels_unregister(void)
{
    wind_tbmodel_unregister(TBMODEL(tb_demo1));
    wind_tbmodel_unregister(TBMODEL(tb_demo));
    return W_ERR_OK;
}
