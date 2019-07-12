
#include "user_log.h"
#include "app.h"

#ifdef USER_DEBUG_FLAG

/*
*******************************************************************************
*/
void _mon_putc (char ch)
{
	SYS_INT_SourceDisable(INT_SOURCE_USART_2_TRANSMIT);
	//USART_TransmitterByteSend_Default(USART_ID_2, ch);
	//DRV_USART_WriteByte(UART_HandleForPrint, ch);
	//PLIB_USART_TransmitterByteSend(USART_ID_2, ch);
	U2TXREG = ch;
    while(!(DRV_USART_TransferStatus(UART_HandleForPrint)&0x08));
	

}

#endif


