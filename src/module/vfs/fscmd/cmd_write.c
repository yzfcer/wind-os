/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_write.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : Write file content command
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




#if (CMD_FS_SUPPORT)
#include "wind_file.h"



static w_err_t cmd_write(w_int32_t argc,char **argv)
{
    w_err_t err;
    w_file_s *file = (w_file_s *)W_NULL;
    char * fullpath = W_NULL;
    w_int32_t len,filelen;
    char *curpath;
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    do
    {
        err =W_ERR_OK;
        curpath = wind_filepath_get_current();
        fullpath = wind_filepath_generate(curpath,argv[1],0);
        WIND_ASSERT_BREAK(fullpath,W_ERR_FAIL,"generate full path failed.");
        file = wind_fopen(fullpath,FMODE_W | FMODE_CRT);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_NOFILE,"open directory or file failed.");
        len = wind_strlen(argv[2]);
        filelen = wind_fwrite(file,(w_uint8_t*)argv[2],len);
        wind_fclose(file);
        if(filelen == len)
        {
            wind_printf("write file OK.\r\n");
            wind_filepath_release(fullpath);
            return W_ERR_OK;
        }
        wind_printf("write file failed.\r\n");     
        
    }while(0);

    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    return err;
}








COMMAND_DISC(write)
{
    wind_printf("to operate write file system.\r\n");
}

COMMAND_USAGE(write)
{
    wind_printf("write <file> <content>:--to write content into a file.\r\n");
    wind_printf("file : the destination file to write in.\r\n");
    wind_printf("content : what to write into the file.\r\n");
	
}

COMMAND_MAIN(write,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"write") == 0)
        return cmd_write(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(write);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
