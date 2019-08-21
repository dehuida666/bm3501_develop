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
#include "bt_hardware_profile.h"
#include "bt_app.h"
#include "bt_data.h"
#include "bt_command_send.h"
#include "bt_command_decode.h"
#include "bt_volume.h"
#include "bt_multi_spk.h"
#include "bt_line_in.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "timer.h"
//#include "yda174dsp_handler.h"
#include "nf8230dsp_handler.h"
#include "app.h"
#include "key.h"

#include "led.h"
#include "user_battery_management.h"
#include "user_tone.h"

#include "key_handler.h"

//===============================================
//  VARIABLES CLAIM
//===============================================
BTAPP_TASK_STATE BTAPP_TaskState;
uint8_t BTAPP_TaskRequest;
BTAPP_STATUS BTAPP_Status;        // BM64 internal system status

bool BT_button_manual_enter_pairing_flag = true;
uint8_t BT_button_manual_reconnect_to_X = 0;

bool Broadcast_connecting_more_tone_flag = false;

//linkback to all device, diffin, 2019-6-18 >>
//variables declaration
#ifdef RECONNECT_TO_PDL
BTAPP_PAIR_RECORD BTAPP_PairRecord[8];
uint8_t BT_LinkBackTaskState;
enum {
    BT_LINKBACK_TASK_IDLE,
    BT_LINKBACK_TASK_START,
    BT_LINKBACK_TASK_NEXT_START,
    BT_LINKBACK_TASK_LB,
    BT_LINKBACK_TASK_LB_WAIT
};
uint8_t BT_LinkBackTarget;
uint32_t BT_LinkBackTimeWait;
#endif
//linkback to all device, diffin, 2019-6-18 <<
BT_PORTS BTAPP_Ports;
uint16_t BTAPP_timer1ms;
//uint16_t DSP_UnmuteTimer1ms; 

#ifdef _BLE_ADV_CTRL_BY_MCU
BLE_ADV_TYPE BLE_advType;
uint8_t BT_deviceName[16+1];
uint8_t BT_MasterAddr[6];
uint8_t BT_groupstatus;
uint8_t BT_groupLinkEvent;
uint8_t BT_btmState;
uint8_t BT_bleAdvFeature = 1;
#endif

#ifdef _CODE_FOR_APP
#define DSP_EQ_SIZE    84
#define DSP_EQ_SEND_INTERVAL   100		   //  100ms
#define BTEQ_MAX_INDEX		   11		   // 84bytes + 1-seq each packet ==> 84/8 = 11 packets
uint16_t BTEQ_timer1ms;
 enum {
	 MULTI_SPK_EQ_ALL = 0,
	 MULTI_SPK_EQ_MASTER,
	 MULTI_SPK_EQ_SLAVE,
 } multiSpk_eq_route;
uint8_t BTEQ_SendIndex;
uint8_t BTEQ_RecIndex;
uint8_t dspEQ[DSP_EQ_SIZE+6];
#endif

#ifdef _STORE_VOLUME_TO_EEPROM
uint16_t saveVol_timer1ms = 15000;
bool saveVolTimeOutFlag = false;
uint8_t a2dpVolSave, hfpVolSave, lineInVolSave;
uint8_t trebleVolSave,bassVolSave;
static void allVolumeCheckAndStoreToEEPROM( void );
#endif
#ifdef _BATTERY_CHARGE_DETECT
uint16_t batteryChargeDetect_timer1ms = 3000;
bool batteryChargeDetectTimeOutFlag = false;
uint8_t currentBatteryLevel; //0-100
bool batteryIsCharging = false;

uint16_t powerOffDelay_timer1ms = 0;
bool powerOffDelayTimeOutFlag = false;

uint16_t User_sys_nosignal_time;
uint16_t sys_timer100ms = 100;
bool sys_timer100msTimeOutFlag = false;

uint16_t batteryLowAutoPowerOff_100mstimer = 0;

bool resetAllVolumeFlag = false;
uint16_t resetAllVolume_timer1ms = 0;
bool resetAllVolumeTimeOutFlag = false;

uint16_t batteryDisplayDelay_timer1ms = 0;
bool batteryDisplayDelayTimeOutFlag = false;


//DC detect, 2019-6-29 >>
//variables declaration
uint8_t DC_DetectTaskState;
enum {
    DC_DETECT_TASK_PLUG_IN,
    DC_DETECT_TASK_PULL_OUT,
    DC_DETECT_TASK_HAVE_PLUGGED_IN,
    DC_DETECT_TASK_HAVE_PULLED_OUT
};
//DC detect, 2019-6-29 <<

#define Battery_ADC_Value_Compensation     20


static void batteryDetect( void );
static uint8_t bat_convert_advalue_to_level(uint16_t ad_value);
static void bat_calculate_average_value (uint16_t ad_value);

#endif

static void led_power_on_failed_indicate();
uint16_t led_power_on_failed_timer1ms = 0;
bool ledPoweronFailedFlag = false;
uint8_t led_power_on_failed_cnt = 0;


 uint16_t batteryLedOnForDCIn_timer1ms = 0;
 bool batteryLedOnForDCOutTimeOutFlag = false;
 //bool DC_Plug_In_Flag = false;


 uint16_t btDelayToChangeVolMode_timer1ms = 100;
 bool btDelayToChangeVolModeTimeOutFlag = false;

 VOLUME_MODE btDelayMode;
 bool isSyncToBTM;

 bool BatteryLowDecreaseVolume3dB_flag = false;

 /*-----------------------------------------------------------------------------*/
/**/
/*-----------------------------------------------------------------------------*/
///////////////////////////////////////////////////////////////////////////////
 //=======================================================================
//  FUNCTION IMPLEMENTION
//========================================================================

//================================================
// Init
//================================================
void BTAPP_Init( void )
{
    //Set_LED_Style(LED_0, LED_OFF, 500, 500);
    
    BTAPP_TaskState = BT_STATE_INITIALIZE_START;
    
    BTAPP_Status.status = BT_SYSTEM_POWER_OFF;
    BTMSPK_Init();
    BTMA2DP_Init();
    BTMHFP_Init();
    BTM_LINE_IN_Init();
    
    BT_CommandDecodeInit();
    BT_CommandSendInit();
    BTVOL_Init();
    BTMDATA_Init();
    
    BTAPP_Ports.PORT_1.value = 0;
    BTAPP_Ports.PORT_2.value = 0;
    BTAPP_Ports.PORT_3.value = 0;
    BTAPP_Ports.PORT_4.value = 0;
#ifdef _CODE_FOR_APP    
	BTEQ_timer1ms = 0;
	BTEQ_SendIndex = BTEQ_MAX_INDEX+1;	// assum it finish the packet sending
	BTEQ_RecIndex = 0;		// expect to see first packet
#endif

	User_sys_nosignal_time = 0;

	currentBatteryLevel = 255;

	resetAllVolumeFlag = false;

	BT_button_manual_enter_pairing_flag = true;
	BT_button_manual_reconnect_to_X = 0;

	Broadcast_connecting_more_tone_flag = false;

	BTAPP_Status.linkedDeviceNumber = 0;

	BatteryLowDecreaseVolume3dB_flag = false;

}

