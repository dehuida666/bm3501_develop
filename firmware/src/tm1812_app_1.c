#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "tm1812_app.h"
#include "timer.h"


/*********************************************************************************
  * @brief  : power on tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void TM1812_PowerOn(void)
{
    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2);
}


/*********************************************************************************
  * @brief  : power off tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void TM1812_PowerOff(void)
{
    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2);  
}


/*********************************************************************************
  * @brief  : write one byte to tm1812
  * @param  : data:the data write to tm1812
  * @retval : None
  * @intruduction : 
*************************************************************************************/
int TM1812_WriteOneByte(uint8_t Data) 
{ 
	int BitIndex = 0; 

	for(BitIndex = 8*sizeof(Data)-1; BitIndex >= 0; BitIndex--) 
	{ 
		if(Data & (0x01 << BitIndex)) 
		{ 
			DIN_BIT_1(); 
		} 
		else 
		{ 
			DIN_BIT_0(); 
		} 	
	}
}


/*********************************************************************************
  * @brief  : reset tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
int TM1812_Reset(void) 
{ 
	PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_2);  
	delay_us(15); 
	return 0; 
}




/* ** @Brief: Transmit LED data 
** @Parameter: 1) Din, data input port 2) Data, transmit data 3) Size, data size 
** @Return: 1) 0, transmit data successful 2) -1, data is null or data size illegal 
**/ 
int TM1812_TransmitData(const uint8_t* Data, uint8_t Size) 
{ 	
	int ByteIndex = 0; 
//	if((NULL == Data) || (Size != ONE_RESOLUTION_SIZE)) 
	{ 
//		return -1; 
	}
	
/* Disable all maskable interrupts */ 
	//PLIB_INT_Disable( INT_ID_0 );
	for(ByteIndex = 0; ByteIndex < Size; ByteIndex++) 
	{ 
		TM1812_WriteOneByte(Data[ByteIndex]); 
	} 	
/* Enable microcontroller interrupts */ 
	//SYS_INT_Enable();
	
	TM1812_Reset();
	return 0; 
}



/*********************************************************************************
  * @brief  : a simple test of tm1812
  * @param  : 
  * @retval : None
  * @intruduction : 
*************************************************************************************/
void TM1812_Test(void) 
{ 
	uint8_t Data[ONE_RESOLUTION_SIZE] = {0}; 
	uint8_t DataSize = sizeof(Data) / sizeof(Data[0]); 
	uint8_t DataIndex = 0; 
	static int ColorFlag = 0; 
	static int direction = 0;
	uint8_t current_led = 0;

	

	memset(Data, 0, DataSize); 

	//Data[ColorFlag] = 0xFF;
	#if 1

	//for(DataIndex = 0; DataIndex < 12; DataIndex++) 
	{ 
		current_led = ColorFlag;
		if((ColorFlag == 9) || (ColorFlag == 10) || (ColorFlag == 11))
		{
			current_led	= current_led - 3;
		}
		else if((ColorFlag == 6) || (ColorFlag == 7) || (ColorFlag == 8))
		{
			current_led	= current_led + 3;
		}

		Data[current_led] = 0xFF;
		
	} 
	#endif
	TM1812_TransmitData(Data, DataSize); 

	if(direction)
	{
		ColorFlag--;
		if(ColorFlag == 0) 
		{ 
			direction = 0; 
		}
	}
	else
	{
		ColorFlag++;
		if(ColorFlag > 10) 
		{ 
			direction = 1; 
		} 

	}

	#if 0

	for(DataIndex = 0; DataIndex < DataSize; DataIndex++) 
	{ 
		if(1 == ColorFlag) /* Red */ 
		{ 
			if(((DataIndex+1) % 3) != 1) 
			{ 
				Data[DataIndex] = 0x00; 
			} 
			else 
			{ 
				Data[DataIndex] = 0xFF; 
			} 
		} 
		else if(2 == ColorFlag) /* Green */ 
		{ 
			if(((DataIndex+1) % 3) != 2) 
			{ 
				Data[DataIndex] = 0x00; 
			} 
			else 
			{ 
				Data[DataIndex] = 0xFF; 
			} 
		} 
		else if(3 == ColorFlag) /* Blue */ 
		{ 
			if(((DataIndex+1) % 3) != 0) 
			{ 
				Data[DataIndex] = 0x00; 
			} 
			else 
			{ 
				Data[DataIndex] = 0xFF; 
			} 
		} 
	} 
	TM1812_TransmitData(Data, DataSize); 

	memset(Data, 0, DataSize); 

	for(DataIndex = DataSize - 1; DataIndex >= 0; DataIndex--) 
	{ 
		if(1 == ColorFlag) /* Red */ 
		{ 
			if(((DataIndex+1) % 3) != 1) 
			{ 
				Data[DataIndex] = 0x00; 
			} 
			else 
			{ 
				Data[DataIndex] = 0xFF; 
			} 
		} 
		else if(2 == ColorFlag) /* Green */ 
		{ 
			if(((DataIndex+1) % 3) != 2) 
			{ 
				Data[DataIndex] = 0x00; 
			} 
			else 
			{ 
				Data[DataIndex] = 0xFF; 
			}
		} 
		else if(3 == ColorFlag) /* Blue */ 
		{ 
			if(((DataIndex+1) % 3) != 0) 
			{ 
				Data[DataIndex] = 0x00; 
			} 
			else 
			{ 
				Data[DataIndex] = 0xFF; 
			} 
		} 
		TM1812_TransmitData(Data, DataSize); 
	} 
#endif
	
	
}



