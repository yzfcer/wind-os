/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    :  file_port.c
** Author      :      Jason Zhou
** Version     :     1.0
** Date        :        2018/11/25
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
#if 0
void *wind_malloc(w_uint32_t size)
{
    return malloc(size);
}

void wind_free(void *ptr)
{
    free(ptr);
}
#endif

w_int32_t read_file(char *filename,w_int32_t offset,w_uint8_t *buff,w_int32_t size)
{
    errno_t errno;
    FILE *file;
    w_uint32_t len;
    errno = fopen_s(&file,filename,"rb");
    WIND_ASSERT_RETURN(errno == 0,0);
    WIND_ASSERT_RETURN(file != W_NULL,0);

    fseek(file,offset,SEEK_SET); 
    len = fread(buff,1,size,file);
    WIND_ASSERT_TODO_RETURN(len > 0,fclose(file),W_ERR_FAIL);
    fclose(file);
    return len;
}


w_int32_t read_long_file(char *path,w_int32_t offset,w_uint8_t **buff)
{
    errno_t errno;
    FILE*file;
    w_int32_t flen;
    errno = fopen_s(&file,path,"rb");
    WIND_ASSERT_RETURN(errno == 0,0);
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
    errno_t errno;
    FILE *file;
    errno = fopen_s(&file,filename,"wb");
    WIND_ASSERT_RETURN(errno == 0,0);
    WIND_ASSERT_RETURN(file != W_NULL,0);
    
    fseek(file,offset,SEEK_SET); 
    len = fwrite(data,1,len,file);
    fclose(file);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

