/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_fs.c
** 创   建   人: 周江村
** 最后修改日期: 2018/5/13 20:24:37
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2018/5/13 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 文件系统命令
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_string.h"
#include "wind_file.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (WIND_CONSOLE_SUPPORT && WIND_FS_SUPPORT)

/********************************************内部变量定义**********************************************/
#define BUF_SIZE 128


/********************************************内部函数定义*********************************************/




static w_err_t fs_cmd_pwd(void)
{
    char *curpath = wind_file_get_current_path();
    console_printf("%s\r\n",curpath);
    return ERR_OK;
}

static w_err_t fs_cmd_cd(w_int32_t argc,char **argv)
{
    w_bool_t isexist;
    char *fullpath;
    char *curpath = wind_file_get_current_path();
    if(argc < 3)
        return ERR_FAIL;
    fullpath = wind_full_path_generate(curpath,argv[2],1);
    isexist = wind_file_existing(fullpath);
    if(!isexist)
    {
        console_printf("directory:\"%s\" is NOT exist.\r\n",fullpath);
        wind_full_path_release(fullpath);
        return ERR_FILE_NOT_EXIT;
    }
    wind_file_set_current_path(fullpath);
    wind_full_path_release(fullpath);
    return ERR_OK;
}

static w_err_t mknode(w_int32_t argc,char **argv,w_uint16_t isdir)
{
    w_bool_t isexist;
    file_s *file;
    char * fullpath;
    char *curpath = wind_file_get_current_path();
    if(argc < 3)
        return ERR_INVALID_PARAM;
    fullpath = wind_full_path_generate(curpath,argv[2],isdir);
    isexist = wind_file_existing(fullpath);
    if(isexist)
    {
        console_printf("directory has been existing.\r\n");
        wind_full_path_release(fullpath);
        wind_file_close(file);
        return ERR_FAIL;
    }
    
    file = wind_file_open(fullpath,FMODE_CRT);
    if(file == NULL)
        console_printf("make directory failed.");
    wind_file_close(file);
    wind_full_path_release(fullpath);
    return ERR_OK;
}

static w_err_t fs_cmd_mkdir(w_int32_t argc,char **argv)
{
    return mknode(argc,argv,1);
}

static w_err_t fs_cmd_touch(w_int32_t argc,char **argv)
{
    return mknode(argc,argv,0);
}

static w_err_t fs_cmd_rm(w_int32_t argc,char **argv)
{
    w_err_t err;
    file_s *file;
    w_bool_t isexist;
    char *curpath = wind_file_get_current_path();
    char * fullpath = wind_full_path_generate(curpath,argv[2],1);
    isexist = wind_file_existing(fullpath);
    if(!isexist)
    {
        console_printf("open directory or file failed.\r\n");
        wind_full_path_release(fullpath);
        return ERR_FILE_NOT_EXIT;
    }
    err = wind_file_remove(file);
    wind_file_close(file);
    wind_full_path_release(fullpath);
    return err;
}

static w_err_t fs_cmd_ls(w_int32_t argc,char **argv)
{
    w_int32_t i;
    file_s *file;
    char *fullpath,*sub;
    char *curpath = wind_file_get_current_path();
    if(argc >= 3)
       fullpath = wind_full_path_generate(curpath,argv[2],1);
    else
        fullpath = wind_full_path_generate(curpath,curpath,1);
    WIND_ASSERT_RETURN(fullpath != NULL,ERR_FAIL);
    file = wind_file_open(fullpath,FMODE_R);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n");
        wind_free(fullpath);
        return ERR_FILE_NOT_EXIT;
    }
    for(i = 0;;i ++)
    {
        sub = wind_file_subfile(file,i);
        if(sub == NULL)
            break;
        console_printf("%-24s ",sub);
        if(i%4 == 3)
            console_printf("\r\n");
    }
    console_printf("\r\n");
    wind_file_close(file);
    wind_full_path_release(fullpath);
    return ERR_OK;
}

