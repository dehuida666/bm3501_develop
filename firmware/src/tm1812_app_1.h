#ifndef _TM1812_APP_H    /* Guard against multiple inclusion */
#define _TM1812_APP_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "system_config.h"
#include "system_definitions.h"



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

    
#define    ONE_RESOLUTION_SIZE    12 

typedef enum 
{
	LED_VOLUME = 0x01,
	LED_TREBLE = 0x02,
	LED_BASS = 0x04
}SOUND_LED_SELECT;



#define  DIN_BIT_1() \
do {  \
	PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2);\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    Nop();Nop();Nop();Nop(); Nop();Nop();Nop();\
    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2);\
    Nop();Nop();\
}while(0)
   

#define  DIN_BIT_0() \
do{  \
    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2); \
    Nop();Nop();Nop();Nop();Nop();Nop();Nop(); \
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();  \
    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2);  \
    Nop();Nop();Nop();Nop();Nop();Nop();  \
    Nop();Nop();Nop();Nop();Nop();Nop(); \
    Nop();Nop();Nop();Nop();Nop();Nop(); \
    Nop();Nop();Nop();Nop();Nop();Nop(); \
}while(0)



typedef struct _example_struct_t {
    /* Describe structure member. */
    int some_number;

    /* Describe structure member. */
    bool some_flag;

} example_struct_t;



extern void TM1812_LEDInit(void); 

extern void TM1812_DataPowerOn(void); 

extern void TM1812_DataPoweOff(void); 

extern int TM1812_WriteOneByte(uint8_t Data); 

extern int TM1812_Reset(void); 

extern int TM1812_TransmitData(const uint8_t* Data, uint8_t Size); 

void TM1812_Test(void);



    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
