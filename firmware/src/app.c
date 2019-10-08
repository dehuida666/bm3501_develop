/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "bt_hardware_profile.h"
#include "led.h"
//#include "user_key.h"
#include "bt_app.h"
#include "bt_line_in.h"
#include "tm1812_app.h"
//#include "user_event_handler.h"
#include "key.h"
#include "nf8230dsp_handler.h"
#include "user_battery_management.h"
#include "user_i2c.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
DRV_HANDLE I2C_Handle;
DRV_HANDLE UART_Handle;
DRV_HANDLE UART_HandleForPrint;




// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */

	SYS_POWER_ON();

#if 0

	if(DC_PULL_OUT){
	 if(!SYS_POWER_STATUS_GET())
	 	SYS_POWER_ON();
	}
#endif	 

	delay_ms(200);
#ifdef USER_DEBUG_FLAG
	UART_HandleForPrint = DRV_USART_Open(DRV_USART_INDEX_1, 0);
#endif

	IIC_Init();

	TM1812_LEDInit();
	NF8230dsp_init();
	bt_disableUartTransferIntr();
	BTAPP_Init();
    AnalogAudioDetectInit();
	User_BatteryManagementInit();
	DRV_TMR0_Start();
	DRV_TMR1_Start();
	LED_init();
	KEY_Initialize();
	//I2C_Handle = DRV_I2C_Open( DRV_I2C_INDEX_0, 0 );
	UART_Handle = DRV_USART_Open(DRV_USART_INDEX_0, 0);
	DRV_ADC_Open();
    
	#ifdef USER_DEBUG_FLAG
	SYS_INT_SourceDisable(INT_SOURCE_USART_2_TRANSMIT);
	SYS_INT_SourceDisable(INT_SOURCE_USART_2_RECEIVE);
    SYS_INT_SourceDisable(INT_SOURCE_USART_2_ERROR);
    #endif

	if(!IS_BATTERY_DETECT_Enable)
	{
		BATTERY_DETECT_SetEnable();

	}

	DC_DetectTaskStart();
	
	//User_EventHandlerInit();

	User_Log("app init compelete\n");
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
	KEY_Task();
	BTAPP_Task();
	AnalogAudioDetectTask();
	NF8230dsp_task();
	tm1812_task();
	User_BatteryManagementTask();
}

uint16_t GetADCValue(DRV_ADC_INPUTS_POSITIVE ch)
{
	ADC_SAMPLE adc_value = 0;
	DRV_ADC_PositiveInputSelect(ADC_MUX_A, ch);
	DRV_ADC_Start();
	while(!DRV_ADC_SamplesAvailable());
	adc_value = DRV_ADC_SamplesRead(0);
	DRV_ADC_Stop();

	return (uint16_t)adc_value;

}

void User_AutoOffProcess(void)
{
  if(User_sys_nosignal_time)
  {    
  	User_sys_nosignal_time--;
	//User_Log("User_sys_nosignal_time %d\n",User_sys_nosignal_time);
    if(User_sys_nosignal_time == 0)
    { 
        User_Log("User_AutoOffProcess\n");
		BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);
		AutoPowerOffTone_flag = true;
    }
  }
  else
  {
  
     User_sys_nosignal_time = User_NOSIGNAL_TIME_MAX;
  }
}


 

/*******************************************************************************
 End of File
 */
