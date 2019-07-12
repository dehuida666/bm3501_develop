/*
  Project Name: BM89 EVB                            Module name:yda174dsp_handler.c
  Bluetooth Chip: IS1689-151                        Bluetooth UI Version:
  MCU chip: SH79F6488
  Oscillator: IRIC                                  Freq: 12MHz
  Author: Jerry.Chen
  Kick off date: 2014-04-03
  Copyright: 2014 ISSC Technologies Corp.. All Rights Reserved.
*/

#ifndef _YDA174DSP_HANDLER_H_
#define _YDA174DSP_HANDLER_H_
#include <stdbool.h>
#include <stdint.h>

//#define GET_ARRAY_LEN(array)  ( sizeof(array)/sizeof (array[0] ) )


/*---------------------------------------------------------
  General Constant Define
*/

//Host controller command
#define REGISTER_ACESS_CMD                          0x00                //Lower 5 nibble for register address
#define DSPSTART_CMD                                0x40
#define DSPSTOP_CMD                                 0x41
#define PRESETROM0                                  0x70
#define PRESETROM1                                  0x71
#define PRESETROM2                                  0x72
#define PRESETROM3                                  0x73
#define PRROMSTOP                                   0x43
#define RAM_WRITE_READ                              0x80
#define WRITE_WORK0TO3                              0x64                //lower 2 nibble for addr
#define READ_WORK0TO3                               0x64                //lower 2 nibble for addr


//YDA174 register define
#define POEN0                                       0x00                //GPIO Direction contrl register. 0:input; 1:output
#define POEN1                                       0x01
#define PIDT0                                       0x02                //GPIO input port data register.
#define PIDT1                                       0x03
#define PODT0                                       0x04                //GPIO output port register
#define PODT1                                       0x05
#define SYSMD                                       0x06                //System error handler setting register
#define AIFMD                                       0x07                //AIF Mode Setting Register
#define AIFIFMT                                     0x08                //This register sets audio interface input data format.
#define AIFOFMT                                     0x09                //This register sets audio interface output data format
#define MDSPMD                                      0x0A                //This register bit selects WRAM memory word mode
#define SDSPMD                                      0x0B                //This register enables or disables DC-Cut Filter operation
#define DAMPMD                                      0x0C                //This register bit mutes the power amplifier output
#define IRQN                                        0x0D                //This register bit specifies a signal status to be sent to IRQ_N pin
#define FSM                                         0x0E                //This register bit sets the XIN pin input frequency
#define RAMCLR                                      0x0F                //This register controls zero clear operation of MPRAM contents

#define MUTE                                        0x10                //This register bit sets the Quick Mute
#define DSPSTART                                    0x11                //This register bit starts or stops MDSP2 operation

#define MDSPREQ                                     0x13                //The host processor will put processing requests to MDSP2 on this register
#define MDSPDAT                                     0x14                //The host processor will put data for a processing request to MDSP2 on this register
#define MIREQ                                       0x15                //MDSP2 firmware will put requests to the host controller on this register
#define MIDAT                                       0x16                //MDSP2 will put data for a request to the host controller on this register
#define DPLLFS                                      0x17                //This register holds the SRC I/O sampling frequency ratio
#define ZDET                                        0x18                //Indicates the zero input condition detected on some of the input channel
#define ID                                          0x19                //This register holds the device ID
#define FMST                                        0x1A                //This register bit indicates the playback status of the FM synthesizer contents
#define MDSPST                                      0x1B                //This register bit indicates an MDSP2 processing error has occurred

#define STEP0                                       0x1E                //This register holds the maximum number of instructions executed in the MDSP2 firmware for a cycl
#define STEP1                                       0x1F

#define SDSPST                                      0x20                //This register indicates input audio clock framing errors

#define PLLERR                                      0x23                //PLL Abnormal Oscillation Detection Register

#define PLIMIT1                                     0x2D                //This register specifies a limit value of the power limiter
#define PLIMIT2                                     0x2E
#define GAIN                                        0x2F                //This register specifies audio signal gain at the input of Power Limiter


#define PWRDWN                                      0x33                //Power-down Setting Register




//Register contents, refer to YDA174A30
#define POEN0_DEFAULT                               0x00
#define POEN0_GPIO8TO11_IN                          0x00                //bit3:0
#define POEN0_GPIO8TO11_OUT                         0x0F

#define POEN1_DEFAULT                               0x00
#define POEN1_GPIO3TO7_IN                           0x00                //bit7:3
#define POEN1_GPIO3TO7_OUT                          0xF8

#define SYSMD_DEFAULT                               0x00
#define SYSMD_DIS_SET_0                             0x04                //bit2
#define SYSMD_IRQ_N_CTRL                            0x02                //bit1,The result of the logical NOR operation on PLLERR, MDSPERR, and SDSPERR bits.
#define SYSMD_MIREQ_CTRL                            0x01                //bit0,The result of the logical OR operation on PLLERR, MDSPERR, and SDSPERR bits.

#define AIFMD_DEFAULT                               0x10
#define AIFMD_EN_IN_INSIDE                          0x20                //bit5
#define AIFMD_EN_GPIO_FUNC                          0x10                //bit4
#define AIFMD_AUDIO_MODE3                           0x03                //bit3~0
#define AIFMD_AUDIO_MODE2                           0x02                //bit3~0
#define AIFMD_AUDIO_MODE1                           0x01                //bit3~0


