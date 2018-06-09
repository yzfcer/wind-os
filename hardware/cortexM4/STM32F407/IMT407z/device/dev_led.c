#include "wind_config.h"
#include "wind_debug.h"
#include "wind_dev.h"
#include "led.h"
#if WIND_DRVFRAME_SUPPORT

static w_uint8_t ledst[3] = {0,0,0};
w_err_t   led_open(dev_s *dev)
{
    w_int32_t id = dev->devid;
    WIND_ASSERT_RETURN(id >= 0,ERR_PARAM_OVERFLOW);
    WIND_ASSERT_RETURN(id < 3,ERR_PARAM_OVERFLOW);
    LED_Init(0x01 << id);
    return ERR_OK;
}

w_err_t  led_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return ERR_OK;
}

w_int32_t led_read(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    w_int32_t id = dev->devid;
    WIND_ASSERT_RETURN(id >= 0,ERR_PARAM_OVERFLOW);
    WIND_ASSERT_RETURN(id < 3,ERR_PARAM_OVERFLOW);
    buf[0] = ledst[id];
    return 1;
}

w_int32_t led_write(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    w_int32_t id = dev->devid;
    WIND_ASSERT_RETURN(id >= 0,ERR_PARAM_OVERFLOW);
    WIND_ASSERT_RETURN(id < 3,ERR_PARAM_OVERFLOW);
    buf[0]?LED_On(id):LED_Off(id);
    ledst[id] = buf[id];
    return 1;
}

w_err_t   led_close(dev_s *dev)
{
    return ERR_OK;
}

const dev_ops_s led_ops = 
{
    NULL,
    led_open,
    led_ioctl,
    led_read,
    led_write,
    led_close
};

dev_s led_dev[3] = 
{
    WIND_DEV_DEF("led0",0,&led_ops),
    WIND_DEV_DEF("led1",1,&led_ops),
    WIND_DEV_DEF("led2",2,&led_ops),
};
#endif

