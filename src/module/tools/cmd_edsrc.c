/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_edsrc.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : Edit source file and align it
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 20:24:37
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
#include "wind_file.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if (CMD_EDSRC_SUPPORT)
static w_err_t do_handle_file(w_file_s *file)
{
    wind_printf("do_handle_file %s\r\n",file->fullpath);
    return W_ERR_OK;
}

static w_err_t handle_file(w_file_s *file,char *suffix)
{
    w_int32_t i;
    w_err_t err;
	char *fullname = (char *)W_NULL;
    w_file_s *sub,*dir;
    err = W_ERR_OK;
    wind_debug("handle_file path:%s\r\n",file->fullpath);
    if(file->isdir == 0)
        return do_handle_file(file);
    for(i = 0;;i ++)
    {
        sub = wind_freaddir(file);
        if(sub == W_NULL)
            break;
        if((wind_strcmp(sub->filename,".") == 0) ||
            (wind_strcmp(sub->filename,"..") == 0))
            continue;

        fullname = wind_filepath_generate(file->fullpath,sub->filename,sub->isdir);
        WIND_ASSERT_RETURN(fullname != W_NULL,W_ERR_FAIL);
        
        dir = wind_fopen(fullname,FMODE_R);
        WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_FAIL);
        
        err = handle_file(dir,suffix);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        
        err = wind_fclose(dir);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        
        err = wind_filepath_release(fullname);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    return W_ERR_OK;
    
}


COMMAND_DISC(edsrc)
{
    wind_printf("[*PC*] to edit source file and align it.\r\n");
}

COMMAND_USAGE(edsrc)
{
    wind_printf("edsrc <src_dir> <suffix>:--to edit source files in the directory.\r\n");
}

COMMAND_MAIN(edsrc,argc,argv)
{
    w_file_s *file;
    w_err_t err;
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_FAIL);
    file = wind_fopen(argv[1],FMODE_R);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_FAIL);
    err = handle_file(file,argv[2]);
    wind_fclose(file);
    return err;
}

COMMAND_DEF(edsrc);


#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
