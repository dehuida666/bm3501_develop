#include "bt_hardware_profile.h"
#include "bt_app.h"
#include "user_battery_management.h"


uint16_t detect_timer100ms = 100;
bool detect_timer100msTimeOutFlag = false;

uint16_t usb_chargeOutTimer10ms = 10;
bool usb_chargeOutTimer10msTimeOutFlag = false;

uint16_t usb_chargeCompleteTimer1ms = 0;
bool usb_chargeCompleteTimer1msTimeOutFlag = false;



NTC_TEMPERATURE_LEVEL ntc_TemperatureLevel;
static uint16_t ntc_getAdVaule(void);
static void ntc_adValueHandle(void);
static void battery_chargeHandle(void);
static void USB_chargeHandle(void);
static uint8_t usb_chargeDetectPinHandler(void);
static void usb_calculateAdValue(bool on_off);

static bool isUSBChargeComplete = false;
static bool isBatteryChargeComplete = false;

static uint8_t Pin_USBChargeDetectGet;


enum
{
	PIN_BATTERY_CHARGE_HIGH,
	PIN_BATTERY_CHARGE_LOW,
};

enum
{
	PIN_USB_CHARGE_HIGH,
	PIN_USB_CHARGE_LOW,
};


void User_BatteryManagementInit(void)
{
	ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL3;
	isUSBChargeComplete = false;
	isBatteryChargeComplete = false;
	Pin_USBChargeDetectGet = PIN_USB_CHARGE_HIGH;
}


void BatteryManagement_Timer1MS_event(void)
{
	if(detect_timer100ms)
	{
		detect_timer100ms--;
		if(detect_timer100ms == 0){
			detect_timer100ms = 100;
			detect_timer100msTimeOutFlag = true;
		}

	}

	if(usb_chargeOutTimer10ms)
	{
		usb_chargeOutTimer10ms--;
		if(usb_chargeOutTimer10ms == 0){
			usb_chargeOutTimer10ms = 10;
			usb_chargeOutTimer10msTimeOutFlag = true;
		}

	}

	if(usb_chargeCompleteTimer1ms)
	{
		usb_chargeCompleteTimer1ms--;
		if(usb_chargeCompleteTimer1ms == 0)
		{
			usb_chargeCompleteTimer1msTimeOutFlag = true;

		}

	}
}



void User_BatteryManagementTask(void)
{
	if(detect_timer100msTimeOutFlag)
	{
		detect_timer100msTimeOutFlag = false;
		ntc_adValueHandle();
		battery_chargeHandle();
		if((BTAPP_GetStatus() == BT_STATUS_ON) || (BTAPP_GetStatus() == BT_STATUS_READY))
		{
			if(IS_USB_CHARGE_Enable)
				usb_calculateAdValue(ON);
		}

	}

	if(usb_chargeOutTimer10msTimeOutFlag)
	{
		usb_chargeOutTimer10msTimeOutFlag = false;
		//Pin_USBChargeDetectGet = usb_chargeDetectPinHandler();
		USB_chargeHandle();

	}

}
/*
********************************************************************************
*USB charge
********************************************************************************
*/
#if 0
static uint8_t usb_chargeDetectPinHandler(void)
{
	static uint8_t pin_usbChargeDetectStatus = PIN_USB_CHARGE_HIGH;
	static uint8_t usb_charge_cnt = 5;
	
	switch(pin_usbChargeDetectStatus)
	{
		case PIN_USB_CHARGE_HIGH:
			if(USB_CHARGE_DETECT_GET())
			{
				usb_charge_cnt = 5;

			}
			else
			{
				if(usb_charge_cnt){
					usb_charge_cnt--;
					if(usb_charge_cnt == 0)
					{
						usb_charge_cnt = 5;
						pin_usbChargeDetectStatus = PIN_USB_CHARGE_LOW;
						if(usb_chargeCompleteTimer1msTimeOutFlag){
							usb_chargeCompleteTimer1msTimeOutFlag = false;
							if(!DC_PULL_OUT)
							{
								USB_CHARGE_SetEnable();
								usb_calculateAdValue(OFF);
								User_Log("USB_CHARGE_SetEnable\n");
							}
							else{
								if(currentBatteryLevel > 40){
									USB_CHARGE_SetEnable();
									usb_calculateAdValue(OFF);
									User_Log("USB_CHARGE_SetEnable\n");
								}
							}	
						}
					}
				}
			}
			break;

		case PIN_USB_CHARGE_LOW:
			if(!USB_CHARGE_DETECT_GET())
			{
				usb_charge_cnt = 5;

			}
			else
			{
				if(usb_charge_cnt){
					usb_charge_cnt--;
					if(usb_charge_cnt == 0)
					{
						usb_charge_cnt = 5;
						pin_usbChargeDetectStatus = PIN_USB_CHARGE_HIGH;
					}
				}
			}
			break;

		default:
			break;

	}

	return pin_usbChargeDetectStatus;
}
#endif

