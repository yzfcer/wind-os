/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_cat.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : Display file content operation command
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
#include "wind_file.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_CAT_SUPPORT

#define BUF_SIZE 128

static w_err_t cmd_cat(w_int32_t argc,char **argv)
{
    w_err_t err;
    w_file_s *file = (w_file_s *)W_NULL;
    char * fullpath = W_NULL;
    w_uint8_t *buff = W_NULL;
    w_int32_t len;
    char *curpath;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        curpath = wind_filepath_get_current();
        fullpath = wind_filepath_generate(curpath,argv[1],0);
        file = wind_fopen(fullpath,FMODE_R);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_NOFILE,"open file failed.");
        WIND_ASSERT_BREAK(file->isdir == 0, W_ERR_INVALID,"can not show a directory content");
        buff = wind_malloc(BUF_SIZE+1);
        WIND_ASSERT_BREAK(buff != W_NULL,W_ERR_MEM,"alloc buffer failed.");
        wind_printf("\r\n---------%s---------\r\n",fullpath);
        while(1)
        {
            wind_memset(buff,0,BUF_SIZE+1);
            len = wind_fread(file,buff,BUF_SIZE);
            if(len > 0)
                wind_printf("%s",(char*)buff);
            else
                break;
        }
        wind_printf("\r\n---------%s---------\r\n",fullpath);        
        wind_fclose(file);

    }while(0);

    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    if(buff != W_NULL)
        wind_free(buff);
    return err;
}






COMMAND_DISC(cat)
{
    wind_printf("to operate cat file system.\r\n");
}

COMMAND_USAGE(cat)
{
    wind_printf("cat <file>:--to show file content.\r\n");
    wind_printf("file : the file need to show its content.\r\n");
}

COMMAND_MAIN(cat,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"cat") == 0)
        return cmd_cat(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(cat);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
