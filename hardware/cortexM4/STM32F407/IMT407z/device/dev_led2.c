#include "wind_dev.h"
#include "wind_debug.h"
#include "led.h"
static w_uint8_t ledst = 0;
w_err_t   led2_open(dev_s *dev)
{
    LED_Init(0x04);
    return ERR_OK;
}

w_err_t  led2_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return ERR_OK;
}

w_int32_t led2_read(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
	buf[0] = ledst;
    return 1;
}

w_int32_t led2_write(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    buf[0]?LED_On(2):LED_Off(2);
    ledst = buf[0];
    return 1;
}

w_err_t   led2_close(dev_s *dev)
{
    return ERR_OK;
}

const dev_ops_s led2_ops = 
{
    led2_open,
    led2_ioctl,
    led2_read,
    led2_write,
    led2_close
};

dev_s led2_dev = 
{
    WIND_DEV_MAGIC,
    {NULL,NULL},
    "led2",
    B_FALSE,
    NULL,
    &led2_ops
};



