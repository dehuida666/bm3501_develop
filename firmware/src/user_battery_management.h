#ifndef USER_BATTERY_MANAGEMENT_H
#define USER_BATTERY_MANAGEMENT_H
#include <stdbool.h>
#include <stdint.h>


#define  NTC_TEMPERATURE_ADVALUE1		837    //2.7V, -10C
#define  NTC_TEMPERATURE_ADVALUE2		754    //2.43V, 0C
#define  NTC_TEMPERATURE_ADVALUE3		332    //1.07V, 45C
#define  NTC_TEMPERATURE_ADVALUE4		294    //0.95V, 50C

typedef enum
{
	NTC_TEMPERATURE_LEVEL1,
	NTC_TEMPERATURE_LEVEL2,
	NTC_TEMPERATURE_LEVEL3,
	NTC_TEMPERATURE_LEVEL4,
	NTC_TEMPERATURE_LEVEL5,

}NTC_TEMPERATURE_LEVEL;


void BatteryManagement_Timer1MS_event(void);
void User_BatteryManagementTask(void);
uint8_t NTC_getTemperatureLevel(void);
bool NTC_isTemperatureOverLimit(void);
bool USB_isChargecomplete(void);


#endif

