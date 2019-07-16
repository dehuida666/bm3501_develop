#ifndef _TM1812_APP_H    /* Guard against multiple inclusion */
#define _TM1812_APP_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "system_config.h"
#include "system_definitions.h"
#include "led.h"
#include "nf8230dsp_handler.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define SET_BIT(x,n)    x |= (1<<n)
#define CLEAR_BIT(x,n)  x &= (~(1<<n)) 
#define GET_BIT(x,n) 	((x>>n) & 1) //用宏得到某数的某位

/*-- one bit operation --*/
#define ClrBitOfByte(Reg, ClrBitMap)   Reg &= (uint8_t)(~(ClrBitMap))
#define ClrBit(Reg, ClrBitMap)   Reg &= (~(ClrBitMap))
#define SetBit(Reg, SetBitMap)   Reg |= (SetBitMap)

#define SetVal(Reg, SetBitMap)   Reg = (SetBitMap)
#define GetVal(Reg, SetBitMap)   (Reg & (SetBitMap))

#define TestBit(Reg, SetBitMap)     (((Reg) & (SetBitMap)) == (SetBitMap))
#define Test1Bit(Reg, SetBitMap)     ((Reg) & (SetBitMap))
#define TestBitNot(Reg, SetBitMap)  (((Reg) & (SetBitMap)) == 0)


/*-- Define one bit mask --*/
#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

#define BIT8    0x0100
#define BIT9    0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000

#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000


#define		TM1812_LED_INDEX_MAX			12
#define     ONE_RESOLUTION_SIZE    	12 

#define 	ON 						1
#define  	OFF						0

#define 	LED_BROADCAST_MASK 		BIT8
#define 	LED_VBASS_MASK 			BIT9
#define 	LED_VMASTER_MASK 		BIT10
#define 	LED_VTREBLE_MASK 		BIT11



#define  DIN_BIT_1() \
do {  \
	SET_LED_DIN_HIGH();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    SET_LED_DIN_LOW();\
    Nop();Nop();\
}while(0)
   

#define  DIN_BIT_0() \
do{  \
    SET_LED_DIN_HIGH(); \
    Nop();Nop();Nop();Nop();Nop();Nop();Nop(); \
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();  \
    SET_LED_DIN_LOW();  \
    Nop();Nop();Nop();Nop();Nop();Nop();  \
    Nop();Nop();Nop();Nop();Nop();Nop(); \
    Nop();Nop();Nop();Nop();Nop();Nop(); \
    Nop();Nop();Nop();Nop();Nop();Nop(); \
}while(0)


typedef enum
{
	LEVEL_LEDS_PRIORITY_NONE = 0,
	LEVEL_LEDS_PRIORITY_0 = BIT0,//POWER_ON, Clear pdl
	LEVEL_LEDS_PRIORITY_1 = BIT1,//volume
	LEVEL_LEDS_PRIORITY_2 = BIT2,//bt status

}LEVEL_LEDS_PRIORITY;

extern void TM1812_LEDInit(void); 

extern void TM1812_DataPowerOn(void); 

extern void TM1812_DataPoweOff(void); 

extern int TM1812_WriteOneByte(uint8_t Data); 

extern int TM1812_Reset(void); 

extern int TM1812_TransmitData(const uint8_t* Data, uint8_t Size); 

extern void led_on_off(uint16_t index,uint8_t type);

extern void led_toggle(uint16_t index,uint8_t times);

extern void led_excute(void);

extern void User_LedPatternDisplay();

extern void User_SetLedPattern(LED_PATTERN_INDEX index);
extern void User_DispalyVolumeMode(USER_VOLUME_MODE mode);
extern void User_DisplayVolumeLevel();
extern void TM1812_timer_1ms();

extern void tm1812_task();
extern void User_LedBroadcastConnectedOFF();
extern void User_LedBroadcastConnectedON();

extern void User_LedClearPDLOFF();

void User_LedPrimaryPairingOFF();



void TM1812_Test(void);



    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
