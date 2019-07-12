/*****************************************************************************
                     Microchip Technology Incorporated

    FileName:        key.c
    Dependencies:    See Includes Section
    Processor:       PIC24, PIC32

Copyright � 2009-2013 Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

******************************************************************************/
#include <xc.h>
#define KEY_C
#include "key.h"
#include "key_handler.h"
#include "bt_hardware_profile.h"
#include "service.h"
#include "app.h"
#include "bt_app.h"



////////////////////////////////////////////////////////////////////////////////
#define KEY_NUMBER_OF 6

const uint8_t keyInitMap[KEY_NUMBER_OF] =
{
    KEY_ID_POWER,
	KEY_ID_BROADCAST,
	KEY_ID_BT,
	KEY_ID_VOL_UP,
    KEY_ID_PLAY_PAUSE,
    KEY_ID_VOL_DN,
};

const uint8_t keyInitMapMode[KEY_NUMBER_OF] =
{
	KEY_MODE_DUAL_ACTION,
	KEY_MODE_DUAL_ACTION,
	KEY_MODE_DUAL_ACTION,
	KEY_MODE_DUAL_ACTION,
	KEY_MODE_TREBLE_ACTION,
    KEY_MODE_DUAL_ACTION,
};

KEY_STATE keyState[KEY_NUMBER_OF];
uint8_t keyTimer = 0;

//static void KEY_Process ( KEY_STATE *key, uint16_t time );
////////////////////////////////////////////////////////////////////////////////