//================================================
// Task
//================================================
void BTAPP_Task(void) {
#if 1
	static BTAPP_TASK_STATE BTAPP_TaskOldState = BT_STATE_INITIALIZE_START;
	if(BTAPP_TaskState != BTAPP_TaskOldState){
		//User_Log("BT state changed\n");
		User_Log("BT current state %d\n",BTAPP_TaskState);
		//User_Log("BT old state %d\n",BTAPP_TaskOldState);
		BTAPP_TaskOldState = BTAPP_TaskState;
	}
#endif

    switch (BTAPP_TaskState) {
        case BT_STATE_INITIALIZE_START:
            BTAPP_timer1ms = 100;
            BTAPP_TaskState = BT_STATE_INIT_MFB_HIGH;
            break;

        case BT_STATE_INIT_MFB_HIGH:
            if (!BTAPP_timer1ms) {
                BTAPP_timer1ms = 20;
                BM6X_MFB_SetHigh();
                BTAPP_TaskState = BT_STATE_INIT_RESET_HIGH;
            }
            break;

        case BT_STATE_INIT_RESET_HIGH:
            if (!BTAPP_timer1ms) {
                BM6X_RESET_SetHigh();
                BT_CommandDecodeInit();
                BTAPP_timer1ms = 300; //wait 300ms
                BTAPP_TaskState = BT_STATE_INIT_RESET_HIGH_WAIT;
            }
            break;

        case BT_STATE_INIT_RESET_HIGH_WAIT:
            if (!BTAPP_timer1ms) //check 300ms times up
            {
                BTAPP_TaskState = BT_STATE_INIT_COMMAND_START;
            }
            break;

        case BT_STATE_INIT_COMMAND_START:
            if(BTAPP_Status.status == BT_SYSTEM_POWER_OFF){
                if(BT_IsCommandSendTaskIdle()){
                    BT_SetRXBufferSize();
                    //BT_ReadFWVersion();
                    //BT_ReadUARTVersion();
                    BT_ReadFeatureListCommand();    //new added for MBA
                    BT_ReadDeviceAddressCommand();
                    BT_ReadDeviceNameCommand();
                    BTAPP_TaskState = BT_STATE_INIT_READ_DEVICE_NAME_WAIT;
                    BTAPP_timer1ms = 2000;      //set 2000ms time out
                }
            }
            break;

        case BT_STATE_INIT_READ_DEVICE_NAME_WAIT:
            if(!BTAPP_timer1ms)
            {
                BTAPP_timer1ms = 1000;
                BT_ReadDeviceNameCommand();        //retry
            }
            break;

#ifdef _STORE_VOLUME_TO_EEPROM            
        case BT_STATE_INIT_READ_EEPROM:
            BT_Read_Vendor_EEPROM(0,5);
            BTAPP_TaskState = BT_STATE_INIT_READ_EEPROM_WAIT;
            BTAPP_timer1ms = 1000; //set 1000 time out
            break;
         
        case BT_STATE_INIT_READ_EEPROM_WAIT:
            if(!BTAPP_timer1ms)
            {
                BTAPP_timer1ms = 1000;
                BT_Read_Vendor_EEPROM(0,5);        //retry
            }
            break;
#endif
        case BT_STATE_INIT_BLE_ADV_START:
#ifdef _BLE_ADV_CTRL_BY_MCU            
            //BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            //BLE_forceUpdate();
#endif            
            BTAPP_TaskState = BT_STATE_INIT_BLE_ADV_WAIT;
		 	BTAPP_timer1ms = 10;
         break;
         
		case BT_STATE_INIT_BLE_ADV_WAIT:
			if( !BTAPP_timer1ms )
			{
					BTAPP_TaskState = BT_STATE_POWER_OFF;
			}
		break;
        
        case BT_STATE_POWER_ON_START:
            
            BTAPP_TaskState = BT_STATE_POWER_DSP_WAIT;
            break;
            
        case BT_STATE_POWER_DSP_WAIT:    
            
			if ( BT_IsCommandSendTaskIdle() )
            {
                BT_MMI_ActionCommand(POWERON_BUTTON_PRESS, 0); //POWER ON button pressed command
                BT_MMI_ActionCommand(POWERON_BUTTON_RELEASE, 0); //POWER ON button released command
                BTAPP_TaskState = BT_STATE_POWER_ON;//BT_STATE_POWER_ON_BUTTON_PRESSED;
                break;
            }
            if (BTAPP_TaskRequest == BT_REQ_SYSTEM_OFF)
            {
                BTAPP_TaskState = BT_STATE_POWER_OFF;
                BTAPP_TaskRequest = BT_REQ_NONE; //clear request
            }
            break;
            
            //BTAPP_TaskState = BT_STATE_POWER_DSP_WAIT;
            //break;
            

        case BT_STATE_POWER_ON:		

#ifdef _BLE_ADV_CTRL_BY_MCU             
			BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            //BLE_forceUpdate();            
            
            BT_ReadNSpkStatus();        //this command issues 0x33 event, and 0x33 event will issue BLE_forceUpdate()
#else
			if(currentBatteryLevel > 100)
			{
				currentBatteryLevel = User_GetCurrentBatteryLevel();
			}
			//BT_UpdateBatteryLevel(currentBatteryLevel);      //battery level init set to 100%
            //BTVOL_InitVolMode();
#endif            
            //BTAPP_timer1ms = 2000;
			BTAPP_TaskState = BT_STATE_READ_PAIR_RECORD;

			//BTAPP_TaskState = BT_STATE_READ_PAIR_RECORD;//BT_STATE_BLE_ADV_WAIT;		
            break;

        //case BT_STATE_BLE_ADV_WAIT:
            //if( !BTAPP_timer1ms )
            //{
        //        BTAPP_TaskState = BT_STATE_READ_PAIR_RECORD;//BT_STATE_VOL_SYNC;
            //}
        //    break;

        //case BT_STATE_VOL_SYNC:
        //    if(BT_IsCommandSendTaskIdle()) {
        //        BTVOL_InitVolMode();
        //        BTAPP_TaskState = BT_STATE_READ_PAIR_RECORD;
        //    }
        //    break;

		

        case BT_STATE_READ_PAIR_RECORD:					
            if(BT_IsCommandSendTaskIdle()){
                BT_GetPairRecordCommand();
                BTAPP_TaskState = BT_STATE_READ_PAIR_RECORD_WAIT;
                BTAPP_timer1ms = 1000; //set 1000 time out
                //BTAPP_timer1ms = 100;
            }
            break;

        case BT_STATE_READ_PAIR_RECORD_WAIT:
            if (!BTAPP_timer1ms) {              //time out
                BTAPP_Status.pairedRecordNumber = 0;
                BT_GetPairRecordCommand();
                BTAPP_timer1ms = 1000; //set time out
            }
            break;

		case BT_STATE_VOL_SYNC:
			if(is_ntp8230g_idle())
            {
                ntp8230g_start();
            }
			
			if(!is_ntp8230g_ready())
                break;
			
            BTVOL_InitVolMode();
			
			BTAPP_TaskState = BT_STATE_PLAY_POWER_ON_TONE;
            break;

		case BT_STATE_PLAY_POWER_ON_TONE:				
			if(BT_IsCommandSendTaskIdle()){
				//BT_PlayTone(TONE_PowerOn);//power on	
				//Tone_PlayPowerOn();
				Tone_PlayVoicePrompt(TONE_PowerOn);
				//Tone_PlayVoicePrompt(TONE_PowerOn);
				User_Log("Send power on tone\n");
				BTAPP_TaskState = BT_STATE_READ_LINKED_MODE;
				BTAPP_timer1ms = 2000;
			}
			break;

        case BT_STATE_READ_LINKED_MODE:
			if (BTAPP_timer1ms)
				break;
			
            if(BT_IsCommandSendTaskIdle()) {
#ifdef _UNSUPPORT_3A_EVENT
                BTAPP_Status.lastMSPKSlaveCounter = 0;
                BTMSPK_BackToLastGroupMode();
                //Set_LED_Style(LED_1, LED_BLINK, 500, 500); //1HZ for linkback process
                BTAPP_TaskState = BT_STATE_READ_LINKED_MODE_WAIT;
                BTAPP_timer1ms = 400;          //set 1 seconds for time out waiting
#else
                BTAPP_Status.lastMSPKSlaveCounter = 0;
                BT_ReadBTMLinkModeCommand();
                //Set_LED_Style(LED_1, LED_BLINK, 500, 500); //1HZ for linkback process
                BTAPP_TaskState = BT_STATE_READ_LINKED_MODE_WAIT;
                BTAPP_timer1ms = 1000;          //set 1 seconds for time out waiting
#endif
            }
            break;

		

        case BT_STATE_READ_LINKED_MODE_WAIT:
            if(!BTAPP_timer1ms)         //time out
            {
                if(BTAPP_Status.pairedRecordNumber != 0)
                {
                    BT_LinkBackToLastDevice();
                    BTAPP_TaskState = BT_STATE_BT_RUNNING;//BT_STATE_LINKBACK_TO_LAST_DEVICE;
                }
                else
                {
                    BT_MMI_ActionCommand(ANY_MODE_ENTERING_PAIRING, 0);
                    BTAPP_TaskState = BT_STATE_BT_RUNNING;
                }
            }
            break;

        case BT_STATE_LINKBACK_START:
            if(BTAPP_timer1ms)  //2019
                break;//2019
                
            if(BT_IsCommandSendTaskIdle())
            {
                switch(BTAPP_Status.lastLinkedMode)
                {
                    case 0:     //single mode
                    case 1:     //multi point
                        if(BTAPP_Status.pairedRecordNumber != 0)
                        {
//linkback to all device, diffin, 2019-6-18 >>	
// start linkback task to linkback to all device, not use original command, which is to linkback to last device.
#ifdef RECONNECT_TO_PDL

                            if(!BTM_LINE_IN_IsPlugged())
                            	BT_LinkbackTaskStart();//BT_LinkBackTaskState = BT_LINKBACK_TASK_START;                            
#else
//linkback to all device, diffin, 2019-6-18 <<		
							BT_LinkBackToLastDevice();
							

#endif

                            //BTAPP_TaskState = BT_STATE_LINKBACK_TO_LAST_DEVICE;
                            BTAPP_TaskState = BT_STATE_BT_RUNNING;
                        }
                        else
                        {
							if(!BTM_LINE_IN_IsPlugged())
                            	BT_MMI_ActionCommand(ANY_MODE_ENTERING_PAIRING, 0);
                            //BT_LinkbackStatus = BT_PAIRING_START;
                            BTAPP_TaskState = BT_STATE_BT_RUNNING;
                        }
                        break;
					#if 0
                    case 2:     //NSPK master
                    case 3:     //NSPK slave
                    case 4:     //broadcast master
                    case 5:     //broadcast slave
                    	if(BTAPP_Status.lastLinkedMode == 4)
                    	{
							User_SetLedPattern(led_broadcast_master);
						}
						else if(BTAPP_Status.lastLinkedMode == 5)
						{
							User_SetLedPattern(led_broadcast_slave);
						}
						
                        BTMSPK_BackToLastGroupMode();
                        BTAPP_TaskState = BT_STATE_BT_RUNNING;
                        //BTAPP_Status.groupLinkingBack = true;
                        break;
					#else
					case 2:     //NSPK master
				        BTMSPK_TriggerStereoModeMaster();
				        BTAPP_TaskState = BT_STATE_BT_RUNNING;
				        break;
				    case 3:     //NSPK slave
				        BTMSPK_TriggerStereoModeSlave();
				        BTAPP_TaskState = BT_STATE_BT_RUNNING;
				        break;
				    case 4:     //broadcast master
				    	//User_SetLedPattern(led_broadcast_master);
				        BTMSPK_TriggerConcertModeMaster();
						
				        BTAPP_TaskState = BT_STATE_BT_RUNNING;
				        break;
				    case 5:     //broadcast slave
				    	//User_SetLedPattern(led_broadcast_slave);
				        BTMSPK_TriggerConcertModeSlave();
						
				        BTAPP_TaskState = BT_STATE_BT_RUNNING;
				        break;

					default:
						break;
		           #endif
                }
            }
            break;


        case BT_STATE_BT_RUNNING:
            if(BTAPP_TaskRequest == BT_REQ_SYSTEM_OFF)
                BTAPP_TaskState =  BT_STATE_POWER_OFF_START;
            BTAPP_TaskRequest = BT_REQ_NONE;

            if (BTAPP_TaskState == BT_STATE_BT_RUNNING) {
				if(NTC_isTemperatureOverLimit())
				{
					User_Log("NTC Power off \n");
					BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);

				}
            }
            break;

        case BT_STATE_POWER_OFF_START:
			if(!User_IsRingToneStopped())
				break;
            if(BT_IsCommandSendTaskIdle()){
                if( BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_NSPK_MASTER            //NSPK master
                        || BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE      //NSPK slave
                        || BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER    //Broadcast master
                        || BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING )    //Broadcast master
                {
                    BT_MMI_ActionCommand(POWER_OFF_ALL_SPK, 0 );
                    BTAPP_TaskState = BT_STATE_POWER_OFF_WAIT_NSPK_EVENT;
					User_Log("POWER_OFF_ALL_SPK\n");
                }
                else
                {
                    BT_MMI_ActionCommand(POWEROFF_BUTTON_PRESS, 0 );      //broadcast slave don't support POWER_OFF_ALL_SPK function
                    BT_MMI_ActionCommand(POWEROFF_BUTTON_RELEASE, 0 );
                    BTAPP_TaskState = BT_STATE_POWER_OFF_WAIT;		
					User_Log("BT_STATE_POWER_OFF_WAIT\n");
                }
            }
			else
			{
				User_Log("BT_IsCommandSendTaskIdle == false\n");
			}
            break;
        
        case BT_STATE_POWER_OFF_WAIT_NSPK_EVENT:
			User_Log("BT_STATE_POWER_OFF_WAIT_NSPK_EVENT\n");
            break;
            
        case BT_STATE_POWER_OFF_START_NSPK:
            if(BT_IsCommandSendTaskIdle()){
                BT_MMI_ActionCommand(POWEROFF_BUTTON_PRESS, 0 );
                BT_MMI_ActionCommand(POWEROFF_BUTTON_RELEASE, 0 );
                BTAPP_TaskState = BT_STATE_POWER_OFF_WAIT;
            }
            break;

        case BT_STATE_POWER_OFF_WAIT:
            if( BTAPP_Status.status == BT_SYSTEM_POWER_OFF )
            {
                BTAPP_TaskState = BT_STATE_POWER_OFF;
				if(DC_PULL_OUT)
				{
					Set_LED_Style(LED_2,LED_OFF,500,500);
					Set_LED_Style(LED_1,LED_OFF,500,500);
					Set_LED_Style(LED_0,LED_OFF,500,500);
				}
				powerOffDelay_timer1ms = 3000;//add by zx
				User_Log("BT_STATE_POWER_OFF\n");
				User_sys_nosignal_time = 0;
				
            }

            break;

        case BT_STATE_POWER_OFF:
            if (BTAPP_TaskRequest != BT_REQ_NONE) {
                if (BTAPP_TaskRequest == BT_REQ_SYSTEM_ON) {
                    BTAPP_TaskState = BT_STATE_POWER_ON_START;
                    BTAPP_TaskRequest = BT_REQ_NONE; //clear request
                    currentBatteryLevel = User_GetCurrentBatteryLevel();
                    User_LEDDisplayNoChargeBatteryLevel(currentBatteryLevel);
					User_Log("BT_STATE_POWER_ON_START\n");
                    
                }
            }
			else
			{
				if((!Is_KeyWorking()) && (powerOffDelay_timer1ms == 0) && (!batteryLedOnForDCIn_timer1ms))
			    {
					//User_Log("enter stop mode\n");
					User_PowerOffEvent();
					
			    }

			}
            break;

        default:
            break;

    }

    if (BTAPP_TaskState >= BT_STATE_INIT_COMMAND_START) {
        BT_CommandDecodeMain();
        BT_CommandSendTask();
#ifdef _CODE_FOR_APP        
		//BT_SendEQBytes();       //sequencialy send EQ bytes received from APP to slave
#endif
#ifdef _STORE_VOLUME_TO_EEPROM
        if( saveVolTimeOutFlag )
        {
            saveVolTimeOutFlag = false;
			if(resetAllVolumeFlag == false)
            	allVolumeCheckAndStoreToEEPROM();
        }
#endif
#ifdef _BATTERY_CHARGE_DETECT
        if(batteryChargeDetectTimeOutFlag)
        {
            batteryChargeDetectTimeOutFlag = false;
            batteryDetect();
        }
#endif
#ifdef RECONNECT_TO_PDL

		BT_LinkbackTask();//linkback to all device, diffin, 2019-6-18
#endif

		DC_DetectTask();
		BTMTONE_Task();


		BTVOL_DelayChangeVolMode();

		if(resetAllVolumeTimeOutFlag)
		{
			resetAllVolumeTimeOutFlag = false;
			BTAPP_ResetEEPROMtoDefault();
		}

		if(ledPoweronFailedFlag)
		{
			ledPoweronFailedFlag = false;
			led_power_on_failed_indicate();

		}

		if(powerOffDelayTimeOutFlag)
		{
			powerOffDelayTimeOutFlag = false;
			//User_PowerOffEvent();
		}

		if(batteryDisplayDelayTimeOutFlag == true)
		{
			
			if(BT_IsCommandSendTaskIdle())
			{
				batteryDisplayDelayTimeOutFlag = false;
				BT_UpdateBatteryLevel(currentBatteryLevel);
				User_Log("BT_UpdateBatteryLevel = %d\n",currentBatteryLevel);
			}
		}

		if(sys_timer100msTimeOutFlag)
		{
			sys_timer100msTimeOutFlag = false;			
			if(BTAPP_GetStatus() > BT_STATUS_OFF)
			{
				if(
					(!DC_PULL_OUT) && 
					((BTM_LINE_IN_IsPlugged()) || 
					  BTAPP_isBTConnected() ||
					  //BTMA2DP_getA2DPLinkStatus(BTMA2DP_getActiveDatabaseIndex()) || 
					 (BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE)
					)
				)
				{
					User_sys_nosignal_time = 0;
					//User_Log("BTM_LINE_IN_IsPlugged %d\n",BTM_LINE_IN_IsPlugged());
					//User_Log("BTMA2DP_getA2DPLinkStatus %d\n",BTMA2DP_getA2DPLinkStatus(BTMA2DP_getActiveDatabaseIndex()));
				}
				else
				{
					if((DC_PULL_OUT) && 
						((BTM_LINE_IN_IsPlugged()) || 
						  (BTMA2DP_GetA2DPCodecStatus() == BT_A2DP_ACTIVE) ||
						 //(BTMA2DP_GetPlayingStatus() == BT_PLAYING_PLAYING) ||  //if BT is disconnected, the status 
						 (BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE) ||
						 (BTMHFP_GetCallStatus() != BT_CALL_IDLE)
						)
					)
					{
						User_sys_nosignal_time = 0;
						//User_Log("BTMA2DP_GetPlayingStatus %d\n",BTMA2DP_GetPlayingStatus());
					}
					else{
						User_AutoOffProcess();
					}
				}


				if(batteryLowAutoPowerOff_100mstimer)
				{
					batteryLowAutoPowerOff_100mstimer--;
					if(batteryLowAutoPowerOff_100mstimer == 0)
					{
						BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);
						User_Log("battery low and power off\n");
					}
				}

			}

		}
    }
}

