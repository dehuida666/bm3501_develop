
#ifndef SERVICE_H
#define SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "system_config.h"
#include "system_definitions.h"

void uartPrintArry(uint8_t* data, uint16_t dataSize);
void uartPrintMsg(char* text);
extern uint16_t readAD5(void);      //implemented in drv_adc_static.c
extern uint16_t readAD4(void);      //implemented in drv_adc_static.c
extern uint16_t readAD3(void);      //implemented in drv_adc_static.c
        


#endif
