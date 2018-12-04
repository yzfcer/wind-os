#include "config.h"
#include "disc.h"
#include "partition.h"
#include "fs.h"
#include "ui.h"
#include "efs.h"
#include "file.h"
#include "mkfs.h"
#include "ioman.h"
#include "debug.h"
#include "extract.h"
#include "plibc.h"

#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
//#define File1Name "/test/test_framework.txt"
#define File1Name "test1/test2/testT/Test1111.txt"
#define File2Name "test1/test2/testT/Test2222.txt"
#define File3Name "test1/test2/testT/Test6666.txt"


eint32 fat32_format(EmbeddedFileSystem *fs,char *blkname);
EmbeddedFileSystem efs;

w_err_t test_write_file(char *path,w_uint8_t *data,w_int32_t lenth)
{
    File *fw;
    esint8 res;
    wind_debug("write file:%s",path);
    fw = (File*)malloc(sizeof(File));
    if(0 == file_fopen(fw,&efs.myFs,path,MODE_READ))
    {
        file_fclose(fw);
        free(fw);
        return W_ERR_OK;
    }
    res = file_fopen(fw,&efs.myFs,path,MODE_WRITE);
    WIND_ASSERT_RETURN(res == 0,W_ERR_FAIL);
    lenth = file_fwrite(fw,0,lenth,data);
    WIND_ASSERT_RETURN(lenth > 0,W_ERR_FAIL);
    file_fclose(fw);
    free(fw);
    wind_notice("fs_flushFs");
    fs_flushFs(&efs.myFs);
}

w_err_t test_read_file(char *path,w_uint8_t *data,w_int32_t lenth)
{
    File *fr;
    esint8 res;
    wind_debug("read file:%s",path);
    fr = (File*)malloc(sizeof(File));
    res = file_fopen(fr,&efs.myFs,path,MODE_READ);
    WIND_ASSERT_RETURN(res == 0,W_ERR_FAIL);
    lenth = file_fread(fr,0,lenth,data);

    WIND_ASSERT_RETURN(lenth > 0,W_ERR_FAIL);
    file_fclose(fr);
    free(fr);
}

void file_test(void)
{
    w_int32_t i,len;
    char *filename[3] = {File1Name,File2Name,File3Name};
    w_uint8_t buf[4096];
    mkdir(&efs.myFs,"test1");
    mkdir(&efs.myFs,"test1/test2");
    mkdir(&efs.myFs,"test1/test2/testT");
    for(i = 0;i < 3;i ++)
    {
        wind_memset(buf,0,sizeof(buf));
        wind_strcpy(buf,filename[i]);
        len = wind_strlen(buf);
        test_write_file(filename[i],buf,len);
    }
    wind_notice("fs_flushFs");
    fs_flushFs(&efs.myFs);
    for(i = 0;i < 3;i ++)
    {
        wind_memset(buf,0,sizeof(buf));
        
        test_read_file(filename[i],buf,sizeof(buf));
        wind_printf("######## file content:\r\n");
        wind_printf("%s\r\n",buf);
    }
    
}


int main(int argc, char** argv)
{
    w_int32_t res;
	File *fr=0;
	File *fw=0;
	
	
    if_load(NULL,"fat32.img");
    res = efs_init(&efs,"test");
    wind_notice("efs_init %s",res == 0?"OK":"ERROR");
    if(res != 0)
    {
        fat32_format(&efs,"test");
        res = efs_init(&efs,"test");
        wind_notice("efs_init %s",res == 0?"OK":"ERROR");
        if(res != 0)
        {
            wind_error("fs init error.");
            return -1;
        }
    }
    file_test();
    fs_flushFs(&efs.myFs);
    if_flush(&efs.myCard,"fat32.img");
    system("pause");
}

