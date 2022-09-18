#include <rtthread.h>
#include <rthw.h>
#include <board.h>
#define THREAD_PRIORITY      10
#define THREAD_TIMESLICE     5
#define THREAD_STACK_SIZE 1024
static rt_thread_t thread1;
static rt_thread_t thread2;
struct ipi_irq ipi;


static void ipi_function(void *parameter)
{
    rt_kprintf("run ipi function!\n");
}

static void thread1_entry(void *parameter)
{
    rt_kprintf("thread1 entry run!\n");
    while (1);
    
}

static void thread2_entry(void *parameter)
{
    rt_kprintf("thread2 entry start!\n");
    rt_thread_delay(1000);
    rt_kprintf("thread2 entry run again!\n");
}


int test_ipi(void)
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);

    if(core==0){
        rt_thread_delay(2000);
        ipi.ipi_type = IPI_RESCHEDULE;
        trigger_ipi_irq(&ipi, 1);
        led_r_on();
        
    }else{
        thread1 = rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, 2000*THREAD_TIMESLICE);
        if(thread1 != RT_NULL){
            thread1->bind_cpu=1;
            rt_thread_startup(thread1);
        }
        thread2 = rt_thread_create("thread2", thread2_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY-5, THREAD_TIMESLICE);
        if(thread2 != RT_NULL){
            thread2->bind_cpu=1;
            rt_thread_startup(thread2);
        }
    }

    return 0;
}