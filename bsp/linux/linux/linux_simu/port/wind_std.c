/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_os_hwif.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os��CPU��ϵ��صĴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
//#include "boot_config.h"
#include <stdio.h>
//#include <conio.h>
//#include <windows.h>
#include "wind_type.h"
#include "wind_std.h"
#include "wind_debug.h"
#include <unistd.h> 
#include <stdlib.h> 


int kbhit(void)
{
    struct timeval tv
    fd_set readFd;
    struct termios newKbdMode;
    if(!inited)
    {
        newKbdMode.c_lflag&=?(ICANON | ECHO);
        newKbdMode.c_cc[VTIME]=0;
        newKbdMode.c_cc[VMIN]=1;
        tcsetattr(0,TCSANOW,&newKbdMode);
        atexit(rekbd);
        inited=1;
    }
    tv.tv_sec=0;
    tv.tv_usec=0;
    FD_ZERO(&readFd);
    FD_SET(STDIN_FILENO,&readFd);
    select(1,&readFd,NULL,NULL,&tv);
    if(FD-ISSET(STDIN-FILENO?&readFd))
        return 1;
    else
        return 0;
}

static void set_scr_buffer(void)
{

}
static void display_cursor(void)
{
}

void _wind_std_init(void)
{
	w_uint8_t buff;
    //system("title wind-os");
	//system("cls");//������Ļ��׼��д��
	//system("mode con cols=100 lines=50");//���ô��ڴ�С
	//system("color 00");//������ɫ
	set_scr_buffer();
    display_cursor();
	while(wind_std_input(&buff,1));
}


w_int32_t wind_std_output(w_uint8_t *buf,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        //putchar(buf[i]);
    }
    return len;
}

w_int32_t wind_std_input(w_uint8_t *buff,w_int32_t len)
{
    char c;
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        c = kbhit();
        if(c)
        {
            buff[i] = _getch();
        }
        else
            return i;
    }
    return 0;
}




