/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-5-30      Bernard      the first version
 */

#ifndef BOARD_H__
#define BOARD_H__

#include <rtconfig.h>
#include "gpiohs.h"
#include "pin_config.h"

#if PKG_KENDRYTE_SDK_VERNUM < 0x0054
#error The version of Kendryte sdk is too old, please update to V0.5.4 or newer
#endif

extern unsigned int __bss_start;
extern unsigned int __bss_end;

#define RT_HW_HEAP_BEGIN    (void*)&__bss_end
#define RT_HW_HEAP_END      (void*)(0x80000000 + 6 * 1024 * 1024)

void rt_hw_board_init(void);

void hardware_init(void);
void rgb_all_off(void);
void rgb_all_on(void);
void init_rgb(void);

void led_g_on(void);
void led_r_on(void);
void led_b_on(void);

#endif
