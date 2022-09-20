#include<rtthread.h>
#include<board.h>
#include<sysctl.h>
#include<clint.h>
#include<entry.h>
#include<encoding.h>
#include<rthw.h>
#include<test_tool.h>

#define THREAD_PRIORITY 5
#define THREAD_STACK_SIZE 1024
#define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;
static rt_thread_t tid_monitor = RT_NULL;
static int para1 = 0;
static int para2 = 2;

/* 线程1 入口函数*/
static void thread_entry(void *parameter)
{
    int cal_num = 80000;
    while(1){
        int wait_start = rt_cpu_self()->tick;
        int delay_time = 1000*(*(int *)parameter);
        if(delay_time){
            srand(wait_start);
            rt_thread_delay(rand()%delay_time);
        }
        primary_cal_test(cal_num);
        int end = rt_cpu_self()->tick;
        switch((end-wait_start)%3){
            case 0:
                rgb_all_off();
                led_b_on();
                break;
            case 1:
                rgb_all_off();
                led_r_on();
                break;
            case 2:
                rgb_all_off();
                led_g_on();
                break;
            default:
                break;
        }
    }


}

static void moniter_thread_entry(void *parameter){
    int count=0;
    while(1){
        rt_thread_delay(1000);
        for(int i=0;i<RT_CPUS_NR;i++){
            rt_kprintf("cpu[%d] usage:",i);
            rt_kprintf(" %d%%\n", (int)get_cpu_usage(i));
            rt_kprintf("total_tick:%d, idle_tick:%d\n",rt_cpu_index(i)->recent_total_ticks,rt_cpu_index(i)->idle_ticks);
        }
        list_thread();
    }
}

void test(){
    // 测试1 存在空闲核
    // 核0持续处理任务 核1空闲
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);
    if(core==0){
        tid1 = rt_thread_create("thread1", thread_entry, &para1, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid1 != RT_NULL){
            rt_thread_startup(tid1);
        }
    }else{
        tid_monitor = rt_thread_create("monitor", moniter_thread_entry, RT_NULL, THREAD_STACK_SIZE*2, 5, THREAD_TIMESLICE);
        if(tid_monitor != RT_NULL)
            tid_monitor->bind_cpu = 0;
            rt_thread_startup(tid_monitor);
        rt_thread_delay(3000);
        tid1 = rt_thread_create("thread2", thread_entry, &para2, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid1 != RT_NULL){
            rt_thread_startup(tid1);
        }
        rt_thread_delay(3000);
        tid1 = rt_thread_create("thread3", thread_entry, &para2, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
        if(tid1 != RT_NULL){
            rt_thread_startup(tid1);
        }
    }
}

int test_cpu_rate()
{
   test1(); //测试1 存在空闲核
   return 0;
}
