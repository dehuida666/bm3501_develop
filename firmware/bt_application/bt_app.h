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
#ifndef BT_APP_H
#define BT_APP_H

#include <stdbool.h>
#include <stdint.h>
#include "bt_general_event_define.h"
#include "bt_volume.h"


#define MCU_MAJOR_VERSION 1
#define MCU_MINOR_VERSION 3
#define MCU_SUB_VERSION   6

//r1.25
// 1. for VOL+ and VOL- buttons which have repeat events, modify repeat rate from 100ms to 200ms,
// 2. for stereo mode or concert mode slave, if it gets events  showing it is HFP mode or line in mode, when doing volume up or volume down, MCU sends C02 00 30/31 instead of sending C23 command.
// 3. added process for 1B 06 event in all 3 modes, A2DP mode, HFP mode and line-in mode. Previous code only process the event in A2DP mode
// 4. if current status is pairing statutes, when user long press PAIRING button, don?t send pairing MMI_ACTION command, don?t reset LED status

//r1.26
// 1. fixed LED indication issue for stereo mode master/slave

//r1.30
/* 1. bt_app, bt_volume, bt_multi_spk, bt_command_send: added volume sync from stereo mode master to stereo mode slave by customized command 
 * 2. defined macro _SUPPORT_SIMPLE_33_EVENT for 114 ROM. With it defined, bt_app will not get master address from 33 event, master will send master address over 2A command, and slave will get master address over 34 event
 * 3. bt_multi_spk.c: when connected as a slave, if size < 9, will not save master address, because this will cause illegel RAM access.
 * 4. bt_command_send.c: BT_SendAppBTMStatus() is changed to use channel index, not just 0.
 * 5. bt_command_decode.c: two places of calling BT_SendSPPAllData() are modified to use chanel index, not database index
 * 6. bt_data.c: checkCommandWaitingForACK(), add judging NumCommandForACK first, NumCommandForACK must be >= 1, then start to judge cmd_id.
 * 7. add LED4 to indicate LE advertising connectable or non-connectable, add LED5 to indicate LE Transparent service connection status, add LED6 to indicate SPP connection status, add LED7 to indicate iAP connection status
 * 8. bt_data.c: added two subroutines: bool BTMDATA_isBLETransparentServiceConnected( void ), bool BTMDATA_isBLEANCSServiceConnected( void )
 * 9. bt_command_decode.c: two place to call BTMDATA_isBLEConnected() are replaced with BTMDATA_isBLETransparentServiceConnected(), this is more reasonable.
 * 10. try to support MBA BLE app and bt_chat SPP/iAP app at one code
 * 
 * 11. return ACK to APP for 0xCC command from APP
 * 12. decode MMI_ACTION for individual slave, shend by Vendor_Cmd, and process corresponding Vendor_Event at slave side.
 * 13. added static bool filterEventForwardToDemoApp( uint8_t event_id ) for filtering event to be forwarding to demo app
 * 14. added handler for 0x30 event, this means unexpeted reset event, when receving this, call BTAPP_Init() to initialize BTM system.
 */

//r1.31
/*
 * 1. modified play/pause short press and double click function, added operation for waiting call and hold call.
 */

//r1.32
/*
 * 1. changed beacon data first byte upper 4bits from 0 to 1, this means it is category 1 format.
 */

//r1.32b
/*
 * 1. added macro _NO_EVENT_WHEN_LINE_IN_OUT to include code invoking function BTM_LINE_IN_EventHandler(), so removed this from this build.
 */

//r1.33
/*
 * 1. advertising content missed support feature byte, added this byte. (bt_app.c)
 * 2. code optimization for non-APP build. (bt_app.c)
 * 3. added volume sync from master to slave in concert mode when user press volume up/down buttons.(bt_volume.c)
 * 4. fixed a critial error: if command_send_task used the space during waiting for command ACK to send ACK_TO_EVENT, 
 * if command ACK status is error, or there is no ACK returned for command, there is chance that UART communication will be frozen.
 * solution is is clear BT_SendingAck.AckDataSendingFlag flag before calling UART_TransferFirstByte() to resend. (bt_command_send.c)
 */

//r1.34
/*
 * 1. modify DFU_timer(key_handler.c) definition to volatile type, because it might be optimized by compiler, which causes UART upgrade doesn't work.
 */

