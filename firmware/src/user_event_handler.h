#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H
#include <stdbool.h>
#include <stdint.h>

#define EVENT_TASK_PERIOD         25
#define PLUS					  1
#define MINUS					  0

typedef enum
{
	/* Application's state machine's initial state. */
	

	/* TODO: Define states used by the application state machine. */
	SYS_STATE_STANDBY = 0,
	SYS_STATE_TO_STBY = 1,
	SYS_STATE_TO_ON = 10,
	SYS_STATE_ON = 11,

} SYS_STATES;

//source
typedef enum
{
	/* Application's state machine's initial state. */
	USER_MODE_BT = 0,
	USER_MODE_AUX,
	USER_MODE_MIC,
	USER_MODE_STANDBY,

	/* TODO: Define states used by the application state machine. */

} SYS_MODES;

typedef struct
{
    /* The application's current state */
    SYS_STATES state;

    /* TODO: Define any additional data used by the application. */
	SYS_MODES user_sys_mode;

} SYS_DATA;


extern void EventHandlers_Timer1MS_event( void );

extern void User_SwitchPower();
extern void User_EventHandlerInit();
extern void User_EventHandlerTask();
extern void User_PlayRelease();
extern void User_PlayDoublePress();
extern void Set_KeyPlayReleaseCnt(uint8_t value);
extern bool Get_KeyPlayReleaseFlag();
extern void User_PlayHold();
extern void User_PlusMinusHold(uint8_t dir);
extern void User_PowerPress();
extern void User_BroadcastPress();
extern void User_BroadcastHold();
extern void User_BT();
extern void User_BTHold();
extern void User_BTVLongPress();



#endif