//linkback to all device, diffin, 2019-6-18 >>
#ifdef RECONNECT_TO_PDL

void BT_LinkbackTaskStart ( void )
{
    if(BTAPP_Status.status == BT_SYSTEM_STANDBY)    //start link back task only when BT link is standby
        BT_LinkBackTaskState = BT_LINKBACK_TASK_START;
}

void BT_LinkbackTaskNextXStart ( uint8_t X_device)//start link back from the x addr, and  won't stop at last if x is last addr
{
    //if(BTAPP_Status.status == BT_SYSTEM_STANDBY)    //start link back task only when BT link is standby
        BT_LinkBackTaskState = BT_LINKBACK_TASK_NEXT_START;
		BT_LinkBackTarget = X_device;
}

void BT_LinkbackTaskNext ( void )//start link back from the 2nd addr, and stop at last
{
    BT_LinkBackTarget++;
    if(BT_LinkBackTarget > BTAPP_Status.pairedRecordNumber){
        BT_LinkBackTarget = 1;
		BT_LinkBackTaskState = BT_LINKBACK_TASK_IDLE;
    }
	else{
		
    	BT_LinkBackTaskState = BT_LINKBACK_TASK_LB;
	}
}
void BT_LinkbackTaskStop ( void )
{
	BT_CancelPage();
    BT_LinkBackTaskState = BT_LINKBACK_TASK_IDLE;
	BT_LinkBackTarget = 1;
}
bool BT_LinkbackTaskRunning ( void )
{
    if(BT_LinkBackTaskState != BT_LINKBACK_TASK_IDLE)
        return true;
    else
        return false;
}
void BT_LinkbackTask( void )
{
    uint8_t i;
    bool flag;
	
    switch(BT_LinkBackTaskState)
    {
        case BT_LINKBACK_TASK_START:
			User_Log("BT_LINKBACK_TASK_START\n");
            BT_LinkBackTarget = 1;
            BT_LinkBackTaskState = BT_LINKBACK_TASK_LB;
            break;

		case BT_LINKBACK_TASK_NEXT_START:
			User_Log("BT_LINKBACK_TASK_NEXT_START\n");
            BT_LinkBackTarget++;
			if(BT_LinkBackTarget > BTAPP_Status.pairedRecordNumber)
            	BT_LinkBackTarget = 1;
            BT_LinkBackTaskState = BT_LINKBACK_TASK_LB;
            break;
			
        case BT_LINKBACK_TASK_LB:
			User_Log("BT_LINKBACK_TASK_LB\n");
            flag = false;
            for(i = 0; i< BTAPP_Status.pairedRecordNumber; i++)
            {
                if(BT_LinkBackTarget == BTAPP_PairRecord[i].linkPriority)
                {
					User_Log("BT_LinkBackTarget = %d\n",BT_LinkBackTarget);
                    BT_LinkBackToDeviceByBTAddress(&BTAPP_PairRecord[i].linkBdAddress[0]);
                    BT_LinkBackTaskState = BT_LINKBACK_TASK_LB_WAIT;
                    BT_LinkBackTimeWait = 30000;    //max. 30s time out
                    flag = true;					
					
                    break;
                }
            }
            if(!flag)
            {
                BT_LinkBackTarget++;
                if(BT_LinkBackTarget > BTAPP_Status.pairedRecordNumber)
                    BT_LinkBackTarget = 1;
            }
            break;
        case BT_LINKBACK_TASK_LB_WAIT:
            if(!BT_LinkBackTimeWait)
            {
				User_Log("BT_LINKBACK_TASK_LB_WAIT\n");
                BT_LinkBackTarget++;
                if(BT_LinkBackTarget > BTAPP_Status.pairedRecordNumber)
                    BT_LinkBackTarget = 1;
                BT_LinkBackTaskState = BT_LINKBACK_TASK_LB;
            }
            break;
        default:
            break;
    }
}
#endif
//linkback to all device, diffin, 2019-6-18 <<

void DC_DetectTask(void)
{
	switch(DC_DetectTaskState)
	{
		case DC_DETECT_TASK_PULL_OUT:
			batteryLedOnForDCOutTimeOutFlag = false;
			if(BTAPP_GetStatus() == BT_STATUS_OFF){
				batteryLedOnForDCIn_timer1ms = 5000;
				if(!SYS_POWER_STATUS_GET())
					SYS_POWER_ON();
		
			}
			else
			{
				batteryLedOnForDCIn_timer1ms = 0;	
				
				
			}

			if(currentBatteryLevel > 100)
			{
				currentBatteryLevel = User_GetCurrentBatteryLevel();
			}

			User_LEDDisplayNoChargeBatteryLevel(currentBatteryLevel);
			
			User_Log("DC pull out\n");
			User_Log("batteryLedOnForDCIn_timer1ms = %d\n",batteryLedOnForDCIn_timer1ms);
			DC_DetectTaskState = DC_DETECT_TASK_HAVE_PULLED_OUT;
			break;

		case DC_DETECT_TASK_HAVE_PULLED_OUT:
			if(batteryLedOnForDCOutTimeOutFlag)
			{
				batteryLedOnForDCOutTimeOutFlag = false;
				
				Set_LED_Style(LED_2,LED_OFF,500,500);
				Set_LED_Style(LED_1,LED_OFF,500,500);
				Set_LED_Style(LED_0,LED_OFF,500,500);

				if(SYS_POWER_STATUS_GET())
					SYS_POWER_OFF();
				
				User_Log("batteryLedOnForDCOutTimeOut\n");
			}

			if(!DC_PULL_OUT)
			{
				DC_DetectTaskState = DC_DETECT_TASK_PLUG_IN;
			}
			break;

		case DC_DETECT_TASK_PLUG_IN:
			batteryLedOnForDCIn_timer1ms = 0;
			batteryLedOnForDCOutTimeOutFlag = false;
			if(currentBatteryLevel > 100)
			{
				currentBatteryLevel = User_GetCurrentBatteryLevel();
			}
			
			User_LEDDisplayChargeBatteryLevel(currentBatteryLevel);

			BatteryLowDecreaseVolume3dB_flag = false;
			ntp8230g_set_volume(VOLUME,volume_master_step);
			
			User_Log("DC plug in\n");
			User_Log("User_LEDDisplayChargeBatteryLevel = %d\n",currentBatteryLevel);
			
			DC_DetectTaskState = DC_DETECT_TASK_HAVE_PLUGGED_IN;
			break;		

		case DC_DETECT_TASK_HAVE_PLUGGED_IN:
			if(DC_PULL_OUT)
			{
				DC_DetectTaskState = DC_DETECT_TASK_PULL_OUT;
			}
			
			if(batteryLowAutoPowerOff_100mstimer)
				batteryLowAutoPowerOff_100mstimer = 0;
			
			break;
			

	}

}

