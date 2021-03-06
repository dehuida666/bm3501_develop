

#include <xc.h>
#include "bt_hardware_profile.h"
#include "bt_app.h"
#include "nf8230dsp_handler.h"
#include "bt_command_send.h"
#include "user_tone.h"
#include "bt_hfp.h"
#include "user_i2c.h"


#if defined (__XC32__)      //PIC32 harmony based driver

extern DRV_HANDLE I2C_Handle;
//DRV_I2C_BUFFER_HANDLE I2C_bufferHandle = NULL;

#endif



//I2C_MESSAGE_STATUS i2c_status;
#if defined (__XC32__)      //PIC32 harmony based driver
#define DSP_I2C_ADDR_LR 0x54
#define DSP_I2C_ADDR_SW 0x56
#else               //MCC I2C driver
#define DSP_I2C_ADDR1 0x6C//0xD8
#endif

//#define VOL_MAX 9
#define MASTER_VOLUME_REG 0x2e
#define TREBLE_CH1_VOLUME_REG 0x2f
#define TREBLE_CH2_VOLUME_REG 0x30
//#define BASS_VOLUME_REG 0x31
#define treble_step 3
#define bass_step 7
#define VOLUME_0db 0xCF

#define vol_tone   0xCF


extern uint8_t I2C_limitedTimer;
extern uint16_t DSP_Timer;

uint8_t ntp8230g_state;

uint8_t volume_master_step;
uint8_t volume_treble_step;
uint8_t volume_bass_step;

uint8_t sound_is_mute_flag;

uint8_t I2C_limitedTimer;
uint16_t DSP_Timer;

bool is_master_volume_decrease_20dB_flag = false;
uint16_t master_volume_decrease_20dB_timer1ms = 0;
bool master_volume_decrease_20dB_TimeOutFlag = false;

bool set_master_volume_temp_Flag = false;

uint8_t Volume_100msTimer = 100;
bool Volume_100msTimeOutFlag = false;

uint16_t ringTone_1msTimer = 0;
bool ringTone_1msTimeOutFlag = false;


typedef struct _NSP_INIT_STRUCT_
{
	unsigned char RegAddr;
	unsigned char RegData;
} NSP_INIT_STRUCT;

typedef struct _COEFFICIENT_STRUCT_
{
	unsigned char RegAddr;
	unsigned int RegData;
} COEFFICIENT_STRUCT;


static const uint8_t volume_master_step_gain[VOL_MAX] = 
{
	0x00,//-150
	0x53,//-62
	0x7F,//-40
	0x93,//-30
	0x9B,//-26
	0xA3,//-22
	0xA9,//-19
	0xAF,//-16
	0xB5,//-13
	0xBB,//-10
	0xC1,//-7
	0xC5,//-5
	0xC9,// -3
	0xCB,// -2
	0xCD,// -1
	0xD1,// 1
	0xD5, // 3

};

static const uint8_t volume_master_SW_step_gain[VOL_MAX] = 
{
	0x00,
	0x6A,//-50.5
	0x7F,//-40
	0x97,//-28
	0x9F,//-24
	0xA9,//-19
	0xB1,//-15
	0xB7,//-12
	0xBD,//-9
    0xC1,//-7
	0xC5,//-5
	0xC9,//-3
	0xCB,//-2
	0xCD,// -1
	0xCF, //0
	0xD3, // 2
	0xD7, // 4

};

//20190727    Invalid, The volume will not be set.

static const COEFFICIENT_STRUCT volume_treble_frequency_point[] = 
{
	//-8	
	0x37, 0x0f5bc16f,
	0x38, 0x10af3802,
	0x39, 0x0f11b2a6,
	0x3a, 0x115ee712,
	0x3b, 0x10c5502c,
	
	//-6	
	0x37, 0x1007b951,
	0x38, 0x10db6a2f,
	0x39, 0x0f3819f2,
	0x3a, 0x115cf99f,
	0x3b, 0x10c24f58,
	
	//-4	
	0x37, 0x1027ad9c,
	0x38, 0x1189485e,
	0x39, 0x0f6861f8,
	0x3a, 0x115af05a,
	0x3b, 0x10bf2e91,
	
	// -2	
	0x37, 0x104f2dda,
	0x38, 0x11abab45,
	0x39, 0x1012825f,
	0x3a, 0x1158c9dc,
	0x3b, 0x10bbed64,

	//0	
	0x37, 0x11000000,
	0x38, 0x11f66fc8,
	0x39, 0x106d8cba,
	0x3a, 0x11766fc8,
	0x3b, 0x10ed8cba,
	
	// 2
	0x37, 0x111e29a5,
	0x38, 0x1285efda,
	0x39, 0x1068362b,
	0x3a, 0x11541f44,
	0x3b, 0x10b50898,

	// 4
	0x37, 0x11436c07,
	0x38, 0x12a72179,
	0x39, 0x1111f126,
	0x3a, 0x1151981c,
	0x3b, 0x10b164af,

	// 6	
	0x37, 0x11716e6c,
	0x38, 0x12d0664e,
	0x39, 0x11374083,
	0x3a, 0x114eed9a,
	0x3b, 0x10ad9fd6,

	// 8	
	0x37, 0x12151c72,
	0x38, 0x1381d542,
	0x39, 0x1165db2e,
	0x3a, 0x114c1e22,
	0x3b, 0x10a9ba55,
};

