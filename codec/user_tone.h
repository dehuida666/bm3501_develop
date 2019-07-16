
#ifndef __USER_TONE_H__
#define __USER_TONE_H__

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


typedef enum {
    TONE_TASK_IDLE,
    TONE_TASK_START,
    TONE_TASK_POWER_ON,
    TONE_TASK_BT_PAIRING,
}TONE_TASK_STATE;




typedef enum {
    BT_EVENT_TTS_STATUS = 0,
   
}BT_TTS_EVENTS;

void BTMTONE_Task(void);

void BTMTONE_EventHandler( BT_TTS_EVENTS event, uint8_t* paras, uint16_t size );
void Tone_PlayPowerOn(void);
void Tone_PlayBTPairing(void);



#endif

