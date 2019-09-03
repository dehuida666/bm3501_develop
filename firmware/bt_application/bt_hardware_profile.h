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
#ifndef BT_HARDWARE_PROFILE_H
#define BT_HARDWARE_PROFILE_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "system_config.h"
#include "system_definitions.h"
#include "tm1812_app.h"
#include "led.h"



//#define _COMMAND_DEBUG

//build option for code included
#define _BLE_ADV_CTRL_BY_MCUx
#define _CODE_FOR_APP
#define _STORE_VOLUME_TO_EEPROM
#define _BATTERY_CHARGE_DETECT

#define RECONNECT_TO_PDL


//#define _UNSUPPORT_3A_EVENT     //3A event is used to return link mode, it is supported by MSPK but not support by 104 ROM
//#define _SUPPORT_SIMPLE_33_EVENT //33 event is to report multi speaker status. For MSPK, it return connected status with peer address. But for 104 ROM, there is no address, so need to define this macro

//define command size, command buffer size for command in and out
#define     UR_TX_BUF_SIZE              5000//500         //maximum buffer size for command sending
#define     UR_RX_BUF_SIZE              500         //maximum buffer size for command receiving
#define     BT_CMD_SIZE_MAX				200         //maximum size for a single command
#define     QUEQUED_CMD_MAX                 30      //maximum number of command in buffer to be sending 
#define     QUEQUED_ACK_MAX                 30      //maximum number of ack command in buffer to be sending
#define     ACK_TIME_OUT_MS                 800     //number of ms to be waiting for ack
#define     INTERVAL_AFTER_CMD_ACK_NG          10   //interval(ms) before re-transmit this command in case of no ack and ack with error
#define     INTERVAL_AFTER_CMD_ACK_OK          10   //interval(ms) before send next command



//BM6X BT configuration
#define BM6X_MFB_SetHigh()      PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_8)
#define BM6X_MFB_SetLow()       PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_8)

#define BM6X_RESET_SetHigh()    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define BM6X_RESET_SetLow()     PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)

#define BT_P3_7_STATUS_GET()      (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_7))

#define LINE_IN_DETECT_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_9))
#define MIC_DETECT_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4))
#define DC_DETECT_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_15))
#define DC_PULL_OUT   DC_DETECT_STATUS_GET()

#define USB_CHARGE_SetEnable()          PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define USB_CHARGE_SetDisable()         PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define USB_CHARGE_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5))
#define IS_USB_CHARGE_Enable   	 USB_CHARGE_STATUS_GET()

#define USB_CHARGE_DETECT_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_5))


#define BATTERY_CHARGE_SetDisable()          PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_14)
#define BATTERY_CHARGE_SetEnable()        	 PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_14)
#define BATTERY_CHARGE_STATUS_GET()   		(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_14))
#define IS_BATTERY_CHARGE_Disable  	 		BATTERY_CHARGE_STATUS_GET()

//battery charge
#define BATTERY_DETECT_SetEnable()       		PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define BATTERY_DETECT_SetDisable()      		PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define BATTERY_DETECT_STATUS_GET()       	PLIB_PORTS_PinToggle( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define IS_BATTERY_DETECT_Enable   			(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2))



#define AUX_MIC_SW_Output()         PLIB_PORTS_PinDirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4)
#define AUX_MIC_SW_SetHigh()        PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4)
#define AUX_MIC_SW_SetLow()         PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4)
#define AUX_MIC_SW_STATUS_GET()   	(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4))


//P2_0
#define BT_P2_0_SetOutput()         //PLIB_PORTS_PinDirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define BT_P2_0_SetHigh()           //PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)
#define BT_P2_0_SetLow()            //PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10)

//EAN
#define BT_EAN_SetOutput()          //PLIB_PORTS_PinDirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11)
#define BT_EAN_SetHigh()            //PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11)
#define BT_EAN_SetLow()             //PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11)

//UART RX
#define UART_RX_SetInput()         PLIB_PORTS_PinDirectionInputSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_0)
//UART TX
#define UART_UR_TX_SetInput()         PLIB_PORTS_PinDirectionInputSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_1)

//DSP configuration
#define DSP_RESET_SetHigh()         PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_6)
#define DSP_RESET_SetLow()          PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_6)
#define DSP_POWER_DN_SetHigh()      //PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_7)
#define DSP_POWER_DN_SetLow()       //PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_7)
#define DSP_MUTE_SetHigh()          //PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define DSP_MUTE_SetLow()           //PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define DSP_PROT_SetInput()         //PLIB_PORTS_PinDirectionInputSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_6)
#define DSP_PROT_SetOutput()         //PLIB_PORTS_PinDirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_6)
#define DSP_IRQ_SetInput()          //PLIB_PORTS_PinDirectionInputSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_4)
#define DSP_IRQ_SetOutput()          //PLIB_PORTS_PinDirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_4)

//LED configuration
#define LED0_ON()       PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_7)
#define LED0_OFF()      PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_7)
#define LED0_TOGGLE()   PLIB_PORTS_PinToggle( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_7)
#define LED0_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_7))

#define LED1_ON()       PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_8)
#define LED1_OFF()      PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_8)
#define LED1_TOGGLE()       PLIB_PORTS_PinToggle( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_8)
#define LED1_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_8))

#define LED2_ON()       PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_9)
#define LED2_OFF()      PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_9)
#define LED2_TOGGLE()       PLIB_PORTS_PinToggle( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_9)
#define LED2_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_9))


//SYS power on/off
#define SYS_POWER_ON()       		PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)
#define SYS_POWER_OFF()      		PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)
#define SYS_POWER_TOGGLE()       	PLIB_PORTS_PinToggle( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)
#define SYS_POWER_STATUS_GET()   	(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0))


//Button configuration
#define POWER_KEY_STATUS_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_1))//POWER KEY

//Power configuration
#define ENABLE_DSP_POWER()      //PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)
#define DISABLE_DSP_POWER()     //PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)

//Others
#define CHARGE_STATUS1_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_10))
#define CHARGE_STATUS2_GET()   (PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_7))


#define BATTERY_STATUS_GET()   //(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4))
#define BT_SYS_POWER_STATUS_GET()   //(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_9))
#define BT_P0_0_STATUS_GET()      //(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9))
#define BT_P2_7_STATUS_GET()      //(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_10))
#define BT_LED_1_STATUS_GET()     //(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12))  
#define BT_LED_2_STATUS_GET()     //(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_13))  
#define SET_SWITCH_TO_USB()         //PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_0)
#define SET_SWITCH_TO_UART()         //PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_0)

#define SET_LED_DIN_LOW() PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3);
#define SET_LED_DIN_HIGH() PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3);



#define BT_UART_RX_BYTE_MODEL
#define BT_UART_TX_BYTE_MODEL

#ifdef BT_UART_RX_BYTE_MODEL
#define BT_UART_RX_ISR      BT_CommandDecode_AddByteToBuffer
#endif

#ifdef BT_UART_TX_BYTE_MODEL
void HM_UART_WriteOneByte(uint8_t byte);
#define BT_UART_TX_ISR      BT_CommandSend_SendByte
#define UART_WriteOneByte   HM_UART_WriteOneByte
#endif

void bt_issueUartTransfer( void );
void bt_disableUartTransferIntr( void );
void bt_temporaryDisableUartTransferIntr( void );
void bt_restoreUartTransferIntr( void );

#endif
