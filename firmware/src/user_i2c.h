#ifndef _USER_I2C_H_
#define _USER_I2C_H_
#include <stdint.h>

#define SW_I2C

#define SW_I2C_2_SDA_IO_PIN                    PORT_CHANNEL_B, PORTS_BIT_POS_9
#define SW_I2C_2_SCL_IO_PIN                    PORT_CHANNEL_B, PORTS_BIT_POS_8


#ifdef SW_I2C
unsigned char IIC_Read(unsigned char device, unsigned char address);

void IIC_Write(unsigned char device, unsigned char address, unsigned char bytedata);

void IIC_Init(void);
uint8_t User_I2C_WriteBytes(uint8_t *Wdata, uint8_t num, uint8_t Address);

#endif

#endif

