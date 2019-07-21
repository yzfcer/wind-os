/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: module_tools.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/11/09 20:24:37
** 描        述: 创建文件系统命令模块
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/11/09 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "efs.h"
#include <io.h>
#include "io.h"
#include "file.h"
#include "file_port.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_MKFS_SUPPORT)
#define BUFF_LEN 0x10000

static EmbeddedFileSystem efs;
static w_uint8_t *buff = W_NULL;
static w_err_t mk_fat32_file(char *root_dir,struct _finddata_t *fileinfo,char *newfile)
{
    File *fw;
    esint8 res;
    w_int32_t flen;
    wind_notice("root:%s",root_dir);
    wind_notice("file:%s",newfile);
    
    return W_ERR_OK;
}

static w_err_t mk_fat32_dir(char *root_dir,struct _finddata_t *fileinfo,char *newdir)
{
    wind_notice("root:%s",root_dir);
    wind_notice("dir :%s",newdir);
    return W_ERR_OK;
}

static int copy_dir(char *root_dir,char *path)
{
    struct _finddata_t fileinfo;
    int File_Handle;
    int i=0;
    char newpath[256];
    wind_memset(newpath,0,sizeof(newpath));
    wind_sprintf(newpath,"%s\\*.*",path);
    File_Handle = _findfirst(newpath,&fileinfo);
    if(File_Handle==-1)
    {
        wind_printf("dir is NOT exist.\n");
        return 0;
    }
    do
    {
        wind_memset(newpath,0,sizeof(newpath));
        wind_sprintf(newpath,"%s\\%s",path,fileinfo.name);
        if(_A_ARCH & fileinfo.attrib)
        {
            wind_debug("FILE : %s\\%s\n",path,fileinfo.name);
            mk_fat32_file(root_dir,&fileinfo,newpath);
        }
        else if(_A_SUBDIR & fileinfo.attrib)
        {
            wind_debug("DIR  : %s\\%s\n",path,fileinfo.name);
            mk_fat32_dir(root_dir,&fileinfo,newpath);
            if((wind_strcmp(".",fileinfo.name) != 0)&&(wind_strcmp("..",fileinfo.name) != 0))
            {
                copy_dir(root_dir,newpath);
            }
        }
        i++;
    }while(0==_findnext(File_Handle,&fileinfo));
    _findclose(File_Handle);
    return 0;
}





/********************************************内部变量定义**********************************************/



extern w_err_t fat32_format(EmbeddedFileSystem *fs,char *blkname);
//mkfs fat32 <dir> <fsfile> <fssize> 
static w_err_t mkfs_fat32(w_int32_t argc,char **argv)
{
    char *file;
    WIND_ASSERT_RETURN(argc >= 5,W_ERR_INVALID);
    file = malloc(wind_strlen(argv[1])+10);
    wind_memset(file,0,wind_strlen(argv[1])+10);
    wind_strcpy(file,argv[2]);
    wind_strcat(file,"\\*.*");
    //copy_dir();
    return W_ERR_OK;
}


/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(mkfs)
{
    wind_printf("[*PC*] to display mkfs string that you input.\r\n");
}

COMMAND_USAGE(mkfs)
{
    wind_printf("mkfs fat32 <dir> <fsfile> <fssize>:--to  make a file system image.\r\n");
}

//mkfs fat32 <dir> <fsfile> <fssize> 
COMMAND_MAIN(mkfs,argc,argv)
{
    w_err_t err = W_ERR_OK;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    buff = wind_malloc(BUFF_LEN);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_MEM);
    if(wind_strcmp(argv[1],"fat32") == 0)
    {
        wind_memset(&efs,0,sizeof(efs));
        err = mkfs_fat32(argc,argv);
        if(err != W_ERR_OK)
            wind_error("make fat32 file system failed.");
        
        copy_dir("F:\\color","F:\\color");
    }
    else
        wind_printf("mkfs command invalid.\r\n");
    wind_free(buff);
    buff = W_NULL;
    return err;
}

COMMAND_DEF(mkfs);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
