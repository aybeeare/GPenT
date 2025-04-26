#include "comms_module.h"

// UART GPIO config for ESP32-CAM board

#define TXD_PIN 1 // GPIO1
#define RXD_PIN 3 // GPIO3 

void config_uart(){

    const uart_port_t uart_num = UART_NUM_0; // UART controller 0 connected to GPIO1 and 3

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS, 
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, 1024, 0, 0, NULL, 0);
}

