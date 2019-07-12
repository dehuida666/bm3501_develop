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
#include "timer.h"
//#include "user_key.h"
#include "bt_app.h"
//#include "bt_ble_adv.h"
#include "bt_command_send.h"
#include "bt_command_decode.h"
//#include "yda174dsp_handler.h"
#include "bt_line_in.h"
#include "led.h"
#include "key.h"
#include "nf8230dsp_handler.h"
#include "user_battery_management.h"


void Timer_1ms(void)
{
    BTAPP_Timer1MS_event();
    BT_CommandSend1MS_event();
    KEY_Timer1MS_event();
    DSP_Timer1MS_event();
    AnalogAudioDetect_Timer1MS_event();
	//EventHandlers_Timer1MS_event();
    //KeyHandlers_Timer1MS_event();
    LED_timer_1ms();
	TM1812_timer_1ms();
    //BTMData_1msTimer();
    BatteryManagement_Timer1MS_event();
}

void Timer_312_5us(void)
{
}

/*********************************************************************************
  * @brief  : the delay function,delay n milseconds
  * @param  : milliSeconds:the time you need to delay,uinit ms
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void delay_ms(uint32_t milliSeconds)
{
    uint32_t frequencyHz = milliSeconds * (SYS_CLK_SystemFrequencyGet()/2000);  //while occupy 2 period
    while(frequencyHz--);
}


/********************************************************************************
  * @brief  : the delay function,delay n milseconds
  * @param  : uSeconds: the time you need to delay,uint us
  * @retval : None
  * @intruduction : send the factory information to server
*******************************************************************************/
void delay_us(uint32_t uSeconds)
{
    uint32_t frequencyHz = uSeconds * (SYS_CLK_SystemFrequencyGet()/2000000);  //while occupy 2 period
    while(frequencyHz--);
}

