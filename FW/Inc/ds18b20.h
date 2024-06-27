/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 * 2019-07-15     WillianChan     the first version.
 *
 */

#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "main.h"
#include "util.h"
#include "parser.h"

#define CONNECT_SUCCESS  0
#define CONNECT_FAILED   1

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define DS1820_OPTION_0					0x00//9bit
#define DS1820_OPTION_1					0x01//10bit
#define DS1820_OPTION_2					0x02//11bit
#define DS1820_OPTION_3					0x03//12bit

#define DS1820_SWT_0					100//93.75ms
#define DS1820_SWT_1					200//187.5ms
#define DS1820_SWT_2					400//375ms
#define DS1820_SWT_3					800//750ms


//struct ds18b20_device
//{
//    rt_base_t pin;
//    rt_mutex_t lock;
//};
//typedef struct ds18b20_device *ds18b20_device_t;

uint8_t ds18b20_init(void);
int32_t ds18b20_get_temperature(void);
void ds18b20_start(void);
void ds18b20_set(void);
//int rt_hw_ds18b20_init(const char *name, struct rt_sensor_config *cfg);

#endif /* __DS18B20_H_ */


