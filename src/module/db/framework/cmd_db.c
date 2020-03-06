/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: cmd_DB.c
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: DBģ���������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "db_entry.h"
#if (CMD_DB_SUPPORT)
static w_err_t cmd_db_list(void)
{
    return db_entry_print_all();
 }

static w_err_t cmd_db_detail(char *dbname)
{
    w_db_s *db;
    db = db_get_byname(dbname);
    WIND_ASSERT_RETURN(db != W_NULL,W_ERR_NO_OBJ);
    return db_entry_print_data(db);
}

static w_err_t cmd_db_create(char *dbname)
{
    return db_entry_create(dbname,DB_ATTR_DEFAULT_ITEM);
}

static w_err_t cmd_db_delete(char *dbname)
{
    w_db_s *entry = db_get_byname(dbname);
    return db_entry_destroy(entry);
}


COMMAND_DISC(db)
{
    wind_printf("to operate db system.\r\n");
}

COMMAND_USAGE(db)
{
    wind_printf("db list:--to show DB list.\r\n");
    wind_printf("db detail <dbname>:--to show DB list.\r\n");
    wind_printf("db create <dbname>:--to create DB model.\r\n");
    wind_printf("db delete <dbname>:--to delete DB model.\r\n");
}

COMMAND_MAIN(db,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
        return cmd_db_list();
    else if(wind_strcmp(argv[1],"detail") == 0)
        return cmd_db_detail(argv[2]);
    else if(wind_strcmp(argv[1],"create") == 0)
        return cmd_db_create(argv[2]);
    else if(wind_strcmp(argv[1],"delete") == 0)
        return cmd_db_delete(argv[2]);
    return W_ERR_OK;
}

COMMAND_DEF(db);
#endif