static const COEFFICIENT_STRUCT volume_bass_frequency_point[] = 
{
	//-10		
	0x1e, 0x107f2196,
	0x1f, 0x11feba39,
	0x20, 0x107e53dd,
	0x21, 0x117eba39,
	0x22, 0x10fd7573,	
	
	//-7	
	0x1e, 0x107f6873,
	0x1f, 0x11feed9a,
	0x20, 0x107e73c2,
	0x21, 0x117eed9a,
	0x22, 0x10fddc34,	
	
	//-4		
	0x1e, 0x107faae5,
	0x1f, 0x11ff18e4,
	0x20, 0x107e87e3,
	0x21, 0x117f18e4,
	0x22, 0x10fe32c8,	
	
	// -2		
	0x1e, 0x107fd5b6,
	0x1f, 0x11ff31e3,
	0x20, 0x107e8f11,
	0x21, 0x117f31e3,
	0x22, 0x10fe64c7,	

	//0	
	0x1e, 0x11000000,
	0x1f, 0x11ffaa1a,
	0x20, 0x107f54a6,
	0x21, 0x117faa1a,
	0x22, 0x10ff54a6,	
	
	// 2		
	0x1e, 0x11001528,
	0x1f, 0x11ff5c12,
	0x20, 0x107e8ed4,
	0x21, 0x117f5c12,
	0x22, 0x10feb925,	

	// 4		
	0x1e, 0x11002a9c,
	0x1f, 0x11ff6dce,
	0x20, 0x107e8766,
	0x21, 0x117f6dce,
	0x22, 0x10fedc9d,	

	// 6			
	0x1e, 0x110040a2,
	0x1f, 0x11ff7d9d,
	0x20, 0x107e7af8,
	0x21, 0x117f7d9d,
	0x22, 0x10fefc3c,	

	// 8		
	0x1e, 0x11005786,
	0x1f, 0x11ff8bb8,
	0x20, 0x107e6963,
	0x21, 0x117f8bb8,
	0x22, 0x10ff1870,
};




/*-----------------------------------------------------------------------------*/
//eq data
//LR
const NSP_INIT_STRUCT NTP8230_Eq1_Data_LR[]={
  0x02, 0x00,
  0x00, 0x00,
  0x2e, 0xd5,
  0x2d, 0x00,
  0x2f, 0xcf,
  0x30, 0xcf,
  0x31, 0xcf,
  0x15, 0xf3,
  0x16, 0xb3,
  0x2c, 0x43,
  0x52, 0x90,
  0x62, 0x1a,
  0x01, 0x00,
  0x03, 0x00,
  0x04, 0x4e,
  0x05, 0x4e,
  0x06, 0x00,
  0x07, 0x36,
  0x08, 0x36,
  0x32, 0x00,
  0x0f, 0x00,
  0x10, 0x00,
  0x11, 0x00,
  0x12, 0x00,
  0x13, 0x00,
  0x14, 0x00,
  0x33, 0x00,
  0x34, 0x00,
  0x55, 0x0f,
  0x2b, 0x0b,
  0x35, 0x08,
  0x36, 0x1a,
  0x37, 0x2c,
  0x7e, 0x03,
};
const COEFFICIENT_STRUCT NTP8230_Eq2_Data_LR[]={
  0x46, 0x092010ab,
  0x47, 0x092010ab,
  0x48, 0x20000000,
  0x49, 0x107d7fbd,
  0x4a, 0x20000000,
  0x4b, 0x014923fc,
  0x4c, 0x024923fc,
  0x4d, 0x014923fc,
  0x4e, 0x117ebcbd,
  0x4f, 0x10fd7fc3,
  0x50, 0x107ebfdf,
  0x51, 0x10febfdf,
  0x52, 0x20000000,
  0x53, 0x107d7fbd,
  0x54, 0x20000000,
  0x55, 0x107ebe4f,
  0x56, 0x11febe4f,
  0x57, 0x107ebe4f,
  0x58, 0x117ebcbd,
  0x59, 0x10fd7fc3,
};
const NSP_INIT_STRUCT NTP8230_Eq3_Data_LR[]={
  0x7e, 0x00,
  0x1c, 0xa5,
  0x1d, 0x31,
  0x1e, 0x9f,
  0x1f, 0x70,
  0x25, 0x11,
  0x22, 0x9f,
  0x23, 0x70,
  0x20, 0x00,
  0x7e, 0x03,
};
const COEFFICIENT_STRUCT NTP8230_Eq4_Data_LR[]={
  0x0, 0x107e72f0,
  0x1, 0x11fe72f0,
  0x2, 0x107e72f0,
  0x3, 0x117e71c3,
  0x4, 0x10fce83c,
  0x5, 0x116bd1ff,
  0x6, 0x12bdea7b,
  0x7, 0x111ef5a4,
  0x8, 0x113aa386,
  0x9, 0x10932c68,
  0x14, 0x106ebc30,
  0x15, 0x11888054,
  0x16, 0x0f52c112,
  0x17, 0x11088054,
  0x18, 0x0fb03971,
  0x19, 0x107ed48f,
  0x1a, 0x11fd9fae,
  0x1b, 0x107c7ad3,
  0x1c, 0x117d9fae,
  0x1d, 0x10fb4f61,
  0x1e, 0x107ebe4e,
  0x1f, 0x11febe4e,
  0x20, 0x107ebe4e,
  0x21, 0x117ebcbb,
  0x22, 0x10fd7fc1,
  0x23, 0x11013e5b,
  0x24, 0x11fb8517,
  0x25, 0x10750219,
  0x26, 0x117b8517,
  0x27, 0x10f77ecf,
  0x28, 0x1101fde8,
  0x29, 0x11f20742,
  0x2a, 0x1068b419,
  0x2b, 0x11720742,
  0x2c, 0x10ecafe9,
  0x2d, 0x107cd761,
  0x2e, 0x11f6a78b,
  0x2f, 0x1074b96c,
  0x30, 0x1176a78b,
  0x31, 0x10f190cf,
  0x32, 0x1073a380,
  0x33, 0x11b90c2e,
  0x34, 0x1037b5df,
  0x35, 0x11390c2e,
  0x36, 0x10ab595f,
  0x37, 0x11000000,
  0x38, 0x11f66fc8,
  0x39, 0x106d8cba,
  0x3a, 0x11766fc8,
  0x3b, 0x10ed8cba,

};

