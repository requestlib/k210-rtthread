#include <rtthread.h>
#include <rthw.h>
#include <board.h>
#define THREAD_PRIORITY      10
#define THREAD_TIMESLICE     5
#define THREAD_STACK_SIZE 1024
static rt_thread_t thread1;


/* 线程 1 接收邮箱1的内容 */
static void thread1_entry(void *parameter)
{
    int start = read_cycle();
    rt_thread_delay(1000);
    int end = read_cycle();
    // list_thread();
    rt_kprintf("one second = %d cycle\n",end-start);
}


int test_mcycle(void)
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);

    thread1 = rt_thread_create("thread1",thread1_entry,RT_NULL,THREAD_STACK_SIZE,THREAD_PRIORITY,THREAD_TIMESLICE);
    // thread1.bind_cpu=0;
    rt_thread_startup(thread1);

    return 0;
}