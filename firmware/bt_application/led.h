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
#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    LED_0,
    LED_1,
    LED_2,
    LED_PATTERN,
    LED_INDEX_MAX,
} LED_INDEX;

typedef enum {
    LED_NOT_CTRL,
    LED_BLINK,
    LED_ON,
    LED_OFF,
}LED_ACTION;


typedef enum {
    led_none,
    led_pairing,
    led_broadcast_master,
    led_broadcast_master_connecting,
    led_broadcast_slave,
    led_broadcast_connect_master,
    led_broadcast_connect_slave,
    led_broadcast_connect_off,
    led_hfp_active,
    led_bt_status_off,
    led_vmaster,
    led_vtreble,
    led_vbass,
    led_voloff,
    led_clr_pdl,
    led_power,
} LED_PATTERN_INDEX;

LED_PATTERN_INDEX led_effect_index;
LED_PATTERN_INDEX led_effect_index_prev;


void LED_init(void);
void LED_timer_1ms(void);
void Set_LED_Style(LED_INDEX index, LED_ACTION action, uint16_t LED_on_period_1ms, uint16_t LED_off_period_1ms);

#endif
