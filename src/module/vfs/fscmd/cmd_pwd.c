/****************************************Copyright (c)**************************************************
**                                       ??  ??  ??  ??
** ??   ??   ??: cmd_pwd.c
** ??   ??   ??: Jason Zhou
** ??????????: 2018/5/13 20:24:37
** ??        ??: ?????????????
**  
**--------------????汾???----------------------------------------------------------------------------
** ??????: Jason Zhou
** ??  ??: v1.0
** ?????: 2018/5/13 20:24:37
** ?衡??: ???汾
**
**--------------????汾???----------------------------------------------------------------------------
** ?????: 
** ?????: 
** ?衡??: 
** ???????C??????????????????ɡ?------------??纣????????????????------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_string.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************????????***********************************************/

#if (CMD_FS_SUPPORT)
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
