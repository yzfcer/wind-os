#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "ff.h"
#include "file_port.h"
//#define File1Name "/test/test_framework.txt"
#define File1Name "/test1/test2/testT/Test1111.txt"
#define File2Name "/test1/test2/testT/Test2222.txt"
#define File3Name "/test1/test2/testT/Test3333.txt"
FATFS fat32_fsobj;           /* Filesystem object */

w_err_t test_write_file(char *path,w_uint8_t *data,w_int32_t lenth)
{
    FIL fil;
    FRESULT res;
    UINT bw;
    wind_notice("write file:%s",path);
    res = f_open(&fil,path, FA_READ);
    if(res == FR_OK)
    {
        wind_notice("file:%s exists",path);
        f_close(&fil);
        return W_ERR_OK;
    }
    res = f_open(&fil,path, FA_CREATE_NEW | FA_WRITE);
    WIND_ASSERT_RETURN(res == FR_OK,W_ERR_FAIL);
    res = f_write(&fil,data, lenth,&bw);
    WIND_ASSERT_RETURN(res == 0,W_ERR_FAIL);
    f_close(&fil);
}

w_err_t test_read_file(char *path,w_uint8_t *data,w_int32_t lenth)
{
    FIL fil;
    FRESULT res;
    UINT br;
    wind_notice("read file:%s",path);
    res = f_open(&fil,path, FA_READ);
    if (res != FR_OK)
    {
        wind_error("open file:%s failed",path);
        return  W_ERR_FAIL;
    }

    res = f_read(&fil, data, lenth,&br);
    if (br <= 0)
        wind_error("read file:%s failed",path);
    f_close(&fil);
    return W_ERR_OK;
}

void file_test(void)
{
    w_int32_t i,len;
    char buf[64];
    char *filename[3] = {File1Name,File2Name,File3Name};
    f_mkdir("/test1");
    f_mkdir("/test1/test2");
    f_mkdir("/test1/test2/testT");
    for(i = 0;i < 3;i ++)
    {
        wind_memset(buf,0,sizeof(buf));
        wind_strcpy(buf,filename[i]);
        len = wind_strlen(buf);
        
        test_write_file(filename[i],buf,len);
    }
    
    for(i = 0;i < 3;i ++)
    {
        wind_memset(buf,0,sizeof(buf));
        
        test_read_file(filename[i],buf,sizeof(buf));
        wind_printf("######## file content:\r\n");
        wind_printf("%s\r\n",buf);
    }
    
}

static BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */

int main(int argc, char** argv)
{
    FIL fil;            /* File object */
    FRESULT res;        /* API result code */
    UINT bw;            /* Bytes written */
    BYTE buff[32];
    //disk_load(0,"fatfs.img");

    res = f_mkfs("/test2", 2048, work, sizeof(work));
    if (res != FR_OK)
    {
        wind_error("mkfs failed.");
        return  -1;
    }
    res = f_mount(&fat32_fsobj, "/test2", 0);
    if (res != FR_OK)
    {
        wind_error("mount fs failed.");
        return  -1;
    }
    file_test();

    res = f_mount(0, "", 0);
    if (res != FR_OK)
    {
        wind_error("mount fs failed.");
        return  -1;
    }
    //disk_flush(0,"fatfs.img");
    system("pause");
}

