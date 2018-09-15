/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_blkdev.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 块设备操作命令
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.10.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.10.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_blkdev.h"
#include "wind_conv.h"
#include "wind_var.h"
#include "wind_heap.h"
#if (WIND_CONSOLE_SUPPORT && CMD_BLKDEV_SUPPORT)

COMMAND_DISC(blkdev)
{
    console_printf("conreol one block device.\r\n");
}

COMMAND_USAGE(blkdev)
{
    console_printf("blkdev list:to show all block devices.\r\n");
    console_printf("blkdev <dev> read <addr> <blknum>:to read date from the block device.\r\n");
    console_printf("blkdev <dev> write <addr> <string>:to write into the block device.\r\n");
    console_printf("blkdev <dev> erase <addr> <blknum>:to erase the block device sectors.\r\n");
}


static w_uint8_t buffer[512];
COMMAND_MAIN(blkdev,argc,argv)
{
    w_bool_t res;
    w_blkdev_s *dev;
    w_uint8_t *buff;
    w_addr_t addr;
    w_err_t err = W_ERR_FAIL;
    
    if(0 == wind_strcmp(argv[1],"list"))
    {
        wind_blkdev_print(&g_core.blkdevlist);
        return W_ERR_OK;
    }
    WIND_ASSERT_RETURN(argc == 5,W_ERR_INVALID);
    dev = wind_blkdev_get(argv[1]);
    wind_blkdev_open(dev);
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_INVALID);
    res = wind_atoui(argv[3],(w_uint32_t*)&addr);
    WIND_ASSERT_RETURN(res == W_TRUE,W_ERR_INVALID);
    buff = buffer;
    wind_memset(buff,0,dev->blksize);
    if(0 == wind_strcmp(argv[2],"read"))
    {
        err = wind_blkdev_read(dev,addr,buff,1);
        if(wind_strlen((char*)buff) >= sizeof(buffer))
        {
            wind_error("data is too long.\r\n");
            err = W_ERR_FAIL;
        }
        else
        {
            console_printf("%s",buff);
            err = W_ERR_OK;
        }
    }
    else if(0 == wind_strcmp(argv[2],"write"))
    {
        wind_strcpy((char*)buff,argv[4]);
        err = wind_blkdev_write(dev,addr,buff,1);
        err = W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[2],"write"))
    {
        err = wind_blkdev_erase(dev,addr,1);
        err = W_ERR_OK;
    }
    else 
        err = W_ERR_FAIL;
    wind_blkdev_close(dev);
    return err;
}

COMMAND_DEF(blkdev);

#endif

