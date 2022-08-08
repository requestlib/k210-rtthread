#include<rtthread.h>
#include<board.h>
#include<sysctl.h>
#include <clint.h>
#include<entry.h>


#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 1024
#define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;

/* 线程1 入口函数*/
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;
    while(1)
    {
        /*线程1 低优先级 一直打印计数值*/
        rt_kprintf("thread1 count: %d\n", count++);
        rt_thread_mdelay(500);
    }
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

/* 线程2 入口函数*/
static void thread2_entry(void* param)
{
    rt_uint32_t count = 0;

    /* 线程 2 高优先级 , 可抢占线程1*/
    for(count = 0; count < 10; count++)
    {
        /*线程2 打印计数值*/
        rt_kprintf("thread2 count: %d\n", count);
    }
    rt_kprintf("thread2 exit\n");
}

int core1_function(void *ctx)
{
    uint64_t core = current_coreid();
    rt_kprintf("Core %ld Hello world\n", core);
    while(1);
}

int main()
{

    // rt_kprintf("hello main\n");
    // /*创建线程1 名称是thread1 入口是 thread1_entry*/
    // tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);

    // /*如果获得线程控制块, 启动这个线程*/
    // if(tid1 != RT_NULL)
    //     rt_thread_startup(tid1);

    // /*初始化线程2 名称是thread2 入口是 thread2_entry*/
    // rt_thread_init(&thread2, "thread2", thread2_entry, RT_NULL, &thread2_stack, sizeof(thread2_stack), THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    // rt_thread_startup(&thread2);

    // return 0;

    uint64_t core = current_coreid();
    rt_kprintf("Core %ld Hello world\n", core);
    // rt_kprintf("%d tick ++\n",rt_hw_cpu_id());
    // rt_hw_secondary_cpu_up();
    // rt_kprintf("double cores tasks done!\n");
    // while(1)
    //     continue;
    return 0;
}
