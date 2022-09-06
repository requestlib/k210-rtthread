#include<rtthread.h>
#include<board.h>
#include<sysctl.h>
#include <clint.h>
#include<entry.h>
#include<encoding.h>
#include<rthw.h>
#include<test_app.h>

#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 1024
#define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;


/* 线程1 入口函数*/
static void thread1_entry(void *parameter)
{
    rt_tick_t start_time = rt_cpu_self()->tick;

    int cal_num = 20000;
	double result = primary_cal_test(cal_num);
	// for(int i=0;i<factorial_number;i++){
	// 	rt_kprintf("%d res: %lld\n",i,*(src_array+i));
	// }

    rt_tick_t end_time = rt_cpu_self()->tick;
    // unsigned long start_cycle = read_csr(mcycle);
    rt_kprintf("total time spent: %d\n", end_time-start_time); 
}

int main()
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);

    tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if(tid1 != RT_NULL)
        rt_thread_startup(tid1);

    if(core == 1)list_thread();

    return 0;
}
