#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "tm1812_app.h"
#include "timer.h"
#include "led.h"
#include "bt_hfp.h"
#include "bt_hardware_profile.h"
#include "bt_app.h"
#include "bt_multi_spk.h"



uint16_t ledFlag = 0;			//led µÆ±êÖ¾
uint8_t colorBuffer[TM1812_LED_INDEX_MAX] = {0};
uint8_t ledIndex = 0;

uint16_t ledvolume_timer1ms = 0;
bool ledvolumeTimeOutFlag = false;

uint8_t ledvolume0_timer1ms = 0;
bool ledvolume0TimeOutFlag = false;
uint8_t ledvolume0_blink_cnt = 0;

uint8_t ledvolume16_timer1ms = 0;
bool ledvolume16TimeOutFlag = false;
uint8_t ledvolume16_blink_cnt = 0;


uint16_t ledClrPdl_timer1ms = 0;
bool ledClrPdlTimeOutFlag = false;

LEVEL_LEDS_PRIORITY led_priority_ctl_bit;

/*-----------------------------------------------------------------
*/

static void hfp_open_led_Indicate(bool on_off);


/*-----------------------------------------------------------------
*/
static display_volume0_start()
{
	ledvolume0_timer1ms = 200;
	ledvolume0_blink_cnt = 6;
	ledvolume0TimeOutFlag = false;

}

static display_volume16_start()
{
	ledvolume16_timer1ms = 200;
	ledvolume16_blink_cnt = 6;
	ledvolume16TimeOutFlag = false;

}


/*-----------------------------------------------------------------
*/

void TM1812_timer_1ms()
{
	if(ledvolume_timer1ms)
    {
       --ledvolume_timer1ms;
       if(ledvolume_timer1ms == 0)
       {
           ledvolumeTimeOutFlag = true;
       }
    }

	if(ledvolume0_timer1ms)
    {
       --ledvolume0_timer1ms;
       if(ledvolume0_timer1ms == 0)
       {
           ledvolume0TimeOutFlag = true;
       }
    }

	if(ledvolume16_timer1ms)
    {
       --ledvolume16_timer1ms;
       if(ledvolume16_timer1ms == 0)
       {
           ledvolume16TimeOutFlag = true;
       }
    }

	if(ledClrPdl_timer1ms)
	{
		ledClrPdl_timer1ms--;
		if(ledClrPdl_timer1ms == 0)
		{
			ledClrPdlTimeOutFlag = true;
		}
	}

}

/*********************************************************************************
  * @brief  : init tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/

void TM1812_LEDInit(void)
{
	TM1812_Reset();
	led_priority_ctl_bit = LEVEL_LEDS_PRIORITY_NONE;

	ledvolume_timer1ms = 0;
	ledvolumeTimeOutFlag = false;

	ledvolume0_timer1ms = 0;
	ledvolume0TimeOutFlag = false;
	ledvolume0_blink_cnt = 0;

	ledvolume16_timer1ms = 0;
	ledvolume16TimeOutFlag = false;
	ledvolume16_blink_cnt = 0;

	ledClrPdl_timer1ms = 0;
	ledClrPdlTimeOutFlag = false;

}



/*********************************************************************************
  * @brief  : power on tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void TM1812_PowerOn(void)
{
    SET_LED_DIN_HIGH();
}


/*********************************************************************************
  * @brief  : power off tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void TM1812_PowerOff(void)
{
    SET_LED_DIN_LOW(); 
}


/*********************************************************************************
  * @brief  : write one byte to tm1812
  * @param  : data:the data write to tm1812
  * @retval : None
  * @intruduction : 
*************************************************************************************/
int TM1812_WriteOneByte(uint8_t Data) 
{ 
	int BitIndex = 0; 

	for(BitIndex = 8*sizeof(Data)-1; BitIndex >= 0; BitIndex--) 
	{ 
		if(Data & (0x01 << BitIndex)) 
		{ 
			DIN_BIT_1(); 
		} 
		else 
		{ 
			DIN_BIT_0(); 
		} 	
	}
}


