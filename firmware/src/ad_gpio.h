/**
  ******************************************************************************
  * @file    ad_gpio.h
  * @author  nod
  * @version v1.0.0
  * @brief   
  ******************************************************************************
  * @copyright
  *                  all rights reserved (www.3nod.com.cn)
  ******************************************************************************
  */
  
#ifndef __AD_GPIO_H
#define __AD_GPIO_H

//#include "platform_config.h"
#include "bt_hardware_profile.h"

typedef enum
{
    GPIO_DIGITAL_OUTPUT,
    GPIO_DIGITAL_INPUT ,
}gpio_mode_t;


typedef enum
{
    GPIO_PORT_A = PORT_CHANNEL_A,
    GPIO_PORT_B = PORT_CHANNEL_B,
    GPIO_PORT_C = PORT_CHANNEL_C,
   // GPIO_PORT_D = PORT_CHANNEL_D,
   // GPIO_PORT_E = PORT_CHANNEL_E,
   // GPIO_PORT_F = PORT_CHANNEL_F,
   // GPIO_PORT_G = PORT_CHANNEL_G,
}gpio_port_t;

typedef enum
{
    GPIO_PIN_0 = PORTS_BIT_POS_0,
    GPIO_PIN_1 = PORTS_BIT_POS_1,
    GPIO_PIN_2 = PORTS_BIT_POS_2,
    GPIO_PIN_3 = PORTS_BIT_POS_3,
    GPIO_PIN_4 = PORTS_BIT_POS_4,
    GPIO_PIN_5 = PORTS_BIT_POS_5,
    GPIO_PIN_6 = PORTS_BIT_POS_6,
    GPIO_PIN_7 = PORTS_BIT_POS_7,
    GPIO_PIN_8 = PORTS_BIT_POS_8,
    GPIO_PIN_9 = PORTS_BIT_POS_9,
    GPIO_PIN_10 = PORTS_BIT_POS_10,
    GPIO_PIN_11 = PORTS_BIT_POS_11,
    GPIO_PIN_12 = PORTS_BIT_POS_12,
    GPIO_PIN_13 = PORTS_BIT_POS_13,
    GPIO_PIN_14 = PORTS_BIT_POS_14,
    GPIO_PIN_15 = PORTS_BIT_POS_15,
}gpio_pin_t;

void gpio_input_init(gpio_port_t port_id, gpio_pin_t pin_id);
void gpio_output_init(gpio_port_t port_id, gpio_pin_t pin_id, uint8_t gpio_value, uint8_t is_od);
void gpio_init(gpio_port_t port_id, gpio_pin_t pin_id, gpio_mode_t mode);
void gpio_set(gpio_port_t port_id, gpio_pin_t pin_id);
void gpio_reset(gpio_port_t port_id, gpio_pin_t pin_id);
void gpio_toggle(gpio_port_t port_id, gpio_pin_t pin_id);
bool gpio_read(gpio_port_t port_id, gpio_pin_t pin_id);
void gpio_pull_up(gpio_port_t port_id, gpio_pin_t pin_id);

#endif
