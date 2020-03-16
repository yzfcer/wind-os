/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: module_db.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: dbģ�����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2015/1/24 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ���������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_debug.h"
#include "db_if.h"
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_DB_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/
#if CMD_DB_SUPPORT
COMMAND_DECLARE(db);
COMMAND_DECLARE(tbmodel);
#endif


/********************************************global functions**********************************************/

MODULE_INIT(db)
{
    w_err_t err;
#if CMD_DB_SUPPORT
    err = wind_cmd_register(COMMAND(db));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_cmd_register(COMMAND(tbmodel));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
#endif
    err = _wind_db_mod_init();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

MODULE_EXIT(db)
{
    w_err_t err = W_ERR_OK;
#if CMD_DB_SUPPORT
    err = wind_cmd_unregister(COMMAND(db));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = wind_cmd_unregister(COMMAND(tbmodel));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
#endif
    return err;
}

MODULE_DEF(db, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