static uint16_t usb_getAdVaule(void)
{
	uint16_t usb_adValue;

	usb_adValue = GetADCValue(ADC_INPUT_POSITIVE_AN12);

	return usb_adValue;

}
static void usb_calculateAdValue(bool on_off)
{
	uint16_t adValue;
	static uint16_t usb_detect_value[5] = {0};
	static uint8_t usb_detect_cnt = 0;
	uint16_t usb_detectValueAvrg = 0;
	uint8_t i;

	if(on_off)
	{
		adValue = usb_getAdVaule();
			
		usb_detect_value[usb_detect_cnt++] = adValue;

		if(usb_detect_cnt >= 5)
		{
			usb_detect_cnt = 0;
			for(i =0; i < 5; i++)
			{
				usb_detectValueAvrg += usb_detect_value[i];
			}

			usb_detectValueAvrg = usb_detectValueAvrg/5;

			User_Log("USB Chrage value = %d\n",usb_detectValueAvrg);

			if(usb_detectValueAvrg <= 1)//Charge complete
			{
				if(!isUSBChargeComplete)
					isUSBChargeComplete = true;
			}
			else
			{
				if(isUSBChargeComplete)
					isUSBChargeComplete = false;
			}		

		}
	}
	else
	{
		usb_detect_cnt = 0;
		isUSBChargeComplete = false;
	}

}

static void usb_ChargeOutEnableCharge(void)
{
	//if(usb_chargeCompleteTimer1msTimeOutFlag)
	{
		//usb_chargeCompleteTimer1msTimeOutFlag = false;
		if(!DC_PULL_OUT)
		{
			USB_CHARGE_SetEnable();
			usb_calculateAdValue(OFF);
			User_Log("USB_CHARGE_SetEnable\n");
		}
		else{
			if(currentBatteryLevel > 40){
				USB_CHARGE_SetEnable();
				usb_calculateAdValue(OFF);
				User_Log("USB_CHARGE_SetEnable\n");
			}
		}	
	}

}


