/*
  Project Name: BM89 EVB                            Module name:yda174dsp_handler.c
  Bluetooth Chip: IS1689-151                        Bluetooth UI Version:
  MCU chip: SH79F6488
  Oscillator: IRIC                                  Freq: 12MHz
  Author: Jerry.Chen
  Kick off date: 2014-04-03
  Copyright: 2014 ISSC Technologies Corp.. All Rights Reserved.
*/

#include <xc.h>
#include "bt_hardware_profile.h"
#include "bt_app.h"
#include "yda174dsp_handler.h"


#if defined (__XC32__)      //PIC32 harmony based driver
#define DELAY_NUM 500
extern DRV_HANDLE I2C_Handle;
DRV_I2C_BUFFER_HANDLE I2C_bufferHandle = NULL;
#else       //8bit MCC based driver
#define DELAY_NUM 80
I2C_MESSAGE_STATUS i2c_status;
#endif

#if 1 //def      PEQTab_TAB
uint8_t  PEQTab[ ] = {
	0x80, 0x80, 0x00,
	0x20, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x20, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0x20, 0x0D, 0xD3,
	0xC0, 0xE5, 0xB5,
	0x1F, 0x0F, 0x95,
	0x3F, 0x1A, 0x4B,
	0xE0, 0xE2, 0x98,
	0x24, 0x4E, 0x5F,
	0xC3, 0x7C, 0x18,
	0x19, 0xED, 0xBE,
	0x34, 0x98, 0x7C,
	0xE9, 0xAF, 0x4F,
	0x1F, 0x26, 0x25,
	0xC4, 0x49, 0x86,
	0x1D, 0x70, 0x5D,
	0x3B, 0xB6, 0x7A,
	0xE3, 0x69, 0x7E,
	0x20, 0x0D, 0xF6,
	0xC1, 0x1F, 0x42,
	0x1F, 0x0D, 0x39,
	0x3E, 0xE0, 0xBE,
	0xE0, 0xE4, 0xD1,
	0x1E, 0x73, 0x0B,
	0xC7, 0x5C, 0x25,
	0x1A, 0x76, 0xF7,
	0x38, 0xA3, 0xDA,
	0xE7, 0x15, 0xFC,
	0x77, 0xAD, 0x73,
	0x05, 0xE7, 0x38,
	0x4F, 0x66, 0x5A,
	0x7A, 0x18, 0xC8,
	0x38, 0xEC, 0x33,
	0x1F, 0xBC, 0x8D,
	0xC1, 0x9F, 0xFA,
	0x1E, 0xAC, 0xB8,
	0x3E, 0x60, 0x05,
	0xE1, 0x96, 0xB9,
	0x5C, 0x20, 0x91,
	0x1B, 0x22, 0x17,
	0x57, 0xA0, 0xEE,
	0x64, 0xDD, 0xE9,
	0x4C, 0x3E, 0x81,
	0x1F, 0xDF, 0xD4,
	0xC0, 0x40, 0x56,
	0x1F, 0xDF, 0xD4,
	0x3F, 0xBF, 0x68,
	0xE0, 0x40, 0x16,
	0x4B, 0x34, 0x04,
	0x69, 0x97, 0xF7,
	0x4B, 0x34, 0x04,
	0x5F, 0x9C, 0x06,
	0x32, 0xCB, 0xF4,
	0x1F, 0xD9, 0xF1,
	0xC0, 0x4C, 0x1C,
	0x1F, 0xD9, 0xF1,
	0x3F, 0xB3, 0x62,
	0xE0, 0x4B, 0x9A,
	0x7F, 0xA4, 0xE9,
	0x00, 0xB6, 0x2E,
	0x7F, 0xA4, 0xE9,
	0x64, 0x30, 0xDD,
	0x65, 0x9D, 0x38,

};
#endif

