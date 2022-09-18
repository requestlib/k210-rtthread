#include <rtthread.h>
#include <rthw.h>
#include <board.h>
#define THREAD_PRIORITY      10
#define THREAD_TIMESLICE     5
#define OP_NR_MAIL 15
#define OP_NR_SEM 100
#define OP_NR_MUT 100
#define OP_NR_MQ 100
#define OP_NR_EVE 100

/* 邮箱控制块 */
static struct rt_mailbox mb1;
/* 用于放邮件的内存池 */
static char mb_pool1[128];
static char mb_str1[] = "first mail.";
static char mb_str3[] = "over";

/* 指向信号量的指针 */
static rt_sem_t dynamic_sem = RT_NULL;
static rt_mutex_t mutex = RT_NULL;

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
static struct rt_thread thread1;
static char thread2_stack[1024];
static struct rt_thread thread2;
static char thread3_stack[1024];
static struct rt_thread thread3;
static char thread4_stack[1024];
static struct rt_thread thread4;

/* 线程 1 互斥量获取 */
static void thread1_entry(void *parameter)
{
    static rt_err_t result;
    static rt_uint8_t number = 0;
    int start = read_cycle();
    while(number<OP_NR_MUT)
    {
        /* 永久方式等待信号量，获取到信号量，则执行 number 自加的操作 */
        result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
        number++;
    }
    int end = read_cycle();
    rt_kprintf("mutex:%d\n",end-start);
    while(1);
}

/* 线程 2 获取信号量 */
static void thread2_entry(void *parameter)
{
    static rt_err_t result;
    static rt_uint8_t number = 0;
    int start = read_cycle();
    while(number<OP_NR_SEM)
    {
        /* 永久方式等待信号量，获取到信号量，则执行 number 自加的操作 */
        result = rt_sem_take(dynamic_sem, RT_WAITING_FOREVER);
        number++;
    }
    int end = read_cycle();
    rt_kprintf("sem:%d\n",end-start);
    while(1);
}



/* 线程 3 发送 互斥量*/
static void thread3_entry(void *parameter)
{
    for(int i=0;i<OP_NR_MUT;i++) rt_mutex_release(mutex);
    while(1);
}

static void thread4_entry(void *parameter)
{
    for(int i=0;i<OP_NR_SEM;i++) rt_sem_release(dynamic_sem);
    while(1);
}

int main(void)
{
    int core = current_coreid();
    rt_kprintf("Core %d Hello world \n", core);
    if(core==1){
        mutex = rt_mutex_create("mut", RT_IPC_FLAG_PRIO);   //初始化互斥量 

        //thread1
        rt_thread_init(&thread1,"thread1",thread1_entry,RT_NULL,&thread1_stack[0],sizeof(thread1_stack),THREAD_PRIORITY, THREAD_TIMESLICE);
        thread1.bind_cpu=1;
        rt_thread_startup(&thread1);

        rt_thread_init(&thread3,"thread3",thread3_entry,RT_NULL,&thread3_stack[0],sizeof(thread3_stack),THREAD_PRIORITY, THREAD_TIMESLICE);
        thread3.bind_cpu=1;
        rt_thread_startup(&thread3);

    }else{
        /* 创建一个动态信号量，初始值是 0 */
        dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_PRIO);

        rt_thread_init(&thread2,"thread2",thread2_entry,RT_NULL,&thread2_stack[0],sizeof(thread2_stack),THREAD_PRIORITY, THREAD_TIMESLICE);
        thread2.bind_cpu=0;
        rt_thread_startup(&thread2);

        rt_thread_init(&thread4,"thread4",thread4_entry,RT_NULL,&thread4_stack[0],sizeof(thread4_stack),THREAD_PRIORITY, THREAD_TIMESLICE);
        thread4.bind_cpu=0;
        rt_thread_startup(&thread4);


    }
    return 0;
}