void KEY_Initialize ( void )
{
    uint8_t i;
    for(i = 0; i< KEY_NUMBER_OF; i++)
    {
        keyState[i].id = keyInitMap[i];
        keyState[i].mode = keyInitMapMode[i];
        keyState[i].state = 0;
        keyState[i].time = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
uint16_t SYSTEM_GetKeys ( void )
{
    uint16_t state = 0;
	uint16_t adval;

	adval = GetADCValue(ADC_INPUT_POSITIVE_AN3);
	
    if(!POWER_KEY_STATUS_GET())
	     state |= (1 << 0);
	if(adval < KEY1)
		 state |= 1 << 1;
	else if((adval >= KEY2 - KEYT) && (adval < KEY2 + KEYT))
		 state |= 1 << 2;
	else if((adval >= KEY3 - KEYT) && (adval < KEY3 + KEYT))
		 state |= 1 << 3;
	else if((adval >= KEY4 - KEYT) && (adval < KEY4 + KEYT))
		 state |= 1 << 4;
	else if((adval >= KEY5 - KEYT) && (adval < KEY5 + KEYT))
		 state |= 1 << 5;
	
    
    state &= 0b0000000000111111;

    return  ( state );
}
////////////////////////////////////////////////////////////////////////////////

void KEY_Timer1MS_event(void)
{
    if(keyTimer)
        --keyTimer;
}

void KEY_Task ( void )
{
    uint8_t i;
    uint16_t state;
	

    if(keyTimer)
        return;
    keyTimer = KEY_TASK_PERIOD;	

	state = SYSTEM_GetKeys();

    for ( i = 0; i < KEY_NUMBER_OF; i ++ )
    {
        keyState[i].state = state & 1;
        state >>= 1;
        KEY_Process ( &keyState[i], KEY_TASK_PERIOD );
    }
}

////////////////////////////////////////////////////////////////////////////////

void KEY_Process ( KEY_STATE *key, uint16_t time )
{
    if ( key->mode == KEY_MODE_DISABLED )
    {
        return;
    }

    if ( key->state == true )
    {
		User_sys_nosignal_time = 0;//key pressed
        if(key->time < 60000)
            key->time += time;
    }
    else
    {
        if( key->time_last_off < 60000 )
            key->time_last_off += time;
    }

    switch ( key->mode )
    {
        case KEY_MODE_CONTINUOUS:
            if ( key->state == true )
            {
                KEY_Handler ( key->id, KEY_EVENT_PRIMARY );     //send button every KEY_TASK_PERIOD ms
                key->last_action = KEY_EVENT_PRIMARY;
            }
            
            if (( key->time != 0 ) && ( key->state == false ))      //when key is released
            {
                key->time_last_off = 0;
            }
            break;

        case KEY_MODE_AUTOREPEAT:
            if (( key->time != 0 )
            && ( key->time < KEY_AUTOREPEAT_DELAY )
            && ( key->state == false ))
            {
                KEY_Handler ( key->id, KEY_EVENT_PRIMARY );     //for repeating button, send once if pressing time is short
                key->last_action = KEY_EVENT_PRIMARY;
            }
            else if (( key->time > KEY_AUTOREPEAT_DELAY + KEY_AUTOREPEAT_RATE ) && ( key->state == true ))
            {
                KEY_Handler ( key->id, KEY_EVENT_PRIMARY );     //for repeating button, send button repeatly
                key->time -= KEY_AUTOREPEAT_RATE;
                key->last_action = KEY_EVENT_PRIMARY;
            }
            
            if (( key->time != 0 ) && ( key->state == false ))      //when key is released
            {
                key->time_last_off = 0;
            }
            break;
            
        case KEY_MODE_SINGLE_ACTION:
            if (( key->time != 0 ) && ( key->state == false ))
            {
                KEY_Handler ( key->id, KEY_EVENT_PRIMARY );     //send button only once for every button pressing action, send button out at KEY_TASK_PERIOD ms
                key->last_action = KEY_EVENT_PRIMARY;
            }
            
            if (( key->time != 0 ) && ( key->state == false ))      //when key is released
            {
                key->time_last_off = 0;
            }
            break;

        case KEY_MODE_DUAL_ACTION:
            if (( key->time != 0 )
            && ( key->time < KEY_SINGLE_ACTION_DELAY )
            && ( key->state == false ))
            {
                KEY_Handler ( key->id, KEY_EVENT_PRIMARY );  //send a short pressing button for once at false edge
                key->last_action = KEY_EVENT_PRIMARY;
            }
            else if (( key->time == KEY_LONG_ACTION_DELAY ) && ( key->state == true ))
            {
                KEY_Handler ( key->id, KEY_EVENT_SECONDARY ); //send a long pressing button when button is holding
                key->last_action = KEY_EVENT_SECONDARY;
            }
            else if(( key->time != 0 )
            && ( key->time > KEY_LONG_ACTION_DELAY )
            && ( key->state == false ))
            {
                KEY_Handler ( key->id, KEY_EVENT_SECONDARY_RELEASED );
            }
            
            if (( key->time != 0 ) && ( key->state == false ))      //when key is released
            {
                key->time_last_off = 0;
            }
            break;

        case KEY_MODE_TREBLE_ACTION:
            //single click check
            if( ( key->time == 0 ) 
	            && ( key->state == false )
	            && ( key->time_last_off > KEY_TREBLE_ACTION_OFF_DELAY)
	            && (key->last_action == KEY_EVENT_PRIMARY) )
            {
                KEY_Handler ( key->id, KEY_EVENT_PRIMARY/*KEY_EVENT_SINGLE_CLICK*/ );
                key->last_action = KEY_EVENT_NOT_HANDLE;
            }
            
            if (( key->time != 0 )
            && ( key->time < KEY_SINGLE_ACTION_DELAY )
            && ( key->state == false ))
            {
                //double click check
                if(key->time_last_off <= KEY_TREBLE_ACTION_OFF_DELAY && key->last_action == KEY_EVENT_PRIMARY)
                {
                    KEY_Handler ( key->id, KEY_EVENT_DOUBLE_CLICK );
                    key->last_action = KEY_EVENT_DOUBLE_CLICK;
                }
                else    //button short press check
                {
                    //KEY_Handler ( key->id, KEY_EVENT_PRIMARY );  //send a short pressing button for once at false edge
                    key->last_action = KEY_EVENT_PRIMARY;
                }
            }
            else if (( key->time == KEY_LONG_ACTION_DELAY ) && ( key->state == true ))        //button long press check
            {
                KEY_Handler ( key->id, KEY_EVENT_SECONDARY ); //send a long pressing button when button is holding
                key->last_action = KEY_EVENT_SECONDARY;
            }
            
            if (( key->time != 0 ) && ( key->state == false ))      //when key is released
            {
                key->time_last_off = 0;
            }
            break;
            
        default:
            break;
    }

    if(( key->id == KEY_ID_BT )            //hold BT button for 8 seconds, to reset EEPROM
            && ( key->time == 8000 )       //8 seconds hold
            && ( key->state == true ))      //hold status
    {
        KEY_Handler ( key->id, KEY_EVENT_LONG_TIME_HOLD );
    }

    
    if ( key->state == false )
    {
        key->time = 0;
    }
}


bool Is_KeyWorking(void)
{
    uint8_t i;
	for(i = 0; i< KEY_NUMBER_OF; i++)
    {
		if(keyState[i].state != 0)
			return true;

		if(keyState[i].time != 0)
			return true;

    }

	return false;

}

////////////////////////////////////////////////////////////////////////////////
