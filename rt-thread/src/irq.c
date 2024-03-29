/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-02-24     Bernard      first version
 * 2006-05-03     Bernard      add IRQ_DEBUG
 * 2016-08-09     ArdaFu       add interrupt enter and leave hook.
 * 2018-11-22     Jesven       rt_interrupt_get_nest function add disable irq
 * 2021-08-15     Supperthomas fix the comment
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdef.h>

#ifdef RT_USING_HOOK

static void (*rt_interrupt_enter_hook)(void);
static void (*rt_interrupt_leave_hook)(void);

/**
 * @ingroup Hook
 *
 * @brief This function set a hook function when the system enter a interrupt
 *
 * @note The hook function must be simple and never be blocked or suspend.
 *
 * @param hook the function point to be called
 */
void rt_interrupt_enter_sethook(void (*hook)(void))
{
    rt_interrupt_enter_hook = hook;
}

/**
 * @ingroup Hook
 *
 * @brief This function set a hook function when the system exit a interrupt.
 *
 * @note The hook function must be simple and never be blocked or suspend.
 *
 * @param hook the function point to be called
 */
void rt_interrupt_leave_sethook(void (*hook)(void))
{
    rt_interrupt_leave_hook = hook;
}
#endif /* RT_USING_HOOK */

/**
 * @addtogroup Kernel
 */

/**@{*/

#ifdef RT_USING_SMP
#define rt_interrupt_nest rt_cpu_self()->irq_nest
#else
volatile rt_uint8_t rt_interrupt_nest = 0;
#endif /* RT_USING_SMP */


/**
 * @brief This function will be invoked by BSP, when enter interrupt service routine
 *
 * @note Please don't invoke this routine in application
 *
 * @see rt_interrupt_leave
 */
void rt_interrupt_enter(void)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    rt_interrupt_nest ++;
    RT_OBJECT_HOOK_CALL(rt_interrupt_enter_hook,());
    rt_hw_interrupt_enable(level);

    RT_DEBUG_LOG(RT_DEBUG_IRQ, ("irq has come..., irq current nest:%d\n",
                                rt_interrupt_nest));
}
RTM_EXPORT(rt_interrupt_enter);


/**
 * @brief This function will be invoked by BSP, when leave interrupt service routine
 *
 * @note Please don't invoke this routine in application
 *
 * @see rt_interrupt_enter
 */
void rt_interrupt_leave(void)
{
    rt_base_t level;

    RT_DEBUG_LOG(RT_DEBUG_IRQ, ("irq is going to leave, irq current nest:%d\n",
                                rt_interrupt_nest));

    level = rt_hw_interrupt_disable();
    RT_OBJECT_HOOK_CALL(rt_interrupt_leave_hook,());
    rt_interrupt_nest --;
    rt_hw_interrupt_enable(level);
}
RTM_EXPORT(rt_interrupt_leave);


/**
 * @brief This function will return the nest of interrupt.
 *
 * User application can invoke this function to get whether current
 * context is interrupt context.
 *
 * @return the number of nested interrupts.
 */
RT_WEAK rt_uint8_t rt_interrupt_get_nest(void)
{
    rt_uint8_t ret;
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    ret = rt_interrupt_nest;
    rt_hw_interrupt_enable(level);
    return ret;
}
RTM_EXPORT(rt_interrupt_get_nest);

RTM_EXPORT(rt_hw_interrupt_disable);
RTM_EXPORT(rt_hw_interrupt_enable);

/**@}*/

/**
 * ipi - inner process interrupt
 * 
 * mcause:
 *  IPI_RESCHEDULE ：control all cores to run rt_schedule()
    IPI_CALL_FUNC： control one core to run specific function.
*/

/**
 * @brief trigger an ipi interrupt
 * 
 * @param ipi_cause: which kind of ipi interrupt
 * @param func: run function on specific core
 * @param param: function parameters
 */
void trigger_ipi_irq(ipi_irq_t ipi_irq, rt_uint8_t dest_core_id){

    switch (ipi_irq->ipi_type)
    {
    case IPI_CALL_FUNC:
        if(dest_core_id>=RT_CPUS_NR){ //all cpu ipi
            for(int i=0;i<RT_CPUS_NR;i++){
                struct rt_cpu *dest_cpu = rt_cpu_index(dest_core_id);
                dest_cpu->ipi_type=IPI_CALL_FUNC;
                dest_cpu->ipi_func=ipi_irq->func;
                dest_cpu->param=ipi_irq->parameter;
                clint_ipi_send(i);
            }
        }
        else{
            for(int i=0;i<RT_CPUS_NR;i++){
                if(i==dest_core_id){
                    struct rt_cpu *dest_cpu = rt_cpu_index(dest_core_id);
                    dest_cpu->ipi_type=IPI_CALL_FUNC;
                    dest_cpu->ipi_func=ipi_irq->func;
                    dest_cpu->param=ipi_irq->parameter;
                    clint_ipi_send(dest_core_id);
                }
                else rt_cpu_index(i)->ipi_type=RT_NULL;
            }
        }
        break;
    case IPI_RESCHEDULE:
        if(dest_core_id>=RT_CPUS_NR){ //all cpu ipi
            for(int i=0;i<RT_CPUS_NR;i++){
                struct rt_cpu *dest_cpu = rt_cpu_index(dest_core_id);
                dest_cpu->ipi_type=IPI_RESCHEDULE;
                clint_ipi_send(i);
            }
        }
        else{
            for(int i=0;i<RT_CPUS_NR;i++){
                if(i==dest_core_id){
                    struct rt_cpu *dest_cpu = rt_cpu_index(dest_core_id);
                    dest_cpu->ipi_type=IPI_RESCHEDULE;
                    clint_ipi_send(dest_core_id);
                }
                else rt_cpu_index(i)->ipi_type=RT_NULL;
            }
        }
        break;
    default:
        break;
    }

}