/*********************************************************************************
  * @brief  : reset tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
int TM1812_Reset(void) 
{ 
	SET_LED_DIN_LOW();  
	delay_us(24); 
	return 0; 
}




/* ** @Brief: Transmit LED data 
** @Parameter: 1) Din, data input port 2) Data, transmit data 3) Size, data size 
** @Return: 1) 0, transmit data successful 2) -1, data is null or data size illegal 
**/ 
int TM1812_TransmitData(const uint8_t* Data, uint8_t Size) 
{ 	
	int ByteIndex = 0; 
//	if((NULL == Data) || (Size != ONE_RESOLUTION_SIZE)) 
	{ 
//		return -1; 
	}
	
/* Disable all maskable interrupts */ 
	PLIB_INT_Disable( INT_ID_0 );
	for(ByteIndex = 0; ByteIndex < Size; ByteIndex++) 
	{ 
		TM1812_WriteOneByte(Data[ByteIndex]); 
	} 	
/* Enable microcontroller interrupts */ 
	SYS_INT_Enable();
	
	TM1812_Reset();
	return 0; 
}

/*-----------------------------------------------------------------
*/

//index:[bit0--bit11] indicate [led0--led11]
void led_on_off(uint16_t index,uint8_t type)
{
    uint16_t i;
	
    for(i = 0; i< TM1812_LED_INDEX_MAX; i++)
    {
		if(GET_BIT(index,i))
		{
			if(type == ON)
			{			
				if(i<2)
				{
					SET_BIT(ledFlag,i+4);
				}
				else if(i<5)
				{
					SET_BIT(ledFlag,i+7);
				}
				else if(i < 8)
				{
					SET_BIT(ledFlag,i+1);
				}
				else if(i <= 12)
				{
					SET_BIT(ledFlag,i-8);
				}
			}
			else
			{
				
				if(i<2)
				{
					CLEAR_BIT(ledFlag,i+4);
				}
				else if(i<5)
				{
					CLEAR_BIT(ledFlag,i+7);
				}
				else if(i < 8)
				{
					CLEAR_BIT(ledFlag,i+1);
				}
				else if(i <= 12)
				{
					CLEAR_BIT(ledFlag,i-8);
				}
			}
		}
    }

	led_excute();

}


void led_toggle(uint16_t index,uint8_t times)
{	
    uint16_t i;
	static uint8_t count = 0;

	if(count <= times)
	{
	    for(i = 0; i< TM1812_LED_INDEX_MAX; i++)
	    {
			if(GET_BIT(index,i))
			{			
				if(i<2)
				{
					if(GET_BIT(ledFlag,i+4))
					{
						CLEAR_BIT(ledFlag,i+4);
					}
					else
					{
						SET_BIT(ledFlag,i+4);
					}
				}
				else if(i<5)
				{
					if(GET_BIT(ledFlag,i+7))
					{
						CLEAR_BIT(ledFlag,i+7);
					}
					else
					{
						SET_BIT(ledFlag,i+7);
					}
				}
				else if(i < 8)
				{
					if(GET_BIT(ledFlag,i+1))
					{
						CLEAR_BIT(ledFlag,i+1);
					}
					else
					{
						SET_BIT(ledFlag,i+1);
					}
				}
				else if(i <= 12)
				{
					if(GET_BIT(ledFlag,i-8))
					{
						CLEAR_BIT(ledFlag,i-8);
					}
					else
					{
						SET_BIT(ledFlag,i-8);
					}
				}
			}
	    }

		led_excute();
		count++;
	}
	else
	{
		led_on_off(0xFF,OFF);
	}

}



//index:[bit0--bit11] indicate [led0--led11]
void led_excute(void)
{
    uint16_t i;
	
    for(i = 0; i< TM1812_LED_INDEX_MAX; i++)
    {
		if(GET_BIT(ledFlag,i))
		{
			colorBuffer[i] = 0xFF;
		}
		else
		{
			colorBuffer[i] = 0x00;
		}
	}

	TM1812_TransmitData(colorBuffer, ONE_RESOLUTION_SIZE);
}

