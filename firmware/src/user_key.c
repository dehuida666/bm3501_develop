
#include <xc.h>
#define KEY_C
#include "user_key.h"
#include "user_event_handler.h"

#include "bt_hardware_profile.h"
#include "app.h"

/*
*******************************************************************************
**  MacroDefine
*******************************************************************************
*/
uint8_t user_keyval;
uint8_t user_keyvalbk;
uint8_t userkeyTimer = 0;
static uint8_t keyst;
static uint8_t keyholdtimecnt;
uint8_t key_hold_cnt;


/*
-----------------------------------------------------------------------------
*/

void USER_KEY_Timer1MS_event(void)
{
    if(userkeyTimer)
        --userkeyTimer;
}


/*
-----------------------------------------------------------------------------
*/
void User_KEY_InitData(void)
{
    user_keyval = KEYNO0;
    user_keyvalbk = KEYNO0;
    keyst = 0;
    keyholdtimecnt = 0;
    key_hold_cnt = 0;
}


/*
-----------------------------------------------------------------------------
*/
void User_KEY_PressDispatch( uint8_t keyno )
{
	
    uint8_t ad_key;
    key_hold_cnt = 1;
        
      
    if(keyno == KEY_POWER)
    {

    }
    else if(keyno == KEY_VOLADD)
    {

    }
    else if(keyno == KEY_VOLSUB)
    {
		
    }

    else if(keyno ==  KEY_PLAY)
    {
		
		if(Get_KeyPlayReleaseFlag())
		{
			//double press
			User_PlayDoublePress();
			Set_KeyPlayReleaseCnt(0);
		}

    }
	else if(keyno ==  KEY_BROADCAST)
    {
		User_Log("KEY_BROADCAST\n");

    }
	else if(keyno ==  KEY_BT)
    {
		User_Log("KEY_BT\n");

    }
		  
			
}
/*
-----------------------------------------------------------------------------
*/
void User_KEY_PressRleaseDispatch( uint8_t keyno )
{
        
  if(key_hold_cnt==0)
    return;
  
    if(keyno == KEY_VOLADD)
    {
		User_PlusMinus(PLUS);

    }
    else if(keyno == KEY_VOLSUB)
    {
		User_PlusMinus(MINUS);

    }
    else if(keyno == KEY_PLAY)
    {
		Set_KeyPlayReleaseCnt(6);
    }
	else if(keyno == KEY_POWER)
    {
		User_PowerPress();

    }

}

/*
-----------------------------------------------------------------------------
*/
void User_KEY_HoldFirstDispatch( uint8_t keyno )
{ 
  uint8_t ad_key;
  
  if(key_hold_cnt==0)
    return;
  
    if(keyno == KEY_PLAY)
    {
		
    }
    else if(keyno == KEY_VOLADD)
    {

    }
    else if(keyno == KEY_VOLSUB)
    {
     
    }
    
}
/*
-----------------------------------------------------------------------------
*/
void User_KEY_HoldFreqDispatch( uint8_t keyno )
{  
  
  if(key_hold_cnt==0)
    return;
  
    if(keyno == KEY_VOLADD)
    {
		if(key_hold_cnt == 13) // 3sec
		{
			User_Log("HOLD_PLUS\n");
			User_PlusMinusHold(PLUS);
			key_hold_cnt = 14;
		}
		else
		{
		++key_hold_cnt;
		}

    }
    else if(keyno == KEY_VOLSUB)
    {
		if(key_hold_cnt == 13) // 3sec
		{
			User_Log("HOLD_MINUS\n");
			User_PlusMinusHold(MINUS);
			key_hold_cnt = 14;
		}
		else
		{
		++key_hold_cnt;
		}

    }
    else if(keyno == KEY_PLAY)
    {
		if(key_hold_cnt == 13) // 3sec
		{
			User_Log("HOLD_PLAY\n");
			User_PlayHold();
			key_hold_cnt = 14;
		}
		else
		{
		++key_hold_cnt;
		} 
		
    }
    else if(keyno == KEY_POWER)
    {
		if(key_hold_cnt == 13) // 3sec
		{
			User_Log("HOLD_POWER\n");
			User_SwitchPower();
		key_hold_cnt = 14;
		}
		else
		{
		++key_hold_cnt;
		} 
    }

    
}
/*
-----------------------------------------------------------------------------
*/
void User_KEY_HoldReleaseDispatch( uint8_t keyno )
{
  
  if(key_hold_cnt==0)
    return;
  
  if(keyno == KEY_VOLADD)
  {

  }
  else if(keyno == KEY_VOLSUB)
  {
 
  }  
  else if(keyno == KEY_PLAY)
  {
  
  }
  
}