void DC_DetectTaskStart(void)
{
	if(DC_PULL_OUT)
	{
		DC_DetectTaskState = DC_DETECT_TASK_HAVE_PULLED_OUT;
	}
	else
	{
		DC_DetectTaskState = DC_DETECT_TASK_PLUG_IN;
	}

}


//================================================
//BT status checking
//================================================
uint8_t BTAPP_GetStatus(void)
{
    if(BTAPP_TaskState == BT_STATE_POWER_OFF)
        return BT_STATUS_OFF;
    else if(BTAPP_TaskState >= BT_STATE_POWER_ON && BTAPP_TaskState < BT_STATE_BT_RUNNING)
        return BT_STATUS_ON;
    else if(BTAPP_TaskState == BT_STATE_BT_RUNNING)
        return BT_STATUS_READY;
    else
        return BT_STATUS_NONE;
}

//================================================
// BT Event Handler
//================================================
void BTAPP_EventHandler(BT_APP_EVENTS event, uint8_t* paras, uint16_t size )
{
	uint8_t i=0;
    uint8_t nspk_link_status, nspk_link_event;
#ifdef _SUPPORT_SIMPLE_33_EVENT
    uint8_t send_master_address[7];
#endif

	User_Log("BTAPP_EventHandler event : 0x%02x\n",event);
    switch(event)
    {
        case BT_EVENT_CMD_SENT_ACK_OK:
            break;
            
        case BT_EVENT_CMD_SENT_ACK_ERROR:
            BT_GiveUpThisCommand();
            break;
            
        case BT_EVENT_CMD_SENT_NO_ACK:
            if(*paras == 0x04 || *paras == 0x02)        //MMI_ACTION or MUSIC_CTRL
                BT_GiveUpThisCommand();
			else
			{
				User_Log("BT_EVENT_CMD_SENT_NO_ACK : 0x%02x\n",*paras);
			}
            break;
            
        case BT_EVENT_NSPK_VENDOR_CMD:
#ifdef _CODE_FOR_APP    
		if( (paras[0x03] & 0xf0) == 0x80 )      //Vendor EQ handle
		{
			// Vendor EQ commands
			paras[0x03] -= 0x80;
			if(paras[0x03] == 0x00)
			{
				for(i=0; i<DSP_EQ_SIZE; i++)
					dspEQ[i] = 0;
	
				dspEQ[0] = 0xAA;	 //header byte 0
				dspEQ[1] = 0x00;	 //header byte 1
				dspEQ[2] = 84+2;	 //length
				dspEQ[3] = 0x30;		//command ID
				dspEQ[4] = 0x13;	   //link_index, set to 0
	
				BTEQ_RecIndex = 0;
			}
			else if(paras[0x03] >= BTEQ_MAX_INDEX)
			{
				// Seq out range, do nothing.
				//break;
			}
			
			if(paras[0x03] == BTEQ_RecIndex)
			{
				if(paras[2] == 9)
				{
					for(i=0; i<8;i++)
						dspEQ[5 + (BTEQ_RecIndex * (VENDOR_CMD_SIZE-1)) + i] = paras[4 +i]; 
				}
				else 
				{
					for(i=0; i<4;i++)
						dspEQ[5 + (BTEQ_RecIndex * (VENDOR_CMD_SIZE-1)) + i] = paras[4 +i]; 
				}	
				BTEQ_RecIndex++;
				if(BTEQ_RecIndex == BTEQ_MAX_INDEX)
				{
					BTEQ_RecIndex = 0;
					// the EQ data completed
					BT_SendBytesAsEQCommandWithRedoChecksum(dspEQ, 90);				
				}			
			}
			else
			{
				// Get the wrong seq-packet.
				// Ignore all the packet until see the first packet.
				
			}
			return;
		}
		else        //other Vendor events handle
		{
            switch( paras[0x03] )
            {
                //case 1:
                case 0xFE:      //vol sync to slave
                    //BTVOL_EventHandler(BT_EVENT_AVRCP_ABS_VOLUME_CHANGED, &paras[3], 2 );
                    BTVOL_GotSyncVolFromMasterByVendorCmd(paras[4], paras[5]);
                    break;
                case 0xFD:
#ifdef _SUPPORT_SIMPLE_33_EVENT
    #ifdef _BLE_ADV_CTRL_BY_MCU                            
			BLE_SaveMasterAddr( &paras[4] );
            BLE_advUpdateGroupStatus(SPEAKER_nSPK_SLAVE);
            BLE_UpdateAdvType(SCANNABLE_UNDIRECT_ADV); //non-connectable
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
    #endif 
#endif
                    break;
                case 0xFC:
                    if( BTAPP_Status.BDAddr[0] == paras[4] 
                            && BTAPP_Status.BDAddr[1] == paras[5]
                            && BTAPP_Status.BDAddr[2] == paras[6]
                            && BTAPP_Status.BDAddr[3] == paras[7] )
                    {
                        BT_MMI_ActionCommand(paras[10], 0);
                    }
                    break;
                default:
                    break;
            }
		} 
#endif            
            break;
            
        case BT_EVENT_MSPK_STANDBY:
//linkback to all device, diffin, 2019-6-18 >>	
//start linkback task when multi speaker reports idle status.
//but this may cause other side effect, may need more accurate condtion to start the task.	
#ifdef RECONNECT_TO_PDL
            if(size && paras[0])
            {
                if(!BT_LinkbackTaskRunning())
                    ;//BT_LinkbackTaskStart();
            }
#endif
//linkback to all device, diffin, 2019-6-18 <<		

			if(Broadcast_go_to_slave_flag)
			{
				Broadcast_go_to_slave_flag = false;
				ACL_disconnect_to_enter_slave_flag = false;
                if(BTAPP_isBTConnected()){
					BT_DisconnectAllProfile();
					ACL_disconnect_to_enter_slave_flag = true;
					BT_button_manual_enter_pairing_flag = false;
					BT_button_manual_reconnect_to_X = 0;
                }
				else
                	BTMSPK_TriggerConcertModeSlave();
			}

			if(Broadcast_go_to_master_flag)
			{
				Broadcast_go_to_master_flag = false;
				BTMSPK_TriggerConcertModeMaster();
				
			}
			//User_LedBroadcastConnectedOFF();
			//User_SetLedPattern(led_broadcast_connect_off);
			User_Log("BT_EVENT_MSPK_STANDBY\n");
			
			//User_SetLedPattern(led_off);

            //if( BTAPP_Status.status == BT_SYSTEM_STANDBY && BTAPP_Status.groupLinkingBack == true)
            //{
            //    BTAPP_Status.groupLinkingBack = false;
            //    BT_LinkBackToLastDevice();
            //}
#ifdef _BLE_ADV_CTRL_BY_MCU                    
			BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
			BLE_advUpdateGroupStatus(SPEAKER_SINGLE);	
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif              
            break;
        case BT_EVENT_MSPK_CONNECTING:
			BT_PlayTone(TONE_BroadcastPairing);
			User_Log("BT_EVENT_MSPK_CONNECTING\n");
			
#ifdef _UNSUPPORT_3A_EVENT   
            if( BTAPP_TaskState == BT_STATE_READ_LINKED_MODE )
            {
                BTAPP_TaskState = BT_STATE_BT_RUNNING;
            }
#endif
#ifdef _BLE_ADV_CTRL_BY_MCU
			//BLE_advUpdateGroupStatus(SPEAKER_CONNECTING);       //EA app
            BLE_advUpdateGroupStatus(SPEAKER_SINGLE);               //v1.16 app
			BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_MSPK_CONNECTED_AS_NSPK_MASTER:
#ifdef _BLE_ADV_CTRL_BY_MCU
            BLE_advUpdateGroupStatus(SPEAKER_nSPK_MASTER);
			BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif
#ifdef _SUPPORT_SIMPLE_33_EVENT
            //send master address to slave, only for stereo mode, not needed for concert mode
            send_master_address[0] = 0xFD;
            send_master_address[1] = BTAPP_Status.BDAddr[0];
            send_master_address[2] = BTAPP_Status.BDAddr[1];
            send_master_address[3] = BTAPP_Status.BDAddr[2];
            send_master_address[4] = BTAPP_Status.BDAddr[3];
            send_master_address[5] = BTAPP_Status.BDAddr[4];
            send_master_address[6] = BTAPP_Status.BDAddr[5];
            BT_Vendor_SendCommand(send_master_address, 7);
#endif
            break;
        case BT_EVENT_MSPK_CONNECTED_AS_NSPK_SLAVE:
		
#ifndef _SUPPORT_SIMPLE_33_EVENT        //undefine this macro for MSPK, define it for 104 ROM
#ifdef _BLE_ADV_CTRL_BY_MCU                            
			BLE_SaveMasterAddr( BTMSPK_GetMasterBDAddress() );
            BLE_advUpdateGroupStatus(SPEAKER_nSPK_SLAVE);
            BLE_UpdateAdvType(SCANNABLE_UNDIRECT_ADV); //non-connectable
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif
#endif
            break;
        case BT_EVENT_MSPK_CONNECTED_AS_BROADCAST_MASTER:
			//BT_PlayTone(TONE_Connected);
			User_SetLedPattern(led_broadcast_connect_master);
			//User_LedBroadcastConnectedON();
			User_Log("BT_EVENT_MSPK_CONNECTED_AS_BROADCAST_MASTER\n");
//linkback to all device, diffin, 2019-6-18 >>		
//start linkback task when master connected one slave
//need to check linkback task is in idle, to avoid repeat task starting.	
#ifdef RECONNECT_TO_PDL

                if(!BT_LinkbackTaskRunning())
                    ;//BT_LinkbackTaskStart();
#endif
//linkback to all device, diffin, 2019-6-18 <<					

#ifdef _BLE_ADV_CTRL_BY_MCU
            BLE_advUpdateGroupStatus(SPEAKER_BROADCAST_MASTER_ADDING);
			BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_MSPK_CONNECTED_AS_BROADCAST_SLAVE:
			//User_LedBroadcastConnectedON();
			//BT_PlayTone(TONE_Connected);
			User_SetLedPattern(led_broadcast_connect_slave);
			User_Log("BT_EVENT_MSPK_CONNECTED_AS_BROADCAST_SLAVE\n");
            
#ifdef _BLE_ADV_CTRL_BY_MCU
			BLE_SaveMasterAddr( BTMSPK_GetMasterBDAddress() );
            BLE_advUpdateGroupStatus(SPEAKER_BROADCAST_SLAVE);
            BLE_UpdateAdvType(SCANNABLE_UNDIRECT_ADV); //non-connectable
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif              
            break;
        case BT_EVENT_MSPK_BROADCAST_MASTER_CONNECTING_MORE:
			//Set_LED_Style(LED_BROADCASTP, LED_ON, 500, 500);
			if(Broadcast_connecting_more_tone_flag)
			{
				BT_PlayTone(TONE_BroadcastPairing);
				Broadcast_connecting_more_tone_flag = false;
			}
			User_SetLedPattern(led_broadcast_master_connecting);
			
			User_Log("BT_EVENT_MSPK_BROADCAST_MASTER_CONNECTING_MORE\n");
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app
            //BLE_advUpdateGroupStatus(SPEAKER_BROADCAST_MASTER_ADDING);
			//BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            //BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            //BLE_advUpdateLinkStatus(nspk_link_event);
            //BLE_forceUpdate();
#endif             
            break;
        case BT_EVENT_MSPK_BROADCAST_MASTER_CONNECTING_END:
			//User_LedPrimaryPairingOFF();
			User_SetLedPattern(led_broadcast_connect_master);
			User_Log("BT_EVENT_MSPK_BROADCAST_MASTER_CONNECTING_END\n");
     
            //if( BTAPP_Status.status == BT_SYSTEM_STANDBY && BTAPP_Status.groupLinkingBack == true)
            //{
            //    BTAPP_Status.groupLinkingBack = false;
            //    BT_LinkBackToLastDevice();
            //}
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app
            BLE_advUpdateGroupStatus(SPEAKER_BROADCAST_MASTER);
			BLE_UpdateAdvType(CONNECTABLE_UNDIRECT_ADV);
            BTMSPK_GetNSPKLinksStatusAndEvent( &nspk_link_status, &nspk_link_event );
            BLE_advUpdateLinkStatus(nspk_link_event);
            BLE_forceUpdate();
#endif             
            break;

        case BT_EVENT_MSPK_SYNC_AUDIO_CHANNEL:
            if(*paras == 0x01)      //L+L
            {
            }
            else if(*paras == 0x02)     //R+R
            {
 
            }
            else
            {

            }
            break;

        case BT_EVENT_HFP_LINK_CONNECTED:
			if(User_getLinkedDeviceNumber() < 2)
				BTAPP_Status.linkedDeviceNumber++;
			
            BTAPP_Status.status = BT_SYSTEM_CONNECTED;
			batteryDisplayDelay_timer1ms = 2000;
			#ifdef RECONNECT_TO_PDL
			if(User_getLinkedDeviceNumber() >= 2)
            	BT_LinkbackTaskStop(); //linkback to all device, diffin, 2019-6-18
            else if((BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER) ||
					(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING)
			)
            {
				BT_LinkbackTaskStop(); //linkback to all device, diffin, 2019-6-18
			}
            else if(BT_LinkbackTaskRunning())
				BT_LinkbackTaskNext();
            #endif
			//BT_UpdateBatteryLevel(currentBatteryLevel);
			User_Log("BT_EVENT_HFP_LINK_CONNECTED\n");
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_CONN_ALL))
                BLE_forceUpdate();
