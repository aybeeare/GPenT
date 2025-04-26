
#ifndef COMMS_MODULE_H
#define COMMS_MODULE_H

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#ifdef __cplusplus
extern "C" {
#endif 


// user defined functions
void config_uart(void);


#ifdef __cplusplus
}
#endif

#endif
