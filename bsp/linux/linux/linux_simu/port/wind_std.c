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
#include "wind_type.h"
#include "wind_std.h"
#include "wind_debug.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>


int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

int sh_getch(void) 
{
    int cr;
    struct termios nts, ots;

    if (tcgetattr(0, &ots) < 0) // ??????(0??????)???
        return EOF;

    nts = ots;
    cfmakeraw(&nts); // ?????Raw?????????????????????????
    if (tcsetattr(0, TCSANOW, &nts) < 0) // ??????????
        return EOF;

    cr = getchar();
    if (tcsetattr(0, TCSANOW, &ots) < 0) // ?????????
        return EOF;

    return cr;
}

w_err_t _wind_std_init(void)
{
	//w_int32_t cnt = 0;
	w_uint8_t buff;
    //system("title wind-os");
	//system("cls");//清理屏幕，准备写入
	//system("mode con cols=100 lines=50");//设置窗口大小
	//system("color 00");//设置颜色
	//set_scr_buffer();
    //display_cursor();
	while(wind_std_input(&buff,1));
	return W_ERR_OK;
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
            buff[i] = sh_getch();
        }
        else
            return i;
    }
    return 0;
}




