/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : tb_demo.c
** Author      : Jason Zhou
** Last Date   : 2017/11/23
** Description : 
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2017/11/23
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "tb_model.h"
#include "db_demo.h"
#include "db_entry.h"
//定义数据库结构

TBMODEL_START(tb_demo)
TBMODEL_ITEM_A(tb_demo,w_int32_t,key,DB_ATTR_KEY_ITEM)
TBMODEL_ITEM(tb_demo,w_int32_t,ademo)
TBMODEL_ITEM(tb_demo,w_int8_t,bdemo)
TBMODEL_ITEM(tb_demo,w_int16_t,cdemo)
TBMODEL_END
TBMODEL_DEF(tb_demo,DB_ATTR_SINGLE)



