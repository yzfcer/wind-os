#include "wind_type.h"
//#include "console_framework.h"
#include "wind_cmd.h"
CMD_DECLARE(list);
CMD_DECLARE(echo);
//CMD_DECLARE(datetime);
CMD_DECLARE(mem);
CMD_DECLARE(stati);
CMD_DECLARE(test);


void register_all_cmd(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,COMMAND(list),1);
    wind_cmd_register(&ctrl->cmd_list,COMMAND(echo),1);
    //wind_cmd_register(&ctrl->cmd_list,COMMAND(datetime),1);
    wind_cmd_register(&ctrl->cmd_list,COMMAND(stati),1);
    wind_cmd_register(&ctrl->cmd_list,COMMAND(mem),1);
    wind_cmd_register(&ctrl->cmd_list,COMMAND(test),1);
}

w_bool_t console_have_user(char *user)
{
    return B_TRUE;
}

