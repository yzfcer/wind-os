#include "wind_config.h"
#include "wind_debug.h"
#include "wind_dev.h"
#include "led.h"
#if WIND_DRVFRAME_SUPPORT

static w_uint8_t ledst = 0;
w_err_t   led0_open(dev_s *dev)
{
    LED_Init(0x01);
    return ERR_OK;
}

w_err_t  led0_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return ERR_OK;
}

w_int32_t led0_read(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
	buf[0] = ledst;
    return 1;
}

w_int32_t led0_write(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    buf[0]?LED_On(0):LED_Off(0);
    ledst = buf[0];
    return 1;
}

w_err_t   led0_close(dev_s *dev)
{
    return ERR_OK;
}

const dev_ops_s led0_ops = 
{
    led0_open,
    led0_ioctl,
    led0_read,
    led0_write,
    led0_close
};

dev_s led0_dev = 
{
    WIND_DEV_MAGIC,
    {NULL,NULL},
    "led0",
    B_FALSE,
    NULL,
    &led0_ops
};
#endif