const NSP_INIT_STRUCT NTP8230_Eq5_Data_LR[]={
  0x7e, 0x04,
};

const COEFFICIENT_STRUCT NTP8230_Eq6_Data_LR[]={
  0xa, 0x107dc4d0,
  0xb, 0x11efe5c9,
  0xc, 0x1072f3a6,
  0xd, 0x116fe5c9,
  0xe, 0x10f0b875,
  0xf, 0x1070af67,
  0x10, 0x11b8443e,
  0x11, 0x105fd739,
  0x12, 0x1138443e,
  0x13, 0x10d0869f,
};


const NSP_INIT_STRUCT NTP8230_Eq7_Data_LR[]={ 
  0x7e, 0x00,
  0x0c, 0x3f,
  0x17, 0x1f,
  0x19, 0x15,
  0x18, 0x1f,
  0x1a, 0x15,
  0x1b, 0x15,  
  0x56, 0x20,
  0x28, 0x04,
  0x27, 0x00,
  0x26, 0x00,
};

//SW
const NSP_INIT_STRUCT NTP8230_Eq1_Data_Bass[]={
  0x02, 0x00,
  0x00, 0x00,
  0x2e, 0xd7,
  0x2d, 0x00,
  0x2f, 0xcf,
  0x30, 0xcf,
  0x31, 0xcf,
  0x15, 0xf7,
  0x16, 0xb3,
  0x2c, 0x43,
  0x52, 0x90,
  0x62, 0x1a,
  0x01, 0x00,
  0x03, 0x42,
  0x04, 0x42,
  0x05, 0x42,
  0x06, 0x42,
  0x07, 0x36,
  0x08, 0x36,
  0x32, 0x00,
  0x0f, 0x00,
  0x10, 0x00,
  0x11, 0x00,
  0x12, 0x00,
  0x13, 0x00,
  0x14, 0x00,
  0x33, 0x00,
  0x34, 0x00,
  0x55, 0x0f,
  0x2b, 0x0b,
  0x35, 0x08,
  0x36, 0x18,
  0x37, 0x12,
  0x7e, 0x03,
};
const COEFFICIENT_STRUCT NTP8230_Eq2_Data_Bass[]={
  0x46, 0x092010ab,
  0x47, 0x092010ab,
  0x48, 0x20000000,
  0x49, 0x107d7fbd,
  0x4a, 0x20000000,
  0x4b, 0x014923fc,
  0x4c, 0x024923fc,
  0x4d, 0x014923fc,
  0x4e, 0x117ebcbd,
  0x4f, 0x10fd7fc3,
  0x50, 0x107ebfdf,
  0x51, 0x10febfdf,
  0x52, 0x20000000,
  0x53, 0x107d7fbd,
  0x54, 0x20000000,
  0x55, 0x107ebe4f,
  0x56, 0x11febe4f,
  0x57, 0x107ebe4f,
  0x58, 0x117ebcbd,
  0x59, 0x10fd7fc3,
};
const NSP_INIT_STRUCT NTP8230_Eq3_Data_Bass[]={
  0x7e, 0x00,
  0x1c, 0x8d,
  0x1d, 0x32,
  0x1e, 0xb3,
  0x1f, 0x33,
  0x25, 0x11,
  0x22, 0x9a,
  0x23, 0x32,
  0x20, 0x00,
  0x7e, 0x03,
};
const COEFFICIENT_STRUCT NTP8230_Eq4_Data_Bass[]={
  0x5, 0x11002c85,
  0x6, 0x11ff27ac,
  0x7, 0x107df74f,
  0x8, 0x117f27ac,
  0x9, 0x10fe5059,
  0x14, 0x107ff0cb,
  0x15, 0x11ffe996,
  0x16, 0x107fe2ba,
  0x17, 0x117fe996,
  0x18, 0x10ffd385,
  0x1e, 0x11000000,
  0x1f, 0x11ffaa1a,
  0x20, 0x107f54a6,
  0x21, 0x117faa1a,
  0x22, 0x10ff54a6,
  0x23, 0x11000000,
  0x24, 0x11ffe332,
  0x25, 0x107fc6d6,
  0x26, 0x117fe332,
  0x27, 0x10ffc6d6,
  0x28, 0x107ca5c6,
  0x29, 0x11fb8034,
  0x2a, 0x107a6ec0,
  0x2b, 0x117b8034,
  0x2c, 0x10f71485,
  0x2d, 0x0d5181d3,
  0x2e, 0x0eca8c48,
  0x2f, 0x0d440731,
  0x30, 0x117d4efb,
  0x31, 0x10faac04,
};
const NSP_INIT_STRUCT NTP8230_Eq5_Data_Bass[]={
  0x7e, 0x00,
  0x0c, 0x3f,
  0x17, 0x1f,
  0x19, 0x15,
  0x18, 0x1f,
  0x1a, 0x15,
  0x1b, 0x00,
  0x56, 0x20,
  0x28, 0x04,
  0x27, 0x00,
  0x26, 0x00,
};
const NSP_INIT_STRUCT NTP8230_Eq_OFF[]={
	0x26,0x07,
	0x0c,0x00,
	0x17,0x00,
	0x18,0x00,
	0x19,0x00,
	0X1A,0X00,
	0x26,0x00
};

const NSP_INIT_STRUCT NTP8230_Eq_ON[]={
	0x26,0x07,
	0x0c,0x3f,
	0x17,0x1F,
	0x18,0x1F,
	0X19,0X15,
	0X1A,0X15,
	0x26,0x00
};



/*-----------------------------------------------------------------------------*/