#endif

            break;
        case BT_EVENT_HFP_LINK_DISCONNECTED:
			User_Log("BT_EVENT_HFP_LINK_DISCONNECTED\n");
            break;
        case BT_EVENT_SPP_LINK_CONNECTED:
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_CONN_ALL))
                BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_IAP_LINK_CONNETED:
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_CONN_ALL))
                BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_SPP_IAP_LINK_DISCONNECTED:
            break;
        case BT_EVENT_A2DP_LINK_CONNECTED:			
        case BT_EVENT_A2DP_LINK_CHANGED:
			if(
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER) &&
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING)

			)
			{
				User_SetLedPattern(led_bt_status_off);
			}			
			
            BTAPP_Status.status = BT_SYSTEM_CONNECTED;
			BT_PlayTone(TONE_Connected);
			//if(BT_EVENT_A2DP_LINK_CONNECTED == event)
			{
              if(BT_IsCommandSendTaskIdle())
			  {
                BT_GetPairRecordCommand();
              }
			}
			
			#ifdef RECONNECT_TO_PDL
            //BT_LinkbackTaskStop(); //linkback to all device, diffin, 2019-6-18   
            #endif
			
			User_Log("BT_EVENT_A2DP_LINK_CONNECTED\n");
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_CONN_ALL))
                BLE_forceUpdate();
#endif             
            break;
        case BT_EVENT_A2DP_LINK_DISCONNECTED:			
			User_Log("BT_EVENT_A2DP_LINK_DISCONNECTED\n");
			
            break;

		case BT_EVENT_ACL_DISCONNECTED:
			if(User_getLinkedDeviceNumber())
				BTAPP_Status.linkedDeviceNumber--;
			User_Log("BT_EVENT_ACL_DISCONNECTED\n");
			if(ACL_disconnect_to_enter_slave_flag)
			{
				if(User_getLinkedDeviceNumber() == 0){
					ACL_disconnect_to_enter_slave_flag = false;
					BTMSPK_TriggerConcertModeSlave();
				}
			}

			if(ACL_disconnect_to_enter_master_flag)
			{
				if(User_getLinkedDeviceNumber() == 0){
					ACL_disconnect_to_enter_master_flag = false;
					BTMSPK_TriggerConcertModeMaster();
				}

			}

			if(BT_button_manual_reconnect_to_X == 1)//reconnect to  2ed device
			{
				if(User_getLinkedDeviceNumber() == 0){
					BT_button_manual_reconnect_to_X = 0;
					BT_LinkbackTaskNextXStart(1);
				}
			}
			else if(BT_button_manual_reconnect_to_X == 2)//reconnect to  3rd device
			{
				if(User_getLinkedDeviceNumber() == 0){
					BT_button_manual_reconnect_to_X = 0;
					BT_LinkbackTaskNextXStart(2);
				}
			}

			if(BT_button_manual_enter_pairing_flag){
				if(User_getLinkedDeviceNumber() == 0)
					BTAPP_EnterBTPairingMode();
			}
			else if(User_getLinkedDeviceNumber() == 0)
			{
				BT_button_manual_enter_pairing_flag = true;
			}
				
			break;
				
        case BT_EVENT_SYS_POWER_ON:			
			User_sys_nosignal_time = 0;
            BTAPP_Status.status = BT_SYSTEM_POWER_ON;
            User_SetLedPattern(led_power);
			User_LedPowerIndicate(ON);
			User_Log("BT_EVENT_SYS_POWER_ON\n");
            
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_STBY))
                BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_SYS_POWER_OFF:
            BTAPP_Status.status = BT_SYSTEM_POWER_OFF;
			//BT_PlayTone(TONE_PowerOff);
			User_SetLedPattern(led_voloff);
			User_LedPrimaryPairingOFF();
			User_SetLedPattern(led_bt_status_off);
			User_LedPowerIndicate(OFF);
			//User_LedBroadcastConnectedOFF();
			User_LedClearPDLOFF();
			User_Log("BT_EVENT_SYS_POWER_OFF\n");
			
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_OFF))
                BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_SYS_STANDBY:
			//if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_STANDBY)
				//User_SetLedPattern(led_off);
			User_Log("BT_EVENT_SYS_STANDBY\n");
            BTAPP_Status.status = BT_SYSTEM_STANDBY;
            
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_STBY))
                BLE_forceUpdate();
#endif            
            break;

        case BT_EVENT_SYS_PAIRING_START:
			if(BTAPP_Status.status == BT_SYSTEM_POWER_OFF)
				break;
            BTAPP_Status.status = BT_SYSTEM_PAIRING;
			Tone_PlayVoicePrompt(TONE_BTPairing);
            User_SetLedPattern(led_pairing);
			User_Log("BT_EVENT_SYS_PAIRING_START\n");
#ifdef _BLE_ADV_CTRL_BY_MCU         //v1.16 app            
            if(BLE_advUpdateBTMState(BLE_BTM_PAIR))
                BLE_forceUpdate();
#endif            
            break;
        case BT_EVENT_SYS_PAIRING_OK:
			
            break;
        case BT_EVENT_SYS_PAIRING_FAILED:			
            if(
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER) &&
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING)

			)
			{
				User_SetLedPattern(led_bt_status_off);
			}
			User_Log("BT_EVENT_SYS_PAIRING_FAILED\n");
            break;

        case BT_EVENT_LINKBACK_SUCCESS:
			if(
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER) &&
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING)

			)
			{
				User_SetLedPattern(led_bt_status_off);
			}
			#ifdef RECONNECT_TO_PDL
            //BT_LinkbackTaskStop(); //linkback to all device, diffin, 2019-6-18
            #endif
			User_Log("BT_EVENT_LINKBACK_SUCCESS\n");
            break;

        case BT_EVENT_LINKBACK_FAILED:
            if(
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER) &&
				(BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING)

			)
			{
				User_SetLedPattern(led_bt_status_off);
			}	
			#ifdef RECONNECT_TO_PDL
            BT_LinkbackTaskNext(); //linkback to all device, diffin, 2019-6-18
            #endif
			User_Log("BT_EVENT_LINKBACK_FAILED\n");
            break;

       case BT_EVENT_BD_ADDR_RECEIVED:
	   		User_Log("BT_EVENT_BD_ADDR_RECEIVED\n");
           BTAPP_Status.BDAddr[0] = *paras++;
           BTAPP_Status.BDAddr[1] = *paras++;
           BTAPP_Status.BDAddr[2] = *paras++;
           BTAPP_Status.BDAddr[3] = *paras++;
           BTAPP_Status.BDAddr[4] = *paras++;
           BTAPP_Status.BDAddr[5] = *paras;
            break; 
            
	   case BT_EVENT_NAME_RECEIVED:
#ifdef _BLE_ADV_CTRL_BY_MCU           
           BLE_SaveDeviceName(paras);
#endif           
			if(BTAPP_TaskState == BT_STATE_INIT_READ_DEVICE_NAME_WAIT)
			{
				BTAPP_timer1ms = 0;
#ifdef _STORE_VOLUME_TO_EEPROM                
				BTAPP_TaskState = BT_STATE_INIT_READ_EEPROM;
#else
                BTAPP_TaskState = BT_STATE_INIT_BLE_ADV_START;
#endif
			}
			break;

        case BT_EVENT_PAIR_RECORD_RECEIVED:			
#ifdef RECONNECT_TO_PDL

            BTAPP_Status.pairedRecordNumber = *paras++;
