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

static w_dlist_s tblist;
tb_model_s *wind_tbmodel_get(const char *name)
{
    return (tb_model_s*)wind_obj_get(name,&tblist);
}


w_err_t wind_tbmodel_register(tb_model_s *tbmodel)
{
    w_int32_t i;
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
    wind_obj_init(&tbmodel[i].obj,TB_MODEL_MAGIC,tbmodel[i].obj.name,&tblist);
    return W_ERR_OK;
}

w_err_t wind_tbmodel_unregister(tb_model_s *tbmodel)
{
    w_err_t err;
    WIND_ASSERT_RETURN(tbmodel != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(tbmodel->obj.magic == TB_MODEL_MAGIC,W_ERR_INVALID);
    wind_notice("unregister table:%s",tbmodel->obj.name);
    err = wind_obj_deinit(&tbmodel->obj,TB_MODEL_MAGIC,&tblist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return W_ERR_OK;
}

w_err_t wind_tbmodel_print(void)
{
    
    return W_ERR_OK;
}


TBMODEL_DECLARE(tb_demo);
TBMODEL_DECLARE(tb_demo1);

w_err_t wind_tbmodels_register(void)
{
    wind_tbmodel_register(TBMODEL(tb_demo));
    wind_tbmodel_register(TBMODEL(tb_demo1));
}


