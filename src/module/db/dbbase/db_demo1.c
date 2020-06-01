/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : db_demo1.c
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
#include "db_demo1.h"
#include "db_entry.h"

//Define database structure
TBMODEL_START(tb_demo1)
TBMODEL_ITEM_A(tb_demo1,w_int32_t,key,DB_ATTR_KEY_ITEM)
TBMODEL_ITEM(tb_demo1,w_int32_t,ademo1)
TBMODEL_ITEM(tb_demo1,w_int8_t,bdemo1)
TBMODEL_ITEM(tb_demo1,w_int16_t,cdemo1)
TBMODEL_ITEM(tb_demo1,w_bool_t,bodemo1)
TBMODEL_ITEM(tb_demo1,w_str64_t,strdemo)
TBMODEL_END
TBMODEL_DEF(tb_demo1,DB_ATTR_SINGLE)
