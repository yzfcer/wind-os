#include "wind_dev.h"
#include "wind_debug.h"
#include "led.h"
static w_uint8_t ledst = 0;
w_err_t   led1_open(dev_s *dev)
{
    LED_Init(0x02);
    return ERR_OK;
}

w_err_t  led1_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return ERR_OK;
}

w_int32_t led1_read(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
	buf[0] = ledst;
    return 1;
}

w_int32_t led1_write(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
	buf[0]?LED_On(1):LED_Off(1);
	ledst = buf[0];
    return 1;
}

w_err_t   led1_close(dev_s *dev)
{
    return ERR_OK;
}

const dev_ops_s led1_ops = 
{
    led1_open,
    led1_ioctl,
    led1_read,
    led1_write,
    led1_close
};

dev_s led1_dev = 
{
    WIND_DEV_MAGIC,
    {NULL,NULL},
    "led1",
    B_FALSE,
    NULL,
    &led1_ops
};