// @ bluetooth task state define
typedef enum {
    BT_STATE_INITIALIZE_START = 0,
    BT_STATE_INIT_MFB_HIGH,
    BT_STATE_INIT_RESET_HIGH,
    BT_STATE_INIT_RESET_HIGH_WAIT,
    BT_STATE_INIT_COMMAND_START,    //send first command
	BT_STATE_INIT_READ_DEVICE_NAME_WAIT,
#ifdef _STORE_VOLUME_TO_EEPROM
    BT_STATE_INIT_READ_EEPROM,//6
    BT_STATE_INIT_READ_EEPROM_WAIT,
#endif
    BT_STATE_INIT_BLE_ADV_START,
	BT_STATE_INIT_BLE_ADV_WAIT,
    BT_STATE_POWER_ON_START,        //wait 500ms. after 500ms, send SYSTEM_ON and CSB_PAGE command
    BT_STATE_POWER_DSP_WAIT,        //wait DSP is ready
    BT_STATE_POWER_ON,    //12      //system on finished, wait for BT or AUX instruction
    BT_STATE_READ_PAIR_RECORD,
    BT_STATE_VOL_SYNC,
    BT_STATE_PLAY_POWER_ON_TONE,
    BT_STATE_READ_PAIR_RECORD_WAIT,
    BT_STATE_READ_LINKED_MODE,
    BT_STATE_READ_LINKED_MODE_WAIT,
    BT_STATE_LINKBACK_START,
    BT_STATE_BT_RUNNING,//20
    BT_STATE_POWER_OFF_START,
    BT_STATE_POWER_OFF_WAIT_NSPK_EVENT,
    BT_STATE_POWER_OFF_START_NSPK,
    BT_STATE_POWER_OFF_WAIT,
    BT_STATE_POWER_OFF_WAIT2,//add by zx
    BT_STATE_POWER_OFF   //26
} BTAPP_TASK_STATE;

void BTAPP_Init( void );
void BTAPP_Task( void );
void BTAPP_Timer1MS_event( void );
enum {
    BT_SINGLE_PKT = 0,
    BT_FRAGMENTED_START_PKT,  
    BT_FRAGMENTED_CONT_PKT, 
    BT_FRAGMENTED_END_PKT,
};
// @ request define
enum {
    BT_REQ_NONE = 0,
    BT_REQ_SYSTEM_ON,   //BM64 power on request(outside application must control RESET and MFB timing, then call this request)
    BT_REQ_SYSTEM_OFF,  //BM64 power off request(after call this, outside application must control RESET and MFB)
};
void BTAPP_TaskReq(uint8_t request);


typedef enum {
    BT_SYSTEM_INIT,         //init
    BT_SYSTEM_POWER_OFF,    //event
    BT_SYSTEM_POWER_ON,     //event
    BT_SYSTEM_STANDBY,      //event
    BT_SYSTEM_CONNECTED,    //event
    BT_SYSTEM_PAIRING,      //event
} BT_SYSTEM_STATUS;
typedef struct {
    BT_SYSTEM_STATUS status;
    uint8_t pairedRecordNumber;      //paired record
    uint8_t lastLinkedMode;              //linked mode
    uint8_t lastMSPKSlaveCounter;       //slave counter for linked mode
    uint8_t batteryStatus;
    uint8_t batteryLevel;
    uint8_t BDAddr[6];          //device address
    uint8_t FWVer[2];           //BTM firmware version
    uint8_t UARTVer[2];         //UART version
    uint8_t linkedDeviceNumber;  //add by zx
    //bool groupLinkingBack;
} BTAPP_STATUS;
//linkback to all device, diffin, 2019-6-18 >>
#ifdef RECONNECT_TO_PDL

typedef struct {
    uint8_t linkPriority;
    uint8_t linkBdAddress[6];
} BTAPP_PAIR_RECORD;
#endif
//linkback to all device, diffin, 2019-6-18 <<

typedef struct {
    union {
        uint8_t value;
        struct{
            uint8_t PORT_1_0    : 1;
            uint8_t PORT_1_1    : 1;
            uint8_t PORT_1_2    : 1;
            uint8_t PORT_1_3    : 1;
            uint8_t PORT_1_4    : 1;
            uint8_t PORT_1_5    : 1;
            uint8_t PORT_1_6    : 1;
            uint8_t PORT_1_7    : 1;
        } bits;
    } PORT_1;
    union {
        uint8_t value;
        struct{
            uint8_t PORT_2_0    : 1;
            uint8_t PORT_2_1    : 1;
            uint8_t PORT_2_2    : 1;
            uint8_t PORT_2_3    : 1;
            uint8_t PORT_2_4    : 1;
            uint8_t PORT_2_5    : 1;
            uint8_t PORT_2_6    : 1;
            uint8_t PORT_2_7    : 1;
        } bits;
    } PORT_2;
    union {
        uint8_t value;
        struct{
            uint8_t PORT_3_0    : 1;
            uint8_t PORT_3_1    : 1;
            uint8_t PORT_3_2    : 1;
            uint8_t PORT_3_3    : 1;
            uint8_t PORT_3_4    : 1;
            uint8_t PORT_3_5    : 1;
            uint8_t PORT_3_6    : 1;
            uint8_t PORT_3_7    : 1;
        } bits;
    } PORT_3;
    union {
        uint8_t value;
        struct{
            uint8_t PORT_4_0    : 1;
            uint8_t PORT_4_1    : 1;
            uint8_t PORT_4_2    : 1;
            uint8_t PORT_4_3    : 1;
            uint8_t PORT_4_4    : 1;
            uint8_t PORT_4_5    : 1;
            uint8_t PORT_4_6    : 1;
            uint8_t PORT_4_7    : 1;
        } bits;
    } PORT_4;
} BT_PORTS;


