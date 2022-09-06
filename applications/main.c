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
    int factorial_number = 20;
    rt_uint64_t src_array[factorial_number];
	primary_cal_test(src_array, factorial_number);
	for(int i=0;i<factorial_number;i++){
		rt_kprintf("%d res: %lld\n",i,*(src_array+i));
	} 
}

int main()
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);
    /*创建线程1 名称是thread1 入口是 thread1_entry*/
    tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if(tid1 != RT_NULL)
        rt_thread_startup(tid1);

    return 0;
}
