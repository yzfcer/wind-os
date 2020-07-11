/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : file_port.c
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2018/11/25
** Description :  
** Others      : 
** History     :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#include "wind_debug.h"
#include "wind_heap.h"
#include "file_port.h"
#include <stdio.h>
#include <malloc.h>
//#include <stdlib.h>

w_int32_t read_file(char *filename,w_int32_t offset,w_uint8_t *buff,w_int32_t size)
{
    FILE *file;
    w_uint32_t len;
    file = fopen(filename,"rb");
    WIND_ASSERT_RETURN(file != W_NULL,0);

    fseek(file,offset,SEEK_SET); 
    len = fread(buff,1,size,file);
    fclose(file);
    return len;
}


w_int32_t read_long_file(char *path,w_int32_t offset,w_uint8_t **buff)
{
    FILE *file;
    w_int32_t flen;
    file = fopen(path,"rb");
    WIND_ASSERT_RETURN(file != W_NULL,0);

    fseek(file,offset,SEEK_END); 
    flen=ftell(file); 
    *buff = (w_uint8_t*)wind_malloc(flen); 
    WIND_ASSERT_RETURN(*buff != W_NULL,W_ERR_MEM);
    fseek(file,offset,SEEK_SET); 
    flen = fread(*buff,1,flen,file); 
    return flen;
}



w_int32_t write_file(char *filename,w_int32_t offset,w_uint8_t *data,w_int32_t len)
{
    FILE *file;
    file = fopen(filename,"wb");
    WIND_ASSERT_RETURN(file != W_NULL,0);
    
    fseek(file,offset,SEEK_SET); 
    len = fwrite(data,1,len,file);
    fclose(file);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

