/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: tank_module.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: ̹�˴�ս��Ϸ
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
#include "wind_config.h"
#if (WIND_MODULE_GAME_SUPPORT)
#include "wind_std.h"
#include "wind_cmd.h"
#include "wind_module.h"
#include<windows.h>
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_GAME_SUPPORT)

/********************************************internal variables**********************************************/

//----------------------------------------------------------------
COMMAND_DISC(gobang)
{
    wind_printf("[WIN32] to play gobang war game.\r\n");
}

COMMAND_USAGE(gobang)
{
    wind_printf("gobang:--to start gobang game.\r\n");
}
extern int gobang_main(int argc,char **argv);
COMMAND_MAIN(gobang,argc,argv)
{
    gobang_main(argc,argv);
    _wind_std_init();
    return W_ERR_OK;
}
COMMAND_DEF(gobang);
//----------------------------------------------------------------

COMMAND_DISC(pushbox)
{
    wind_printf("[WIN32] to play pushbox game.\r\n");
}

COMMAND_USAGE(pushbox)
{
    wind_printf("pushbox:--to start pushbox game.\r\n");
}
extern int pushbox_main(int argc,char **argv);
COMMAND_MAIN(pushbox,argc,argv)
{
    system("cls");
    pushbox_main(argc,argv);
    _wind_std_init();
    return W_ERR_OK;
}
COMMAND_DEF(pushbox);
//----------------------------------------------------------------


COMMAND_DISC(snake)
{
    wind_printf("[WIN32] to play snakes war game.\r\n");
}

COMMAND_USAGE(snake)
{
    wind_printf("snake:--to start snakes war game.\r\n");
}
extern int snake_main(int argc,char **argv);
COMMAND_MAIN(snake,argc,argv)
{
    snake_main(argc,argv);
    _wind_std_init();
    return W_ERR_OK;
}
COMMAND_DEF(snake);
//----------------------------------------------------------------


COMMAND_DISC(tank)
{
    wind_printf("[WIN32] to play tanks war game.\r\n");
}

COMMAND_USAGE(tank)
{
    wind_printf("tank:--to start tanks war game.\r\n");
}
extern int tank_main(int argc,char **argv);
COMMAND_MAIN(tank,argc,argv)
{
    tank_main(argc,argv);
    _wind_std_init();
    return W_ERR_OK;
}

COMMAND_DEF(tank);
//----------------------------------------------------------------



COMMAND_DISC(tetris)
{
    wind_printf("[WIN32] to play tetris game.\r\n");
}

COMMAND_USAGE(tetris)
{
    wind_printf("tetris:--to start tetris game.\r\n");
}
extern int tetris_main(int argc,char **argv);
COMMAND_MAIN(tetris,argc,argv)
{
    system("cls");
    tetris_main(argc,argv);
    _wind_std_init();
    return W_ERR_OK;
}
COMMAND_DEF(tetris);
//----------------------------------------------------------------


/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/

MODULE_INIT(game)
{
    wind_cmd_register(COMMAND(gobang));
    wind_cmd_register(COMMAND(pushbox));
    wind_cmd_register(COMMAND(snake));
    wind_cmd_register(COMMAND(tank));
    wind_cmd_register(COMMAND(tetris));
    return W_ERR_OK;
}

MODULE_EXIT(game)
{
    wind_cmd_unregister(COMMAND(gobang));
    wind_cmd_unregister(COMMAND(pushbox));
    wind_cmd_unregister(COMMAND(snake));
    wind_cmd_unregister(COMMAND(tank));
    wind_cmd_unregister(COMMAND(tetris));
    return W_ERR_OK;
}

MODULE_DEF(game, 0x0100,"shell");



#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