static void USB_chargeHandle(void)
{	
	static uint8_t pin_usbChargeDetectStatus = PIN_USB_CHARGE_HIGH;
	static uint8_t usb_charge_cnt = 5;
		
	if((BTAPP_GetStatus() == BT_STATUS_OFF) || (BTAPP_GetStatus() == BT_STATUS_NONE))//don't charge
	{
		if(IS_USB_CHARGE_Enable)
		{
			USB_CHARGE_SetDisable();
		}

		if(usb_chargeCompleteTimer1ms)
			usb_chargeCompleteTimer1ms = 0;

		//if(usb_chargeCompleteTimer1msTimeOutFlag == false)
			//usb_chargeCompleteTimer1msTimeOutFlag = true;

		if(pin_usbChargeDetectStatus == PIN_USB_CHARGE_LOW)
			pin_usbChargeDetectStatus = PIN_USB_CHARGE_HIGH;

		usb_charge_cnt = 5;

	}
	else
	{
		if(IS_USB_CHARGE_Enable)//detect adc 
		{			
			if(USB_isChargecomplete())
			{
				if(usb_chargeCompleteTimer1msTimeOutFlag == true)//charge still complete after 30s,then set charge disable 
				{
					usb_chargeCompleteTimer1msTimeOutFlag = false;
					USB_CHARGE_SetDisable();				
					User_Log("USB_CHARGE_SetDisable\n");
				}
				else
				{
					if(!usb_chargeCompleteTimer1ms)
						usb_chargeCompleteTimer1ms = 30000;
				}
			}
			else
			{
				usb_chargeCompleteTimer1ms = 0;
				usb_chargeCompleteTimer1msTimeOutFlag = false;
				
				if(DC_PULL_OUT){
					if(currentBatteryLevel <= 40){
						USB_CHARGE_SetDisable();						
						User_Log("USB_CHARGE_SetDisable\n");
					}
				}

			}
		}
		else//detect jack 
		{
			switch(pin_usbChargeDetectStatus)
			{
				case PIN_USB_CHARGE_HIGH:
					if(USB_CHARGE_DETECT_GET())
					{
						usb_charge_cnt = 5;

					}
					else
					{
						if(usb_charge_cnt){
							usb_charge_cnt--;
							if(usb_charge_cnt == 0)
							{
								usb_charge_cnt = 5;
								pin_usbChargeDetectStatus = PIN_USB_CHARGE_LOW;
								usb_ChargeOutEnableCharge();
							}
						}
					}
					break;

				case PIN_USB_CHARGE_LOW:
					if(!USB_CHARGE_DETECT_GET())
					{
						usb_charge_cnt = 5;

					}
					else
					{
						if(usb_charge_cnt){
							usb_charge_cnt--;
							if(usb_charge_cnt == 0)
							{
								usb_charge_cnt = 5;
								pin_usbChargeDetectStatus = PIN_USB_CHARGE_HIGH;
							}
						}
					}
					break;

				default:
					break;

			}
			//usb_chargeDetectPinHandler();	
			/*
			if(!USB_CHARGE_DETECT_GET())//USB jack plug in
			{
				if(!DC_PULL_OUT)
				{
					USB_CHARGE_SetEnable();
					usb_calculateAdValue(OFF);
					User_Log("USB_CHARGE_SetEnable\n");
				}
				else{
					if(currentBatteryLevel > 40){
						USB_CHARGE_SetEnable();
						usb_calculateAdValue(OFF);
						User_Log("USB_CHARGE_SetEnable\n");
					}
				}
			}
			*/

		}
		
	}

}

bool USB_isChargecomplete(void)
{
	return isUSBChargeComplete;
}

/*
********************************************************************************
*battery charge
********************************************************************************
*/
bool Battery_isChargeComplete(void)
{
	return isBatteryChargeComplete;
}

