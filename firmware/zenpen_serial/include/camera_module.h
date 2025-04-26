#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif 


// user defined functions
esp_err_t camera_init(void); // camera init function
esp_err_t camera_capture(void); // capture function
void capture_and_transmit(void);



#ifdef __cplusplus
}
#endif

#endif