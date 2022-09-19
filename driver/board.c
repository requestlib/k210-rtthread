/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 *2021-06-10      xiaoyu       implement rt_hw_us_delay()
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "tick.h"

#include "drv_uart.h"

#include "encoding.h"
#include "fpioa.h"
#include "dmac.h"
#include "dmalock.h"

void init_bss(void)
{
    unsigned int *dst;

    dst = &__bss_start;
    while (dst < &__bss_end)
    {
        *dst++ = 0;
    }
}

void primary_cpu_entry(void)
{
    extern void entry(void);

    /* disable global interrupt */
    init_bss();
    rt_hw_interrupt_disable();
    entry();
}

#include <clint.h>
#include <sysctl.h>

int freq(void)
{
    rt_uint64_t value = 0;

    value = sysctl_clock_get_freq(SYSCTL_CLOCK_PLL0);
    rt_kprintf("PLL0: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_PLL1);
    rt_kprintf("PLL1: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_PLL2);
    rt_kprintf("PLL2: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
    rt_kprintf("CPU : %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_APB0);
    rt_kprintf("APB0: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_APB1);
    rt_kprintf("APB1: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_APB2);
    rt_kprintf("APB2: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_HCLK);
    rt_kprintf("HCLK: %d\n", value);

    value = clint_get_time();
    rt_kprintf("mtime: %d\n", value);

    return 0;
}
MSH_CMD_EXPORT(freq, show freq info);

#ifdef RT_USING_SMP
extern int rt_hw_clint_ipi_enable(void);
#endif

void rt_hw_board_init(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 400000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    sysctl_clock_set_threshold(SYSCTL_THRESHOLD_APB1, 2);
    /* Init FPIOA */
    fpioa_init();
    /* Dmac init */
    dmac_init();
    dmalock_init();

    /* initalize interrupt */
    rt_hw_interrupt_init();
    /* initialize hardware interrupt */
    rt_hw_uart_init();

    rt_hw_tick_init();

#ifdef RT_USING_SMP
    rt_hw_clint_ipi_enable();
#endif

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_HEAP
    rt_kprintf("heap: [0x%08x - 0x%08x]\n", (rt_ubase_t) RT_HW_HEAP_BEGIN, (rt_ubase_t) RT_HW_HEAP_END);
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    init_rgb();
    rt_spin_lock_init(&_uart_lock);
}
void rt_hw_cpu_reset(void)
{
    sysctl->soft_reset.soft_reset = 1;
    while(1);
}

MSH_CMD_EXPORT_ALIAS(rt_hw_cpu_reset, reboot, reset machine);

/**
 * This function will delay for some us.
 *
 * @param us the delay time of us
 */
void rt_hw_us_delay(rt_uint32_t usec)
{
    rt_uint32_t cycle = read_cycle();
    rt_uint32_t nop_all = usec * sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000000UL;
    while (1)
    {
       if(read_cycle() - cycle >= nop_all)
            break;
    }
}

// LED function

/**
* Function       hardware_init
* @author        Gengyue
* @date          2020.05.27
* @brief         硬件初始化，绑定GPIO口
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void led_hardware_init(void)
{
    /* fpioa映射 */
    fpioa_set_function(PIN_RGB_R, FUNC_RGB_R);
    fpioa_set_function(PIN_RGB_G, FUNC_RGB_G);
    fpioa_set_function(PIN_RGB_B, FUNC_RGB_B);

    fpioa_set_function(PIN_KEY, FUNC_KEY);
}

/**
* Function       rgb_all_off
* @author        Gengyue
* @date          2020.05.27
* @brief         RGB灯灭
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void rgb_all_off(void)
{
    gpiohs_set_pin(RGB_R_GPIONUM, GPIO_PV_HIGH);
    gpiohs_set_pin(RGB_G_GPIONUM, GPIO_PV_HIGH);
    gpiohs_set_pin(RGB_B_GPIONUM, GPIO_PV_HIGH);
}

/**
* Function       rgb_all_on
* @author        Gengyue
* @date          2020.05.27
* @brief         RGB灯亮白灯
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void rgb_all_on(void)
{
    gpiohs_set_pin(RGB_R_GPIONUM, GPIO_PV_LOW);
    gpiohs_set_pin(RGB_G_GPIONUM, GPIO_PV_LOW);
    gpiohs_set_pin(RGB_B_GPIONUM, GPIO_PV_LOW);
}

void led_r_on(void){
    gpiohs_set_pin(RGB_R_GPIONUM, GPIO_PV_LOW);
    gpiohs_set_pin(RGB_G_GPIONUM, GPIO_PV_HIGH);
    gpiohs_set_pin(RGB_B_GPIONUM, GPIO_PV_HIGH);
}

void led_g_on(void){
    gpiohs_set_pin(RGB_R_GPIONUM, GPIO_PV_HIGH);
    gpiohs_set_pin(RGB_G_GPIONUM, GPIO_PV_LOW);
    gpiohs_set_pin(RGB_B_GPIONUM, GPIO_PV_HIGH);
}

void led_b_on(void){
    gpiohs_set_pin(RGB_R_GPIONUM, GPIO_PV_HIGH);
    gpiohs_set_pin(RGB_G_GPIONUM, GPIO_PV_HIGH);
    gpiohs_set_pin(RGB_B_GPIONUM, GPIO_PV_LOW);
}

/**
* Function       init_rgb
* @author        Gengyue
* @date          2020.05.27
* @brief         初始化RGB灯
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void init_rgb(void)
{
    led_hardware_init();
    /* 设置RGB灯的GPIO模式为输出 */
    gpiohs_set_drive_mode(RGB_R_GPIONUM, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(RGB_G_GPIONUM, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(RGB_B_GPIONUM, GPIO_DM_OUTPUT);

    /* 关闭RGB灯 */
    rgb_all_off();
}
