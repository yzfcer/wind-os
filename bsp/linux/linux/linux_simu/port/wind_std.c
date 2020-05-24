/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_os_hwif.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind os stdard input/output function API
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
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
	//newt.c_lflag &= ~(ICANON);
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
		return W_ERR_FAIL;
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