#if 0
uint8_t const VolTab[] = {
  //PRESETROM3 area
  0x37,         //0   -42.0dB    //refer to p209 of firmware manual
  0x33,         //1   -40.0dB    //refer to p209 of firmware manual
  0x2F,         //2   -38.0dB
  0x2B,         //3   -36.0dB
  0x27,         //4   -34.0dB
  0x23,         //5   -32.0dB
  0x1F,         //6   -30.0dB
  0x1B,         //7   -28.0dB
  0x17,         //8   -26.0dB
  0x13,         //9   -24.0dB     initial volume
  0x0F,         //10  -22.0dB
  0x0B,         //11  -20.0dB
  0x07,         //12  -18.0dB
  0x03,         //13  -16.0dB
  //PRESETROM2 area
  0xFF,         //14  -14.0dB
  0xFB,         //15  -12.0dB
  0xF7,         //16  -10.0dB
  0xF3,         //17  -08.0dB
  0xEF,         //18  -06.0dB
  0xEB,         //19  -04.0dB
  0xE7,         //20  -02.0dB
  0xE3,         //21  -00.0dB
  0xDF,         //22  +02.0dB
  0xDB,         //23  +04.0dB
  0xD7,         //24  +06.0dB
  0xD3,         //25  +08.0dB
  0xCF,         //26  +10.0dB
  0xCB,         //27  +12.0dB
  0xC7,         //28  +14.0dB
  0xC3,         //29  +16.0dB
  0xBF          //30  +18.0dB
};
#else
#define		MAX_VOL_TAB		31
uint8_t const VolTab[MAX_VOL_TAB] = {
  //PRESETROM3 area
  0x37,         //0   -42.0dB    //refer to p209 of firmware manual
  0x33,         //1   -40.0dB    //refer to p209 of firmware manual
  0x2f,
  0x2c,
  0x2a,
  0x28,
  0x26,
  0x24,
  0x22,
  0x1f,
  0x1c,
  0x1a,
  0x18,
  0x16,
  0x14,
  0x12,
  0x10,        
  0x0e,        
  0x0c,    
  0x09,         //4   
  0x07,        
  0x05,        
  0x03,         
  0x01,         
  //PRESETROM2 area
  0xfd,
  0xfb,
  0xf9,
  0xf7,
  0xf5,
  0xF3,  
  0xf1,		//-7db

//  0xF7,         //16  -10.0dB
 
};

#endif
struct flags
  {
     unsigned bit0     :1;
     unsigned bit1     :1;
     unsigned bit2     :1;
     unsigned bit3     :1;
     unsigned bit4     :1;
     unsigned bit5     :1;
     unsigned bit6     :1;
     unsigned bit7     :1;
  };
struct flags flag0;
#define yda174_status flag0.bit0
#define play_standby flag0.bit1
uint8_t ext_dsp_vol;
//#define EXT_DSP_INIT_VOL 10//20
uint8_t   ram_bank_bak;
uint8_t I2C_limitedTimer;
uint16_t DSP_Timer;
#define YDADSP_DATA_BUFF_SIEZ 7
uint8_t   yda_data_buff[YDADSP_DATA_BUFF_SIEZ];
//I2C_MESSAGE_STATUS i2c_status;
#if defined (__XC32__)      //PIC32 harmony based driver
#define DSP_I2C_ADDR1 0xD8
#else               //MCC I2C driver
#define DSP_I2C_ADDR1 0x6C//0xD8
#endif


