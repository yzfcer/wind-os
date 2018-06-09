#include "led.h"
#include "wind_timer.h"
#include "wind_dev.h"
#if (WIND_TIMER_SUPPORT && WIND_DRVFRAME_SUPPORT)
timer_s *ledtmr = NULL;
void led0_timer(void *arg)
{
    dev_s *dev;
    w_uint8_t stat;
    dev = wind_dev_get("led0");
    wind_dev_read(dev,&stat,1);
    
    if(stat)
    {
        stat = 0;
        wind_dev_write(dev,&stat,1);
        wind_timer_set_period(ledtmr,1000);
    }
    else
    {
        stat = 1;
        wind_dev_write(dev,&stat,1);
        wind_timer_set_period(ledtmr,100);
    }
}


void led_start(void)
{
    dev_s *dev;
    dev = wind_dev_get("led0");
    wind_dev_open(dev);
    ledtmr = wind_timer_create("led0tmr",100,led0_timer,NULL,B_TRUE);
}
#else
void led_start(void) {}
#endif
