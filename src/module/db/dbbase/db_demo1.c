/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**文   件   名: tb_demo1.c
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
#include "tb_model.h"
#include "db_demo1.h"
#include "db_entry.h"
//定义数据库结构

TBMODEL_START(tb_demo1)
TBMODEL_ITEM_A(tb_demo1,w_int32_t,key,DB_ATTR_KEY_ITEM)
TBMODEL_ITEM(tb_demo1,w_int32_t,ademo1)
TBMODEL_ITEM(tb_demo1,w_int8_t,bdemo1)
TBMODEL_ITEM(tb_demo1,w_int16_t,cdemo1)
TBMODEL_ITEM(tb_demo1,w_bool_t,bodemo1)
TBMODEL_ITEM(tb_demo1,w_str64_t,strdemo)
TBMODEL_END
TBMODEL_DEF(tb_demo1,DB_ATTR_SINGLE)