/*-----------------------------------------------------------------------------
Function name:
  yda174dsp_init
Description:
  This function used to initialize dsp

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
static bool DSP_Write(uint16_t dataLen, uint8_t* data)
{
#if defined (__XC32__)    
    DRV_I2C_BUFFER_EVENT result;
    while( !(I2C_bufferHandle == NULL 
            || DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE 
            || DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR ) )
    {
    }
    I2C_bufferHandle = DRV_I2C_Transmit(I2C_Handle, DSP_I2C_ADDR1, data, dataLen, NULL);

    I2C_limitedTimer = 10;

    while(I2C_limitedTimer)
    {
        result = DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle);
        if(result == DRV_I2C_BUFFER_EVENT_COMPLETE)
        {
            return true;
        }
        else if(result == DRV_I2C_BUFFER_EVENT_ERROR)
        {
            //LED5_TOGGLE();
            return false;
        }
    }
    return false;
#else
    while(I2C_MasterQueueIsFull() == true);

    I2C_MasterWrite(data, dataLen, DSP_I2C_ADDR1, &i2c_status);

    I2C_limitedTimer = 5;
    while(!I2C_IsIdle())
    {
        if(!I2C_limitedTimer)
            return false;
    }    
    return true;
#endif        
}

void DSP_Timer1MS_event(void)
{
    //DSP_Timer++;
    if(DSP_Timer)
        --DSP_Timer;
    if(I2C_limitedTimer)
        --I2C_limitedTimer;
}
void Delay_Some(unsigned int D_Count)
{
    unsigned int i;
    for (i= 0; i< D_Count; i++)
        Nop();
}
enum {
    ST_YDA174_IDLE, //nothing
    ST_YDA174_START,    //start to init
    ST_YDA174_RESET_LOW_WAIT,
    ST_YDA174_RESET_HIGH_WAIT,
    ST_YDA174_INIT_1,
    ST_YDA174_INIT_2,
    ST_YDA174_INIT_3,
    ST_YDA174_INIT_4,
    ST_YDA174_INIT_5,
    ST_YDA174_INIT_6,
    ST_YDA174_INIT_7,
    ST_YDA174_INIT_8,
    ST_YDA174_INIT_9,
    ST_YDA174_INIT_10,
    ST_YDA174_INIT_11,
    ST_YDA174_INIT_12,
    ST_YDA174_INIT_13,
    ST_YDA174_INIT_14,
    ST_YDA174_INIT_15,
    ST_YDA174_INIT_16,
    ST_YDA174_INIT_17,
    ST_YDA174_INIT_18,
    ST_YDA174_INIT_19,
    ST_YDA174_INIT_20,
    ST_YDA174_INIT_21,
    ST_YDA174_INIT_22,
    ST_YDA174_INIT_23,
    ST_YDA174_INIT_24,
    ST_YDA174_INIT_25,
    ST_YDA174_INIT_26,
    ST_YDA174_INIT_27,
    ST_YDA174_INIT_28,
    ST_YDA174_INIT_29,
    //ST_YDA174_INIT_30,
    ST_YDA174_SETUP_EQ = 50,
    ST_YDA174_SETUP_VOL,
    ST_YDA174_RUNNING,
    ST_YDA174_STARTING_POWER_OFF,
    ST_YDA174_POWER_OFF_1,
    ST_YDA174_POWER_OFF_2
};
uint8_t yda174_state;
void yda174dsp_init(void)
{
    ext_dsp_vol = EXT_DSP_INIT_VOL;
    yda174_state = ST_YDA174_IDLE;
}

bool is_yda174dsp_idle(void)
{
    if(yda174_state == ST_YDA174_IDLE)
        return true;
    else
        return false;
}
bool is_yda174dsp_ready(void)
{
    if(yda174_state == ST_YDA174_RUNNING || yda174_state == ST_YDA174_STARTING_POWER_OFF)
        return true;
    else
        return false;
}
void yda174dsp_start(void)
{
    ENABLE_DSP_POWER();
    DSP_Timer = 200;
    yda174_state = ST_YDA174_START;
}


void yda174dsp_task(void)
{
    switch(yda174_state)
    {
        case ST_YDA174_IDLE:
            break;
            
        case ST_YDA174_START:
            if(!DSP_Timer)
            {
                DSP_RESET_SetLow();//RST_DSP = LOW;
                DSP_POWER_DN_SetLow();//DSP_SLEEP = LOW;       //step1-A
                DSP_Timer = 15;
                yda174_state = ST_YDA174_RESET_LOW_WAIT;
            }
            break;
            
        case ST_YDA174_RESET_LOW_WAIT:
            if(!DSP_Timer)
            {
                DSP_RESET_SetHigh();//RST_DSP = HIGH;             //step1-B
                DSP_Timer = 5;
                yda174_state = ST_YDA174_RESET_HIGH_WAIT;
            }
            break;
            
        case ST_YDA174_RESET_HIGH_WAIT:
            if( DSP_Timer )
                break;
            DSP_POWER_DN_SetHigh();//DSP_SLEEP = HIGH;       //step1-C
            DSP_PROT_SetInput();
            DSP_IRQ_SetInput();
        
        case ST_YDA174_INIT_1:
            yda174_state = ST_YDA174_INIT_1;
            yda_data_buff[0] = PRESETROM0;
            yda_data_buff[1] = 0xC1;                    //step2   set to A/V mode, refer to 81/p199 of firmware manual
            Delay_Some(DELAY_NUM);
            if( DSP_Write( 2, yda_data_buff ) )
            {
                //LED4_OFF();
                yda174_state++;
            }
            else
            {
                //LED4_ON();
            }
            break;

        case ST_YDA174_INIT_2:
      /*      yda_data_buff[0] = PRESETROM0;
            yda_data_buff[1] = 0xD5; //5;                //HS 10Band PEQ +0, 3b, refer to 83/p199 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);*/
            yda174_state++;
            break;

        case ST_YDA174_INIT_3:
       /*     yda_data_buff[0] = PRESETROM2;
            yda_data_buff[1] = 0xBE; //step 3    SV SDSP Sel, refer to 139/p207 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);*/
            yda174_state++;
            break;

        case ST_YDA174_INIT_4:
            yda_data_buff[0] = RAM_WRITE_READ;
            yda_data_buff[1] = 0x40;
            yda_data_buff[2] = 0x7F;
            yda_data_buff[3] = 0x00;
            yda_data_buff[4] = 0x07;
            yda_data_buff[5] = 0x00;
            yda_data_buff[6] = 0x07; //step4  ,refer to p76 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(7, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_5:
      /*      yda_data_buff[0] = PRESETROM2;
            yda_data_buff[1] = 0x44; //step5-A       LPS No Delay, refer to 85/p205 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);*/
            yda174_state++;
            break;

        case ST_YDA174_INIT_6:
      /*      yda_data_buff[0] = PRESETROM2;
            yda_data_buff[1] = 0x6D; //P3D DTV Sports, refer to 109/p2XX of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);*/
            yda174_state++;
            break;

        case ST_YDA174_INIT_7:
    /*        yda_data_buff[0] = PRESETROM3;
            yda_data_buff[1] = 0xD6; //D3;  step5-B     DTV Theater/Sports, refer to p163/213 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);*/
            yda174_state++;
            break;

        case ST_YDA174_INIT_8:
            yda_data_buff[0] = AIFMD;
            yda_data_buff[1] = AIFMD_EN_IN_INSIDE; //step7
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_9:
            yda_data_buff[0] = MDSPMD;
            yda_data_buff[1] = MDSPMD_RUN_TRANSFER; //step8
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_10:
            yda_data_buff[0] = GAIN;
            yda_data_buff[1] = GAIN_P6DB; //step9
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_11:
            yda_data_buff[0] = GAIN;
            yda_data_buff[1] = GAIN_N6DB; //GAIN_P6DB;              //step9
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_12:
            yda_data_buff[0] = AIFIFMT;
            yda_data_buff[1] = (AIFIFMT_INPUT_I2S_FORMAT | AIFIFMT_24BIT | AIFIFMT_START_FRAME_FALLING | AIFIFMT_CHANGE_BCLK_FALLING); //step10-A
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_13:
            yda_data_buff[0] = AIFOFMT;
            yda_data_buff[1] = (AIFOFMT_OUTPUT_I2S_FORMAT | AIFOFMT_24BIT | AIFOFMT_START_FRAME_FALLING | AIFOFMT_CHANGE_BLCK_FALLING); //step10-B
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_14:
            yda_data_buff[0] = SDSPMD;
            yda_data_buff[1] = SDSPMD_DEFAULT; //step10-C
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_15:
            yda_data_buff[0] = DAMPMD;
            yda_data_buff[1] = DAMPMD_DEFAULT; //step10-D
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_16:
            yda_data_buff[0] = IRQN;
            yda_data_buff[1] = IRQN_DIS_TO_IRQPIN;
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_17:
            yda_data_buff[0] = FSM;
            yda_data_buff[1] = FSM_DEFAULT; //step10-E
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_18:
            yda_data_buff[0] = PLLERR;
            yda_data_buff[1] = PLLERR_ERR_DETECT_DIS; //step10-F
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_19:
            yda_data_buff[0] = DSPSTART;
            yda_data_buff[1] = (DSPSTART_MDSP2_START | DSPSTART_SDSP_START); //step11
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_20:
            
            yda_data_buff[0] = PRESETROM2;
            yda_data_buff[1] = 0x43; //step12-B      LPS default, refer to 85/p205 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
	
            yda174_state++;
            break;

        case ST_YDA174_INIT_21:
			
            yda_data_buff[0] = PRESETROM1;
            yda_data_buff[1] = 0x31; //step12-B      ER default, refer to 100/p0 of firmware manual
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            break;

        case ST_YDA174_INIT_22:

		yda_data_buff[0] = PRESETROM1;
		yda_data_buff[1] = 0x24;                   // DRC3 music, refer to 115/p2xx of firmware manual
		Delay_Some(DELAY_NUM);
		DSP_Write( 2, yda_data_buff );
            yda174_state++;
            break;

        case ST_YDA174_INIT_23:
		  yda_data_buff[0] = PRESETROM2;
		  yda_data_buff[1] = 0x67;                   //P3D default, refer to 109/p2XX of firmware manual
		  Delay_Some(DELAY_NUM);
		  DSP_Write( 2, yda_data_buff );
            yda174_state++;
            break;

        case ST_YDA174_INIT_24:
		  yda_data_buff[0] = PRESETROM2;
		  yda_data_buff[1] = 0x96;                   //RE
		  Delay_Some(DELAY_NUM);
		  DSP_Write( 2, yda_data_buff );
            yda174_state++;
            break;

        case ST_YDA174_INIT_25:
		  yda_data_buff[0] = PRESETROM2;
		  yda_data_buff[1] = 0x60;                   // PEQ Through
		  Delay_Some(DELAY_NUM);
		  DSP_Write( 2, yda_data_buff );		 
            yda174_state++;
            break;

        case ST_YDA174_INIT_26:
		  yda_data_buff[0] = PRESETROM1;
		  yda_data_buff[1] = 0x4e;                   // GEQ default
		  Delay_Some(DELAY_NUM);
		  DSP_Write( 2, yda_data_buff );	

            yda174_state++;
            break;

        case ST_YDA174_INIT_27:
            yda_data_buff[0] = PRESETROM2;
            yda_data_buff[1] = 0xBe; //step13-A   SV select
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);

            yda_data_buff[0] = PRESETROM0;
            yda_data_buff[1] = 0xf9; //   CF Default
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
			
            yda174_state++;
            break;

        case ST_YDA174_INIT_28:
            yda_data_buff[0] = MUTE; 
            yda_data_buff[1] = (MUTE_MDSP2MUTE_CANCLE | MUTE_SDSPMUTE_CANCLE); //step13-C
            Delay_Some(DELAY_NUM);
            DSP_Write(2, yda_data_buff);
            yda174_state++;
            DSP_Timer = 20;
            break;

        case ST_YDA174_INIT_29:
            if (!DSP_Timer) {
                yda_data_buff[0] = MUTE;
                yda_data_buff[1] = (MUTE_QMUTE_EN | MUTE_MDSP2MUTE_CANCLE | MUTE_SDSPMUTE_CANCLE); //step13-D
                Delay_Some(DELAY_NUM);
                DSP_Write(2, yda_data_buff);
                yda174_state = ST_YDA174_SETUP_EQ;
            }
            break;
            
        case ST_YDA174_SETUP_EQ:
            //yda174_soundeffec0();
            Delay_Some(DELAY_NUM);
            DSP_Write(213, PEQTab );
            yda174_state = ST_YDA174_SETUP_VOL;
            break;
            
        case ST_YDA174_SETUP_VOL:
            yda174_voladj();
            DSP_MUTE_SetHigh();//MUTE_DSP = HIGH;
            yda174_state = ST_YDA174_RUNNING;
            break;
            
        case ST_YDA174_RUNNING:
            if(BTAPP_GetStatus() == BT_STATUS_OFF)
            {
                DSP_Timer = 5000;       //wait 5 seconds
                yda174_state = ST_YDA174_STARTING_POWER_OFF;
            }
            break;
            
        case ST_YDA174_STARTING_POWER_OFF:
            if(BTAPP_GetStatus() == BT_STATUS_READY || BTAPP_GetStatus() == BT_STATUS_ON)
            {
                yda174_state = ST_YDA174_RUNNING;
                break;
            }
            if(!DSP_Timer)
            {
                DSP_MUTE_SetLow();
                DSP_Timer = 5;
                yda174_state = ST_YDA174_POWER_OFF_1;
            }
            break;
            
        case ST_YDA174_POWER_OFF_1:
            if(!DSP_Timer)
            {
                DSP_POWER_DN_SetLow();
                DSP_Timer = 5;
                yda174_state = ST_YDA174_POWER_OFF_2;
            }
            break;
            
        case ST_YDA174_POWER_OFF_2:
            if(!DSP_Timer)
            {
                DSP_RESET_SetLow();
                yda174_state = ST_YDA174_IDLE;
            }
            break;
    }
}


