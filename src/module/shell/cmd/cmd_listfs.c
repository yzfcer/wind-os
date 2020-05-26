/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_listfs.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : 文件系统操作命令
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
#define BUF_SIZE 128


/********************************************internal functions**********************************************/




static w_err_t listfs_cmd_pwd(void)
{
    char *curpath = wind_filepath_get_current();
    wind_printf("%s\r\n",curpath);
    return W_ERR_OK;
}

static w_err_t listfs_cmd_cd(w_int32_t argc,char **argv)
{
    w_bool_t isexist;
    char *fullpath;
    char *curpath = wind_filepath_get_current();
    if(argc < 3)
        return W_ERR_FAIL;
    fullpath = wind_filepath_generate(curpath,argv[2],1);
    isexist = wind_fexist(fullpath);
    if(!isexist)
    {
        wind_printf("directory:\"%s\" is NOT exist.\r\n",fullpath);
        wind_filepath_release(fullpath);
        return W_ERR_NOFILE;
    }
    wind_filepath_set_current(fullpath);
    wind_filepath_release(fullpath);
    return W_ERR_OK;
}

static w_err_t mk_dir_file(w_int32_t argc,char **argv,w_uint16_t isdir)
{
    w_bool_t isexist;
    w_file_s *file = (w_file_s *)W_NULL;
    char * fullpath;
    char *curpath = wind_filepath_get_current();
    if(argc < 3)
        return W_ERR_INVALID;
    fullpath = wind_filepath_generate(curpath,argv[2],isdir);
    isexist = wind_fexist(fullpath);
    if(isexist)
    {
        wind_printf("directory has been existing.\r\n");
        wind_filepath_release(fullpath);
        return W_ERR_FAIL;
    }
    
    file = wind_fopen(fullpath,FMODE_CRT);
    if(file == W_NULL)
        wind_printf("make directory failed.");
    wind_fclose(file);
    wind_filepath_release(fullpath);
    return W_ERR_OK;
}

static w_err_t listfs_cmd_mkdir(w_int32_t argc,char **argv)
{
    return mk_dir_file(argc,argv,1);
}

static w_err_t listfs_cmd_touch(w_int32_t argc,char **argv)
{
    return mk_dir_file(argc,argv,0);
}

static w_err_t listfs_cmd_rm(w_int32_t argc,char **argv)
{
    w_err_t err;
    w_int32_t len;
    char *curpath;
    char * fullpath;
    if(argc < 3)
        return W_ERR_INVALID;
    curpath = wind_filepath_get_current();
    len = wind_strlen(argv[2]);
    fullpath = wind_filepath_generate(curpath,argv[2],argv[2][len-1] == '/'?1:0);
    err = wind_fremove(fullpath);
    wind_notice("remove file :%s %s",fullpath,err == W_ERR_OK?"successed":"failed");
    wind_filepath_release(fullpath);
    return err;
}

static w_err_t listfs_cmd_ls(w_int32_t argc,char **argv)
{
    w_int32_t i;
    w_file_s *file = (w_file_s *)W_NULL;
    w_err_t err;
    char *fullpath = W_NULL;
    w_file_s *sub = (w_file_s *)W_NULL;
    char *curpath = wind_filepath_get_current();
    do 
    {
        err = W_ERR_OK;
        if(argc >= 3)
           fullpath = wind_filepath_generate(curpath,argv[2],1);
        else
            fullpath = wind_filepath_generate(curpath,curpath,1);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"generate curpath error");
        //sub = wind_malloc(sizeof(w_file_s));
        //WIND_ASSERT_BREAK(sub != W_NULL,W_ERR_FAIL,"malloc file error");
        file = wind_fopen(fullpath,FMODE_R);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_NOFILE,"open directory error");

        for(i = 0;;i ++)
        {
            sub = wind_freaddir(file);
            if(sub != W_NULL)
                break;
            wind_printf("%-24s ",sub->realpath);
            wind_error("here must be filename");
            if(i%4 == 3)
                wind_printf("\r\n");
        }
        wind_printf("\r\n");
        wind_fclose(file);
        
    }while(0);
    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    //if(sub != W_NULL)
    //    wind_free(sub);
    return err;

}

