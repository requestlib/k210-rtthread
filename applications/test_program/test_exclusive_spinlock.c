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
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid_monitor = RT_NULL;
rt_uint32_t target_number = 0xfffff;
rt_uint32_t op_number = 0;

/* 线程1 入口函数*/
static void thread1_entry(void *parameter)
{
    while(1){
       int level = rt_hw_local_irq_disable();
       rt_hw_spin_lock(&_cpus_lock);    //获取自旋锁
       rt_hw_spin_unlock(&_cpus_lock); // 释放自旋锁
       rt_hw_local_irq_enable(level);
    }
}

static void thread2_entry(void *parameter)
{
    int start = rt_cpu_self()->tick;
    while(op_number<target_number){
       op_number++;
       op_number+=4;
       op_number-=4;
       op_number*=3;
       op_number/=3;
       op_number+=99;
       op_number-=99;
    }
    int end = rt_cpu_self()->tick;
    int core = rt_hw_cpu_id();
    rt_kprintf("done,total spent time:%d.\n",end-start);
}


static void moniter_thread_entry(void *parameter){
    int count=0;
    while(1){
        rt_thread_delay(1000);
        for(int i=0;i<RT_CPUS_NR;i++){
            rt_kprintf("cpu[%d] usage:",i);
            rt_kprintf(" %d%%\n", (int)get_cpu_usage(i));
            rt_kprintf("total_tick:%d, idle_tick:%d\n",rt_cpu_self()->recent_total_ticks,rt_cpu_self()->idle_ticks);
        }
        list_thread();
    }
}

int test()
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);
    if(core==0){
        tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid1 != RT_NULL){
            tid1->bind_cpu=0;
            rt_thread_startup(tid1);
        }
        // tid_monitor = rt_thread_create("monitor", moniter_thread_entry, RT_NULL, THREAD_STACK_SIZE*2, 30, THREAD_TIMESLICE);
        // if(tid_monitor != RT_NULL)
        //     tid_monitor->bind_cpu = 0;
        //     rt_thread_startup(tid_monitor);
    }else{
        tid2 = rt_thread_create("thread2", thread2_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid2 != RT_NULL){
            tid2->bind_cpu=1;
            rt_thread_startup(tid2);
        }
    }
    return 0;
}
