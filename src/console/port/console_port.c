#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_CONSOLE_SUPPORT
CMD_DECLARE(list);
CMD_DECLARE(echo);
//CMD_DECLARE(datetime);
CMD_DECLARE(mem);
CMD_DECLARE(stati);
CMD_DECLARE(test);
CMD_DECLARE(reset);
CMD_DECLARE(bee);
CMD_DECLARE(led);
CMD_DECLARE(blkdev);



void _wind_register_all_cmd(console_s *ctrl)
{
    wind_cmd_register(COMMAND(list),1);
    wind_cmd_register(COMMAND(echo),1);
    //wind_cmd_register(COMMAND(datetime),1);
    wind_cmd_register(COMMAND(stati),1);
    wind_cmd_register(COMMAND(mem),1);
    wind_cmd_register(COMMAND(test),1);
    wind_cmd_register(COMMAND(reset),1);
    wind_cmd_register(COMMAND(bee),1);
    wind_cmd_register(COMMAND(led),1);
    wind_cmd_register(COMMAND(blkdev),1);
}

w_bool_t console_have_user(char *user)
{
    return B_TRUE;
}

#endif