//linkback to all device, diffin, 2019-6-18 >>
//save all paired device information at this event
            memset(&BTAPP_PairRecord[0], 0, 8*sizeof(BTAPP_PAIR_RECORD));
            for(i = 0; i< BTAPP_Status.pairedRecordNumber; i++)
            {
                BTAPP_PairRecord[i].linkPriority = *paras++;
                BTAPP_PairRecord[i].linkBdAddress[0] = *paras++;
                BTAPP_PairRecord[i].linkBdAddress[1] = *paras++;
                BTAPP_PairRecord[i].linkBdAddress[2] = *paras++;
                BTAPP_PairRecord[i].linkBdAddress[3] = *paras++;
                BTAPP_PairRecord[i].linkBdAddress[4] = *paras++;
                BTAPP_PairRecord[i].linkBdAddress[5] = *paras++;

				User_Log("BTAPP_PairRecord[%d].linkPriority = %d\n",i, BTAPP_PairRecord[i].linkPriority);
				User_Log("BTAPP_PairRecord[%d].linkBdAddress[0] = %d\n",i, BTAPP_PairRecord[i].linkBdAddress[0]);
				User_Log("BTAPP_PairRecord[%d].linkBdAddress[1] = %d\n",i, BTAPP_PairRecord[i].linkBdAddress[1]);
            }
//linkback to all device, diffin, 2019-6-18 <<
#else
			BTAPP_Status.pairedRecordNumber = *paras;

#endif
            if(BTAPP_TaskState == BT_STATE_READ_PAIR_RECORD_WAIT)
            {
                BTAPP_TaskState = BT_STATE_VOL_SYNC;
            }
            break;

        case BT_EVENT_LINK_MODE_RECEIVED:
            BTAPP_Status.lastLinkedMode = *paras++;
            if(BTAPP_Status.lastLinkedMode == 2 || BTAPP_Status.lastLinkedMode == 4)        //NSPK master, Broadcast master
            {
                BTAPP_Status.lastMSPKSlaveCounter = *paras;
            }
            if(BTAPP_TaskState == BT_STATE_READ_LINKED_MODE_WAIT)
            {
                BTAPP_timer1ms = 500; //clear time out timer    //2019
                BTAPP_TaskState = BT_STATE_LINKBACK_START;
            }
            break;

        case BT_EVENT_A2DP_BREAK_IN:	
            break;

        case BT_EVENT_MSPK__SYNC_POWER_OFF:
            BTAPP_TaskState = BT_STATE_POWER_OFF_START_NSPK;
            break;

        case BT_EVENT_PORT_INPUT_CHANGED:
            break;
            
        case BT_EVENT_BTM_VERSION:
            if(paras[0] == 0x01)        //1: FW version
            {
                BTAPP_Status.FWVer[0] = paras[1];
                BTAPP_Status.FWVer[1] = paras[2];
            }
            else                        //0: UART version
            {
                BTAPP_Status.UARTVer[0] = paras[1];
                BTAPP_Status.UARTVer[1] = paras[2];
            }
            break;
        
        case BT_EVENT_BATTERY_LEVEL:
            BTAPP_Status.batteryStatus = paras[0];      //save battery status
            BTAPP_Status.batteryLevel = paras[1];       //save battery level
            break;
        
        case BT_EVENT_FEATURE_LIST_RECEIVED:
#ifdef _BLE_ADV_CTRL_BY_MCU
            BLE_SaveFeatureList(paras[0]);
#endif
            break;
        
        case BT_EVENT_BTM_INITIALIZED:
            if(BTAPP_TaskState > BT_STATE_INIT_READ_DEVICE_NAME_WAIT)
                BTAPP_Init();
            break;
#ifdef _STORE_VOLUME_TO_EEPROM            
        case BT_EVENT_READ_EEPROM_OK:
            BTVOL_InitWithSpecificVolume(paras[0], paras[1], paras[2], paras[3], paras[4]);
            a2dpVolSave = BTVOL_GetCurrentA2DPVol();
            hfpVolSave = BTVOL_GetHFPVol();
            lineInVolSave = BTVOL_GetLineInVol();
			trebleVolSave = BTVOL_GetTrebleVol();
			bassVolSave = BTVOL_GetBassVol();
            if(BTAPP_TaskState == BT_STATE_INIT_READ_EEPROM_WAIT)
                BTAPP_TaskState = BT_STATE_INIT_BLE_ADV_START;
            break; 
#endif

		case BT_EVENT_RINGTONE_INDICATION:
			User_SetRingToneVolume(paras[0], paras[1]);
			break;
            
        default:
            break;
    }
}

//================================================
// BT Task Request
//================================================
void BTAPP_TaskReq(uint8_t request)
{
    BTAPP_TaskRequest = request;
#ifdef RECONNECT_TO_PDL	
    BT_LinkbackTaskStop();//linkback to all device, diffin, 2019-6-18
#endif
}

//================================================
//1ms Timer
//================================================
void BTAPP_Timer1MS_event( void )
{
	#ifdef RECONNECT_TO_PDL
    if(BT_LinkBackTimeWait)		//linkback to all device, diffin, 2019-6-18
        --BT_LinkBackTimeWait;	//linkback to all device, diffin, 2019-6-18
    #endif
    if(BTAPP_timer1ms)
        --BTAPP_timer1ms;
	//if(DSP_UnmuteTimer1ms)
    //{
    //    --DSP_UnmuteTimer1ms;
    //    if(!DSP_UnmuteTimer1ms)
    //    {
    //        DSP_MUTE_SetHigh(); 
    //    }
    //}
#ifdef _CODE_FOR_APP    
    if(BTEQ_timer1ms)
		--BTEQ_timer1ms;
#endif    
#ifdef _STORE_VOLUME_TO_EEPROM  
    if(saveVol_timer1ms)
    {
       --saveVol_timer1ms;
       if(saveVol_timer1ms == 0)
       {
           saveVol_timer1ms = 10000;
           saveVolTimeOutFlag = true;
       }
    }
#endif
#ifdef _BATTERY_CHARGE_DETECT
    if(batteryChargeDetect_timer1ms)
    {
        -- batteryChargeDetect_timer1ms;
        if(batteryChargeDetect_timer1ms == 0)
        {
			if((currentBatteryLevel <= 10) && (DC_PULL_OUT))
				batteryChargeDetect_timer1ms = 500;
			else
            	batteryChargeDetect_timer1ms = 3000;
			
            batteryChargeDetectTimeOutFlag = true;
        }
    }
#endif   

	if(batteryLedOnForDCIn_timer1ms)
	{
		batteryLedOnForDCIn_timer1ms--;
		if(batteryLedOnForDCIn_timer1ms == 0)
		{
			batteryLedOnForDCOutTimeOutFlag = true;
		}
	}

	if(led_power_on_failed_timer1ms)
	{
		led_power_on_failed_timer1ms--;
		if(led_power_on_failed_timer1ms == 0)
		{
			ledPoweronFailedFlag = true;
		}

	}

	if(powerOffDelay_timer1ms)
	{
		powerOffDelay_timer1ms--;
		if(powerOffDelay_timer1ms == 0)
		{
			powerOffDelayTimeOutFlag = true;
		}

	}

	if(sys_timer100ms)
	{
		sys_timer100ms--;
		if(sys_timer100ms == 0){
			sys_timer100ms = 100;
			sys_timer100msTimeOutFlag = true;
		}

	}

	if(resetAllVolume_timer1ms)
	{
		resetAllVolume_timer1ms--;
		if(resetAllVolume_timer1ms == 0)
		{
			resetAllVolumeTimeOutFlag = true;
		}

	}


	if(btDelayToChangeVolMode_timer1ms)
	{
		btDelayToChangeVolMode_timer1ms--;
		if(btDelayToChangeVolMode_timer1ms == 0)
		{
			btDelayToChangeVolModeTimeOutFlag = true;
		}
	}


	if(batteryDisplayDelay_timer1ms)
	{
		batteryDisplayDelay_timer1ms--;
		if(batteryDisplayDelay_timer1ms == 0)
			batteryDisplayDelayTimeOutFlag = true;

	}
}

/*-----------------------------------------------------------------------------*/
//EXTERNAL API interface(for UI)
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
void BTAPP_EnterBTPairingMode( void )
{
    if(BTAPP_TaskState == BT_STATE_BT_RUNNING)
    {
		if(BTAPP_Status.status != BT_SYSTEM_PAIRING)
		{
			if(!BTM_LINE_IN_IsPlugged())
			{
				BT_MMI_ActionCommand(ANY_MODE_ENTERING_PAIRING, 0);
			}
			#ifdef RECONNECT_TO_PDL
            BT_LinkbackTaskStop(); //linkback to all device, diffin, 2019-6-18
            #endif			
		}
    }
}
uint8_t BTAPP_isBTConnected()
{
    if(BTAPP_Status.status == BT_SYSTEM_CONNECTED)
        return 1;
    else
        return 0;
}
/*-----------------------------------------------------------------------------*/
void BTAPP_ResetEEPROMtoDefault( void )
{
    BT_MMI_ActionCommand ( RESET_EEPROM_SETTING, 0);
}

void BTAPP_SetDelayToResetEEPROM(void)
{
	resetAllVolume_timer1ms = 2000;
}


/*-----------------------------------------------------------------------------*/
uint8_t* BT_GetLocalBDAddress( void )
{
    return &BTAPP_Status.BDAddr[0];
}

/*-----------------------------------------------------------------------------*/
uint8_t* Get_BTM_FWVersion( void )      //returned data is 2 bytes, refer to command set
{
    return &BTAPP_Status.FWVer[0];
}

/*-----------------------------------------------------------------------------*/
uint8_t* Get_BTM_UARTVersion( void )    //returned data is 2 bytes, refer to command set
{
    return &BTAPP_Status.UARTVer[0];
}

/*-----------------------------------------------------------------------------*/
void BTAPP_SoundVersion( void )
{
    uint8_t ver[9];
    //MCU version
    ver[0] = MCU_MAJOR_VERSION;
    ver[1] = MCU_MINOR_VERSION;
    ver[2] = MCU_SUB_VERSION;
    //BTM version
    ver[3] = BTAPP_Status.FWVer[0];
    ver[4] = BTAPP_Status.FWVer[1]>>4;
    ver[5] = BTAPP_Status.FWVer[1]&0x0f;
    BT_SoundVersion(ver, 6);
}

/*-----------------------------------------------------------------------------*/
#ifdef _CODE_FOR_APP
void BT_SendEQBytes(void)
{
	if(BTEQ_SendIndex == (BTEQ_MAX_INDEX+1) ||  BTEQ_timer1ms !=0)
	{
		return;
	}
	else if(BTEQ_SendIndex == BTEQ_MAX_INDEX)
	{
		BT_SendBytesAsEQCommandWithRedoChecksum(dspEQ, 90);
		BTEQ_SendIndex++;
	}
	else
	{
		BT_Vendor_EQCmd(dspEQ+5, BTEQ_SendIndex,(BTEQ_SendIndex == (BTEQ_MAX_INDEX - 1)) ? (DSP_EQ_SIZE % (VENDOR_CMD_SIZE-1))+1 : VENDOR_CMD_SIZE);
		BTEQ_SendIndex++;
		if(BTEQ_SendIndex == BTEQ_MAX_INDEX)
		{
			if(multiSpk_eq_route == MULTI_SPK_EQ_SLAVE)
				BTEQ_SendIndex = BTEQ_MAX_INDEX+1;
			else
				BTEQ_timer1ms = DSP_EQ_SEND_INTERVAL;
		}
		else
			BTEQ_timer1ms = DSP_EQ_SEND_INTERVAL;
		
	}
}

