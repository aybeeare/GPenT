#include "camera_module.h"
#include "comms_module.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void app_main(void) {

    // local defines
    int wee_woo = 1;
    u_int8_t SYNC_VALUE = 0x77;
    u_int8_t HEAD_VALUE = 0xBA;
    u_int8_t CB_VALUE = 0xCB;
    u_int8_t AB_VALUE = 0xAB;
    size_t   buf_len;
    char SYNC[1];
    char HEAD[1];
    char CMD[1];
    esp_err_t CAM_INIT;
    const uart_port_t uart_num = UART_NUM_0;

    // Configure UART driver
    config_uart();

    // Configure Camera
    CAM_INIT = camera_init();
    

    while (wee_woo) {

        sensor_t * s = esp_camera_sensor_get();
        
        if (s != NULL){
            uart_write_bytes(uart_num, (const void *)&SYNC_VALUE,1);
            uart_write_bytes(uart_num, (const void *)&HEAD_VALUE,1);
            s->set_exposure_ctrl(s,0); // turns off auto exposure control
            s->set_aec_value(s,1200); // set lower exposure
            s->set_brightness(s,2);
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }

    while(1){
        uart_get_buffered_data_len(uart_num, &buf_len);

        if (buf_len >= 2){

            uart_read_bytes(uart_num, SYNC, sizeof(SYNC), 100/portTICK_PERIOD_MS); // 100 ms timeout
            uart_read_bytes(uart_num, HEAD, sizeof(SYNC), 100/portTICK_PERIOD_MS); 

            if (SYNC[0] == 0xCB){

                SYNC[0] = 0x00; // reset SYNC

                if (HEAD[0] == 0xAB){

                    HEAD[0] = 0x00;
                }

                while (buf_len < 1){
                    uart_get_buffered_data_len(uart_num, &buf_len);
                }

                uart_read_bytes(uart_num, CMD, sizeof(CMD), 100/portTICK_PERIOD_MS);

                if (CMD[1] != 0x00){

                    switch(CMD[1]){

                        case 0x76:

                            capture_and_transmit();
                            break;

                        case 0x69:

                            uart_write_bytes(uart_num, (const void *)&SYNC_VALUE,1);
                            uart_write_bytes(uart_num, (const void *)&HEAD_VALUE,1);

                            for (int i = 0; i < 5; i++){
                                uart_write_bytes(uart_num, (const void *)&CB_VALUE,1);
                                uart_write_bytes(uart_num, (const void *)&AB_VALUE,1);
                                uart_write_bytes(uart_num, (const void *)&CB_VALUE,1);
                                uart_write_bytes(uart_num, (const void *)&AB_VALUE,1);   
                            }

                            break;

                        default:
                            break;
                    }
                }
            }
        }
    }
}