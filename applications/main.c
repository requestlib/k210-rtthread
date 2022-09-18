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
static rt_thread_t tid3 = RT_NULL;
static rt_tick_t start = 0;
static rt_tick_t end = 0;
static rt_spinlock test_spinlock;

/* 线程1 入口函数*/
static void thread1_entry(void *parameter)
{
    //长时间保持自旋锁
    int level = rt_hw_local_irq_disable();
    rt_spin_lock(&test_spinlock);
    primary_cal_test(10000*500);  //9s
    rt_spin_unlock(&test_spinlock);
    rt_hw_local_irq_enable(level);
}

static void thread2_entry(void *parameter)
{
    // int level = rt_spin_lock(&_cpus_lock);
    // rt_spin_unlock(&_cpus_lock, level);
    // rt_kprintf("hh\n");
    led_r_on();
    list_timer();
    while(1);
}

static void thread3_entry(void *parameter)
{
    led_g_on();
    primary_cal_test(10000*500);
    end = rt_cpu_self()->tick;
    rt_kprintf("thread3 total spent time:%d\n",end-start);
}

int main()
{
    int core = current_coreid();
    rgb_all_on();
    rt_kprintf("Core %d Hello world \n", core);
    if(core==0){
        tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid1 != RT_NULL){
            tid1->bind_cpu=0;
            rt_thread_startup(tid1);
        }

    }else{
        start = rt_cpu_self()->tick;
        tid2 = rt_thread_create("thread2", thread2_entry, RT_NULL, THREAD_STACK_SIZE*2, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid2 != RT_NULL){
            tid2->bind_cpu=1;
            rt_thread_startup(tid2);
        }
        rt_thread_delay(100); //1秒后 高优先级线程到来
        led_b_on();
        tid3 = rt_thread_create("thread3", thread3_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY-5, THREAD_TIMESLICE);
        if(tid3 != RT_NULL){
            tid3->bind_cpu=1;
            rt_thread_startup(tid3);
        }
    }
    return 0;
}