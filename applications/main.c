#include<rtthread.h>
#include<board.h>
#include<sysctl.h>
#include <clint.h>
#include<entry.h>
#include<encoding.h>
#include<rthw.h>


#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 1024
#define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;


/* 线程1 入口函数*/
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;
    while(1)
    {
        /*线程1 低优先级 一直打印计数值*/
        rt_kprintf("thread1 count: %d\n", count++);
        rt_thread_delay(1);
    }
}

/* 线程2 入口函数*/
static void thread2_entry(void* param)
{
    rt_uint32_t count = 0;

    /* 线程 2 高优先级 , 可抢占线程1*/
    for(count = 0; count < 20; count++)
    {
        /*线程2 打印计数值*/
        rt_kprintf("thread2 count: %d\n", count);
        rt_thread_delay(1);
    }
    rt_kprintf("thread2 exit\n");
}

int main()
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world\n", core);
    /*创建线程1 名称是thread1 入口是 thread1_entry*/
    tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if(tid1 != RT_NULL)
        rt_thread_startup(tid1);

    // rt_thread_delay(2);

    tid2 = rt_thread_create("thread2", thread2_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if(tid2 != RT_NULL)
        rt_thread_startup(tid2);

    return 0;
}
