/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: windows_filepath.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: windows文件路径处理相关函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"

#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
#include "wind_file.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "hostfs.h"
#include "windows_filepath.h"

#include <windows.h>

//static char *curpath = (char*)W_NULL;

char *windows_filepath_remove_tail(char *path)
{
    w_err_t err;
    w_int32_t len;
    char *new_path = (char *)W_NULL;
    WIND_ASSERT_RETURN(path != W_NULL,(char *)W_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,(char *)W_NULL);
    do
    {
        err = W_ERR_OK;
        new_path = wind_salloc(path,HP_ALLOCID_HOSTFS);
        WIND_ASSERT_BREAK(new_path != W_NULL,W_ERR_MEM,"alloc new_path failed");
        len = wind_strlen(new_path);
        if(new_path[len - 1] == '/')
            new_path[len - 1] = 0;
    }while(0);
    return new_path;
}

hfileattr_e windows_file_type(char *path)
{
    w_err_t err;
    hfileattr_e attr;
    char *realpath = (char*)W_NULL;
    HANDLE handle;
    WIN32_FIND_DATAA FindFileData;
    wind_trace("path=%s",path);
    do
    {
        err = W_ERR_OK;
        realpath = windows_filepath_remove_tail(path);
        WIND_ASSERT_BREAK(realpath != W_NULL,W_ERR_MEM,"alloc realpath failed");
        handle = FindFirstFileA(realpath,&FindFileData);
        WIND_ASSERT_BREAK(realpath != W_NULL,W_ERR_MEM,"FindFirstFileA failed");
        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            attr = HFILE_TYPE_DIR;
        else
            attr = HFILE_TYPE_FILE;
    }while(0);
    if(err != W_ERR_OK)
        attr = HFILE_TYPE_ERROR;
    if(realpath != W_NULL)
        windows_filepath_release(realpath);
    return attr;
}

w_err_t windows_filepath_check_valid(char *path)
{
    w_int32_t i,len;
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    wind_trace("path=%s",path);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0,W_ERR_INVALID);
        
    for(i = 0;i < len; i ++)
    {
        if(path[i] >= 0x2D && path[i] <= 0x39)
            continue;
        if(path[i] >= 0x41 && path[i] <= 0x5A)
            continue;
        if(path[i] >= 0x61 && path[i] <= 0x7A)
            continue;
        if((path[i] == '_')||(path[i] == ':')||(path[i] == '*'))
            continue;
        wind_error("invalid character:%c",path[i]);
        return W_ERR_FAIL;
    }

    len -= 1;
    for(i = 0;i < len;i ++)
    {
        if((path[i] == '/') && (path[i+1] == '/'))
        {
            wind_error("invalid path:%s",path);
            return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

char *windows_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir)
{
    w_err_t err;
    char *path = (char *)W_NULL;
    w_int32_t prelen,relalen,pathlen;
    WIND_ASSERT_RETURN(pre_path != W_NULL,(char *)W_NULL);
    WIND_ASSERT_RETURN(relative_path != W_NULL,(char *)W_NULL);
    wind_trace("pre_path=%s",pre_path);
    wind_trace("relative_path=%s",relative_path);
    prelen = wind_strlen(pre_path);
    WIND_ASSERT_RETURN(prelen >= 2,(char *)W_NULL);
    WIND_ASSERT_RETURN(pre_path[1] == ':',(char *)W_NULL);
    
    
    relalen = wind_strlen(relative_path);
    prelen = wind_strlen(pre_path);
    pathlen = prelen + relalen+3;
    path = (char *)wind_alloc(pathlen,HP_ALLOCID_HOSTFS);
    wind_strcpy(path,pre_path);
    if(pre_path[prelen-1] != '/')
    {
        path[prelen] = '/';
        prelen ++;
    }
    if((relative_path[0] == '/'))
    {
        path[prelen-1] = 0;
        prelen --;
    }
    wind_strcat(path,relative_path);
    
    
    pathlen = wind_strlen(path);
    if((isdir) && (path[pathlen-1] != '/'))
        path[pathlen - 1] = '/';
    err = windows_filepath_check_valid(path);
    if(err != W_ERR_OK)
    {
        wind_free(path);
        path = (char *)W_NULL;
    }
    return path;
}

w_bool_t windows_filepath_isdir(char *path)
{
    w_int32_t len;
    WIND_CHECK_RETURN(path != W_NULL,W_FALSE);
    wind_trace("path=%s",path);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0, W_FALSE);
    WIND_CHECK_RETURN(path[len-1] == '/',W_FALSE);
    return W_TRUE;
}

w_err_t windows_do_remove_dir(char *fullpath)
{
    w_int32_t len;
    w_err_t err;
    char *cmd = (char *)W_NULL;
    wind_trace("fullpath=%s",fullpath);
    do
    {
        err = W_ERR_OK;
        len = wind_strlen(fullpath) + 1;
        len += sizeof("rd /s /q ");
        cmd = (char *)hostfs_mem_malloc(len);
        WIND_ASSERT_BREAK(cmd != W_NULL,W_ERR_MEM,"alloc cmd failed");
        wind_strcpy(cmd,"rd /s /q ");
        wind_strcat(cmd,fullpath);
        wind_strrpc(cmd,'/','\\');
        wind_printf(cmd);
        system(cmd);
    }while(0);
    if(cmd != W_NULL)
        hostfs_mem_free(cmd);
    return err;
    
}



