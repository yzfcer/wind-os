/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_user.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2019/2/4 
** ��        ��: �����������
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019/2/4 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_user.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if (WIND_CONSOLE_SUPPORT && CMD_USER_SUPPORT)

/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/
static w_err_t add_user(w_int32_t argc,char ** argv)
{
    w_user_s *user;
    WIND_ASSERT_RETURN(argc >= 4,W_ERR_FAIL);
    user = wind_user_create(USER_COMMON,argv[2],argv[3]);
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}

static w_err_t del_user(w_int32_t argc,char ** argv)
{
    w_err_t err;
    w_user_s *user;
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_FAIL);
    user = wind_user_get(argv[2]);
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_FAIL);
    err = wind_user_destroy(user);
    return err;
}

static w_err_t modify_user(w_int32_t argc,char ** argv)
{
    w_err_t err;
    w_user_s *user;
    WIND_ASSERT_RETURN(argc >= 4,W_ERR_FAIL);
    user = wind_user_get(argv[2]);
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_FAIL);
    err = wind_user_modify_passwd(user,argv[3]);
    return err;
}


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
COMMAND_DISC(user)
{
    wind_printf("to manage user account infomation.\r\n");
}

COMMAND_USAGE(user)
{
    wind_printf("user list:to show all user information.\r\n");
    wind_printf("user add <name> <passwd>:add a new user account.\r\n");
    wind_printf("user del <name>:delete an existing user account.\r\n");
    wind_printf("user modify <name>:modify an existing user's password.\r\n");
}

COMMAND_MAIN(user,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    
    if(wind_strcmp(argv[1],"list") == 0)
    {
        return wind_user_print();
    }
    else if(wind_strcmp(argv[1],"add") == 0)
    {
        return add_user(argc,argv);
    }
    else if(wind_strcmp(argv[1],"del") == 0)
    {
        return del_user(argc,argv);
    }
    else if(wind_strcmp(argv[1],"modify") == 0)
    {
        return modify_user(argc,argv);
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(user);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