static w_err_t fs_cmd_cat(w_int32_t argc,char **argv)
{
    file_s *file;
    char * fullpath;
    w_uint8_t *buff;
    w_int32_t len;
    char *curpath = wind_file_get_current_path();
    if(argc < 3)
        return ERR_INVALID_PARAM;
    fullpath = wind_full_path_generate(curpath,argv[2],0);
    file = wind_file_open(fullpath,FMODE_R);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n",fullpath);
        wind_free(fullpath);
        return ERR_FILE_NOT_EXIT;
    }
    buff = wind_malloc(BUF_SIZE+1);
    if(buff == NULL)
    {
        wind_free(fullpath);
        return ERR_FAIL;
    }
    console_printf("\r\n---------%s---------\r\n",fullpath);
    while(1)
    {
        wind_memset(buff,0,BUF_SIZE+1);
        len = wind_file_read(file,buff,BUF_SIZE);
        if(len > 0)
            console_printf("%s",(char*)buff);
        else
            break;
    }
    console_printf("\r\n---------%s---------\r\n",fullpath);
    wind_file_close(file);
    wind_full_path_release(fullpath);
    wind_free(buff);
    return ERR_OK;
}
    
static w_err_t fs_cmd_write(w_int32_t argc,char **argv)
{
    file_s *file;
    char * fullpath;
    w_int32_t len,filelen;
    char *curpath = wind_file_get_current_path();
    if(argc < 4)
        return ERR_INVALID_PARAM;
    fullpath = wind_full_path_generate(curpath,argv[2],0);
    file = wind_file_open(fullpath,FMODE_W|FMODE_CRT);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n",fullpath);
        wind_full_path_release(fullpath);
        return ERR_FILE_NOT_EXIT;
    }

    len = wind_strlen(argv[3]);
    filelen = wind_file_write(file,(w_uint8_t*)argv[3],len);
    wind_file_close(file);
    if(filelen == len)
    {
        console_printf("write file OK.\r\n");
        wind_full_path_release(fullpath);
        return ERR_OK;
    }
    console_printf("write file failed.\r\n");
    wind_full_path_release(fullpath);
    return ERR_FAIL;
}



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(fs)
{
    console_printf("to operate fs file system.\r\n");
}

COMMAND_USAGE(fs)
{
    console_printf("fs pwd:to show current user path.\r\n");
    console_printf("fs cd:to change current user path.\r\n");
    console_printf("fs mkdir:to make a directory path.\r\n");
    console_printf("fs touch:to make a file.\r\n");
    console_printf("fs rm:to remove a directory or file.\r\n");
    console_printf("fs ls:to show files in a directory.\r\n");
    console_printf("fs cat:to show file context.\r\n");
    console_printf("fs write:to write context into a file.\r\n");
}

COMMAND_MAIN(fs,argc,argv)
{
    //path_init();
    if(argc < 2)
        return ERR_FAIL;
    if(wind_strcmp(argv[1],"pwd") == 0)
        return fs_cmd_pwd();
    else if(wind_strcmp(argv[1],"cd") == 0)
        return fs_cmd_cd(argc,argv);
    else if(wind_strcmp(argv[1],"mkdir") == 0)
        return fs_cmd_mkdir(argc,argv);
    else if(wind_strcmp(argv[1],"touch") == 0)
        return fs_cmd_touch(argc,argv);
    else if(wind_strcmp(argv[1],"ls") == 0)
        return fs_cmd_ls(argc,argv);
    else if(wind_strcmp(argv[1],"cat") == 0)
        return fs_cmd_cat(argc,argv);
    else if(wind_strcmp(argv[1],"write") == 0)
        return fs_cmd_write(argc,argv);
    else if(wind_strcmp(argv[1],"rm") == 0)
        return fs_cmd_rm(argc,argv);
    return ERR_OK;
}

COMMAND_DEF(fs);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
