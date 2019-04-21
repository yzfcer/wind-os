/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: cmd_blkdev.c
**��   ��   ��: Jason Zhou
**����޸�����: 2013.10.19
**��        ��: ���豸��������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.10.19
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2013.10.19
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_blkdev.h"
#include "wind_conv.h"
#include "wind_heap.h"
#if (CMD_BLKDEV_SUPPORT)

COMMAND_DISC(blkdev)
{
    wind_printf("conreol one block device.\r\n");
}

COMMAND_USAGE(blkdev)
{
    wind_printf("blkdev list:--to show all block devices.\r\n");
    wind_printf("blkdev read  <dev> <addr> <blknum>:--to read date from the block device.\r\n");
    wind_printf("blkdev write <dev> <addr> <string>:--to write into the block device.\r\n");
    wind_printf("blkdev erase <dev> <addr> <blknum>:--to erase the block device sectors.\r\n");
}


static w_uint8_t buffer[512];
COMMAND_MAIN(blkdev,argc,argv)
{
    w_bool_t res;
    w_blkdev_s *dev;
    w_uint8_t *buff;
    w_addr_t addr;
    w_err_t err = W_ERR_FAIL;
    
    WIND_ASSERT_RETURN(argc>= 2,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"list"))
    {
        wind_blkdev_print();
        return W_ERR_OK;
    }
    
    WIND_ASSERT_RETURN(argc == 5,W_ERR_INVALID);
    dev = wind_blkdev_get(argv[2]);
    wind_blkdev_open(dev);
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_INVALID);
    res = wind_str_to_uint(argv[3],(w_uint32_t*)&addr);
    WIND_ASSERT_RETURN(res == W_TRUE,W_ERR_INVALID);
    buff = buffer;
    wind_memset(buff,0,dev->blksize);
    if(0 == wind_strcmp(argv[1],"read"))
    {
        err = wind_blkdev_read(dev,addr,buff,1);
        if(wind_strlen((char*)buff) >= sizeof(buffer))
        {
            wind_error("data is too long.\r\n");
            err = W_ERR_FAIL;
        }
        else
        {
            wind_printf("%s",buff);
            err = W_ERR_OK;
        }
    }
    else if(0 == wind_strcmp(argv[1],"write"))
    {
        wind_strcpy((char*)buff,argv[4]);
        err = wind_blkdev_write(dev,addr,buff,1);
        err = W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"erase"))
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

