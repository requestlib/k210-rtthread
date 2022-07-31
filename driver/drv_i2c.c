/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2021-12-24     flybreak    first version
 */

#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "i2c.h"
#include "gpiohs.h"
#include "utils.h"
#include "sleep.h"

#include <fpioa.h>
#include <sysctl.h>

#ifdef RT_USING_I2C

#if !defined(BSP_USING_I2C1) && !defined(BSP_USING_I2C2) && !defined(BSP_USING_I2C3) && !defined(BSP_USING_I2C4)
    #error "Please define at least one BSP_USING_I2Cx"
    /* this driver can be disabled at menuconfig  RT-Thread Components  Device Drivers */
#endif

struct _i2c_bus
{
    struct rt_i2c_bus_device parent;
    i2c_device_number_t based;
    char *device_name;
};

static struct _i2c_bus _i2c_device[] =
{
    #ifdef BSP_USING_I2C1
    {
        .based = I2C_DEVICE_0,
        .device_name = "i2c1"
    },
    #endif
};

static rt_size_t _master_xfer(struct rt_i2c_bus_device *bus, struct rt_i2c_msg msgs[], rt_uint32_t num)
{
    rt_uint32_t send_num;
    int abort_status = 1;
    struct _i2c_bus *i2c_bus;
    struct rt_i2c_msg *msg;

    RT_ASSERT(bus != RT_NULL);
    i2c_bus = rt_list_entry(bus, struct _i2c_bus, parent);
    RT_ASSERT(i2c_bus != RT_NULL);
    for (send_num = 0; send_num < num; send_num++)
    {
        msg = &msgs[send_num];

        if (msg->flags & RT_I2C_RD)
        {
            i2c_init(i2c_bus->based, msg->addr, 7, 200000);

            abort_status = i2c_recv_data(i2c_bus->based, 0, 0, msg->buf, msg->len);

            if (abort_status != 0)
            {
                return 0;
            }
        }
        else
        {
            i2c_init(i2c_bus->based, msg->addr, 7, 200000);
            abort_status = i2c_send_data(i2c_bus->based, msg->buf, msg->len);

            if (abort_status != 0)
            {
                return 0;
            }
        }
    }

    return send_num;
}

static rt_size_t _slave_xfer(struct rt_i2c_bus_device *bus, struct rt_i2c_msg msgs[], rt_uint32_t num)
{
    return 0;
}
static rt_err_t _i2c_bus_control(struct rt_i2c_bus_device *bus, rt_uint32_t cmd, rt_uint32_t arg)
{
    return 0;
}

static const struct rt_i2c_bus_device_ops _ops =
{
    .master_xfer = _master_xfer,
    .slave_xfer = _slave_xfer,
    .i2c_bus_control = _i2c_bus_control,
};

static void hw_i2c_init(void)
{
#ifdef BSP_USING_I2C1
    fpioa_set_function(37, FUNC_I2C0_SDA);
    fpioa_set_function(36, FUNC_I2C0_SCLK);
    sysctl_enable_irq();
    i2c_init(I2C_DEVICE_0, 0x68, 7, 200000);
#endif
}

int rt_hw_i2c_init(void)
{
    rt_uint8_t i = 0;
    rt_uint8_t i2c_bus_num = 0;

    hw_i2c_init();

    i2c_bus_num = sizeof(_i2c_device) / sizeof(_i2c_device[0]);

    for (i = 0; i < i2c_bus_num; i++)
    {
        _i2c_device[i].parent.ops = &_ops;
        rt_i2c_bus_device_register(&_i2c_device[i].parent, _i2c_device[i].device_name);
    }

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_i2c_init);

#endif /* BSP_USING_I2C */
