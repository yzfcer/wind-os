#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_CONSOLE_SUPPORT
CMD_DECLARE(list);
CMD_DECLARE(stati);
CMD_DECLARE(reset);
CMD_DECLARE(echo);
CMD_DECLARE(mem);
#if WIND_DATETIME_SUPPORT
CMD_DECLARE(datetime);
#endif
CMD_DECLARE(test);
#if WIND_BLK_DRVFRAME_SUPPORT
CMD_DECLARE(blkdev);
#endif
#if WIND_LUA_SUPPORT
CMD_DECLARE(lua);
#endif
#if WIND_FS_SUPPORT
CMD_DECLARE(treefs);
#endif
#if WIND_XMODEM_SUPPORT
CMD_DECLARE(xmodem);
#endif
#if WIND_DRVFRAME_SUPPORT
CMD_DECLARE(beep);
CMD_DECLARE(led);
#endif

void _wind_register_all_cmd(console_s *ctrl)
{
    wind_cmd_register(COMMAND(list),1);
    wind_cmd_register(COMMAND(stati),1);
    wind_cmd_register(COMMAND(reset),1);
    wind_cmd_register(COMMAND(echo),1);
    wind_cmd_register(COMMAND(mem),1);
    wind_cmd_register(COMMAND(test),1);
#if WIND_DATETIME_SUPPORT
    wind_cmd_register(COMMAND(datetime),1);
#endif
#if WIND_BLK_DRVFRAME_SUPPORT
    wind_cmd_register(COMMAND(blkdev),1);
#endif
#if WIND_LUA_SUPPORT
    wind_cmd_register(COMMAND(lua),1);
#endif
#if WIND_FS_SUPPORT
    wind_cmd_register(COMMAND(treefs),1);
#endif
#if WIND_XMODEM_SUPPORT
    wind_cmd_register(COMMAND(xmodem),1);
#endif
#if WIND_DRVFRAME_SUPPORT
    wind_cmd_register(COMMAND(beep),1);
    wind_cmd_register(COMMAND(led),1);
#endif

}


#endif
