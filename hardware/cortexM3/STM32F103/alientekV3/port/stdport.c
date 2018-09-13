#include "stdio.h"
#include "wind_std.h"
#include "console_framework.h"
#include "wind_string.h"
#include "lua.h"
#define LUA_MAXINPUT 512
typedef int FILEHANDLE;
#ifndef W_NULL
#define W_NULL (void*)0
#endif

#pragma import(__use_no_semihosting_swi)

#pragma import(_main_redirection)

const char __stdin_name[150];

const char __stdout_name[150];

const char __stderr_name[150];

FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 0;
}

int _sys_close(FILEHANDLE fh)
{
    return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{

    return 0;

}

int _sys_read(FILEHANDLE fh, unsigned char*buf, unsigned len, int mode)
{
    return 0;
}

//检查句柄是否为终端

int _sys_istty(FILEHANDLE fh)
{
    return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    return 0;
}

//刷新句柄关联的缓冲区

int _sys_ensure(FILEHANDLE fh)
{
    return 0;
}

 

//返回文件当前长度

long _sys_flen(FILEHANDLE fh)
{
    return 0;
}

void _sys_exit(int status)
{
    //while(1);
}

int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
{
			 return 0;
}

//将一个字符写入控制台

void _ttywrch(int ch)
{
    wind_std_output((w_uint8_t*)&ch,1);
}

int remove(const char *filename)
{
    return 0;
}

char *_sys_command_string(char *cmd, int len)
{
     return W_NULL;
}

int fputc(int ch, FILE *f)
{ 
	wind_std_output((w_uint8_t*)&ch,1);//你的串口发送字符函数  
	return ch;
}

int clock(void *ptr)
{
    return 0;
}