void BT_SetMultiSpkEQRoute(uint8_t route)
{
	multiSpk_eq_route = route;
}

bool BT_SendMultiSpkEQBytes(uint8_t* data, uint8_t size)
{
    uint8_t i;
    //uint16_t cmd_len;
	uint8_t *ptr = dspEQ;
	
    if((BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_STANDBY ||
    	BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTING ||
    	BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE ||
    	BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE) ||
	   (multiSpk_eq_route == MULTI_SPK_EQ_MASTER))
        return false;

	*(ptr++) = 0xaa;
	*(ptr++) = 0x00;
	*(ptr++) = 84+2;
	*(ptr++) = 0x30;
	*(ptr++) = 0x13;

    for(i=0; i<size; i++)
    {
        *(ptr++) = *(data++);
    }
	BTEQ_SendIndex = 0;
	BTEQ_timer1ms = DSP_EQ_SEND_INTERVAL;  //send later to avoid master eq corrupt
    return true;
}
#endif

#ifdef _BLE_ADV_CTRL_BY_MCU
/*------------------------------------------------------------*/
void BLE_SaveMasterAddr(uint8_t * address)
{
	uint8_t i;
	for(i=0;i<6;i++)
        BT_MasterAddr[i] = address[i];
	//	BT_MasterAddr[i] = address[5-i];
	//BT_MasterAddr[0] = 255 - BT_MasterAddr[0];		
}

/*------------------------------------------------------------*/
void BLE_SaveFeatureList( uint8_t feature )
{
    BT_bleAdvFeature = feature;
}

/*------------------------------------------------------------*/
void BLE_SaveDeviceName(uint8_t* address)
{
    uint8_t i;
	uint8_t len = *address;
	if(len >=17)
		len = 17;
	
    for(i=0; i<17; i++)
		BT_deviceName[i] = *address++;
}

/*------------------------------------------------------------*/
void BLE_advUpdateGroupStatus(BLE_CSB_CONNECTION_STATE group_status)
{
    if(BT_groupstatus != group_status)
    {
		BT_groupstatus = group_status;
    }
}

/*------------------------------------------------------------*/
void BLE_advUpdateLinkStatus(BLE_CSB_STATE group_event)
{
	if(group_event >LINK_CONNECTED && group_event <LINK_MORE_SLAVES)
		return;
	
    if(BT_groupLinkEvent != group_event)
    {
		BT_groupLinkEvent = group_event;
    }
}

/*------------------------------------------------------------*/
bool BLE_advUpdateBTMState(BLE_BTM_STATE btm_state)
{
    if(BT_btmState != btm_state)
    {
        BT_btmState = btm_state;
        return true;
    }
    return false;
}

/*------------------------------------------------------------*/
void BLE_UpdateAdvType(BLE_ADV_TYPE type)
{
    BLE_advType = type;
}
#define _ADV_USE_CATEGORY_1
/*------------------------------------------------------------*/
void BLE_SetAdvData(void)
{
    uint8_t advertisingData[31];
	uint8_t* ptr = advertisingData;
    
	// AD type : Flag
	*ptr++ = 0x02;
	*ptr++ = 0x01;
	*ptr++ = 0x1A;

	// AD type : Service Data
#ifdef _ADV_USE_CATEGORY_1	
	*ptr++ = 20;       //20 for category 1
#else
	*ptr++ = 15;
#endif
	*ptr++ = 0x16;
	*ptr++ = 0xDA;
	*ptr++ = 0xFE;
#ifdef _ADV_USE_CATEGORY_1
    *ptr++ = (1 << 4) | (BTAPP_Status.batteryLevel & 0x0f);
#else    
	*ptr++ = (0 << 4) | (BTAPP_Status.batteryLevel & 0x0f);
#endif
	*ptr++ = BT_btmState;
	*ptr++ = BT_groupstatus;
	*ptr++ = BT_groupLinkEvent;
    *ptr++ = BTAPP_Status.BDAddr[0];
    *ptr++ = BTAPP_Status.BDAddr[1];
    *ptr++ = BTAPP_Status.BDAddr[2];
    *ptr++ = BTAPP_Status.BDAddr[3];
#ifdef _ADV_USE_CATEGORY_1
	*ptr++ = BTAPP_Status.BDAddr[4];    //new added for category 1
    *ptr++ = BTAPP_Status.BDAddr[5];    //new added for category 1
#endif
	if(BT_groupstatus == SPEAKER_SINGLE)	
	{
#ifdef _ADV_USE_CATEGORY_1
        *ptr++ = 0;
        *ptr++ = 0;
#endif
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
	}
	else if((BT_groupstatus == SPEAKER_nSPK_MASTER) ||
			(BT_groupstatus == SPEAKER_BROADCAST_MASTER_ADDING)||
			(BT_groupstatus == SPEAKER_BROADCAST_MASTER))
	{
        *ptr++ = BTAPP_Status.BDAddr[0];
        *ptr++ = BTAPP_Status.BDAddr[1];
        *ptr++ = BTAPP_Status.BDAddr[2];
        *ptr++ = BTAPP_Status.BDAddr[3];
#ifdef _ADV_USE_CATEGORY_1
        *ptr++ = BTAPP_Status.BDAddr[4];    //new added for category 1
        *ptr++ = BTAPP_Status.BDAddr[5];    //new added for category 1
#endif
	}
	else if((BT_groupstatus == SPEAKER_nSPK_SLAVE) ||
			(BT_groupstatus == SPEAKER_BROADCAST_SLAVE))	
	{
        *ptr++ = BT_MasterAddr[0];
        *ptr++ = BT_MasterAddr[1];
        *ptr++ = BT_MasterAddr[2];
        *ptr++ = BT_MasterAddr[3];
#ifdef _ADV_USE_CATEGORY_1
        *ptr++ = BT_MasterAddr[4];  //new added for category 1
        *ptr++ = BT_MasterAddr[5];  //new added for category 1
#endif
	}
#ifdef _ADV_USE_CATEGORY_1
    *ptr++ = BT_bleAdvFeature;      //new added for category 1
	BLE_SetAdvertisingData(advertisingData, 24); 
#else
    BLE_SetAdvertisingData(advertisingData, 19);    
#endif
}

/*------------------------------------------------------------*/
void BLE_SetScanRspData(void)
{
    uint8_t advertisingData[31];
    uint8_t i;
	uint8_t* ptr = advertisingData;
    
	*ptr++ = 1+BT_deviceName[0];
	*ptr++ = 0x09;
	for(i=0; i< BT_deviceName[0]; i++)
		*ptr++ = BT_deviceName[1+i];
    BLE_SetScanResponseData(advertisingData, 2+BT_deviceName[0]);    
}

/*------------------------------------------------------------*/
void BLE_forceUpdate(void)
{
    BLE_EnableAdvertising(false);
    BLE_SetAdvData();
    BLE_SetScanRspData();
    BLE_SetAdvertisingType(BLE_advType);
    BLE_EnableAdvertising(true);
}
#endif
#ifdef _STORE_VOLUME_TO_EEPROM
static void allVolumeCheckAndStoreToEEPROM( void )
{
    uint8_t volChg = false;
    uint8_t volAll[5];

    if (a2dpVolSave != BTVOL_GetCurrentA2DPVol()) {
        a2dpVolSave = BTVOL_GetCurrentA2DPVol();
        volChg = true;
    }
    if (hfpVolSave != BTVOL_GetHFPVol()) {
        hfpVolSave = BTVOL_GetHFPVol();
        volChg = true;
    }
    if (lineInVolSave != BTVOL_GetLineInVol()) {
        lineInVolSave = BTVOL_GetLineInVol();
        volChg = true;
    }

	if (trebleVolSave != BTVOL_GetTrebleVol()) {
        trebleVolSave = BTVOL_GetTrebleVol();
        volChg = true;
    }

	if (bassVolSave != BTVOL_GetBassVol()) {
        bassVolSave = BTVOL_GetBassVol();
        volChg = true;
    }
	
    if (volChg) {
        volAll[0] = a2dpVolSave;
        volAll[1] = hfpVolSave;
        volAll[2] = lineInVolSave;
		volAll[3] = trebleVolSave;
		volAll[4] = bassVolSave;
        BT_Write_Vendor_EEPROM(0, 5, volAll);
    }
}

void allVolumeResetAndStoreToEEPROM( void )
{
	uint8_t volAll[5];

	a2dpVolSave = 8;
	hfpVolSave = 8;
	lineInVolSave = 8;
	trebleVolSave = 4;
	bassVolSave = 6;

	volAll[0] = a2dpVolSave;
    volAll[1] = hfpVolSave;
    volAll[2] = lineInVolSave;
	volAll[3] = trebleVolSave;
	volAll[4] = bassVolSave;
	BT_Write_Vendor_EEPROM(0, 5, volAll);

	resetAllVolumeFlag = true;
}

#endif
#ifdef _BATTERY_CHARGE_DETECT
static void batteryDetect( void )
{
	static uint8_t current_battery_level = 0;

	uint16_t bat_adc_value;    
    
    bat_adc_value = GetADCValue(ADC_INPUT_POSITIVE_AN2); 
	
    //batteryIsCharging = CHARGE_STATUS_GET();

	if(!DC_PULL_OUT){  //if charging , compensate for the adc value
		if(bat_adc_value > Battery_ADC_Value_Compensation){
			if(bat_adc_value < 885)
				bat_adc_value -= Battery_ADC_Value_Compensation;
		}
	}

	bat_calculate_average_value(bat_adc_value);
    
}