/*
**-----------------------------------------------------------------------------
*/
uint8_t User_KEY_ADC1(void)
{
	uint16_t adval;

	adval = GetADCValue(ADC_INPUT_POSITIVE_AN3);

	//User_Log("adc get value %d\n", adval);

	if(!POWER_KEY_STATUS_GET())
	   return KEY_POWER;
	if(adval < KEY1)
		return KEY_BROADCAST;
	else if((adval >= KEY2 - KEYT) && (adval < KEY2 + KEYT))
		return KEY_BT;
	else if((adval >= KEY3 - KEYT) && (adval < KEY3 + KEYT))
		return KEY_VOLADD;
	else if((adval >= KEY4 - KEYT) && (adval < KEY4 + KEYT))
		return KEY_PLAY;
	else if((adval >= KEY5 - KEYT) && (adval < KEY5 + KEYT))
		return KEY_VOLSUB;
	else if(adval > (KEY0-KEYT-KEYT)){
		return KEYNO0;
	}else{
		return KEYERR;
	}
}


unsigned char User_KEY_Number(void)
{ 
	
  	uint8_t ad_key;
 
    ad_key = User_KEY_ADC1();

    return ad_key;


}

void User_KEY_Task( void )
{
    uint8_t key_temp;

	if(userkeyTimer)
        return;
	
    userkeyTimer = USER_KEY_TASK_PERIOD*10;
    
    key_temp = User_KEY_Number();

	

    if(key_temp ==  KEYERR)
    {
        return;
    }
    
    user_keyvalbk = user_keyval;
    user_keyval = key_temp;   

    if((user_keyvalbk == KEYNO0) && (user_keyval == KEYNO0))
    {
        return;
    }
        
    if(user_keyval == KEYNO0)
    {
        if(keyst == ADCKEY_HOLD_FIRST_FLAG)
        {
            /* hold key , then release key work */
            User_KEY_HoldReleaseDispatch(user_keyvalbk);
        }
        else if(keyst == ADCKEY_PRESS_FLAG)
        {
            /* press key , then release key work */
            User_KEY_PressRleaseDispatch(user_keyvalbk); 
        }
        keyst = ADCKEY_NO_EFFECT_FLAG;
    }
    else if(user_keyvalbk == user_keyval)
    {
        if(keyst == ADCKEY_PRESS_FLAG)
        {
            keyholdtimecnt++;
            if(keyholdtimecnt >= KEYHOLDSTEP)
            {
                User_KEY_HoldFirstDispatch(user_keyvalbk);
                keyst = ADCKEY_HOLD_FIRST_FLAG;
                keyholdtimecnt = KEYHOLDSTEP - 2;
            }
        }
        else if(keyst == ADCKEY_HOLD_FIRST_FLAG)
        {
            keyholdtimecnt++;
            if(keyholdtimecnt >= KEYHOLDSTEP)
            {
                User_KEY_HoldFreqDispatch(user_keyval);
                keyholdtimecnt = KEYHOLDSTEP - 2;
            }
        }
    }
    else
    {
        if(user_keyvalbk != KEYNO0)
        {
            if(keyst == ADCKEY_HOLD_FIRST_FLAG)
            {
                /* hold key , then release key work */
                User_KEY_HoldReleaseDispatch(user_keyvalbk);
            }
            else if(keyst == ADCKEY_PRESS_FLAG)
            {
                /* press key , then release key work */
                User_KEY_PressRleaseDispatch(user_keyvalbk); 
            }
            keyst = ADCKEY_NO_EFFECT_FLAG;
            user_keyval = KEYNO0;        
        }
        else
        {
            //User_DelayXms(1);
            
            key_temp = User_KEY_Number();
            
            if(key_temp == user_keyval)
            {
                User_KEY_PressDispatch(user_keyval);
                keyst = ADCKEY_PRESS_FLAG;
            }
            else
            {
                keyst = ADCKEY_NO_EFFECT_FLAG;
                user_keyval = KEYNO0;
            }
        }
        keyholdtimecnt = 0;
    }
}