static void battery_chargeHandle(void)
{
	static uint8_t pin_batteryChargingStatus = PIN_BATTERY_CHARGE_HIGH;
	static uint8_t pin_batteryChargeDoneStatus = PIN_BATTERY_CHARGE_HIGH;
	static uint8_t battery_detect_cnt = 5;
	static uint8_t charge_done_detect_cnt = 5;

	if(NTC_getTemperatureLevel() != NTC_TEMPERATURE_LEVEL3){
		battery_detect_cnt = 5;
		charge_done_detect_cnt = 5;
		if(!IS_BATTERY_CHARGE_Disable)
		{
			isBatteryChargeComplete = true;
			BATTERY_CHARGE_SetDisable();
			User_Log("Battery charge abnormal\n");
		}
		return;
	}

	if(DC_PULL_OUT)
	{
		if(!IS_BATTERY_CHARGE_Disable)
		{
			isBatteryChargeComplete = true;
			BATTERY_CHARGE_SetDisable();
			User_Log("DC pull out\n");
		}
		return;
	}

	if(IS_BATTERY_CHARGE_Disable)
	{
		BATTERY_CHARGE_SetEnable();
		User_Log("Charging Enable\n");
	}
	

	switch(pin_batteryChargingStatus)
	{
		case PIN_BATTERY_CHARGE_HIGH:
			if(CHARGE_STATUS1_GET())
			{
				battery_detect_cnt = 5;

			}
			else
			{
				if(battery_detect_cnt){
					battery_detect_cnt--;
					if(battery_detect_cnt == 0)
					{
						battery_detect_cnt = 5;
						pin_batteryChargingStatus = PIN_BATTERY_CHARGE_LOW;
					}
				}
			}
			break;

		case PIN_BATTERY_CHARGE_LOW:
			if(CHARGE_STATUS1_GET())
			{
				if(battery_detect_cnt){
					battery_detect_cnt--;
					if(battery_detect_cnt == 0)
					{
						battery_detect_cnt = 5;
						pin_batteryChargingStatus = PIN_BATTERY_CHARGE_HIGH;
					}		
				}
			}
			else
			{
				battery_detect_cnt = 5;
			}
			break;

		default:
			break;
	}

	switch(pin_batteryChargeDoneStatus)
	{
		case PIN_BATTERY_CHARGE_HIGH:
			if(CHARGE_STATUS2_GET())
			{
				charge_done_detect_cnt = 5;
				if(pin_batteryChargingStatus == PIN_BATTERY_CHARGE_LOW)//charging
				{
					if(isBatteryChargeComplete)
					{
						isBatteryChargeComplete = false;
						User_Log("Charging\n");
					}
				}
				else
				{
					if(!isBatteryChargeComplete)
					{
						isBatteryChargeComplete = true;
						User_Log("Charging Pin high error\n");
					}					
				}

			}
			else
			{
				if(charge_done_detect_cnt){
					charge_done_detect_cnt--;
					if(charge_done_detect_cnt == 0)
					{
						charge_done_detect_cnt = 5;
						pin_batteryChargeDoneStatus = PIN_BATTERY_CHARGE_LOW;
					}
				}
			}
			break;

		case PIN_BATTERY_CHARGE_LOW:
			if(CHARGE_STATUS2_GET())
			{
				if(charge_done_detect_cnt){
					charge_done_detect_cnt--;
					if(charge_done_detect_cnt == 0)
					{
						charge_done_detect_cnt = 5;
						pin_batteryChargeDoneStatus = PIN_BATTERY_CHARGE_HIGH;
					}		
				}
			}
			else
			{
				charge_done_detect_cnt = 5;
				if(pin_batteryChargingStatus == PIN_BATTERY_CHARGE_HIGH)//charge done
				{
					if(!isBatteryChargeComplete){
						isBatteryChargeComplete = true;
						User_Log("Charging Disable\n");
					}
				}
			}
			break;

		default:
			break;
	}
	

}

/*
********************************************************************************
*NTC detect
********************************************************************************
*/
static uint16_t ntc_getAdVaule(void)
{
	uint16_t ntc_adValue;

	ntc_adValue = GetADCValue(ADC_INPUT_POSITIVE_AN11);

	return ntc_adValue;

}

static void ntc_adValueHandle(void)
{
	uint16_t adValue;
	static uint16_t ntc_detect_value[10] = {0};
	static uint8_t ntc_detect_cnt = 0;
	uint16_t ntc_detectValueAvrg = 0;
	uint8_t i;

	adValue = ntc_getAdVaule();

	ntc_detect_value[ntc_detect_cnt++] = adValue;

	if(ntc_detect_cnt >= 10)
	{
		ntc_detect_cnt = 0;
		for(i =0; i < 10; i++)
		{
			ntc_detectValueAvrg += ntc_detect_value[i];
		}

		ntc_detectValueAvrg = ntc_detectValueAvrg/10;

		if(ntc_detectValueAvrg >= NTC_TEMPERATURE_ADVALUE1)//T < -10C
		{
			ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL1;
		}
		else if(ntc_detectValueAvrg >= NTC_TEMPERATURE_ADVALUE2)// -10C < T < 0C
		{
			ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL2;

		}
		else if(ntc_detectValueAvrg >= NTC_TEMPERATURE_ADVALUE3)// 0C < T < 45C
		{
			ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL3;
		}
		else if(ntc_detectValueAvrg >= NTC_TEMPERATURE_ADVALUE4)// 45C < T < 50C
		{
			ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL4;
		}
		else
		{
			ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL5;// T>50C

		}
		

	}

}


uint8_t NTC_getTemperatureLevel(void)
{
	return ntc_TemperatureLevel;

}

bool NTC_isTemperatureOverLimit(void)
{
	uint8_t ntc_level;

	ntc_level = NTC_getTemperatureLevel();
	if((ntc_level == NTC_TEMPERATURE_LEVEL1) || (ntc_level == NTC_TEMPERATURE_LEVEL5))
		return true;
	else
		return false;

}





