#ifndef _NF8230DSP_HANDLER_H_
#define _NF8230DSP_HANDLER_H_
#include <stdbool.h>
#include <stdint.h>

#define PLUS 1
#define MINUS 0

#define ON 1
#define OFF 0

#define VOL_MAX 17


enum {
    ST_NTP8230G_IDLE, //nothing
    ST_NTP8230G_START,    //start to init
    ST_NTP8230G_RESET_LOW_WAIT,
    ST_NTP8230G_T2_RESET_HIGH_WAIT,
    ST_NTP8230G_T3_RESET_LOW_WAIT,
    ST_NTP8230G_SETUP_EQ,
    ST_NTP8230G_SETUP_EQ_WAIT,
    ST_NTP8230G_RUNNING,
    ST_NTP8230G_STARTING_POWER_OFF,
    ST_NTP8230G_POWER_OFF_1,
    ST_NTP8230G_POWER_OFF_2,
 
};



typedef enum
{
	VOLUME,
	TREBLE,
	BASS,
	
}USER_VOLUME_MODE;

USER_VOLUME_MODE user_volume_mode;

extern uint8_t volume_master_step;
extern uint8_t volume_treble_step;
extern uint8_t volume_bass_step;


/*-----------------------------------------------------------------------------
  This function used to initialize dsp
*/
void NF8230dsp_init(void);
bool is_ntp8230g_idle(void);
bool is_ntp8230g_ready(void);
void ntp8230g_start(void);
void ntp8230g_set_master_volume(uint8_t vol);

extern void ntp8230g_set_volume(USER_VOLUME_MODE mode, uint8_t vol);
extern void User_SoundOnOff(uint8_t on_off,bool a2dp_play);
extern void User_VolumeModeSwitch();

extern void User_VolumeUp();
extern void User_VolumeDn();

extern void NF8230dsp_task(void);

extern void User_SetRingToneVolume(uint8_t Ringtone_Mode, uint8_t status);
extern void DSP_setUserVolumeMode( void );

extern void Ntp8230g_RestoreTrebleAndBass(void);



#endif      //_DSP_HANDLER_H_
