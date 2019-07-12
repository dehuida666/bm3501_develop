/*
*******************************************************************************
*/
#ifndef __USER_KEY_
#define __USER_KEY_

#include <stdbool.h>
#include <stdint.h>


/*
*******************************************************************************
**  Macro define
*******************************************************************************
*/
#define USER_KEY_TASK_PERIOD         10

/* key list definetion relative to ADC value */
#define KEYT            50      /* adc tolerance */
#define KEY0            1023    // 1023 = 0x3FF   /* no key status */

#define KEY1            20       /* adc value */ // 0.3v  Vref 3.30v
#define KEY2            129      //0.4171
#define KEY3            281     // 0.90
#define KEY4            442     // 1.42
#define KEY5            618     // 1.99


#define KEYERR  101
#define KEYNO0  0
#define KEYNO1  1
#define KEYNO2  2
#define KEYNO3  3
#define KEYNO4  4
#define KEYNO5  5
#define KEYNO6  6
#define KEYNO7  7
#define KEYNO8  8
#define KEYNO9  9
#define KEYN10  10
#define KEYN11  11
#define KEYN12  12
#define KEYN13  13
#define KEYN14  14
#define KEYN15  15

#define KEYN16  16
/*
************************************************************
*
************************************************************
*/


#define KEY_BROADCAST     	KEYNO1
#define KEY_BT     			KEYNO2
#define KEY_VOLADD    		KEYNO3
#define KEY_PLAY    		KEYNO4
#define KEY_VOLSUB    		KEYNO5
#define KEY_POWER    		KEYNO6




//extern unsigned char keyst;
#define ADCKEY_NO_EFFECT_FLAG  0
#define ADCKEY_EFFECT_FLAG  1
#define ADCKEY_PRESS_FLAG  2
#define ADCKEY_PRESS_RELEASE_FLAG 3
#define ADCKEY_HOLD_FIRST_FLAG   4
#define ADCKEY_HOLD_FREQ_FLAG  5
#define ADCKEY_HOLD_RELEASE_FLAG  6

#define KEYHOLDSTEP  5
#define KEYPRESSSTEP 2

#define KEYHOLDCNT3S  5
//extern unsigned char keyholdtimecnt;

extern uint8_t USER_keyval;
extern uint8_t USER_keyvalbk;

extern uint8_t key_hold_cnt;


extern uint8_t User_KEY_ADC1(void);
extern void User_KEY_Task( void );
extern void User_KEY_InitData(void);
extern void KEY_Timer1MS_event(void);



#endif
