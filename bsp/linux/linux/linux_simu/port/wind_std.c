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
#include <error.h>
static int kbhit(void)
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



w_err_t _wind_std_init(void)
{
	w_uint8_t buff;
	struct termios term;
	printf("_wind_std_init\n");
	if ( tcgetattr(STDIN_FILENO, &term) == -1 )
	{
		printf("tcgetattr error\n");
		return;
	}

	term.c_cc[VERASE] = '\b';  /* ??'\b' ????? ASCII ?*/

	if ( tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1 )
	{
		printf("tcsetattr error\n");
	}
	while(wind_std_input(&buff,1));
	return W_ERR_OK;
}


w_int32_t wind_std_output(w_uint8_t *buf,w_int32_t len)
{
    w_int32_t i;
	//printf("wind_std_output\n");
    for(i = 0;i < len;i ++)
    {
        putchar(buf[i]);
    }
	
	//printf("wind_std_output out\n");
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
            buff[i] = getchar();
            //printf("kbhit:%d\n",buff[i]);
        }
        else
		{
			return i;
		}
            
    }
    return i;
}




