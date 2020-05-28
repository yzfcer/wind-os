/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_ls.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : Show file list command
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


/*********************************************header file***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************internal variables**********************************************/

/********************************************internal functions**********************************************/
static w_err_t print_filename(w_file_s *file)
{
    w_err_t err;
    w_int32_t len;
    char *buf = W_NULL;
    do
    {
        err = W_ERR_OK;
        len = wind_strlen(file->obj.name);
        buf = wind_alloc(len + 2,HP_ALLOCID_VFS);
        WIND_ASSERT_BREAK(buf != W_NULL,W_ERR_MEM,"");
        wind_memcpy(buf,file->obj.name,len);
        if(file->isdir)
        {
            buf[len] = '/';
            buf[len+1] = 0;
        }
        else
            buf[len] = 0;
        wind_printf("%-24s ",buf);
    }while(0);
    if(buf != W_NULL)
        wind_free(buf);
    return err;
}

static w_err_t cmd_ls(w_int32_t argc,char **argv)
{
    w_int32_t i;
    w_err_t err;
    w_file_s *file = (w_file_s *)W_NULL;
    char *path1;
    char *fullpath = (char*)W_NULL;
    w_file_s *sub = (w_file_s *)W_NULL;
    char *curpath = (char*)W_NULL;
    do 
    {
        err = W_ERR_OK;
        curpath = wind_filepath_get_current();
        if(argc >= 2)
           fullpath = wind_filepath_generate(curpath,argv[1],1);
        else
            fullpath = wind_filepath_generate(curpath,curpath,1);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"generate curpath error");
        path1 = wind_salloc(fullpath,0);
        wind_filepath_release(fullpath);
        fullpath = path1;
        path1 = W_NULL;
        file = wind_fopen(fullpath,FMODE_R);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_NOFILE,"open directory error");

        for(i = 0;;i ++)
        {
            sub = wind_freaddir(file);
            if(sub == W_NULL)
                break;
            //wind_printf("%-24s ",sub->obj.name);
            print_filename(sub);
            if(i % 4 == 3)
                wind_printf("\r\n");
        }
        wind_printf("\r\n");
        wind_fclose(file);
        
    }while(0);
    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    if(sub != W_NULL)
        wind_free(sub);
    return err;
}




/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(ls)
{
    wind_printf("to show file list in a directory.\r\n");
}

COMMAND_USAGE(ls)
{
    wind_printf("ls [dir]:--to show files in the directory.\r\n");
    wind_printf("    dir : the directory to show ,default is current directory.\r\n");
}

COMMAND_MAIN(ls,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"ls") == 0)
        return cmd_ls(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(ls);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
