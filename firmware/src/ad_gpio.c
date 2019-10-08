/**
  ******************************************************************************
  * @file    ad_gpio.c
  * @author  nod
  * @version v1.0.0
  * @brief   
  ******************************************************************************
  * @copyright
  *                  all rights reserved (www.3nod.com.cn)
  ******************************************************************************
  */

//#include "ad_includes.h"
#include "ad_gpio.h"

/**
  * @brief  init gpio output
  * @param  port_id, gpio port
  *         pin_id,  gpio pin
  *         gpio_value, 0 or 1 after init.
  *         is_od, if it open drain
  * @retval None
  */
void gpio_output_init(gpio_port_t port_id, gpio_pin_t pin_id, uint8_t gpio_value, uint8_t is_od)
{
    PLIB_PORTS_PinModePerPortSelect(PORTS_ID_0, port_id, pin_id, PORTS_PIN_MODE_DIGITAL);

    if(is_od)
        PLIB_PORTS_PinOpenDrainEnable(PORTS_ID_0, port_id, pin_id);

    if(gpio_value)
        PLIB_PORTS_PinSet(PORTS_ID_0, port_id, pin_id);
    else
        PLIB_PORTS_PinClear(PORTS_ID_0, port_id, pin_id);

    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, port_id, pin_id);
}

void gpio_input_init(gpio_port_t port_id, gpio_pin_t pin_id)
{
	PLIB_PORTS_PinModePerPortSelect(PORTS_ID_0, port_id, pin_id, PORTS_PIN_MODE_DIGITAL);
	PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, port_id, pin_id);
}
/**
  * @brief  init gpio, gpio level is low
  * @param  port_id, gpio port
  *         pin_id,  gpio pin
  *         mode,    gpio mode
  * @retval None
  */
void gpio_init(gpio_port_t port_id, gpio_pin_t pin_id, gpio_mode_t mode)
{
    switch (mode)
    {
        case GPIO_DIGITAL_OUTPUT:
            PLIB_PORTS_PinModePerPortSelect(PORTS_ID_0, port_id, pin_id,PORTS_PIN_MODE_DIGITAL);
            PLIB_PORTS_PinClear(PORTS_ID_0, port_id, pin_id);
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, port_id, pin_id);
            break;
        case GPIO_DIGITAL_INPUT:
            PLIB_PORTS_PinModePerPortSelect(PORTS_ID_0, port_id, pin_id,PORTS_PIN_MODE_DIGITAL);
            PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, port_id, pin_id);
            break;
        default:
            break;
    }
}

/**
  * @brief  set gpio output to 1
  * @param  port_id, gpio port
  *         pin_id,  gpio pin
  * @retval None
  */
void gpio_set(gpio_port_t port_id, gpio_pin_t pin_id)
{
    PLIB_PORTS_PinSet(PORTS_ID_0, (PORTS_CHANNEL)port_id, (PORTS_BIT_POS)pin_id);
}

/**
  * @brief  set gpio output to 0
  * @param  port_id, gpio port
  *         pin_id,  gpio pin
  * @retval None
  */
void gpio_reset(gpio_port_t port_id, gpio_pin_t pin_id)
{
    PLIB_PORTS_PinClear(PORTS_ID_0, (PORTS_CHANNEL)port_id, (PORTS_BIT_POS)pin_id);
}

/**
  * @brief  toggle gpio output
  * @param  port_id, gpio port
  *         pin_id,  gpio pin
  * @retval None
  */
void gpio_toggle(gpio_port_t port_id, gpio_pin_t pin_id)
{
    PLIB_PORTS_PinToggle(PORTS_ID_0, (PORTS_CHANNEL)port_id, (PORTS_BIT_POS)pin_id);
}

/**
  * @brief  read gpio input value
  * @param  port_id, gpio port
  *         pin_id,  gpio pin
  * @retval None
  */
bool gpio_read(gpio_port_t port_id, gpio_pin_t pin_id)
{
    return PLIB_PORTS_PinGet(PORTS_ID_0, port_id, pin_id);
}

void gpio_pull_up(gpio_port_t port_id, gpio_pin_t pin_id)
{
	PLIB_PORTS_ChangeNoticePullUpPerPortEnable(PORTS_ID_0, port_id, pin_id);
}

