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
#include <conio.h>
#include <windows.h>
#include "wind_type.h"
#include "wind_std.h"
#include "wind_debug.h"

static void set_scr_buffer(void)
{
    HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD BuffSize;
    BuffSize.X = 100;
    BuffSize.Y = 3000;
    SetConsoleScreenBufferSize( hConsoleHandle, BuffSize );
}
static void display_cursor(void)
{
    CONSOLE_CURSOR_INFO cursor_info = {100, 1}; //1��ʾ��ʾ
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);	
}

void _wind_std_init(void)
{
	w_uint8_t buff;
    static w_bool_t init_flag = W_FALSE;
    if(init_flag)
        return;
    init_flag = W_TRUE;
    system("title wind-os");
	system("cls");//������Ļ��׼��д��
	system("mode con cols=100 lines=50");//���ô��ڴ�С
	system("color 00");//������ɫ
	set_scr_buffer();
    display_cursor();
	while(wind_std_input(&buff,1));
}

w_int32_t wind_std_output(w_uint8_t *buf,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        putchar(buf[i]);
    }
    return len;
}

w_int32_t wind_std_input(w_uint8_t *buff,w_int32_t len)
{
    char c;
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        c = _kbhit();
        if(c)
        {
            buff[i] = _getch();
        }
        else
            return i;
    }
    return len;
}




