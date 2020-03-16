/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: module_lua.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: lua�ű���������
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
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_debug.h"
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_LUA_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(lua)
{
    wind_printf("to enter lua shell mode.\r\n");
}

COMMAND_USAGE(lua)
{
    wind_printf("lua:--to run into lua application.\r\n");
    wind_printf("lua test:--to run a test lua script.\r\n");
    wind_printf("lua file <luafile>:--to run a lua script file.\r\n");
}

extern int lua_main (int argc, char **argv);
COMMAND_MAIN(lua,argc,argv)
{
    w_int32_t ret;
    ret = lua_main (argc, argv);
    return ret == 0?W_ERR_OK:W_ERR_FAIL;
}

COMMAND_DEF(lua);


//--------------------------------------------------------------------------
MODULE_INIT(lua)
{
    w_err_t err = W_ERR_NOT_SUPPORT;
    err = wind_cmd_register(COMMAND(lua));
    return err;
}

MODULE_EXIT(lua)
{
        w_err_t err = W_ERR_NOT_SUPPORT;
        err = wind_cmd_unregister(COMMAND(lua));
        return err;
}

MODULE_DEF(lua, 0x0100,"shell");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
