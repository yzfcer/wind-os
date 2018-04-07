#include "wind_dev.h"
#include "wind_debug.h"
w_err_t   gpio_open(void)
{
    return ERR_OK;
}
w_err_t   gpio_ioctl(w_int32_t ctrlpoint,void *param)
{
    return ERR_OK;
}
w_int32_t gpio_read(w_uint8_t *buf,w_uint16_t len)
{
    return 0;
}
w_int32_t gpio_write(w_uint8_t *buf,w_uint16_t len)
{
    return 0;
}
w_err_t   gpio_close(void)
{
    return ERR_OK;
}

const dev_ops_s gpio_ops = 
{
    gpio_open,
    gpio_ioctl,
    gpio_read,
    gpio_write,
    gpio_close
};

dev_s gpio_dev = 
{
    WIND_DEV_MAGIC,
    {NULL,NULL},
    "gpio",
    B_FALSE,
    NULL,
    &gpio_ops
};



