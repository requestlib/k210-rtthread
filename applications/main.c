#include<rtthread.h>
#include<board.h>
#include<sysctl.h>
#include<clint.h>
#include<entry.h>
#include<encoding.h>
#include<rthw.h>
#include<test_tool.h>
static rt_timer_t timer1;
static rt_timer_t timer2;
static rt_ubase_t cnt;

/* 定时器1 超时函数*/
static void timeout1(void *parameter)
{
    rt_kprintf("periodic timer is timeout %d\n", cnt);
    /*运行第十次 停止周期定时器*/
    cnt ++;
    cnt = cnt % 3;
    switch (cnt)
    {
    case 0:
    	gd_led_off(LED3);
    	gd_led_on(LED1);
        break;
    case 1:
    	gd_led_off(LED1);
    	gd_led_on(LED2);
        break;
    case 2:
    	gd_led_off(LED2);
    	gd_led_on(LED3);
        break;
    
    default:
        break;
    }
    
}

/* 定时器2 超时函数*/
static void timeout2(void *parameter)
{
    rt_kprintf("one shot timer is timeout \n");
}


int timer_example()
{
	rt_kprintf("hello main\n");
	gd_led_init(LED1);
	gd_led_init(LED2);
	gd_led_init(LED3);
	/* GPIO Config */

    /* 创建定时器1 周期性定时器*/
    timer1 = rt_timer_create("timer1", timeout1, RT_NULL, 100, RT_TIMER_FLAG_PERIODIC);
    /* 启动定时器1 */
    if(timer1 != RT_NULL)rt_timer_start(timer1);

    /* 创建定时器2 单次定时器*/
    timer2 = rt_timer_create("timer2", timeout2, RT_NULL, 30, RT_TIMER_FLAG_ONE_SHOT);
    /* 启动定时器2 */
    if(timer2 != RT_NULL)rt_timer_start(timer2);

    return 0;
}
