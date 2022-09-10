#include<rtthread.h>
#include<board.h>
#include<sysctl.h>
#include<clint.h>
#include<entry.h>
#include<encoding.h>
#include<rthw.h>
#include<test_tool.h>

#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 1024
#define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid_monitor = RT_NULL;

/* 线程1 入口函数*/
static void thread1_entry(void *parameter)
{
    int start = rt_cpu_self()->tick;
    double result = primary_cal_test(cal_num);
    int end = rt_cpu_self()->tick;
    rt_kprintf("spent ticks:%d\n",end-start);

}

static void moniter_thread_entry(void *parameter){
    // while(1){
        // rt_hw_us_delay(5000000000);
        rt_kprintf("cpu[%d] usage:",rt_hw_cpu_id());
        rt_kprintf(" %d%%\n", (int)get_cpu_usage());
    // }
}

int cpu_rate_test()
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);

    tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if(tid1 != RT_NULL)
        tid1->bind_cpu = 0;
        rt_thread_startup(tid1);

    tid_monitor = rt_thread_create("monitor", moniter_thread_entry, RT_NULL, THREAD_STACK_SIZE*2, 30, THREAD_TIMESLICE);
    if(tid_monitor != RT_NULL)
        rt_thread_startup(tid_monitor);

    return 0;
}
