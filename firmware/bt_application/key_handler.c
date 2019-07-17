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

#define KEY_HANDLER_C
 #include "key_handler.h"
//#include "user_key.h"
#include "bt_app.h"
#include "led.h"
//#include "yda174dsp_handler.h"

#include "bt_volume.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "bt_multi_spk.h"
#include "key.h"
#include "nf8230dsp_handler.h"
#include "bt_app.h"


extern DRV_HANDLE UART_Handle;
volatile uint16_t DFU_timer;
bool Broadcast_disconnect_flag = false;

void KEY_Handler ( uint8_t key, uint8_t event )
{
    switch ( key )
    {
		case KEY_ID_POWER:
			if(event == KEY_EVENT_PRIMARY )//Toggle between Volume, Bass, and Treble control
			{
				User_Log("Power key sp\n");
				if(BTAPP_GetStatus() == BT_STATUS_READY)
				{
					User_VolumeModeSwitch();					
				}

			}
			else if(event == KEY_EVENT_SECONDARY)//Power ON/OFF
			{
				
				User_Log("Power key Lp\n");
				User_SwitchBTPower();

			}
			break;

		case KEY_ID_BT:
			if(event == KEY_EVENT_PRIMARY )//Toggle BT sources
			{
				User_Log("BT key sp\n");
				if(BTAPP_isBTConnected())
				//if( BTMHFP_GetHFPLinkStatus( BTMHFP_GetDatabaseIndex()) || BTMA2DP_getA2DPLinkStatus(BTMA2DP_getActiveDatabaseIndex()))
				{
					if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_STANDBY)
						;//BTMHFP_ToggleActiveDevice();
				}
				else
				{
					if(User_GetPairedRecordNumber())
					{
						if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE)
							BTMSPK_CancelGroup();
						
						User_LinkBackToBTDevice();
					}

				}

			}
			else if(event == KEY_EVENT_SECONDARY)//Enter BT pairing mode
			{
				User_Log("BT key Lp\n");
				if( BTMHFP_GetCallStatus() == BT_CALL_IDLE 
                    && BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE
                    && BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE )
                {
					if(
						(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING) ||
						(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTING)
					)
					{
						BTMSPK_CancelGroupCreation();

					}
					BTAPP_EnterBTPairingMode();
					User_Log("BT pairing\n");
                }
				else if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE )
				{
					BTMSPK_CancelGroup();
					BTAPP_EnterBTPairingMode();

				}

			}
			else if(event == KEY_EVENT_LONG_TIME_HOLD)//Clear pairing list
            {
				User_Log("BT key VLp\n");
				if(BTAPP_GetStatus() >= BT_STATUS_ON){
					#ifdef _STORE_VOLUME_TO_EEPROM
					allVolumeResetAndStoreToEEPROM();
					#endif
					BTAPP_SetDelayToResetEEPROM();	                					
					User_ClearPDL();
				}
            }
			break;
		
        case KEY_ID_PLAY_PAUSE:
			if(BTM_LINE_IN_IsPlugged())
				break;
            if(event == KEY_EVENT_PRIMARY )
            {
				User_Log("PP key sp\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
					User_Log("BTMHFP_GetCallStatus : %d\n",BTMHFP_GetCallStatus());
                    switch(BTMHFP_GetCallStatus())
                    {
                        case BT_CALL_IDLE:
                            BTMA2DP_PlayPauseToggle();                  //toggle play/pause when no calling event   
                            break;
                        case BT_VOICE_DIAL:
                            //BTMHFP_CancelVoiceDial();                   //cancel voice dialing when voice dialing
                            break;
                        case BT_CALL_INCOMMING:
                            BTMHFP_AcceptCall();                        //accept call when call is incoming
                            break;
                        case BT_CALL_OUTGOING:
                        case BT_CALLING:
                            BTMHFP_EndCall();                           //end call when calling
                            break;
                        case BT_CALLING_WAITING:
                            BTMHFP_AcceptWaitingCall();                 //accept the waiting call, but will also end current active all.
                            break;
                        case BT_CALLING_HOLD:
                            BTMHFP_SwitchToHoldCall();                  //switch between the active call and hold call
                            break;
                        default:
                            break;
                    }
                }
            }
            else if(event == KEY_EVENT_DOUBLE_CLICK)
            {
				User_Log("PP key DP\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
                    if( BTMHFP_GetCallStatus() == BT_CALL_IDLE 
                            && BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE
                            && BTMSPK_GetMSPKStatus() != BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE )
                    {
                        BTMHFP_VoiceDial();
                    }
                    else if(( BTMHFP_GetCallStatus() == BT_CALLING) || ( BTMHFP_GetCallStatus() == BT_VOICE_DIAL))
                    {
						//BTMHFP_CancelVoiceDial();                   //cancel voice dialing when voice dialing
						BTMHFP_EndCall();
                       
                    }
                }
            }
            else if(event == KEY_EVENT_SECONDARY)
            {
				User_Log("PP key Lp\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY || BTAPP_GetStatus() == BT_STATUS_ON)
                {
                    //BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);
					switch(BTMHFP_GetCallStatus())
					{
						case BT_VOICE_DIAL:
						    BTMHFP_CancelVoiceDial();                   //cancel voice dialing when voice dialing
						    break;
						case BT_CALL_INCOMMING:
						    BTMHFP_RejectCall();                        //accept call when call is incoming
						    break;
						case BT_CALL_OUTGOING:
						case BT_CALLING:
						    BTMHFP_EndCall();                           //end call when calling
						    break;
						case BT_CALLING_WAITING:
						    BTMHFP_RejectWaitingCallOrReleaseHoldCall();    //reject the waiting incoming call
						    break;
						case BT_CALLING_HOLD:
						    BTMHFP_RejectWaitingCallOrReleaseHoldCall();    //end the hold call
						    break;
						default:
						    break;
					}
                }
                
            }
            else if(event == KEY_EVENT_LONG_TIME_HOLD)
            {
                //BTAPP_ResetEEPROMtoDefault();
            }
            break;
        case KEY_ID_VOL_UP:
			
            if(event == KEY_EVENT_PRIMARY)
            {
				User_Log("V+ key sp\n");
	
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
					if(user_volume_mode == VOLUME)
						BTVOL_VolUp();
					else if(user_volume_mode == TREBLE)
						User_VolumeUp();//BTVOL_VolUp();
					else if(user_volume_mode == BASS)
						User_VolumeUp();//BTVOL_VolUp();
                }
                else
                    Nop();
                    //yda174_vol_up();
            }
            else if(event == KEY_EVENT_SECONDARY)
            {
				User_Log("V+ key Lp\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                    BTMA2DP_PlayNext();//BTAPP_PlayNextSong();
            }
            else if(event == KEY_EVENT_LONG_TIME_HOLD)
            {
				#if 0
                //make BM64 enter FLASH TEST mode, for EEPROM data upgrading
                SET_SWITCH_TO_UART();
                DRV_USART_Close(UART_Handle);
                DRV_USART0_Deinitialize();
                UART_RX_SetInput();
                UART_UR_TX_SetInput();
                BM6X_RESET_SetLow();
                BT_EAN_SetOutput();
                BT_EAN_SetLow();
                BT_P2_0_SetOutput();
                BT_P2_0_SetLow();
                
                Set_LED_Style(LED_0, LED_ON, 250, 250);
                Set_LED_Style(LED_1, LED_ON, 250, 250);
                Set_LED_Style(LED_2, LED_ON, 250, 250);
                DFU_timer = 250;
                while(DFU_timer);
                Set_LED_Style(LED_2, LED_BLINK, 100, 100);
                BM6X_RESET_SetHigh();
                
                while(1);
				#endif
            }
            break;
        case KEY_ID_VOL_DN:
            if(event == KEY_EVENT_PRIMARY)
            {
				User_Log("V- key sp\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
                    if(user_volume_mode == VOLUME)
						BTVOL_VolDown();
					else if(user_volume_mode == TREBLE)
						User_VolumeDn();
					else if(user_volume_mode == BASS)
						User_VolumeDn();
                }
                else
                    ;//yda174_vol_down();
            }
            else if(event == KEY_EVENT_SECONDARY)
            {
				User_Log("V- key lp\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                    BTMA2DP_PlayPrevious();//BTAPP_PlayPreviousSong();
            }
            else if(event == KEY_EVENT_LONG_TIME_HOLD)
            {
                //make BM64 enter FLASH WRITE mode, for flash firmware upgrading
                #if 0
                SET_SWITCH_TO_UART();
                DRV_USART_Close(UART_Handle);
                DRV_USART0_Deinitialize();
                UART_RX_SetInput();
                UART_UR_TX_SetInput();
                BM6X_RESET_SetLow();
                BT_EAN_SetOutput();
                BT_EAN_SetHigh();
                BT_P2_0_SetOutput();
                BT_P2_0_SetLow();
                
                Set_LED_Style(LED_0, LED_ON, 250, 250);
                Set_LED_Style(LED_1, LED_ON, 250, 250);
                Set_LED_Style(LED_2, LED_ON, 250, 250);
                DFU_timer = 250;
                while(DFU_timer);
                Set_LED_Style(LED_0, LED_BLINK, 100, 100);
                Set_LED_Style(LED_1, LED_BLINK, 100, 100);
                BM6X_RESET_SetHigh();
                
                while(1);
				#endif
            }
            break;

        case KEY_ID_BROADCAST:       //NSPK button
            if(event == KEY_EVENT_SECONDARY)
            {
				User_Log("BROADCAST key lp\n");
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
					User_Log("BTMSPK_GetMSPKStatus %d\n",BTMSPK_GetMSPKStatus());
                    switch( BTMSPK_GetMSPKStatus() )
                    {
                        case BT_CSB_STATUS_STANDBY:
                            //BTMSPK_CreatStereoMode();
                            if(BTMA2DP_getA2DPLinkStatus(BTMA2DP_getActiveDatabaseIndex()))
                            	BTMSPK_TriggerConcertModeMaster();
                            break;
                        case BT_CSB_STATUS_CONNECTING:
                            //BTMSPK_CancelGroupCreation();
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_NSPK_MASTER:
                        case BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE:
                            //BTMSPK_CancelGroup();
                            break;
                        case BT_CSB_STATUS_NSPK_MASTER_CONNECTING:
                            //BTMSPK_CancelGroupCreation();
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER:
							BTMSPK_AddMoreSpeaker();
							break;
                        case BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE:
                            BTMSPK_CancelGroup();							
                            break;
                        case BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING:
                            //BTMSPK_CancelGroupCreation();
                            break;
                    }
                }
            }
            else if( event == KEY_EVENT_PRIMARY )
            {
				User_Log("BROADCAST key sp\n");
                switch( BTMSPK_GetMSPKStatus() )
                    {
                        case BT_CSB_STATUS_STANDBY:
							#ifdef RECONNECT_TO_PDL
							BT_LinkbackTaskStop();//linkback to all device, diffin, 2019-6-18
							#endif
                            //BTMSPK_CreatStereoMode();
                            Broadcast_disconnect_flag = false;
                            if(BTAPP_isBTConnected()){
								BT_DisconnectAllProfile();
								Broadcast_disconnect_flag = true;
                            }
							else
                            	BTMSPK_TriggerConcertModeSlave();
							
                            break;
                        case BT_CSB_STATUS_CONNECTING:
                            //BTMSPK_CancelGroupCreation();
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_NSPK_MASTER:
                        case BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE:
                            //BTMSPK_CancelGroup();
                            break;
                        case BT_CSB_STATUS_NSPK_MASTER_CONNECTING:
                            //BTMSPK_CancelGroupCreation();
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER:
							//BTMSPK_AddMoreSpeaker();
							break;
                        case BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE:							
								BTMSPK_CancelGroup();							
                            break;
                        case BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING:
                            //BTMSPK_CancelGroupCreation();
                            break;
                    }
            }
            else if(event == KEY_EVENT_DOUBLE_CLICK)
            {
				#if 0
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
                    switch (BTMSPK_GetMSPKStatus()) 
                    {
                        case BT_CSB_STATUS_STANDBY:
                            BTMSPK_CreatConcertMode();
                            break;
                        case BT_CSB_STATUS_CONNECTING:
                            //ignore it
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_NSPK_MASTER:
                        case BT_CSB_STATUS_CONNECTED_AS_NSPK_SLAVE:
                        case BT_CSB_STATUS_NSPK_MASTER_CONNECTING:
                            //ignore it
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER:
                            BTMSPK_AddMoreSpeaker();
                            break;
                        case BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE:
                            //ignore it
                            break;
                        case BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING:
                            //ignore it
                            break;
                    }
                }
				#endif
            }
            else if(event == KEY_EVENT_LONG_TIME_HOLD)
            {
                if(BTAPP_GetStatus() == BT_STATUS_READY)
                {
                    BTAPP_SoundVersion();
                }
            }
            break;
            
    }
}

void KeyHandlers_Timer1MS_event()
{
    if(DFU_timer)
        --DFU_timer;
}
