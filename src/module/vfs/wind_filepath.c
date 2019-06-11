/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_filepath.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: 文件路径处理相关函数
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
#include "wind_file.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_debug.h"
#if WIND_FS_SUPPORT
static char *curpath = W_NULL;

char *wind_filepath_get_current(void)
{
    return curpath;
}

w_err_t wind_filepath_set_current(char *path)
{
    w_err_t err;
    w_int32_t len;
    err = wind_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    len = wind_strlen(path);
    if(curpath != W_NULL)
        wind_free(curpath);
    curpath = wind_malloc(len + 2);
    wind_memcpy(curpath,path,len+1);
    if(curpath[len-1] != '/')
    {
        curpath[len] = '/';
        curpath[len+1] = 0;
    }
    return W_ERR_OK;
}

char *wind_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir)
{
    w_err_t err;
    char *path;
    w_int32_t len,len1;
    w_int32_t ap = 0;
    len = wind_strlen(relative_path) + 1;
    if(isdir)
        len += 1;
    if(relative_path[0] == '/')
    {
        path = wind_malloc(len+ap);
        wind_memset(path,0,len+ap);
        wind_strcpy(path,relative_path);
    }
    else
    {
        len1 = wind_strlen(pre_path) + 1;
        len += len1;
        path = wind_malloc(len);
        wind_memset(path,0,len);
        wind_strcpy(path,pre_path);
        if(pre_path[len1-1] != '/')
            path[len1] = '/';
        wind_strcat(path,relative_path);
    }
    
    len = wind_strlen(path);
    if(isdir && (path[len-1] != '/'))
        path[len] = '/';
    err = wind_filepath_check_valid(path);
    if(err != W_ERR_OK)
    {
        wind_free(path);
        path = W_NULL;
    }
    return path;
}

char *  wind_filepath_copy(char *path)
{
    char *newpath;
    WIND_ASSERT_RETURN(path != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_NULL);
    newpath = wind_salloc(path);
    WIND_ASSERT_RETURN(newpath != W_NULL,W_NULL);
    return newpath;
}

w_err_t wind_filepath_release(char *path)
{
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    return wind_free(path);
}


w_int32_t wind_split_path(char *path,char **layers,w_int32_t layercnt)
{
    w_int32_t i,j,cnt = 0;
    int len = wind_strlen(path)+1;
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

w_err_t wind_filepath_check_valid(char *path)
{
    w_int32_t i,j,len;
    char chset[] = {'~','!','@','#','$','%','^','&','*','+','=','?','\t','\r','\n'};
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(path[0] == '/',W_ERR_INVALID);
    for(i = 0;i < sizeof(chset);i ++)
    {
        for(j = 0;j < len; j ++)
        {
            if(chset[i] == path[j])
            {
                wind_error("invalid character:%c",path[j]);
                return W_ERR_FAIL;
            }
                
        }
    }
    len = sizeof(chset) - 1;
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

w_err_t wind_filepath_get_parent(char *path)
{
    w_err_t err;
    w_int32_t i,len;
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_ERR_FAIL);
    err = wind_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 1,W_ERR_INVALID);
    if(path[len-1] == '/')
        path[len-1] = '\0';
    for(i = len -1;i > 0;i --)
    {
        if(path[i] == '/')
        {
            path[i+1] = 0;
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_err_t wind_pathfile_get_filename(char *path)
{
    w_err_t err;
    w_int32_t i,len;
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path[0] != 0,W_ERR_FAIL);
    err = wind_filepath_check_valid(path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 1,W_ERR_INVALID);
    if(path[len-1] == '/')
        path[len-1] = '\0';
    for(i = len -1;i > 0;i --)
    {
        if(path[i] == '/')
        {
            path[i+1] = 0;
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}


#endif
