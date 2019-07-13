/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: module_tools.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/11/09 20:24:37
** ��        ��: �����ļ�ϵͳ����ģ��
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018/11/09 20:24:37
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
#include "wind_module.h"



#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_MODULE_TOOLS_SUPPORT
#if CMD_PACK_SUPPORT
w_int32_t pack_main(w_int32_t argc,char **argv);

COMMAND_DISC(pack)
{
    wind_printf("[*PC*] to package binary files to a image file.\r\n");
}

COMMAND_USAGE(pack)
{
    wind_printf("pack cfg <boardname>:--to pack binary files with configuration defined by boardname.cfg.\r\n");
}

COMMAND_MAIN(pack,argc,argv)
{
    return pack_main(argc,argv);
}
COMMAND_DEF(pack);
#endif


MODULE_INIT(tools)
{
#if CMD_MKFS_SUPPORT
    wind_cmd_register(COMMAND(mkfs));
#endif
#if CMD_RCP_SUPPORT
    wind_cmd_register(COMMAND(rcp));
#endif
#if CMD_PACK_SUPPORT
    wind_cmd_register(COMMAND(pack));
#endif
    return W_ERR_OK;
}

MODULE_EXIT(tools)
{
#if CMD_MKFS_SUPPORT
   wind_cmd_unregister(COMMAND(mkfs));
#endif
#if CMD_RCP_SUPPORT
    wind_cmd_unregister(COMMAND(rcp));
#endif
#if CMD_PACK_SUPPORT
   wind_cmd_unregister(COMMAND(pack));
#endif
    return W_ERR_OK;
}

MODULE_DEF(tools, 0x0100,"shell");


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
