
#include <xc.h>
#include "service.h"
#include "app.h"


/////////////////////////////////////////////////////////////////////////////////
void uartPrintArry(uint8_t* data, uint16_t dataSize)
{
    while(dataSize)
    {
        DRV_USART_WriteByte(UART_HandleForPrint, *data++);
        while(!(DRV_USART_TransferStatus(UART_HandleForPrint)&0x08));
        dataSize--;
    }
} 

/////////////////////////////////////////////////////////////////////////////////
void uartPrintMsg(char* text)
{
    uint16_t size = strlen(text);
    while(size)
    {
        DRV_USART_WriteByte(UART_HandleForPrint, *text++);
        while(!(DRV_USART_TransferStatus(UART_HandleForPrint)&0x08));
        size--;
    }
} 