/*-----------------------------------------------------------------------------
Function name:
  yda174_audio_interace_setting
Description:
  This function used to enable playback

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
//void yda174_playback(void)
//{
//  DSP_POWER_DN_SetHigh();//DSP_SLEEP = HIGH;
//  DSP_MUTE_SetHigh();//MUTE_DSP = HIGH;
//  yda174_status = true;
//  play_standby = false;
//}


/*-----------------------------------------------------------------------------
Function name:
  yda174_stop
Description:
  This function used to stop playbck

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
//void yda174_stop(void)
//{
//  yda174_status = false;
//  play_standby = true;
//}



/*-----------------------------------------------------------------------------
Function name:
  yda174_voladj
Description:
  This function used to set yda174 volume

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_voladj(void)
{
	if (ext_dsp_vol >=MAX_VOL_TAB)
	{
		return;
	}

    yda_data_buff[1] = VolTab[ ext_dsp_vol ];     //Software volume
    if ( yda_data_buff[1] >= 0xBF )
    {
        yda_data_buff[0] = PRESETROM2;
    }
    else
    {
        yda_data_buff[0] = PRESETROM3;
    }

    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
}

/*-----------------------------------------------------------------------------
Function name:
  yda174_QMute
Description:
  This function used to set yda174 quite mute

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_SoftMuteOff(void)
{
    yda_data_buff[0] = MUTE;
    yda_data_buff[1] = (MUTE_MDSP2MUTE_CANCLE | MUTE_SDSPMUTE_CANCLE);  //step13-D
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
  
    yda_data_buff[0] = MUTE;
    yda_data_buff[1] = (MUTE_QMUTE_EN | MUTE_MDSP2MUTE_CANCLE | MUTE_SDSPMUTE_CANCLE);  //step13-D
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
}


/*-----------------------------------------------------------------------------
Function name:
  yda174_AllMute
Description:
  This function used to set yda174 all mute

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_SoftMute(void)
{
    yda_data_buff[0] = MUTE;
    yda_data_buff[1] = (MUTE_MDSP2MUTE_CANCLE | MUTE_SDSPMUTE_CANCLE); //step13-D
    Delay_Some(DELAY_NUM);
    DSP_Write(2, yda_data_buff);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
  
    yda_data_buff[0] = MUTE;
    yda_data_buff[1] = MUTE_DEFAULT;  //step13-D
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );  
}

/*-----------------------------------------------------------------------------
Function name:
  yda174_DSPStop
Description:
  This function used to stop yda174 dsp

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_DSPStop(void)
{
    yda_data_buff[0] = DSPSTART;
    yda_data_buff[1] = DSPSTART_DEFAULT;  //step11
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
}

/*-----------------------------------------------------------------------------
Function name:
  yda174_DSPStart
Description:
  This function used to start yda174 dsp

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_DSPStart(void)
{
    yda_data_buff[0] = DSPSTART;
    yda_data_buff[1] = (DSPSTART_MDSP2_START | DSPSTART_SDSP_START );
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
}

/*-----------------------------------------------------------------------------
Function name:
  yda174_setHighSamplingrateInput
Description:
  This function used to enable DCM bit when input sampling rate is over 50KHZ

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_setHighSamplingrateInput(void)
{
    //yda174_SoftMute();    
    //yda174_DSPStop();
    
    yda_data_buff[0] = SDSPMD;
    yda_data_buff[1] = (SDSPMD_DEFAULT | SDSPMD_SRC_MODE_GREATER);              //step10-C
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
  
    yda_data_buff[0] = RAMCLR;
    yda_data_buff[1] = 0x03;
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    //yda174_DSPStart();
    //yda174_SoftMuteOff();
}

/*-----------------------------------------------------------------------------
Function name:
  yda174_setNormalSamplingrateInput
Description:
  This function used to disable DCM bit when input sampling rate is lower or equal to 50KHZ

Notice:
  refer to p168~176 of the firmware manual of yamaha for intialization flow chart
  refer to p29~38 of yda174a30 for i2c interface
  refer to p111~ of yda174a30 for power on flow chart

Input Parameter:
  None
Return:
  None
*/
void yda174_setNormalSamplingrateInput(void)
{
    //yda174_SoftMute();    
    //yda174_DSPStop();
    
    yda_data_buff[0] = SDSPMD;
    yda_data_buff[1] = SDSPMD_DEFAULT;              //step10-C
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );

    yda_data_buff[0] = RAMCLR;
    yda_data_buff[1] = 0x03;
    Delay_Some(DELAY_NUM);
    DSP_Write( 2, yda_data_buff );
    Delay_Some(DELAY_NUM);
    Delay_Some(DELAY_NUM);
    //yda174_DSPStart();
    //yda174_SoftMuteOff();
}

/*-----------------------------------------------------------------------------
 *
*/
void yda174_vol_up(void)
{
    if(!ext_dsp_vol)
        yda174_SoftMuteOff();
    if(ext_dsp_vol < MAX_VOL_TAB)
    {
        ext_dsp_vol++;
        yda174_voladj();
    }
}


/*-----------------------------------------------------------------------------
 *
*/
void yda174_vol_down(void)
{
    if(ext_dsp_vol > 0)
    {
        ext_dsp_vol --;
        yda174_voladj();

        if(!ext_dsp_vol)
            yda174_SoftMute();
    }
}

/*-----------------------------------------------------------------------------
 *
*/
void yda174_SetVol(uint8_t vol)
{
    if(ext_dsp_vol != vol)
    {
        if(!vol)        //new volume is 0, old volume is non-zero.
            yda174_SoftMute();
        else if(!ext_dsp_vol)       //old volume is 0, new volume is non-zero
            yda174_SoftMuteOff();
        if (vol < MAX_VOL_TAB) {
            ext_dsp_vol = vol;
            yda174_voladj();
        }
    }
}