#if 0
w_err_t windows_filepath_set_current(char *path)
{
    w_err_t err;
    w_int32_t len;
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    err = windows_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    len = wind_strlen(path);
    if(curpath != W_NULL)
        wind_free(curpath);
    curpath = (char*)wind_alloc(len + 2,HP_ALLOCID_VFS);
    wind_memcpy(curpath,path,len+1);
    if(curpath[len-1] != '/')
    {
        curpath[len] = '/';
        curpath[len+1] = 0;
    }
    return W_ERR_OK;
}

char * windows_filepath_get_current(void)
{
    if(curpath == W_NULL)
        windows_filepath_set_current("/");
    return curpath;
}

#endif

char * windows_filepath_copy(char *path)
{
    char *newpath;
    WIND_ASSERT_RETURN(path != W_NULL,(char*)W_NULL);
    wind_trace("path=%s",path);
    newpath = (char*)wind_salloc(path,HP_ALLOCID_VFS);
    WIND_ASSERT_RETURN(newpath != W_NULL,(char*)W_NULL);
    return newpath;
}



w_err_t windows_filepath_release(char *path)
{
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    wind_trace("path=%s",path);
    return wind_free(path);
}


w_int32_t windows_filepath_split(char *path,char **layers,w_int32_t layercnt)
{
    w_err_t err;
    w_int32_t i,j,len,cnt = 0;
    WIND_ASSERT_RETURN(path != W_NULL,-1);
    wind_trace("path=%s",path);
    //WIND_ASSERT_RETURN(path[0] == '/',-1);
    WIND_ASSERT_RETURN(layers != W_NULL,-1);
    WIND_ASSERT_RETURN(layercnt >= 2,-1);
    err = windows_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);
    len = wind_strlen(path)+1;
    j = 0;
    for(i = 0;i < layercnt;i ++)
    {
        if(cnt >= layercnt)
            return -1;
        layers[i] = &path[j];
        for(;j < len;j ++)
        {
            if((path[j] == '/') || (path[j] == '\0'))
            {
                cnt ++;
                path[j] = 0;
                j ++;
                break;
            }
        }
        if(j >= len)
            break;
    }
    cnt = (layers[cnt-1][0] != 0?cnt:cnt -1);
    return cnt;
}



char* windows_filepath_get_parent(char *path)
{
    w_err_t err;
    w_int32_t i,len;
    char *tmppath;
    WIND_ASSERT_RETURN(path != W_NULL,(char*)W_NULL);
    wind_trace("path=%s",path);
    //WIND_ASSERT_RETURN(path[0] == '/',(char*)W_NULL);
    err = windows_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,(char*)W_NULL);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len >= 1,(char*)W_NULL);
    tmppath = (char*)wind_salloc(path,HP_ALLOCID_VFS);
    WIND_ASSERT_RETURN(tmppath != W_NULL,(char*)W_NULL);
    if(tmppath[len-1] == '/')
        tmppath[len-1] = '\0';
    for(i = len - 1;i >= 0;i --)
    {
        if(tmppath[i] == '/')
        {
            tmppath[i+1] = 0;
            return tmppath;
        }
    }
    wind_free(tmppath);
    return (char*)W_NULL;
}

char* windows_filepath_get_filename(char *path)
{
    w_err_t err;
    w_int32_t i,len;
    char *tmppath;
    WIND_ASSERT_RETURN(path != W_NULL,(char*)W_NULL);
    wind_trace("path=%s",path);
    //WIND_ASSERT_RETURN(path[0] == '/',(char*)W_NULL);
    err = windows_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,(char*)W_NULL);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len >= 1,(char*)W_NULL);
    tmppath = (char*)wind_salloc(path,HP_ALLOCID_VFS);
    WIND_ASSERT_RETURN(tmppath != W_NULL,(char*)W_NULL);
    if(tmppath[len-1] == '/')
        tmppath[len-1] = '\0';
    if(tmppath[0] == 0)
        return tmppath;
    for(i = len - 1;i >= 0;i --)
    {
        if(tmppath[i] == '/')
        {
            tmppath[i] = 0;
            wind_strcpy(tmppath,&tmppath[i+1]);
            return tmppath;
        }
    }
    wind_free(tmppath);
    return (char*)W_NULL;
}

char* windows_filepath_to_directory(char *path)
{
    w_int32_t len;
    char *newpath;
    WIND_ASSERT_RETURN(path != W_NULL,(char*)W_NULL);
    wind_trace("path=%s",path);
    len = wind_strlen(path);
    if(!windows_filepath_isdir(path))
    {
        newpath = (char*)wind_alloc(len+2,HP_ALLOCID_VFS);
        WIND_ASSERT_RETURN(newpath != W_NULL,(char*)W_NULL);
        wind_strcpy(newpath,path);
        newpath[len] = '/';
        newpath[len+1] = 0;
    }
    else
        newpath = (char*)wind_salloc(path,HP_ALLOCID_VFS);
    return newpath;
}


#endif
