#include "user_i2c.h"
#include "ad_gpio.h"


#ifdef SW_I2C
#define FAILURE                         1
#define SUCCESS                         0

#define I2C_ACK_NG                      1
#define I2C_ACK_OK                      0


#define SET_IIC_SDA_INPUT   gpio_init(SW_I2C_2_SDA_IO_PIN, GPIO_DIGITAL_INPUT);
#define SET_IIC_SDA_OUTPUT  gpio_init(SW_I2C_2_SDA_IO_PIN, GPIO_DIGITAL_OUTPUT);

#define SET_IIC_SCL_INPUT   gpio_init(SW_I2C_2_SCL_IO_PIN, GPIO_DIGITAL_INPUT);
#define SET_IIC_SCL_OUTPUT  gpio_init(SW_I2C_2_SCL_IO_PIN, GPIO_DIGITAL_OUTPUT);

#define IIC_SDA_SET     gpio_set (SW_I2C_2_SDA_IO_PIN)

#define IIC_SCL_SET     gpio_set (SW_I2C_2_SCL_IO_PIN)

#define IIC_SDA_CLR     gpio_reset(SW_I2C_2_SDA_IO_PIN)

#define IIC_SCL_CLR      gpio_reset(SW_I2C_2_SCL_IO_PIN)

unsigned char IIC_SDA (void)
{
    unsigned char data;

    data=gpio_read (SW_I2C_2_SDA_IO_PIN);

    return data;

}

void IIC_Nop (unsigned char i)
{
    i = i * 7;

    while (i)
    {
        Nop();
        i--;
    }
}

void IIC_Init (void)
{
    gpio_init (SW_I2C_2_SDA_IO_PIN, GPIO_DIGITAL_OUTPUT);
    gpio_init (SW_I2C_2_SCL_IO_PIN, GPIO_DIGITAL_OUTPUT);
    IIC_Nop (1);
    IIC_SDA_SET;
    IIC_Nop (1);
    IIC_SCL_SET;
    IIC_Nop (1);
}

void IIC_Start (void)
{
    SET_IIC_SDA_OUTPUT;
    SET_IIC_SCL_OUTPUT;
    IIC_Nop (10);
    IIC_SCL_SET;
    IIC_SDA_SET;
    IIC_Nop (1);
    IIC_SCL_SET;
    IIC_Nop (1);
    IIC_SDA_CLR;
    IIC_Nop (4);
}

void IIC_Stop (void)
{
    IIC_SDA_CLR;
    IIC_Nop (1);
    IIC_SCL_SET;
    IIC_Nop (1);
    IIC_SDA_SET;
    IIC_Nop (1);
}

unsigned char IIC_WaitAck (void)
{
    unsigned char ack=1;
    unsigned char data=1;
    unsigned short i;
    IIC_SCL_CLR;//test
    //IIC_Nop(1);
    IIC_SDA_SET;
    IIC_Nop (2);
    SET_IIC_SDA_INPUT;
    //IIC_Nop(1);
    IIC_SCL_SET;
    IIC_Nop (1);
    i = 0;

    while (i < 1000)
    {
        data=IIC_SDA();
        ack =data;

        if (!data)
        {
            break;
        }

        i++;
    }

    //ack = IIC_SDA();
    /// IIC_SCL_CLR;
    IIC_Nop (1);
    IIC_SDA_CLR;  //test
    SET_IIC_SDA_OUTPUT;
    IIC_Nop (1);
    IIC_SCL_CLR; //test

    IIC_Nop (1);
    return ack;
}

void IIC_WriteByte (unsigned char bytedata)
{
    unsigned char i;
    IIC_SCL_CLR;
    IIC_Nop (1);

    for (i = 0; i < 8; i++)
    {
        if (bytedata & 0x80)
        {
            IIC_SDA_SET;
        }
        else
        {
            IIC_SDA_CLR;
        }

        IIC_Nop (1);
        IIC_SCL_SET;
        IIC_Nop (1);
        IIC_SCL_CLR;
        bytedata = bytedata << 1;
    }

    IIC_Nop (1);
}

void IIC_SendAck (void)
{
    //unsigned char data;
    //ack= IIC_SDA();
    IIC_SDA_SET;
    IIC_Nop (1);
    IIC_SCL_SET;
    IIC_Nop (1);
    IIC_SCL_CLR;
    IIC_Nop (1);
    IIC_SDA_SET;
}