static w_err_t listfs_cmd_cat(w_int32_t argc,char **argv)
{
    w_file_s *file = (w_file_s *)W_NULL;
    char * fullpath;
    w_uint8_t *buff;
    w_int32_t len;
    char *curpath;
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    curpath = wind_filepath_get_current();
    fullpath = wind_filepath_generate(curpath,argv[2],0);
    file = wind_fopen(fullpath,FMODE_R);
    if(file == W_NULL)
    {
        wind_printf("open file %s failed.\r\n",fullpath);
        wind_free(fullpath);
        return W_ERR_NOFILE;
    }
    buff = wind_malloc(BUF_SIZE+1);
    if(buff == W_NULL)
    {
        wind_free(fullpath);
        return W_ERR_FAIL;
    }
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
    wind_filepath_release(fullpath);
    wind_free(buff);
    return W_ERR_OK;
}
    
static w_err_t listfs_cmd_write(w_int32_t argc,char **argv)
{
    w_file_s *file = (w_file_s *)W_NULL;
    char * fullpath;
    w_int32_t len,filelen;
    char *curpath = wind_filepath_get_current();
    if(argc < 4)
        return W_ERR_INVALID;
    fullpath = wind_filepath_generate(curpath,argv[2],0);
    file = wind_fopen(fullpath,FMODE_W | FMODE_CRT);
    if(file == W_NULL)
    {
        wind_printf("open directory or file failed.\r\n",fullpath);
        wind_filepath_release(fullpath);
        return W_ERR_NOFILE;
    }

    len = wind_strlen(argv[3]);
    filelen = wind_fwrite(file,(w_uint8_t*)argv[3],len);
    wind_fclose(file);
    if(filelen == len)
    {
        wind_printf("write file OK.\r\n");
        wind_filepath_release(fullpath);
        return W_ERR_OK;
    }
    wind_printf("write file failed.\r\n");
    wind_filepath_release(fullpath);
    return W_ERR_FAIL;
}



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(listfs)
{
    wind_printf("to operate listfs file system.\r\n");
}

COMMAND_USAGE(listfs)
{
    wind_printf("listfs pwd:--to show current user path.\r\n");
    wind_printf("listfs cd:--to change current user path.\r\n");
    wind_printf("listfs mkdir:--to make a directory path.\r\n");
    wind_printf("listfs touch:--to make a file.\r\n");
    wind_printf("listfs rm:--to remove a directory or file.\r\n");
    wind_printf("listfs ls:--to show files in a directory.\r\n");
    wind_printf("listfs cat:--to show file context.\r\n");
    wind_printf("listfs write:--to write context into a file.\r\n");
}

COMMAND_MAIN(listfs,argc,argv)
{
    //path_init();
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"pwd") == 0)
        return listfs_cmd_pwd();
    else if(wind_strcmp(argv[1],"cd") == 0)
        return listfs_cmd_cd(argc,argv);
    else if(wind_strcmp(argv[1],"mkdir") == 0)
        return listfs_cmd_mkdir(argc,argv);
    else if(wind_strcmp(argv[1],"touch") == 0)
        return listfs_cmd_touch(argc,argv);
    else if(wind_strcmp(argv[1],"ls") == 0)
        return listfs_cmd_ls(argc,argv);
    else if(wind_strcmp(argv[1],"cat") == 0)
        return listfs_cmd_cat(argc,argv);
    else if(wind_strcmp(argv[1],"write") == 0)
        return listfs_cmd_write(argc,argv);
    else if(wind_strcmp(argv[1],"rm") == 0)
        return listfs_cmd_rm(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(listfs);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