void DSP_Timer1MS_event(void)
{
    //DSP_Timer++;
    if(DSP_Timer)
        --DSP_Timer;
    if(I2C_limitedTimer)
        --I2C_limitedTimer;

	if(master_volume_decrease_20dB_timer1ms)
	{
		master_volume_decrease_20dB_timer1ms--;
		if(master_volume_decrease_20dB_timer1ms == 0)
			master_volume_decrease_20dB_TimeOutFlag = true;
	}

	if(Volume_100msTimer)
	{
		Volume_100msTimer--;
		if(Volume_100msTimer == 0){
			Volume_100msTimeOutFlag = true;
			Volume_100msTimer = 100;
		}

	}

	if(ringTone_1msTimer)
	{
		ringTone_1msTimer--;
		if(ringTone_1msTimer == 0)
		{
			ringTone_1msTimeOutFlag = true;

		}

	}
}



#ifdef HW_I2C 
/*-----------------------------------------------------------------------------*/
 static bool DSP_Write_LR(uint16_t dataLen, uint8_t* data)
{
#if defined (__XC32__)    
    DRV_I2C_BUFFER_EVENT result;
    while( !(I2C_bufferHandle == NULL 
            || DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE 
            || DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR ) )
    {
    }
    I2C_bufferHandle = DRV_I2C_Transmit(I2C_Handle, DSP_I2C_ADDR_LR, data, dataLen, NULL);

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

    I2C_MasterWrite(data, dataLen, DSP_I2C_ADDR_LR, &i2c_status);

    I2C_limitedTimer = 5;
    while(!I2C_IsIdle())
    {
        if(!I2C_limitedTimer)
            return false;
    }    
    return true;
#endif        
}




 static bool DSP_Write_SW(uint16_t dataLen, uint8_t* data)
{
#if defined (__XC32__)    
    DRV_I2C_BUFFER_EVENT result;
    while( !(I2C_bufferHandle == NULL 
            || DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE 
            || DRV_I2C_TransferStatusGet(I2C_Handle, I2C_bufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR ) )
    {
    }
    I2C_bufferHandle = DRV_I2C_Transmit(I2C_Handle, DSP_I2C_ADDR_SW, data, dataLen, NULL);

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

    I2C_MasterWrite(data, dataLen, DSP_I2C_ADDR_SW, &i2c_status);

    I2C_limitedTimer = 5;
    while(!I2C_IsIdle())
    {
        if(!I2C_limitedTimer)
            return false;
    }    
    return true;
#endif        
}
#else

static bool DSP_Write_LR(uint16_t dataLen, uint8_t* data)
{
	uint8_t result;
	result = User_I2C_WriteBytes(data, dataLen, DSP_I2C_ADDR_LR);
	//User_Log("User_I2C_WriteBytes %d\n",result);
	return (!result);
}
static bool DSP_Write_SW(uint16_t dataLen, uint8_t* data)
{
	uint8_t result;
	result = User_I2C_WriteBytes(data, dataLen, DSP_I2C_ADDR_SW);
	return (!result);
}

#endif

/********************************************************************/
static unsigned int I2C_Write_NTP8230_LR(unsigned char addr, unsigned char dat)
{
	unsigned char send_buff[2];
	send_buff[0]=addr;
	send_buff[1]=dat;  
#if 0
       return I2cTransfer((NTP8230_SLAVE_ADDRESS_LR| I2C_WRITE), (uint8 *)send_buff,2, 0, 0);
#else
	return DSP_Write_LR(2,send_buff);
#endif	
}

static unsigned int I2C_Write_NTP8230_SW(unsigned char addr, unsigned char dat)
{
	unsigned char send_buff[2];
	send_buff[0]=addr;
	send_buff[1]=dat; 
#if 0
       return  I2cTransfer((NTP8230_SLAVE_ADDRESS_SW| I2C_WRITE), (uint8 *)send_buff,2, 0, 0);
#else
	return DSP_Write_SW(2,send_buff);
#endif
}

static unsigned int I2C_Write_4Byte_NTP8230_LR(unsigned char addr, unsigned int dat)
{
	unsigned char send_buff[5];
	send_buff[0]=addr;
	send_buff[1]=(unsigned char)((dat >> 24) & 0xFF); 
	send_buff[2]=(unsigned char)((dat >> 16) & 0xFF);
	send_buff[3]=(unsigned char)((dat >> 8) & 0xFF);
	send_buff[4]=(unsigned char)((dat >> 0) & 0xFF);
	#if 0

	User_Log("send_buff[1] = %02x\n",send_buff[1]);
	User_Log("send_buff[2] = %02x\n",send_buff[2]);
	User_Log("send_buff[3] = %02x\n",send_buff[3]);
	User_Log("send_buff[4] = %02x\n",send_buff[4]);
	#endif
#if 0
       return I2cTransfer((NTP8230_SLAVE_ADDRESS_LR| I2C_WRITE), (uint8 *)send_buff,2, 0, 0);
#else
	return DSP_Write_LR(5,send_buff);
#endif	
}

static unsigned int I2C_Write_4Byte_NTP8230_SW(unsigned char addr, unsigned int dat)
{
	unsigned char send_buff[5];
	send_buff[0]=addr;
	send_buff[1]=(unsigned char)((dat >> 24) & 0xFF); 
	send_buff[2]=(unsigned char)((dat >> 16) & 0xFF);
	send_buff[3]=(unsigned char)((dat >> 8) & 0xFF);
	send_buff[4]=(unsigned char)((dat >> 0) & 0xFF);
	#if 0

	User_Log("send_buff[1] = %02x\n",send_buff[1]);
	User_Log("send_buff[2] = %02x\n",send_buff[2]);
	User_Log("send_buff[3] = %02x\n",send_buff[3]);
	User_Log("send_buff[4] = %02x\n",send_buff[4]);
	#endif
#if 0
       return I2cTransfer((NTP8230_SLAVE_ADDRESS_LR| I2C_WRITE), (uint8 *)send_buff,2, 0, 0);
#else
	return DSP_Write_SW(5,send_buff);
#endif	
}


static void set_eq_data()
{
	unsigned int i;

	//LR EQ	
	for(i=0;i<(sizeof(NTP8230_Eq1_Data_LR)/2);i++)
    {

        I2C_Write_NTP8230_LR(NTP8230_Eq1_Data_LR[i].RegAddr, NTP8230_Eq1_Data_LR[i].RegData);
    }


	for(i=0;i<(sizeof(NTP8230_Eq2_Data_LR)/sizeof(COEFFICIENT_STRUCT));i++)
    {		
        I2C_Write_4Byte_NTP8230_LR(NTP8230_Eq2_Data_LR[i].RegAddr, NTP8230_Eq2_Data_LR[i].RegData);
    }


	for(i=0;i<(sizeof(NTP8230_Eq3_Data_LR)/2);i++)
    {

        I2C_Write_NTP8230_LR(NTP8230_Eq3_Data_LR[i].RegAddr, NTP8230_Eq3_Data_LR[i].RegData);
    }

	for(i=0;i<(sizeof(NTP8230_Eq4_Data_LR)/sizeof(COEFFICIENT_STRUCT));i++)
    {		
        I2C_Write_4Byte_NTP8230_LR(NTP8230_Eq4_Data_LR[i].RegAddr, NTP8230_Eq4_Data_LR[i].RegData);
    }

	for(i=0;i<(sizeof(NTP8230_Eq5_Data_LR)/2);i++)
    {

        I2C_Write_NTP8230_LR(NTP8230_Eq5_Data_LR[i].RegAddr, NTP8230_Eq5_Data_LR[i].RegData);
    }

	for(i=0;i<(sizeof(NTP8230_Eq6_Data_LR)/sizeof(COEFFICIENT_STRUCT));i++)
    {		
        I2C_Write_4Byte_NTP8230_LR(NTP8230_Eq6_Data_LR[i].RegAddr, NTP8230_Eq6_Data_LR[i].RegData);
    }
	

	for(i=0;i<(sizeof(NTP8230_Eq7_Data_LR)/2);i++)
    {

        I2C_Write_NTP8230_LR(NTP8230_Eq7_Data_LR[i].RegAddr, NTP8230_Eq7_Data_LR[i].RegData);
    }
	

	//BASS EQ
	for(i=0;i<(sizeof(NTP8230_Eq1_Data_Bass)/2);i++)
    {

        I2C_Write_NTP8230_SW(NTP8230_Eq1_Data_Bass[i].RegAddr, NTP8230_Eq1_Data_Bass[i].RegData);
    }



	for(i=0;i<(sizeof(NTP8230_Eq2_Data_Bass)/sizeof(COEFFICIENT_STRUCT));i++)
    {		
        I2C_Write_4Byte_NTP8230_SW(NTP8230_Eq2_Data_Bass[i].RegAddr, NTP8230_Eq2_Data_Bass[i].RegData);
    }

	for(i=0;i<(sizeof(NTP8230_Eq3_Data_Bass)/2);i++)
    {

        I2C_Write_NTP8230_SW(NTP8230_Eq3_Data_Bass[i].RegAddr, NTP8230_Eq3_Data_Bass[i].RegData);
    }

	for(i=0;i<(sizeof(NTP8230_Eq4_Data_Bass)/sizeof(COEFFICIENT_STRUCT));i++)
    {		
        I2C_Write_4Byte_NTP8230_SW(NTP8230_Eq4_Data_Bass[i].RegAddr, NTP8230_Eq4_Data_Bass[i].RegData);
    }

	for(i=0;i<(sizeof(NTP8230_Eq5_Data_Bass)/2);i++)
    {

        I2C_Write_NTP8230_SW(NTP8230_Eq5_Data_Bass[i].RegAddr, NTP8230_Eq5_Data_Bass[i].RegData);
    }

	

}



/*-----------------------------------------------------------------------------*/

void NF8230dsp_init(void)
{
	ntp8230g_state = ST_NTP8230G_IDLE;
	user_volume_mode = VOLUME;
	volume_master_step = 8;
	volume_treble_step = 4;
	volume_bass_step = 6;
	sound_is_mute_flag = 0;
	set_master_volume_temp_Flag = false;
  
}

void NF8230dsp_SetLChannelOnOff(bool on_off)
{
	if(!is_ntp8230g_ready())
		return;
	if(on_off)
	{
		
		I2C_Write_NTP8230_LR(TREBLE_CH2_VOLUME_REG, 0xCF);
	}
	else
	{
		I2C_Write_NTP8230_LR(TREBLE_CH2_VOLUME_REG, 0);

	}

}

void NF8230dsp_SetSWChannelOnOff(bool on_off)
{
	if(!is_ntp8230g_ready())
		return;
	if(on_off)
	{
		I2C_Write_NTP8230_SW(TREBLE_CH1_VOLUME_REG, 0xCF);
		I2C_Write_NTP8230_SW(TREBLE_CH2_VOLUME_REG, 0xCF);

	}
	else
	{
		I2C_Write_NTP8230_SW(TREBLE_CH1_VOLUME_REG, 0);
		I2C_Write_NTP8230_SW(TREBLE_CH2_VOLUME_REG, 0);

	}

}


void NF8230dsp_SetEQOnOff(bool on_off)
{
    uint8_t i;
	if(!is_ntp8230g_ready())
		return;
	if(on_off)
	{
		
		for(i=0;i<(sizeof(NTP8230_Eq_ON)/2);i++)
	    {

	        I2C_Write_NTP8230_LR(NTP8230_Eq_ON[i].RegAddr, NTP8230_Eq_ON[i].RegData);
	    }

		for(i=0;i<(sizeof(NTP8230_Eq_ON)/2);i++)
	    {

	        I2C_Write_NTP8230_SW(NTP8230_Eq_ON[i].RegAddr, NTP8230_Eq_ON[i].RegData);
	    }

	}
	else
	{
		for(i=0;i<(sizeof(NTP8230_Eq_OFF)/2);i++)
	    {

	        I2C_Write_NTP8230_LR(NTP8230_Eq_OFF[i].RegAddr, NTP8230_Eq_OFF[i].RegData);
	    }

		for(i=0;i<(sizeof(NTP8230_Eq_OFF)/2);i++)
	    {

	        I2C_Write_NTP8230_SW(NTP8230_Eq_OFF[i].RegAddr, NTP8230_Eq_OFF[i].RegData);
	    }

	}

}
static void ntp8230g_set_master_volume_MAX_temp()
{
	if(!is_ntp8230g_ready())
		return;
	User_Log("ntp8230g_set_master_volume_MAX_temp\n");
	I2C_Write_NTP8230_LR(MASTER_VOLUME_REG, volume_master_step_gain[VOL_MAX - 1] - 40);// decrease 20dB
	I2C_Write_NTP8230_SW(MASTER_VOLUME_REG, volume_master_SW_step_gain[VOL_MAX - 1] - 40);
}

void ntp8230g_set_master_volume_temp(uint8_t vol_dB)
{
	if(!is_ntp8230g_ready())
		return;

	if(set_master_volume_temp_Flag)
		return;
	
	User_Log("ntp8230g_set_master_volume_temp 0x%02x\n",vol_dB);
	
	if(sound_is_mute_flag)
		User_SoundOnOff(ON, true);
	
	I2C_Write_NTP8230_LR(MASTER_VOLUME_REG, vol_dB/*volume_master_step_gain[vol]*/);
	I2C_Write_NTP8230_SW(MASTER_VOLUME_REG, vol_dB/*volume_master_SW_step_gain[vol]*/);
}


static void ntp8230g_return_master_volume()
{
	if(!is_ntp8230g_ready())
		return;
	User_Log("ntp8230g_return_master_volume  %d\n",volume_master_step);
	ntp8230g_set_master_volume(volume_master_step);
}


void ntp8230g_set_master_volume(uint8_t vol)
{
	if(!is_ntp8230g_ready())
		return;
	
	if(set_master_volume_temp_Flag)
	{
		volume_master_step = vol;
		return;
	}

	#if 0
	if(volume_master_step == vol){
		if(is_master_volume_decrease_20dB_flag == false)
			return;
	}
	#endif
	
	if(vol == 0)
	{
		is_master_volume_decrease_20dB_flag = false;
		User_SoundOnOff(OFF, false);
		volume_master_step = vol;

	}
	else if(vol < (VOL_MAX - 1))
	{
		if(sound_is_mute_flag)
			User_SoundOnOff(ON, true);
		
		volume_master_step = vol;
		is_master_volume_decrease_20dB_flag = false;
		if((currentBatteryLevel <= 11) && (DC_PULL_OUT))//-3dB
		{
			I2C_Write_NTP8230_LR(MASTER_VOLUME_REG, volume_master_step_gain[vol] - 6);
			I2C_Write_NTP8230_SW(MASTER_VOLUME_REG, volume_master_SW_step_gain[vol] - 6);
		}
		else
		{
			I2C_Write_NTP8230_LR(MASTER_VOLUME_REG, volume_master_step_gain[vol]);
			I2C_Write_NTP8230_SW(MASTER_VOLUME_REG, volume_master_SW_step_gain[vol]);

		}
		
	}
	else 
	{
		if(sound_is_mute_flag)
			User_SoundOnOff(ON, true);
		
		volume_master_step = VOL_MAX - 1;
		#if 0    //Remove max volume dip
		if(is_master_volume_decrease_20dB_flag == false)  
		{
			is_master_volume_decrease_20dB_flag = true;
			master_volume_decrease_20dB_timer1ms = 500;
			ntp8230g_set_master_volume_MAX_temp();
		}
		else
		#endif
		{
			is_master_volume_decrease_20dB_flag = false;
			if((currentBatteryLevel <= 11) && (DC_PULL_OUT))//-3dB
			{
				I2C_Write_NTP8230_LR(MASTER_VOLUME_REG, volume_master_step_gain[VOL_MAX - 1] - 6);
				I2C_Write_NTP8230_SW(MASTER_VOLUME_REG, volume_master_SW_step_gain[VOL_MAX - 1] - 6);
			}
			else
			{
				I2C_Write_NTP8230_LR(MASTER_VOLUME_REG, volume_master_step_gain[VOL_MAX - 1]);
				I2C_Write_NTP8230_SW(MASTER_VOLUME_REG, volume_master_SW_step_gain[VOL_MAX - 1]);

			}
		}

	}
	User_Log("volume_master_step = 0x%02x\n",volume_master_step);

}

void ntp8230g_set_treble_volume(uint8_t vol)
{
    uint8_t i = 0;
	if(!is_ntp8230g_ready())
		return;

	volume_treble_step = vol;

	I2C_Write_NTP8230_LR(0x7E,0x03); //Enable download

	for(i=(vol*5);i < (5*(vol + 1));i++)
    {		
        I2C_Write_4Byte_NTP8230_LR(volume_treble_frequency_point[i].RegAddr, volume_treble_frequency_point[i].RegData);
		//User_Log("Treble Vol: 0x%x, 0x%08x\n",volume_treble_frequency_point[i].RegAddr,volume_treble_frequency_point[i].RegData);
    }

	I2C_Write_NTP8230_LR(0x7E,0x00);//Enable download
	
	User_Log("set TREBLE %d\n",vol);

}

void ntp8230g_set_bass_volume(uint8_t vol)
{
    uint8_t i = 0;
    
	if(!is_ntp8230g_ready())
		return;
	
	volume_bass_step = vol;

	I2C_Write_NTP8230_SW(0x7E,0x03); //Enable download

	for(i=(vol*5);i < (5*(vol + 1));i++)
    {		
        I2C_Write_4Byte_NTP8230_SW(volume_bass_frequency_point[i].RegAddr, volume_bass_frequency_point[i].RegData);
		//User_Log("Bass Vol: 0x%x, 0x%08x\n",volume_bass_frequency_point[i].RegAddr,volume_bass_frequency_point[i].RegData);
    }

	I2C_Write_NTP8230_SW(0x7E,0x00);//Enable download

	User_Log("set bass %d\n",vol);


}

void Ntp8230g_RestoreTrebleAndBass(void)
{
	ntp8230g_set_treble_volume(volume_treble_step);
	ntp8230g_set_bass_volume(volume_bass_step);

}

void ntp8230g_set_volume(USER_VOLUME_MODE mode, uint8_t vol)
{
	//if(is_ntp8230g_ready())
	{
		switch(mode)
		{
			case VOLUME:
				ntp8230g_set_master_volume(vol);				
				break;

			case TREBLE:
				ntp8230g_set_treble_volume(vol);
				
				break;

			case BASS:
				ntp8230g_set_bass_volume(vol);
				break;

		}

	}
	
}


void User_SoundOnOff(uint8_t on_off,bool a2dp_play)
{
	const NSP_INIT_STRUCT sound_on[] =  {{0x28,0x04},{0x27,0x00},{0x26,0x00}};
	const NSP_INIT_STRUCT sound_off[] = {{0x26,0x0f},{0x28,0x02},{0x27,0x0f}};
	int i = 0;
	if(on_off)
	{
		if(a2dp_play){
			;//BTMA2DP_PlayStart();
		}
		else
			;//BTMA2DP_PauseStart();
		
		for(i = 0; i < (sizeof(sound_on)/sizeof(NSP_INIT_STRUCT)); i++)
			I2C_Write_NTP8230_LR(sound_on[i].RegAddr, sound_on[i].RegData);

		for(i = 0; i < (sizeof(sound_on)/sizeof(NSP_INIT_STRUCT)); i++)
			I2C_Write_NTP8230_SW(sound_on[i].RegAddr, sound_on[i].RegData);

		sound_is_mute_flag = 0;
		
	}
	else
	{
		if(a2dp_play)
				;//BTMA2DP_PlayStart();			
		else
			;//BTMA2DP_PauseStart();
			
		
		for(i = 0; i < (sizeof(sound_off)/sizeof(NSP_INIT_STRUCT)); i++)
			I2C_Write_NTP8230_LR(sound_off[i].RegAddr, sound_off[i].RegData);

		for(i = 0; i < (sizeof(sound_off)/sizeof(NSP_INIT_STRUCT)); i++)
			I2C_Write_NTP8230_SW(sound_off[i].RegAddr, sound_off[i].RegData);

		sound_is_mute_flag = 1;

	}

}

void User_VolumeModeSwitch()
{
	switch(user_volume_mode)
	{
		case VOLUME:
			user_volume_mode = TREBLE;
			break;

		case TREBLE:
			user_volume_mode = BASS;
			break;

		case BASS:
			user_volume_mode = VOLUME;
			break;

		default:
			break;

	}

	User_DisplayVolumeLevel();
	

}


static void volumeModeUp(USER_VOLUME_MODE mode)
{
	uint8_t* p;
		
	switch(mode)
	{
		case TREBLE:
			p = &volume_treble_step;
		break;

		case BASS:
			p = &volume_bass_step;
		break;
		
		default:
			break;

	}

	if(*p < 8)
    {
        (*p)++;
    }

}

static void volumeModeDn(USER_VOLUME_MODE mode)
{
	uint8_t* p;
		
	switch(mode)
	{
		case TREBLE:
			p = &volume_treble_step;
		break;

		case BASS:
			p = &volume_bass_step;
		break;
		
		default:
			break;

	}

	if(*p > 0)
    {
        (*p)--;
    }

}


void DSP_setUserVolumeMode( void )
{
     uint8_t* p;
	 
    switch(user_volume_mode)
    {
        case TREBLE:
            p = &volume_treble_step;
            break;
        case BASS:
            p = &volume_bass_step;
            break;
        default:
			return;
            break;
    }

    ntp8230g_set_volume(user_volume_mode,*p);
 }


void User_VolumeUp()
{
	volumeModeUp(user_volume_mode);
	DSP_setUserVolumeMode();
	User_DisplayVolumeLevel();

}

void User_VolumeDn()
{
	volumeModeDn(user_volume_mode);
	DSP_setUserVolumeMode();
	User_DisplayVolumeLevel();

}

bool is_ntp8230g_idle(void)
{
    if(ntp8230g_state == ST_NTP8230G_IDLE)
        return true;
    else
        return false;
}

bool is_ntp8230g_ready(void)
{
    if(ntp8230g_state == ST_NTP8230G_RUNNING || ntp8230g_state == ST_NTP8230G_STARTING_POWER_OFF)
        return true;
    else
        return false;
}

void ntp8230g_start(void)
{
    DSP_Timer = 200;
    ntp8230g_state = ST_NTP8230G_START;
}


void NF8230dsp_task(void)
{
	static uint8_t vol_temp = VOL_MAX - 7;
	if(master_volume_decrease_20dB_TimeOutFlag)
	{
		master_volume_decrease_20dB_TimeOutFlag = false;
		ntp8230g_return_master_volume();
		vol_temp = VOL_MAX - 7;
	}

	if(Volume_100msTimeOutFlag)
	{
		Volume_100msTimeOutFlag = false;

		#if 0  //Remove max volume dip

		if(is_master_volume_decrease_20dB_flag)
		{
			if(vol_temp < (VOL_MAX - 1))
				vol_temp++;
			
			ntp8230g_set_master_volume_temp(vol_temp);

		}
		#endif
	}

	if(ringTone_1msTimeOutFlag)
	{
		ringTone_1msTimeOutFlag = false;
		set_master_volume_temp_Flag = false;
		if(BTMHFP_GetCallStatus() == BT_CALL_IDLE)
			NF8230dsp_SetEQOnOff(ON);
		
		ntp8230g_set_master_volume(volume_master_step);
		User_Log("Ring tone timeout\n");
	}
	
	

	//return;


	switch(ntp8230g_state)
    {
        case ST_NTP8230G_IDLE:
            break;
            
        case ST_NTP8230G_START:
            if(!DSP_Timer)
            {
                DSP_RESET_SetLow();//RST_DSP = LOW;
                DSP_Timer = 10;
                ntp8230g_state = ST_NTP8230G_RESET_LOW_WAIT;
            }
            break;
            
        case ST_NTP8230G_RESET_LOW_WAIT:
            if(!DSP_Timer)
            {
                DSP_RESET_SetHigh();
                DSP_Timer = 150;
                ntp8230g_state = ST_NTP8230G_T2_RESET_HIGH_WAIT;
            }
            break;
#if 1
		case ST_NTP8230G_T2_RESET_HIGH_WAIT:
            if(!DSP_Timer)
            {
                DSP_RESET_SetLow();
                DSP_Timer = 5;
                ntp8230g_state = ST_NTP8230G_T3_RESET_LOW_WAIT;
            }
            break;

		case ST_NTP8230G_T3_RESET_LOW_WAIT:
            if(!DSP_Timer)
            {
                DSP_RESET_SetHigh();
                DSP_Timer = 5;
                ntp8230g_state = ST_NTP8230G_SETUP_EQ;
            }
            break;

#endif
            
        case ST_NTP8230G_SETUP_EQ:
			if(!DSP_Timer)
			{
				set_eq_data();
				User_SoundOnOff(OFF, false);
				DSP_Timer = 10;
				ntp8230g_state = ST_NTP8230G_SETUP_EQ_WAIT;
				User_Log("SET EQ data complete\n");
			}
            break;

		case ST_NTP8230G_SETUP_EQ_WAIT:
			if(!DSP_Timer)
			{
				ntp8230g_state = ST_NTP8230G_RUNNING;
				User_Log("SET EQ data wait\n");
			}
            break;

		case ST_NTP8230G_RUNNING:
			if(BTAPP_GetStatus() == BT_STATUS_OFF)
            {
                DSP_Timer = 2000;       //wait 2 seconds
                ntp8230g_state = ST_NTP8230G_STARTING_POWER_OFF;
            }
            break;

		case ST_NTP8230G_STARTING_POWER_OFF:
            if(BTAPP_GetStatus() == BT_STATUS_READY || BTAPP_GetStatus() == BT_STATUS_ON)
            {
                ntp8230g_state = ST_NTP8230G_RUNNING;
                break;
            }
            if(!DSP_Timer)
            {
                //DSP_MUTE_SetLow();
                DSP_Timer = 5;
                ntp8230g_state = ST_NTP8230G_POWER_OFF_1;
            }
            break;
            
        case ST_NTP8230G_POWER_OFF_1:
            if(!DSP_Timer)
            {
                User_SoundOnOff(OFF, false);
                DSP_Timer = 5;
                ntp8230g_state = ST_NTP8230G_POWER_OFF_2;
            }
            break;
            
        case ST_NTP8230G_POWER_OFF_2:
            if(!DSP_Timer)
            {
                DSP_RESET_SetLow();
                ntp8230g_state = ST_NTP8230G_IDLE;
				set_master_volume_temp_Flag = false;
            }
            break;

	}
}


void User_SetRingToneVolume(uint8_t Ringtone_Mode, uint8_t status)
{
	if(!is_ntp8230g_ready())
		return;
	User_Log("Ringtone_Mode 0x%02x\n",Ringtone_Mode);
	if(status == 0x00)//Ringtone playback is going to be stopped
	{
		if(set_master_volume_temp_Flag == true){
			User_Log("Ringtone playback is going to be stopped\n");
			set_master_volume_temp_Flag = false;
			if(BTMHFP_GetCallStatus() == BT_CALL_IDLE)
				NF8230dsp_SetEQOnOff(ON);
			
			ntp8230g_set_master_volume(volume_master_step);
			ringTone_1msTimer = 0;
			ringTone_1msTimeOutFlag = false;
			Tone_playPowerOnFlag = false;
		}
	}
	else//Ringtone playback is going to start
	{
		User_Log("Ringtone playback is going to start\n");
		if(set_master_volume_temp_Flag == false){
			if(Ringtone_Mode == 0xff){
				if(Tone_playPowerOnFlag){
					User_Log("power on tone volume set\n");
					ntp8230g_set_master_volume_temp(vol_tone);
					Tone_playPowerOnFlag = false;
				}
				else
					ntp8230g_set_master_volume_temp(vol_tone);
				
			}
			else if(Ringtone_Mode == TONE_PowerOff)
				ntp8230g_set_master_volume_temp(0xD5);//3dB
			else
				ntp8230g_set_master_volume_temp(vol_tone);

			if(BTMHFP_GetCallStatus() == BT_CALL_IDLE)
				NF8230dsp_SetEQOnOff(OFF);
			set_master_volume_temp_Flag = true;
            
#if 1
			if((Ringtone_Mode == TONE_BroadcastPairing))//0.949s
			{
				ringTone_1msTimer = 1500;
			}
			else if((Ringtone_Mode == TONE_Connected))//0.857s
			{
				ringTone_1msTimer = 1500;
			}
			else if(Ringtone_Mode == TONE_BatteryLow) //2.16s
			{
				ringTone_1msTimer = 2500;
			}
			else if(Ringtone_Mode == 0xff) //2.24s power on
			{
				ringTone_1msTimer = 3000;
			}
			else if(Ringtone_Mode == TONE_BTPairing)//0.924s  BT pairing
			{
				ringTone_1msTimer = 1500;
			}
			else if(Ringtone_Mode == TONE_PowerOff) //1.423s
			{
				ringTone_1msTimer = 2000;
			}
#endif

		}
	}

}



bool User_IsRingToneStopped(void)
{
	return (!set_master_volume_temp_Flag);

}



