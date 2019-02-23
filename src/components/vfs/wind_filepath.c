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
#if WIND_FS_SUPPORT
static char *curpath = W_NULL;

char *wind_file_get_current_path(void)
{
    return curpath;
}

void wind_file_set_current_path(char *path)
{
    w_int32_t len = wind_strlen(path);
    if(curpath != W_NULL)
        wind_free(curpath);
    curpath = wind_malloc(len +1);
    wind_memcpy(curpath,path,len+1);
}

char *wind_full_path_generate(char *pre_path,char *relative_path,w_uint16_t isdir)
{
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
    return path;
}

w_err_t wind_full_path_release(char *path)
{
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


#endif
