#include "config.h"
#include "disc.h"
#include "partition.h"
#include "fs.h"
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

eint32 fat32_format(EmbeddedFileSystem *fs,char *blkname);
EmbeddedFileSystem efs;
int main(int argc, char** argv)
{
    w_int32_t res;
	File *fr=0;
	File *fw=0;
	
	unsigned char buf[4096];
    if_load(NULL);
    res = efs_init(&efs,"test");
    if(res != 0)
    {
        fat32_format(&efs,"test");
        res = efs_init(&efs,"test");
        if(res != 0)
        {
            wind_error("fs init error.");
            return -1;
        }
    }
    
    fr = (File*)malloc(sizeof(File));
    res = file_fopen(fr,&efs.myFs,"/test.txt",MODE_READ);
    if(res != 0)
    {
        wind_memset(buf,'A',sizeof(buf));
        fr = (File*)malloc(sizeof(File));
        res = file_fopen(fw,&efs.myFs,"/test.txt",MODE_WRITE);
        file_fwrite(fw,0,sizeof(buf),buf);
        file_fclose(fw);
        free(fw);
    }
    else
    {
        file_fclose(fr);
        free(fr);
    }
        

    wind_memset(buf,0,sizeof(buf));
    fr = (File*)malloc(sizeof(File));
    res = file_fopen(fr,&efs.myFs,"/test.txt",MODE_READ);
    file_fread(fr,0,sizeof(buf),buf);
    file_fclose(fr);
    fs_flushFs(&efs.myFs);
    if_flush(&efs.myCard);
    free(fr);
    system("pause");
}