#define AIFIFMT_DEFAULT                             0x00
#define AIFIFMT_INPUT_I2S_FORMAT                    0x20                //bit5~4
#define AIFIFMT_24BIT                               0x0C                //bit3~2
#define AIFIFMT_START_FRAME_FALLING                 0x02                //bit1
#define AIFIFMT_CHANGE_BCLK_FALLING                 0x01                //bit0

#define AIFOFMT_DEFAULT                             0x00
#define AIFOFMT_OUTPUT_I2S_FORMAT                   0x20                //bit5~4
#define AIFOFMT_24BIT                               0x0C                //bit3~2
#define AIFOFMT_START_FRAME_FALLING                 0x02                //bit1
#define AIFOFMT_CHANGE_BLCK_FALLING                 0x01                //bit0

#define MDSPMD_DEFAULT                              0x00
#define MDSPMD_RUN_TRANSFER                         0x02                //bit1
#define MDSPMD_WRAM_MODE_32BIT                      0x01                //bit0

#define SDSPMD_DEFAULT                              0x00
#define SDSPMD_DCCUTOFF_EN                          0x04                //bit2
#define SDSPMD_DE_EMPRHASIS_EN                      0x02                //bit1
#define SDSPMD_SRC_MODE_GREATER                     0x01                //bit0

#define DAMPMD_DEFAULT                              0x60
#define DAMPMD_AMPMUTE_EN                           0x40                //bit6
#define DAMPMD_CHG_FREQ                             0x04                //bit2

#define IRQN_DEFAULT                                0x01
#define IRQN_DIS_TO_IRQPIN                          0x01                //bit0

#define FSM_DEFAULT                                 0x00
#define FSM_F22M5_SR44                              0x01                //bit1~0

#define RAMCLR_DEFAULT                              0x00
#define RAMCLR_START_MPRAM_CLEAR                    0x04                //bit2
#define RAMCLR_START_WRAM_CLEAR                     0x02                //bit1
#define RAMCLR_START_SWRAM_CLEAR                    0x01                //bit0

#define MUTE_DEFAULT                                0x00
#define MUTE_QMUTE_EN                               0x08                //bit3
#define MUTE_FMMUTE_CANCLE                          0x04                //bit2
#define MUTE_MDSP2MUTE_CANCLE                       0x02                //bit1
#define MUTE_SDSPMUTE_CANCLE                        0x01                //bit0
#define MUTE_QMUTE_DIS                              0xF7
#define MUTE_MDSP2_SDSPMUTE_DIS                     0xFC

#define DSPSTART_DEFAULT                            0x00
#define DSPSTART_MDSP2_START                        0x02                //bit1
#define DSPSTART_SDSP_START                         0x01                //bit0

#define FMST_DEFAULT                                0x00
#define FMST_EN_STATUS_TO_IRQN                      0x10                //bit4

#define MDSPST_DEFAULT                              0x00
#define MDSPST_CLR_ERRIND_EN                        0x01                //bit0

#define SDSPST_DEFAULT                              0xF0
#define SDSPST_CLR_ERR_FLAG_DIS                     0x80                //bit7
#define SDSPST_CLR_STATUS_IND_DIS                   0x10                //bit4
#define SDSPST_CLR_ERR1_FLAG_DIS                    0x08                //bit3
#define SDSPST_CLR_ERR2_FLAG_DIS                    0x04                //bit2
#define SDSPST_CLR_ERR3_FLAG_DIS                    0x01                //bit0

#define PLLERR_DEFAULT                              0x00
#define PLLERR_ERR_DETECT_DIS                       0x04                //bit2
#define PLLERR_ABNORMAL_PLL                         0x01                //bit0

#define PLIMIT1_DEFAULT                             0xFF
#define PLIMIT1_PWR_LIMIT_MAX                       0xFF

#define GAIN_DEFAULT                                0x00
#define GAIN_P6DB                                   0x01                //bit1~0
#define GAIN_N6DB                                   0x02

#define PWRDWN_DEFAULT                              0x00
#define PWRDWN_PWRDOWN_DIGITAL_BLOCK                0x01                //bit0

#define YDA174_QMUTE_WAIT_TIMER                     30                  //unit:1ms
#define YDA174_DSP_STOP_WAIT_TIMER                  10                  //unit:1ms

/*---------------------------------------------------------
  Function Define
*/




/*-----------------------------------------------------------------------------
  This function used to initialize dsp
*/
void yda174dsp_init(void);
void yda174dsp_task(void);
bool is_yda174dsp_idle(void);
bool is_yda174dsp_ready(void);
void yda174dsp_start(void);


/*-----------------------------------------------------------------------------
  This function used to enable playback
*/
//void yda174_playback(void);


/*-----------------------------------------------------------------------------
  This function used to stop playbck
*/
//void yda174_stop(void);
/*-----------------------------------------------------------------------------
  This function used to start playbck
*/
void yda174_DSPStart(void);
/*-----------------------------------------------------------------------------
  This function used to support input sampling rate > 50KHZ
*/
void yda174_setHighSamplingrateInput(void);
/*-----------------------------------------------------------------------------
  This function used to support input sampling rate <= 50KHZ
*/
void yda174_setNormalSamplingrateInput(void);
void yda174_voladj(void);
void yda174_vol_up(void);
void yda174_vol_down(void);
void yda174_DSPStop(void);
void yda174_SetVol(uint8_t vol);
void DSP_Timer1MS_event(void);
//void mute_on_test(void);
//void mute_off_test(void);

#define EXT_DSP_INIT_VOL 4

#endif      //_DSP_HANDLER_H_
