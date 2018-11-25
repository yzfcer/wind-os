/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  file_port.c
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2018/11/25
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "boot_pack.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_conv.h"
#include "wind_macro.h"
#include <stdio.h>
#include <malloc.h>
//#include <stdlib.h>

void *wind_malloc(w_uint32_t size)
{
    return malloc(size);
}

void wind_free(void *ptr)
{
    free(ptr);
}

w_int32_t read_file(char *filename,w_uint8_t *buff,w_int32_t size)
{
    FILE *file;
    w_uint32_t len;
    file = fopen(filename,"rb");
    WIND_ASSERT_RETURN(file != W_NULL,0);
    len = fread(buff,1,size,file);
    WIND_ASSERT_TODO(len > 0,fclose(file),W_ERR_FAIL);
    fclose(file);
    return len;
}


w_int32_t read_long_file(char *path,w_uint8_t **buff)
{
    FILE*fp;
    w_int32_t flen;
    fp=fopen(path,"rb");
    WIND_ASSERT_RETURN(fp != W_NULL,W_ERR_INVALID);
    fseek(fp,0L,SEEK_END); 
    flen=ftell(fp); 
    *buff = (w_uint8_t*)wind_malloc(flen); 
    WIND_ASSERT_RETURN(*buff != W_NULL,W_ERR_MEM);
    fseek(fp,0L,SEEK_SET); 
    flen = fread(*buff,1,flen,fp); 
    return flen;
}

w_int32_t write_file(char *filename,w_uint8_t *data,w_int32_t len)
{
    FILE *file;
    //w_uint32_t len;
    file = fopen(filename,"wb");
    WIND_ASSERT_RETURN(file != W_NULL,0);
    len = fwrite(data,1,len,file);
    fclose(file);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

