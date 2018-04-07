#include "led.h"
#include "wind_timer.h"
#include "wind_dev.h"
void led0_timer(void *arg)
{
    dev_s *dev;
    w_uint8_t stat[3];
    dev = wind_dev_get("gpio");
    wind_dev_read(dev,stat,3);
    stat[0] = 1- stat[0];
    wind_dev_write(dev,stat,3);
}

void led1_timer(void *arg)
{
    dev_s *dev;
    w_uint8_t stat[3];
    dev = wind_dev_get("gpio");
    wind_dev_read(dev,stat,3);
    stat[1] = 1- stat[1];
    wind_dev_write(dev,stat,3);
}

void led2_timer(void *arg)
{
    dev_s *dev;
    w_uint8_t stat[3];
    dev = wind_dev_get("gpio");
    wind_dev_read(dev,stat,3);
    stat[2] = 1- stat[2];
    wind_dev_write(dev,stat,3);
}

void led_start(void)
{
    dev_s *dev;
    dev = wind_dev_get("gpio");
    wind_dev_open(dev);
    wind_timer_create(300,led0_timer,NULL,B_TRUE);
    wind_timer_create(500,led1_timer,NULL,B_TRUE);
    wind_timer_create(600,led2_timer,NULL,B_TRUE);
}
