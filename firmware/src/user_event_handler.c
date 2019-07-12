
#include <xc.h>
#include "bt_hardware_profile.h"

#include "user_event_handler.h"
#include "user_key.h"
#include "bt_app.h"
#include "led.h"
#include "yda174dsp_handler.h"

#include "bt_volume.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "bt_multi_spk.h"
#include "app.h"
#include "tm1812_app.h"

volatile uint16_t event_timer = 0;
static uint8_t power_on_flag = 0;

uint8_t keypressrelease_cnt;

SYS_DATA sysdata;



void EventHandlers_Timer1MS_event()
{
    if(event_timer)
        --event_timer;
}


void User_EventHandlerInit()
{
	sysdata.state = SYS_STATE_STANDBY;
	sysdata.user_sys_mode = USER_MODE_STANDBY;
	power_on_flag = 0;
	keypressrelease_cnt = 0;
}

void User_EventTask()
{
	if(event_timer)
        return;
	
    event_timer = EVENT_TASK_PERIOD;

	power_on_led_indicate();
	return;
	if(power_on_flag)
	{
		TM1812_Test();
	}

	if(keypressrelease_cnt){
		keypressrelease_cnt--;
		if(keypressrelease_cnt == 0)
			User_PlayRelease();
	}

}


void User_EventHandlerTask()
{
	User_EventTask();
	
	switch(sysdata.state)
	{
		case SYS_STATE_ON:
		{
			if(sysdata.user_sys_mode == USER_MODE_BT)
			{

			}
			else if(sysdata.user_sys_mode == USER_MODE_AUX)
			{

			}
			else if(sysdata.user_sys_mode == USER_MODE_MIC)
			{

			}

			//User_OnTask();
			break;
		}
		case SYS_STATE_TO_ON:
		{
			//User_ToOnTask();
			break;
		}
		case SYS_STATE_TO_STBY:
		{
			//User_ToSTBYTask();
			break;
		}
		case SYS_STATE_STANDBY:
		{
			//User_StandbyTask();
			break;
		}
		default:
		{
			break;
		}
	
	}


	
}

/*---------------------------------------------------------------*/
void Set_KeyPlayReleaseCnt(uint8_t value)
{
	keypressrelease_cnt = value;
}

bool Get_KeyPlayReleaseFlag()
{
	bool flag = false;
	
	if(keypressrelease_cnt)
		flag = true;

		return flag;
}

void Clr_KeyPlayReleaseFlag()
{
	keypressrelease_cnt = 0;
}



void User_SwitchPower()
{
	if(sysdata.state >= SYS_STATE_TO_ON){
		power_on_flag = 0;
		sysdata.state = SYS_STATE_TO_STBY;
	}
	else{
		power_on_flag = 1;
		sysdata.state = SYS_STATE_TO_ON;
	}
}

void User_PlayRelease()
{
	User_Log("User play release\n");
	if(BTAPP_GetStatus() == BT_STATUS_OFF)
    {
        BTAPP_TaskReq(BT_REQ_SYSTEM_ON);        //power on when power off
    }
    else if(BTAPP_GetStatus() == BT_STATUS_READY)
    {
        switch(BTMHFP_GetCallStatus())
        {
            case BT_CALL_IDLE:
                BTMA2DP_PlayPauseToggle();                  //toggle play/pause when no calling event   
                break;
            case BT_VOICE_DIAL:
                BTMHFP_CancelVoiceDial();                   //cancel voice dialing when voice dialing
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

void User_PlayDoublePress()
{
	User_Log("User paly DP\n");
	key_hold_cnt = 0;

}

void User_PlusMinus(uint8_t dir)
{

}

void User_PlayHold()
{

}

void User_PlusMinusHold(uint8_t dir)
{

}

void User_PowerPress()
{

}

void User_BroadcastPress()
{

}

void User_BroadcastHold()
{

}

void User_BT()
{

}

void User_BTHold()
{

}

void User_BTVLongPress()
{

}