static uint8_t bat_convert_advalue_to_level(uint16_t ad_value)
{
	uint16_t bat_adc_value = ad_value;
	uint8_t bat_level = 0;
	
	if(bat_adc_value <= 710)//2.29v  10
	{
		bat_level = 0;
	}
	#if 0
	else if(bat_adc_value <= 741)//2.39v 10.4
	{
		bat_level = 10;
	}
	else if(bat_adc_value < 763)//2.46v 10.7
	{
		bat_level = 11;
	}
	#else
	else if(bat_adc_value <= 726)//2.34v 10.2
	{
		bat_level = 10;
	}
	else if(bat_adc_value < 754)//2.43v 10.5
	{
		bat_level = 11;
	}
	#endif
	else if(bat_adc_value < 785)//2.53v 11
	{
		bat_level = 20;
	}
	else if(bat_adc_value < 800)//2.58v 11.2
	{
		bat_level = 30;

	} 
	else if(bat_adc_value < 814)//2.625v 11.4
	{
		bat_level = 40;
	}
	else if(bat_adc_value < 828)//2.67v 11.6
	{
		bat_level = 50;
	}	
	else if(bat_adc_value < 841)//2.71v 11.8
	{
		bat_level = 60;
	}
	else if(bat_adc_value < 856)//2.76v 12
	{
		bat_level = 70;

	}
	else if(bat_adc_value < 871)//2.81v 12.2
	{
		bat_level = 80;
	}
	else if(bat_adc_value < 885)//
	{
		bat_level = 90;
			
	}
	else if(bat_adc_value >= 885)//2.856v   12.4
	{
		bat_level = 100;
	}

	return bat_level;
}

static void bat_calculate_average_value (uint16_t ad_value)
{
	uint8_t bat_level = 0;
	uint16_t bat_adc_value_avrg = 0;
	static uint16_t battery_detect_value[12] = {0};
	static uint8_t battery_detect_cnt = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t bat_temp = 0; 
	
	battery_detect_value[battery_detect_cnt++] = ad_value;

	if(battery_detect_cnt >= 12)
	{
		battery_detect_cnt = 0;
		for(j = 0; j < 11; j++)
		{
			for(i = 0; i < (11-j); i++)
			{
				if(battery_detect_value[i] > battery_detect_value[i+1])
				{
					bat_temp = battery_detect_value[i];
					battery_detect_value[i] = battery_detect_value[i+1];
					battery_detect_value[i+1] = bat_temp;

				}

			}

		}
		for(i = 1; i < 11; i++)
		{
			bat_adc_value_avrg += battery_detect_value[i];
			User_Log("battery_detect_value[%d] = %d\n",i,battery_detect_value[i]);
		}

		bat_adc_value_avrg = bat_adc_value_avrg/10;		

		bat_level = bat_convert_advalue_to_level(bat_adc_value_avrg);
		User_Log("Detected battery level = %d\n",bat_level);
		User_Log("CurrentBatteryLevel = %d\n", currentBatteryLevel);

		if(DC_PULL_OUT)
		{
			if(currentBatteryLevel > bat_level){
				if(currentBatteryLevel == 20)
				{
					currentBatteryLevel = 11;
				}
				else if(currentBatteryLevel == 11)
				{
					currentBatteryLevel = 10;
				}
				else
					currentBatteryLevel = currentBatteryLevel - 10;
			}
		}
		else
		{
			if(bat_level > currentBatteryLevel)
			{
				if(currentBatteryLevel == 10)
					currentBatteryLevel = 11;
				else if(currentBatteryLevel == 11)
					currentBatteryLevel = 20;
				else
					currentBatteryLevel = currentBatteryLevel + 10;
			}
		}

		

		if(BTAPP_isBTConnected())//( BTMHFP_GetHFPLinkStatus( BTMHFP_GetDatabaseIndex()) || BTMA2DP_getA2DPLinkStatus(BTMA2DP_getActiveDatabaseIndex()))
		{
			User_Log("BT_UpdateBatteryLevel = %d\n", currentBatteryLevel);
			//BT_UpdateBatteryLevel(currentBatteryLevel);
			if(batteryDisplayDelay_timer1ms == 0)
			{
				if(BT_IsCommandSendTaskIdle())
					BT_UpdateBatteryLevel(currentBatteryLevel);
			}
		}
		
		
		if(!DC_PULL_OUT)
			User_LEDDisplayChargeBatteryLevel(currentBatteryLevel);
		else
			User_LEDDisplayNoChargeBatteryLevel(currentBatteryLevel);
		
	}

}

#endif

uint8_t User_GetCurrentBatteryLevel()
{
	uint16_t bat_adc_value = 0;
	bat_adc_value = GetADCValue(ADC_INPUT_POSITIVE_AN2);
	if(!DC_PULL_OUT){  //if charging , compensate for the adc value
		if(bat_adc_value > Battery_ADC_Value_Compensation)
		{
			if(bat_adc_value < 885)
				bat_adc_value -= Battery_ADC_Value_Compensation;
		}
	}
	User_Log("bat_adc_value = %d\n",bat_adc_value);
	return bat_convert_advalue_to_level(bat_adc_value);

}

void User_LEDDisplayChargeBatteryLevel(uint8_t level)
{
	if(!DC_PULL_OUT)
	{
		Set_LED_Style(LED_2,LED_OFF,500,500);
		Set_LED_Style(LED_1,LED_OFF,500,500);
		Set_LED_Style(LED_0,LED_OFF,500,500);
	

		User_Log("battery level %d\n", level);

		if(level <= 40)//<=40% battery: flash Red
		{
			Set_LED_Style(LED_2,LED_BLINK,1000,1000);

		}
		else if(level <= 70)//41-70% battery: flash Yellow(R & G)
		{
			Set_LED_Style(LED_1,LED_BLINK,1000,1000);
			Set_LED_Style(LED_2,LED_BLINK,1000,1000);

		}
		else if(level < 100)//71-100% battery: flash Green
		{
			Set_LED_Style(LED_1,LED_BLINK,1000,1000);
			

		}
		else
		{
			Set_LED_Style(LED_1,LED_ON,1000,1000);

		}		

	}

}

void User_LEDDisplayNoChargeBatteryLevel(uint8_t level)
{
	if(DC_PULL_OUT)
	{
		Set_LED_Style(LED_2,LED_OFF,500,500);
		Set_LED_Style(LED_1,LED_OFF,500,500);
		Set_LED_Style(LED_0,LED_OFF,500,500);

		if(BTAPP_GetStatus() == BT_STATUS_OFF)
		{
			if(batteryLedOnForDCIn_timer1ms == 0)
			{
				return;
			}
		}

		User_Log("User_LEDDisplayNoChargeBatteryLevel %d\n",level);

		#if 0
		
		if(level == 0)
		{
			if(BTAPP_GetStatus() > BT_STATUS_OFF){
				if(batteryLowAutoPowerOff_100mstimer == 0){
					batteryLowAutoPowerOff_100mstimer = 3000;
					//BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);
					//BT_PlayTone(0xC3);
					User_Log("battery low and will Power OFF\n");
				}
			}
		}
		else 
		#endif
		if(level <= 10)
		{
			if(BTAPP_GetStatus() > BT_STATUS_OFF){
				if(batteryLowAutoPowerOff_100mstimer == 0){
					batteryLowAutoPowerOff_100mstimer = 3000;//300s
					BT_PlayTone(TONE_BatteryLow);
				}

				if(level == 0){
					if(batteryLowAutoPowerOff_100mstimer)
						batteryLowAutoPowerOff_100mstimer = 30; //3s
				}
			}
			Set_LED_Style(LED_2,LED_BLINK,500,500);
			User_Log("battery low\n");
		}
		else if(level <= 40)//<=40% battery: solid Red
		{
			Set_LED_Style(LED_2,LED_ON,500,500);
			if(level <= 11)
			{
				if(!BatteryLowDecreaseVolume3dB_flag){
					BatteryLowDecreaseVolume3dB_flag = true;
					ntp8230g_set_volume(VOLUME,volume_master_step);
				}
			}

		}
		else if(level <= 70)//41-70% battery: solid Yellow(R & G)
		{
			Set_LED_Style(LED_1,LED_ON,500,500);
			Set_LED_Style(LED_2,LED_ON,500,500);

		}
		else if(level <= 100)//71-100% battery: solid Green
		{
			Set_LED_Style(LED_1,LED_ON,500,500);
		}

	}

}


void User_LinkBackToBTDevice( void )
{
	if(BTAPP_Status.pairedRecordNumber != 0)
    {
        BT_LinkBackToLastDevice();
        //BT_LinkbackTaskStart();
    }

}

uint8_t User_GetPairedRecordNumber( void )
{
	return BTAPP_Status.pairedRecordNumber;

}


void User_PowerOffEvent( void )
{
	if((BTAPP_GetStatus() == BT_STATUS_OFF) || (BTAPP_GetStatus() == BT_STATUS_NONE)){
		if(SYS_POWER_STATUS_GET())
		{
			SYS_POWER_OFF();
			User_Log("SYS_POWER_OFF\n");
		}
	}

}

void User_PowerOnEvent( void )
{
	
#if 1
	if(!SYS_POWER_STATUS_GET())
		SYS_POWER_ON();

	if(!DC_PULL_OUT)
	{
		//LED_init();
		led_effect_index = led_none;
		led_effect_index_prev = led_none;
		NF8230dsp_init();
		TM1812_Reset();
		bt_disableUartTransferIntr();

		BTAPP_Init();
	    AnalogAudioDetectInit();
	}
#endif
    BTAPP_TaskReq(BT_REQ_SYSTEM_ON);        //power on when power off
	
}

void User_SwitchBTPower( void )
{
	if((BTAPP_GetStatus() == BT_STATUS_OFF) || (BTAPP_GetStatus() == BT_STATUS_NONE))
    {
		if(
			((User_GetCurrentBatteryLevel() == 0) && (DC_PULL_OUT)) ||
				NTC_isTemperatureOverLimit()
			)
		{
				led_power_on_failed_timer1ms = 100;
				led_power_on_failed_cnt = 6;
				User_Log("Power ON failed\n");
				return;

		}
		User_Log("Power ON\n");
		User_PowerOnEvent();
		
    }
	else
	{

		User_Log("Power OFF status %d\n",BTAPP_GetStatus());
		BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);
	}

}

static void led_power_on_failed_indicate()
{
	if(led_power_on_failed_cnt){
		led_power_on_failed_cnt--;
		led_power_on_failed_timer1ms = 100;
		LED2_TOGGLE();

		if(!led_power_on_failed_cnt)
		{
			if(DC_PULL_OUT)
				if(SYS_POWER_STATUS_GET())
					SYS_POWER_OFF();

		}
	}
}


void BTVOL_DelayChangeVolMode( void )
{
	#if 0
	if(btDelayToChangeVolMode_timer1ms)
	{
		if(!BTMA2DP_GetA2DPCodecStatus())
		{
			User_Log("A2DP Codec inactive\n");
			btDelayToChangeVolMode_timer1ms = 0;
			btDelayToChangeVolModeTimeOutFlag = true;
		}
	}
	#endif
	
	if(btDelayToChangeVolModeTimeOutFlag){		
		btDelayToChangeVolModeTimeOutFlag = false;
		BTVOL_ChangeVolMode(btDelayMode,isSyncToBTM);

	}
		

}

void BTVOL_StartChangeVolMode(VOLUME_MODE mode, bool syncToBTM )
{
	btDelayToChangeVolMode_timer1ms = 800;
	btDelayMode = mode;
	isSyncToBTM = syncToBTM;
}

uint8_t User_getLinkedDeviceNumber(void)
{
	return BTAPP_Status.linkedDeviceNumber;
}



