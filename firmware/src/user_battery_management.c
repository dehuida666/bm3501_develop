#include "bt_hardware_profile.h"
#include "bt_app.h"
#include "user_battery_management.h"


uint16_t detect_timer100ms = 100;
bool detect_timer100msTimeOutFlag = false;

NTC_TEMPERATURE_LEVEL ntc_TemperatureLevel;
static uint16_t ntc_getAdVaule(void);
static void ntc_adValueHandle(void);
static void battery_chargeHandle(void);
static void USB_chargeHandle(void);

static bool isUSBChargeComplete = false;

enum
{
	PIN_BATTERY_CHARGE_HIGH,
	PIN_BATTERY_CHARGE_LOW,
};

void User_BatteryManagementInit(void)
{
	ntc_TemperatureLevel = NTC_TEMPERATURE_LEVEL3;
	isUSBChargeComplete = false;
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
}



void User_BatteryManagementTask(void)
{
	if(detect_timer100msTimeOutFlag)
	{
		detect_timer100msTimeOutFlag = false;
		ntc_adValueHandle();
		battery_chargeHandle();
		USB_chargeHandle();

	}

}
/*
********************************************************************************
*USB charge
********************************************************************************
*/
static uint16_t usb_getAdVaule(void)
{
	uint16_t usb_adValue;

	usb_adValue = GetADCValue(ADC_INPUT_POSITIVE_AN12);

	return usb_adValue;

}
static void usb_calculateAdValue(void)
{
	uint16_t adValue;
	static uint16_t usb_detect_value[10] = {0};
	static uint8_t usb_detect_cnt = 0;
	uint16_t usb_detectValueAvrg = 0;
	uint8_t i;

	adValue = usb_getAdVaule();
		
	usb_detect_value[usb_detect_cnt++] = adValue;

	if(usb_detect_cnt >= 10)
	{
		usb_detect_cnt = 0;
		for(i =0; i < 10; i++)
		{
			usb_detectValueAvrg += usb_detect_value[i];
		}

		usb_detectValueAvrg = usb_detectValueAvrg/10;

		User_Log("USB Chrage value = %d\n",usb_detectValueAvrg);

		if(usb_detectValueAvrg == 0)//Charge complete
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


static void USB_chargeHandle(void)
{	
	if((BTAPP_GetStatus() == BT_STATUS_OFF) || (BTAPP_GetStatus() == BT_STATUS_NONE))//don't charge
	{
		if(IS_USB_CHARGE_Enable)
		{
			USB_CHARGE_SetDisable();
		}

	}
	else
	{		
		if(DC_PULL_OUT){
			if(currentBatteryLevel <= 40)
			{
				if(IS_USB_CHARGE_Enable)
					USB_CHARGE_SetDisable();
			}
			else
			{
				if(!IS_USB_CHARGE_Enable)
					USB_CHARGE_SetEnable();
			}
		}
		else
		{
			if(!IS_USB_CHARGE_Enable)
				USB_CHARGE_SetEnable();
		}

		return;
		
		if(IS_USB_CHARGE_Enable)//detect adc 
		{
			usb_calculateAdValue();
			if(USB_isChargecomplete())
			{
				USB_CHARGE_SetDisable();
			}
			else
			{
				if(DC_PULL_OUT){
					if(currentBatteryLevel <= 40)
						USB_CHARGE_SetDisable();
				}

			}
		}
		else//detect jack 
		{
			if(!USB_CHARGE_DETECT_GET())//USB jack plug in
			{
				if(!DC_PULL_OUT)
				{
					USB_CHARGE_SetEnable();
				}
				else{
					if(currentBatteryLevel > 40)
						USB_CHARGE_SetEnable();
				}
			}

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
static void battery_chargeHandle(void)
{
	static uint8_t pin_batteryChargingStatus = PIN_BATTERY_CHARGE_HIGH;
	static uint8_t pin_batteryChargeDoneStatus = PIN_BATTERY_CHARGE_HIGH;
	static uint8_t battery_detect_cnt = 5;

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