/*-----------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

static void power_led_indicate(bool on_off)
{
	static uint8_t i = 0;
	static uint16_t ledData = 0;
	static uint16_t count = 0;

	if(on_off)
	{
		if(count < 300)
		{
			count++;
			if(count >= 12)
			{
				if(i < 9)
				{
					led_on_off(ledData,ON);
					SET_BIT(ledData,i++);
					count = 0;
				}
			}
		}
		else if(count == 300)		// 3s
		{
			ledData = 0xff;
			led_on_off(ledData,OFF);
			count++;
		}
		

	}
	else
	{
		//Set_LED_Style(LED_POWER, LED_OFF, 500, 500);
		//led_on_off(ledData,OFF);
		count = 0;
		ledData = 0;
		i = 0;
		

	}

	
}


static void clear_PDL_led_indicate(bool on_off)
{
	static uint8_t i = 0;
	static uint16_t ledData = 0;
	static uint16_t count = 0;

	if(on_off)
	{
		if(count < 100)
		{
			count++;
			if(count >= 40)
			{
				if(i < 9)
				{
					led_on_off(ledData,ON);
					SET_BIT(ledData,i++);
					count = 0;
				}
			}
		}
		else if(count == 500)		// 1s
		{
			led_on_off(ledData,OFF);
			count++;
		}
		

	}
	else
	{
		//Set_LED_Style(LED_POWER, LED_OFF, 500, 500);
		//led_on_off(ledData,OFF);
		count = 0;
		ledData = 0;
		i = 0;
		

	}

	
}


static void enter_bt_pairing_led_indicate(bool on_off)
{
    static uint8_t i = 0;
	static uint8_t direction = 1;
	uint16_t ledData = 0;
	static uint16_t count = 0;


	if(on_off)
	{
		count++;
		if(0 == count%6)
		{
			if(direction)
			{
				//led_on_off(LED_BROADCAST_MASK,OFF);	
				led_on_off(0xFF,OFF);
				SET_BIT(ledData,i++);
				led_on_off(ledData,ON);	

				if(i == 7)//last led will be lit on reverse
				{
					direction = 0;
					count = 0;
				}
			}
			else
			{
				//led_on_off(LED_BROADCAST_MASK,OFF);	
				led_on_off(0xFF,OFF);
				SET_BIT(ledData,i--);
				led_on_off(ledData,ON);
				if(0 == i)
				{
					direction = 1;
					count = 0;
				}
			}
		}
	}
	else
	{
		//Set_LED_Style(LED_PAIRING, LED_OFF, 500, 500);//exit led on process
		//led_on_off(0xFF,OFF);
		 i = 0;
		 direction = 1;
		 count = 0;
	}
}




static void broadcast_primary_pairing_led_indicate(bool on_off)
{
    static uint8_t i = 0;
	uint16_t ledData = 0;
	static uint16_t count = 0;

	if(on_off)
	{
		count++;
		if(0 == count%6)
		{
			//led_on_off(LED_BROADCAST_MASK,OFF);	
			led_on_off(0xFF,OFF);
			SET_BIT(ledData,i++);
			led_on_off(ledData,ON);					

			if(i > 7)
			{
				i = 0;
				count = 0;
			}
		}
		
	}
	else
	{
		//Set_LED_Style(LED_BROADCASTP, LED_OFF, 500, 500); //exit led on process
		//led_on_off(0xFF,OFF);
		i = 0;
		count = 0;
	}
}


static void broadcast_secondary_pairing_led_indicate(bool on_off)
{
    static int8_t i = 7;
	uint16_t ledData = 0;
	static uint16_t count = 0;

	if(on_off)
	{
		count++;
		if(0 == count%6)
		{
			//led_on_off(LED_BROADCAST_MASK,OFF);	
			led_on_off(0xFF,OFF);
			SET_BIT(ledData,i--);
			led_on_off(ledData,ON);	

			if(i < -1)
			{
				i = 7;
				ledData = 0;
				
			}
		}
		
	}
	else
	{
		//Set_LED_Style(LED_BROADCASTS, LED_OFF, 500, 500);//exit led on process
		//led_on_off(0xFF,OFF);
		i = 7;
		count = 0;
	}
}

static void power_on_off_led_indicate(bool on_off)
{
	if(on_off)
		led_on_off(LED_BROADCAST_MASK,ON);	
	else
		led_on_off(LED_BROADCAST_MASK,OFF);	
}

static void broadcast_connected_led_master_indicate(bool on_off)
{
	if(on_off){
		led_on_off(LED_VMASTER_MASK,ON);	
		led_on_off(LED_VTREBLE_MASK,ON);
		led_on_off(LED_VBASS_MASK,ON);
	}
	else{
		led_on_off(LED_VMASTER_MASK,OFF);	
		led_on_off(LED_VTREBLE_MASK,OFF);
		led_on_off(LED_VBASS_MASK,OFF);
	}

}

static void broadcast_connected_led_slave_indicate(bool on_off)
{
	if(on_off){
		led_on_off(LED_VMASTER_MASK,OFF);	
		led_on_off(LED_VTREBLE_MASK,ON);
		led_on_off(LED_VBASS_MASK,ON);
	}
	else{
		led_on_off(LED_VMASTER_MASK,OFF);	
		led_on_off(LED_VTREBLE_MASK,OFF);
		led_on_off(LED_VBASS_MASK,OFF);
	}

}


#if 0
static void broadcast_connected_led_indicate(bool on_off)
{
	if(on_off)
		led_on_off(LED_BROADCAST_MASK,ON);	
	else
		led_on_off(LED_BROADCAST_MASK,OFF);	
}

#endif

static void vmaster_led_indicate(bool on_off)
{
	if(on_off){
		led_on_off(LED_VMASTER_MASK,ON);	
		led_on_off(LED_VTREBLE_MASK,OFF);
		led_on_off(LED_VBASS_MASK,OFF);
	}
	else
		led_on_off(LED_VMASTER_MASK,OFF);	
}

static void vtreble_led_indicate(bool on_off)
{
	if(on_off){
		led_on_off(LED_VTREBLE_MASK,ON);
		led_on_off(LED_VMASTER_MASK,OFF);
		led_on_off(LED_VBASS_MASK,OFF);
	}
	else
		led_on_off(LED_VTREBLE_MASK,OFF);	
}

static void vbass_led_indicate(bool on_off)
{
	if(on_off){
		led_on_off(LED_VTREBLE_MASK,OFF);
		led_on_off(LED_VMASTER_MASK,OFF);
		led_on_off(LED_VBASS_MASK,ON);	
	}
	else
		led_on_off(LED_VBASS_MASK,OFF);	
}

static void hfp_open_led_Indicate(bool on_off)
{
	static uint8_t led_hfp_cnt = 0;
	if(on_off)
	{
		//if(BTMHFP_GetCallStatus() != BT_CALL_IDLE)
		{
			led_hfp_cnt++;
			if(led_hfp_cnt == 1){
				led_on_off(BIT7 | BIT0, ON);
			}
			else if(led_hfp_cnt == 51){
				led_on_off(BIT6 | BIT1, ON);
			}
			else if(led_hfp_cnt == 101){
				//led_on_off(BIT7 | BIT0, OFF);
				led_on_off(BIT5 | BIT2, ON);
			}
			else if(led_hfp_cnt == 151){
				//led_on_off(BIT6 | BIT1, OFF);
				led_on_off(BIT4 | BIT3, ON);
			}
			else if(led_hfp_cnt == 201){
				//led_on_off(BIT5 | BIT2, OFF);
				
			}
			else if(led_hfp_cnt > 250)
			{
				led_on_off(0xff, OFF);
				led_hfp_cnt = 0;
			}


		}
	}
	else
	{
		led_hfp_cnt = 0;

	}

	

}


static void User_LedPatternIndicateOFF()
{
	power_led_indicate(OFF);
	//User_LedPowerIndicate(OFF);
	enter_bt_pairing_led_indicate(OFF);
	//broadcast_primary_pairing_led_indicate(OFF);
	broadcast_secondary_pairing_led_indicate(OFF);
	hfp_open_led_Indicate(OFF);
	//broadcast_connected_led_indicate(OFF);
	led_on_off(0xFF,OFF);
}

static void User_LedVolumeModeOFF()
{
	led_on_off(0xFF,OFF);
	vmaster_led_indicate(OFF);
	vtreble_led_indicate(OFF);
	vbass_led_indicate(OFF);
}


void User_LedClearPDLOFF()
{
	clear_PDL_led_indicate(OFF);
}

void User_LedPrimaryPairingOFF()
{
	User_Log("User_LedPrimaryPairingOFF\n");
	broadcast_primary_pairing_led_indicate(OFF);
}
void User_LedBroadcastConnectedMasterOFF()
{
	User_Log("User_LedBroadcastConnectedMasterOFF\n");
	//broadcast_connected_led_master_indicate(OFF);
}

void User_LedBroadcastConnectedSlaveOFF()
{
	User_Log("User_LedBroadcastConnectedSlaveOFF\n");
	//broadcast_connected_led_slave_indicate(OFF);
}



void User_LedBroadcastConnectedOFF()
{
	User_Log("User_LedBroadcastConnectedON\n");
	//broadcast_connected_led_indicate(ON);
	broadcast_connected_led_master_indicate(OFF);
}

void User_LedPowerIndicate(bool on_off)
{
	power_on_off_led_indicate(on_off);

}



#if 1
void User_LedPatternDisplay()
{
	static LED_PATTERN_INDEX index = led_none;

	if((index != led_effect_index) && (index != led_bt_status_off) && (index != led_none))
	{
		User_LedPatternIndicateOFF();
	}
	
	index = led_effect_index;
	
	switch(led_effect_index)
	{
		case led_power:
			power_led_indicate(ON);
			break;

		case led_pairing:
			enter_bt_pairing_led_indicate(ON);
			break;

		case led_broadcast_master:
			broadcast_primary_pairing_led_indicate(ON);
			break;

		case led_broadcast_master_connecting:
			broadcast_primary_pairing_led_indicate(ON);
			broadcast_connected_led_master_indicate(ON);
			break;

		case led_broadcast_slave:
			broadcast_secondary_pairing_led_indicate(ON);
			break;
#if 0
		case led_broadcast_connect:
			broadcast_connected_led_indicate(ON);
			led_effect_index = led_none;
			break;
#endif

		case led_broadcast_connect_master:
			broadcast_connected_led_master_indicate(ON);
			//led_effect_index = led_none;
			break;

		case led_broadcast_connect_slave:
			broadcast_connected_led_slave_indicate(ON);
			//led_effect_index = led_none;
			break;

		case led_broadcast_connect_off:
			broadcast_connected_led_slave_indicate(OFF);
			led_effect_index = led_none;
			break;

		case led_hfp_active:
			hfp_open_led_Indicate(ON);
			break;

		case led_vmaster:
			vmaster_led_indicate(ON);
			break;

		case led_vtreble:
			vtreble_led_indicate(ON);
			break;

		case led_vbass:
			vbass_led_indicate(ON);
			break;

		case led_voloff:
			User_LedVolumeModeOFF();
			led_effect_index = led_none;
			break;

		case led_bt_status_off:
			User_LedPatternIndicateOFF();
			led_effect_index = led_none;
			break;

		case led_clr_pdl:
			clear_PDL_led_indicate(ON);			
			break;

		default:
			
			break;

	}

}

#endif

void User_DispalyVolumeMode(USER_VOLUME_MODE mode)
{
	switch(mode)
	{
		case VOLUME:
			User_SetLedPattern(led_vmaster);
			break;

		case TREBLE:
			User_SetLedPattern(led_vtreble);
			break;

		case BASS:
			User_SetLedPattern(led_vbass);
			break;
			
		default:
			break;
    }

}



void User_DisplayVolumeLevel()
{
	int i = 0;
	uint8_t step = 0;

	User_DispalyVolumeMode(user_volume_mode);
	ledvolume_timer1ms = 5000;

	
	if(user_volume_mode == VOLUME)
	{
		step = volume_master_step;
		if(step == 0)
		{
			led_on_off(0xff,OFF);
			display_volume0_start();//blink 3 times
			//User_Log("volume_master_step = %d\n", volume_master_step);
			return;

		}
		else if(step == 16)
		{
			led_on_off(0xff,ON);
			display_volume16_start();//blink 3 times
			//User_Log("volume_master_step = %d\n", volume_master_step);
			return;

		}
		for(i = 0; i < 8; i++)
		{
			if(i < (step + 1)/2)
			{
				led_on_off(1 << i,ON);
			}
			else
			{
				led_on_off(1 << i,OFF);
			}

		}

	}
	else 
	{
		if(user_volume_mode == TREBLE)
			step = volume_treble_step;
		else
			step = volume_bass_step;

		if(step == 4)
		{
			led_on_off(1 << 3,ON);
			led_on_off(1 << 4,ON);
			led_on_off(0xE7,OFF);

		}
		else if(step < 4)
		{
			for(i = 0; i < 8; i++)
			{
				if(i == step)
					led_on_off(1 << i,ON);
				else
					led_on_off(1 << i,OFF);				

			}

		}
		else
		{
			for(i = 0; i < 8; i++)
			{
				if(i == (step - 1))
					led_on_off(1 << i,ON);
				else
					led_on_off(1 << i,OFF);

			}
		}

	}


}



void User_SetLedPattern(LED_PATTERN_INDEX index)
{
	User_Log("set index = %d\n",index);

	if(index <= led_bt_status_off)//set bt status led index
	{
		led_effect_index_prev = index;//just record bt status led effect index in order to return display
		if(!ledvolume_timer1ms)//vol led off, display bt led.
		{
			led_effect_index = index;
			Set_LED_Style(LED_PATTERN, LED_ON, 500, 500);		
		}
	}
	else
	{
		led_effect_index = index;
		Set_LED_Style(LED_PATTERN, LED_ON, 500, 500);
	}


}

void User_ReturnDisLEDBTStatus()
{
	if(!ledvolume_timer1ms)
	{
		if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_MASTER)
		{
			User_SetLedPattern(led_broadcast_connect_master);
		}
		else if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_BROADCAST_MASTER_CONNECTING)
		{
			User_SetLedPattern(led_broadcast_master_connecting);
		}
		else if(BTMSPK_GetMSPKStatus() == BT_CSB_STATUS_CONNECTED_AS_BROADCAST_SLAVE)
		{
			User_SetLedPattern(led_broadcast_connect_slave);
		}
		else if(led_effect_index_prev <= led_bt_status_off)
			User_SetLedPattern(led_effect_index_prev);

	}
		

}

void User_ClearPDL()
{
	User_SetLedPattern(led_clr_pdl);
	ledClrPdl_timer1ms = 6500;
}

void tm1812_task()
{
	if(ledvolumeTimeOutFlag)
	{
		ledvolumeTimeOutFlag = false;
		User_SetLedPattern(led_voloff);
		user_volume_mode = VOLUME;
		User_ReturnDisLEDBTStatus();

	}

	if(ledvolume0TimeOutFlag)//volume level 0
	{
		ledvolume0TimeOutFlag = false;
		if(ledvolume0_blink_cnt)
		{
			ledvolume0_blink_cnt--;
			if((ledvolume0_blink_cnt % 2) == 0)
			{
				led_on_off(1 << 0,ON);
			}
			else
			{
				led_on_off(1 << 0,OFF);
			}

			

			if(ledvolume0_blink_cnt != 0)
				ledvolume0_timer1ms = 100;
			

		}

	}

	if(ledvolume16TimeOutFlag)//volume level 16
	{
		ledvolume16TimeOutFlag = false;
		if(ledvolume16_blink_cnt)
		{
			ledvolume16_blink_cnt--;
			if((ledvolume16_blink_cnt % 2) == 0)
			{
				led_on_off(1 << 7,ON);
			}
			else
			{
				led_on_off(1 << 7,OFF);
			}

			

			if(ledvolume16_blink_cnt != 0)
				ledvolume16_timer1ms = 100;
			

		}

	}

	

	if(ledClrPdlTimeOutFlag)
	{
		ledClrPdlTimeOutFlag = false;
		BTAPP_TaskReq(BT_REQ_SYSTEM_OFF);
	}

}


/*********************************************************************************
 *
 *
*********************************************************************************/
void TM1812_DisplayLEDPatternByPriority(LED_PATTERN_INDEX index)
{
	LEVEL_LEDS_PRIORITY led_priority_ctl_bit_current = LEVEL_LEDS_PRIORITY_NONE;
	
	if(index < led_bt_status_off)
	{
		led_effect_index_prev = index;//just record bt status led effect index in order to return display
		SetBit(led_priority_ctl_bit, LEVEL_LEDS_PRIORITY_2);
	}
	else if(index <= led_voloff)
	{
		SetBit(led_priority_ctl_bit, LEVEL_LEDS_PRIORITY_1);
	}
	else if((index == led_power) || (index == led_clr_pdl))
	{
		SetBit(led_priority_ctl_bit, LEVEL_LEDS_PRIORITY_0);
	}
	
	
	if(Test1Bit(led_priority_ctl_bit, LEVEL_LEDS_PRIORITY_0))
	{
		if(Test1Bit(led_priority_ctl_bit_current, LEVEL_LEDS_PRIORITY_0)){
			led_effect_index = index;
			Set_LED_Style(LED_PATTERN, LED_ON, 500, 500);
		}
	}
	else if(Test1Bit(led_priority_ctl_bit, LEVEL_LEDS_PRIORITY_1))
	{
		if(Test1Bit(led_priority_ctl_bit_current, LEVEL_LEDS_PRIORITY_0)){
			led_effect_index = index;
			Set_LED_Style(LED_PATTERN, LED_ON, 500, 500);
		}
	}
	else if(Test1Bit(led_priority_ctl_bit, LEVEL_LEDS_PRIORITY_2))
	{
		led_effect_index = index;
		Set_LED_Style(LED_PATTERN, LED_ON, 500, 500);
	}


}



/*********************************************************************************
  * @brief  : a simple test of tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void TM1812_Test(void) 
{ 
	static uint8_t i = 0;
	static uint16_t ledData = 0;

	led_on_off(ledData,ON);
	SET_BIT(ledData,i++);
	if(i > 13)
	{
		i = 0;
		led_on_off(ledData,OFF);
		ledData = 0;
	}

}



