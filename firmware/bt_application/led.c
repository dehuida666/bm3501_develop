/******************************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PICmicro(r) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
********************************************************************/
#include <xc.h>
#include "led.h"
#include "bt_hardware_profile.h"
#include "tm1812_app.h"


struct {
    uint16_t LED_timer;
    uint16_t LED_on_period;
    uint16_t LED_off_period;
    LED_ACTION LED_action;
} LED[LED_INDEX_MAX];

static void LED_process( uint16_t index );

void LED_init(void)
{
    uint16_t i;
    for(i = 0; i< LED_INDEX_MAX; i++)
    {
        LED[i].LED_action = LED_NOT_CTRL;
        LED[i].LED_timer = 100;
        LED[i].LED_on_period = 100;
        LED[i].LED_off_period = 100;
    }

	led_effect_index = led_none;
	led_effect_index_prev = led_none;
}

void LED_timer_1ms(void)
{
    uint16_t i;
    for(i = 0; i< LED_INDEX_MAX; i++)
    {
        --LED[i].LED_timer;
        if(!LED[i].LED_timer)
        {
			if(isClrPdlWorking() && (i != LED_PATTERN))
				return;
            LED_process(i);
        }
    }
}

void LED_process( uint16_t index )
{
    if(index < LED_INDEX_MAX)
    {
        if(LED[index].LED_action == LED_NOT_CTRL)
        {
            
        }
        else if(LED[index].LED_action == LED_ON)
        {
            switch(index)
            {
                case LED_0:
                    LED0_ON();
                    break;
                case LED_1:
                    LED1_ON();
                    break;
                case LED_2:
                    LED2_ON();
                    break;

				case LED_PATTERN:
                    User_LedPatternDisplay();
                    break;

                default:
                    break;
            }
            LED[index].LED_timer = 10;
        }
        else if(LED[index].LED_action == LED_OFF)
        {
            switch(index)
            {
                case LED_0:
                    LED0_OFF();
                    break;
                case LED_1:
                    LED1_OFF();
                    break;
                case LED_2:
                    LED2_OFF();
                    break;

				case LED_PATTERN:	
					
					break;


                default:
                    break;
            }
            LED[index].LED_timer = 500;
        }
        else
        {
            switch(index)
            {
                case LED_0:
                    LED0_TOGGLE();
                    if(LED0_STATUS_GET())
                        LED[index].LED_timer = LED[index].LED_on_period;
                    else
                        LED[index].LED_timer = LED[index].LED_off_period;
                    break;
                case LED_1:
					LED1_TOGGLE();
                    if(LED1_STATUS_GET())
                        LED[index].LED_timer = LED[index].LED_on_period;
                    else
                        LED[index].LED_timer = LED[index].LED_off_period;
                    break;
                case LED_2:
					LED2_TOGGLE();
                    if(LED2_STATUS_GET())
                        LED[index].LED_timer = LED[index].LED_on_period;
                    else
                        LED[index].LED_timer = LED[index].LED_off_period;
                    break;

                default:
                    break;
            }
        }
    }
}

void Set_LED_Style(LED_INDEX index, LED_ACTION action, uint16_t LED_on_period_1ms, uint16_t LED_off_period_1ms)
{
	if(isClrPdlWorking() && (index != LED_PATTERN))
		return;
	
    LED[index].LED_action = action;
    LED[index].LED_on_period = LED_on_period_1ms;
    LED[index].LED_off_period = LED_off_period_1ms;
    if(action == LED_BLINK)
    {
        switch(index)
        {
            case LED_0:
                LED0_OFF();
                break;
            case LED_1:
                LED1_OFF();
                break;
            case LED_2:
                LED2_OFF();
                break;

        }
    }
	
    LED_process(index);
}

