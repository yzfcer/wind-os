/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_pwd.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : Show file directory path command
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018/5/13 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_string.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************????????***********************************************/

#if CMD_PWD_SUPPORT
#include "wind_file.h"
/********************************************???????????**********************************************/


/********************************************???????????*********************************************/

static w_err_t cmd_pwd(w_int32_t argc,char **argv)
{
    char *fullpath;
    char *curpath = wind_filepath_get_current();
	if(argc >= 2)
	   fullpath = wind_filepath_generate(curpath,argv[1],1);
	else
		fullpath = wind_filepath_generate(curpath,curpath,1);
    wind_printf("%s\r\n",fullpath);
    return W_ERR_OK;
}

/********************************************??????????**********************************************/



/********************************************??????????**********************************************/
COMMAND_DISC(pwd)
{
    wind_printf("to operate pwd file system.\r\n");
}

COMMAND_USAGE(pwd)
{
    wind_printf("pwd:--to show current user path.\r\n");
}

COMMAND_MAIN(pwd,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"pwd") == 0)
        return cmd_pwd(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(pwd);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
