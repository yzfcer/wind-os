/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_os_hwif.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的CPU体系相关的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
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
    CONSOLE_CURSOR_INFO cursor_info = {100, 1}; //1表示显示
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);	
}

void _wind_std_init(void)
{
	w_uint8_t buff;
    system("title wind-os");
	system("cls");//清理屏幕，准备写入
	system("mode con cols=100 lines=50");//设置窗口大小
	system("color 00");//设置颜色
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
        if(c && (c <= 128) && (c >= 0))
        {
            buff[i] = _getch();
			//wind_printf("_kbhit:%d\r\n",buff[i]);
        }
        else
            return i;
    }
    return len;
}