#if 0
unsigned char IIC_ReadByte (void)
{
    unsigned char i;
    unsigned char data;
    unsigned char bytedata;
    bytedata = 0x00;
    SET_IIC_SDA_INPUT;
    IIC_SCL_CLR;

    for (i = 0; i < 8; i++)
    {
        IIC_SDA_SET;
        IIC_SCL_SET;
        IIC_Nop (1);
        data=IIC_SDA();
        bytedata = (bytedata << 1) | data;
        IIC_SCL_CLR;
        IIC_Nop (1);
    }

    SET_IIC_SDA_OUTPUT;
    return bytedata;
}
#endif
unsigned char IIC_ReadByte (void)
{
    unsigned char i;
    unsigned char data;
    unsigned char bytedata;
    bytedata = 0x00;
    IIC_SDA_SET;
    IIC_Nop (1);
    SET_IIC_SDA_INPUT;
    IIC_Nop (1);

    for (i = 0; i < 8; i++)
    {

        IIC_SCL_SET;
        IIC_Nop (1);
        bytedata = bytedata << 1;
        data=IIC_SDA();

        if (data)
        {
            bytedata = bytedata|0x01;     // 若接收到的位为1，则数据的最后一位置1
        }
        else
        {
            bytedata = bytedata&0xfe;     // 否则数据的最后一位置0
        }

        IIC_Nop (1);
        IIC_SCL_CLR;
        IIC_Nop (1);
    }

    SET_IIC_SDA_OUTPUT;
    IIC_SCL_CLR;
    IIC_Nop (1);
    return bytedata;
}

void IIC_Write (unsigned char device, unsigned char address, unsigned char bytedata)
{
    unsigned char i;

    for (i = 0; i < 3;/*RETRY_MAX;*/ i++)
    {
        IIC_Start();
        IIC_WriteByte (device);

        if (IIC_WaitAck() == FAILURE)
        {
            //debug_printf ("111\r\n");
            IIC_Stop();
            continue;
        }

        IIC_WriteByte (address);

        if (IIC_WaitAck() == FAILURE)
        {
            //debug_printf ("222\r\n");
            IIC_Stop();
            continue;
        }

        IIC_WriteByte (bytedata);

        if (IIC_WaitAck() == FAILURE)
        {
            //debug_printf ("333\r\n");
            IIC_Stop();
            continue;
        }

        IIC_Stop();

        break;
    }
}

unsigned char IIC_Read (unsigned char device, unsigned char address)
{
    unsigned char i;
    unsigned char bytedata;

    for (i = 0; i < 3;/*RETRY_MAX;*/ i++)
    {
        IIC_Start();
        IIC_WriteByte (device);

        if (IIC_WaitAck() == FAILURE)
        {
            //debug_printf ("4\r\n");
            IIC_Stop();
            continue;
        }

        IIC_WriteByte (address);

        if (IIC_WaitAck() == FAILURE)
        {
            //debug_printf ("5\r\n");
            IIC_Stop();
            continue;
        }

#if 1
        IIC_Start();
        IIC_WriteByte (device | 0x01);
#if 1

        if (IIC_WaitAck() == FAILURE)
        {
            //debug_printf ("7\r\n");
            IIC_Stop();
            continue;
        }

#endif
#endif
        bytedata = IIC_ReadByte();
        IIC_SendAck();
        IIC_Stop();
        break;
    }

    return bytedata;
}

/*********************************************************************************/
uint8_t User_I2C_WriteBytes(uint8_t *Wdata, uint8_t num, uint8_t Address)
{  
	IIC_Start();
	IIC_WriteByte(Address);
	if( IIC_WaitAck()== I2C_ACK_OK)
	{
		while(num)
		{
			IIC_WriteByte(*Wdata++);
			if( IIC_WaitAck()== I2C_ACK_OK){
				num--;
			}
			else{
				IIC_Stop();
				return I2C_ACK_NG;
			}
		}
		IIC_Stop();
		return I2C_ACK_OK;
	}
	else{
		IIC_Stop();
		return I2C_ACK_NG;
	}  
}


#endif

