#include<test_tool.h>
#include<rtthread.h>
#include<rtdef.h>

// 四则运算
double primary_cal_test(int cal_num){
    double result=0.0;
    for(int i=1;i<=cal_num;i++){
        result = 123456789+result;
    }
    for(int i=1;i<=cal_num;i++){
        result = result-98765432;
    }
    for(int i=1;i<=cal_num;i++){
        result = 123456789*result;
    }
    for(int i=1;i<=cal_num;i++){
        result = result/3;
    }
    return result;

}


//展示所有线程状态
long list_thread(void)
{
    rt_list_t *next_node = (rt_list_t *)RT_NULL;
    const char *item_title = "thread";
    struct rt_object_information *info;
    rt_list_t *list_head;

    info = rt_object_get_information((enum rt_object_class_type)RT_Object_Class_Thread);
    list_head = &info->object_list;
    next_node = list_head->next;

    int level = rt_spin_lock(&_cpus_lock);
#ifdef RT_USING_SMP
    rt_kprintf("%-*.s cpu bind pri  status      sp     stack size max used left tick  error\n", RT_NAME_MAX, item_title);
    rt_kprintf(" ---  ---- ---  ------- ---------- ----------  ------  ---------- ---\n");
#else
    rt_kprintf("%-*.s pri  status      sp     stack size max used left tick  error\n", RT_NAME_MAX, item_title);
    rt_kprintf(" ---  ------- ---------- ----------  ------  ---------- ---\n");
#endif /*RT_USING_SMP*/

    while (next_node != list_head)
    {
        struct rt_object *obj;
        struct rt_thread thread_info, *thread;

        obj = rt_list_entry(next_node, struct rt_object, list);
        if ((obj->type & ~RT_Object_Class_Static) != RT_Object_Class_Thread)
        {
            continue;
        }
        /* copy info */
        rt_memcpy(&thread_info, obj, sizeof thread_info);

        thread = (struct rt_thread *)obj;
        rt_uint8_t stat;
        rt_uint8_t *ptr;

#ifdef RT_USING_SMP
        if (thread->oncpu != RT_CPU_DETACHED)
            rt_kprintf("%-*.*s %3d %3d %4d ", RT_NAME_MAX, RT_NAME_MAX, thread->name, thread->oncpu, thread->bind_cpu, thread->current_priority);
        else
            rt_kprintf("%-*.*s N/A %3d %4d ", RT_NAME_MAX, RT_NAME_MAX, thread->name, thread->bind_cpu, thread->current_priority);

#else
        rt_kprintf("%-*.*s %3d ", RT_NAME_MAX, RT_NAME_MAX, thread->name, thread->current_priority);
#endif /*RT_USING_SMP*/
        stat = (thread->stat & RT_THREAD_STAT_MASK);
        if (stat == RT_THREAD_READY)        rt_kprintf(" ready  ");
        else if (stat == RT_THREAD_SUSPEND) rt_kprintf(" suspend");
        else if (stat == RT_THREAD_INIT)    rt_kprintf(" init   ");
        else if (stat == RT_THREAD_CLOSE)   rt_kprintf(" close  ");
        else if (stat == RT_THREAD_RUNNING) rt_kprintf(" running");

        ptr = (rt_uint8_t *)thread->stack_addr;
        while (*ptr == '#')ptr ++;

        rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
                    thread->stack_size + ((rt_ubase_t)thread->stack_addr - (rt_ubase_t)thread->sp),
                    thread->stack_size,
                    (thread->stack_size - ((rt_ubase_t) ptr - (rt_ubase_t) thread->stack_addr)) * 100
                    / thread->stack_size,
                    thread->remaining_tick,
                    thread->error);

        next_node = next_node->next;
    }
    rt_spin_unlock(&_cpus_lock, level);
    return 0;
}