enum {
    BT_STATUS_NONE,
    BT_STATUS_OFF,
    BT_STATUS_ON,
    BT_STATUS_READY
};

extern uint16_t User_sys_nosignal_time;
#define User_NOSIGNAL_TIME_MAX  (30*60*10)

uint8_t currentBatteryLevel;

bool BT_button_manual_enter_pairing_flag;
uint8_t BT_button_manual_reconnect_to_X;

uint8_t BTAPP_GetStatus(void);

void BTAPP_EventHandler(BT_APP_EVENTS event, uint8_t* paras, uint16_t size );
void BTAPP_EnterBTPairingMode( void );
uint8_t BTAPP_isBTConnected();
void BTAPP_ResetEEPROMtoDefault( void );
void BTAPP_SetDelayToResetEEPROM(void);

uint8_t* BT_GetLocalBDAddress( void );
uint8_t* Get_BTM_FWVersion( void );
uint8_t* Get_BTM_UARTVersion( void );
void BTAPP_SoundVersion( void );

void User_LinkBackToBTDevice( void );
void User_LEDDisplayChargeBatteryLevel(uint8_t level);
void User_LEDDisplayNoChargeBatteryLevel(uint8_t level);

uint8_t User_GetCurrentBatteryLevel();

void User_PowerOffEvent( void );
void User_PowerOnEvent( void );
void User_SwitchBTPower( void );
uint8_t User_GetPairedRecordNumber( void );

void BTVOL_StartChangeVolMode(VOLUME_MODE mode, bool syncToBTM);
void BTVOL_DelayChangeVolMode( void );
uint8_t User_getLinkedDeviceNumber(void);


#ifdef _CODE_FOR_APP
void BT_SendEQBytes(void);
bool BT_SendMultiSpkEQBytes(uint8_t* data, uint8_t size);
void BT_SetMultiSpkEQRoute(uint8_t route);
#endif

#ifdef _STORE_VOLUME_TO_EEPROM
void allVolumeResetAndStoreToEEPROM( void );
#endif


//void BT_SaveDeviceName(uint8_t* address);

//linkback to all device, diffin, 2019-6-18 >>
#ifdef RECONNECT_TO_PDL

void BT_LinkbackTaskStart ( void );
void BT_LinkbackTaskNext ( void );
void BT_LinkbackTaskStop ( void );
bool BT_LinkbackTaskRunning ( void );
void BT_LinkbackTask( void );
void BT_LinkbackTaskNextXStart ( uint8_t X_device);

#endif
//linkback to all device, diffin, 2019-6-18 <<

//DC detect, 2019-6-29 >>
void DC_DetectTask(void);
void DC_DetectTaskStart(void);


//DC detect, 2019-6-29 <<



#ifdef _BLE_ADV_CTRL_BY_MCU
typedef enum {
    SPEAKER_SINGLE = 0,
    SPEAKER_nSPK_MASTER = 1,
    SPEAKER_nSPK_SLAVE = 4,
    SPEAKER_BROADCAST_MASTER_ADDING = 5,
    SPEAKER_BROADCAST_SLAVE = 6,
	SPEAKER_BROADCAST_MASTER = 7,
	SPEAKER_CONNECTING = 0xf,   //NC in v1.16 app
} BLE_CSB_CONNECTION_STATE;

typedef enum {
    LINK_STBY = 0,
    LINK_BUSY = 1,
    LINK_CONNECTING = 2,
    LINK_CONNECTED = 3,
	LINK_MORE_SLAVES = 9,
} BLE_CSB_STATE;

typedef enum {
    CONNECTABLE_UNDIRECT_ADV = 0,           //connectable
    CONNECTABLE_RESERVED = 1,
    SCANNABLE_UNDIRECT_ADV = 2,
    NON_CONNECTABLE_UNDIRECT_ADV = 3,       //non-connectable
} BLE_ADV_TYPE;

typedef enum
{
	BLE_BTM_OFF=0,
	BLE_BTM_PAIR,
	BLE_BTM_STBY,
	BLE_BTM_CONN_HF,
	BLE_BTM_CONN_A2DP,
	BLE_BTM_CONN_SPP,
	BLE_BTM_CONN_ALL
} BLE_BTM_STATE;

void BLE_SaveMasterAddr(uint8_t * address);
void BLE_SaveFeatureList( uint8_t feature );
void BLE_SaveDeviceName(uint8_t* address);
void BLE_UpdateAdvType(BLE_ADV_TYPE type);
void BLE_advUpdateGroupStatus(BLE_CSB_CONNECTION_STATE group_status);
void BLE_advUpdateLinkStatus(BLE_CSB_STATE group_event);
bool BLE_advUpdateBTMState(BLE_BTM_STATE btm_state);
void BLE_SetAdvData(void);
void BLE_SetScanRspData(void);
void BLE_forceUpdate(void);
#endif

#endif
