/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  file_port.h
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
#include "wind_type.h"

//void *wind_malloc(w_uint32_t size);

//void wind_free(void *ptr);

w_int32_t read_file(char *filename,w_int32_t offset,w_uint8_t *buff,w_int32_t size);

w_int32_t read_long_file(char *path,w_int32_t offset,w_uint8_t **buff);

w_int32_t write_file(char *filename,w_int32_t offset,w_uint8_t *data,w_int32_t len);


