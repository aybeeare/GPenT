#include "camera_module.h"
#include "driver/gpio.h"
#include "comms_module.h"
#include "string.h"

// ESP-S Pin Map to OV2640 Camera
#define CAM_PIN_PWDN    32 
#define CAM_PIN_RESET   -1 
#define CAM_PIN_XCLK    0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39 
#define CAM_PIN_D4      36 
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

const uart_port_t uart_num = UART_NUM_0;

static camera_config_t camera_config = {
    .pin_pwdn  = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,
  
    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,
  
    .xclk_freq_hz = 20000000,//EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
  
    .pixel_format = PIXFORMAT_GRAYSCALE,//YUV422,GRAYSCALE,RGB565,JPEG e.g "PIXFORMAT_JPEG"
    //.frame_size = FRAMESIZE_UXGA,//QQVGA/UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.
    .frame_size = FRAMESIZE_CIF, 
    .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1, //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY//CAMERA_GRAB_LATEST. Sets when buffers should be filled
    };

// constants defined...
u_int8_t INIT_FAIL = 0xCC;
u_int8_t INIT_SUCCESS = 0xAA;
u_int8_t CAPTURE_FAIL = 0xDD;
u_int8_t CAPTURE_SUCCESS = 0xBB;
u_int8_t SYNC = 0x77;
u_int8_t HEAD = 0xBA;

esp_err_t camera_init(){

    //power up the camera if PWDN pin is defined
    if(CAM_PIN_PWDN != -1){
        gpio_set_direction(CAM_PIN_PWDN, GPIO_MODE_OUTPUT);
        gpio_set_level(CAM_PIN_PWDN, 0);
    }

    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK) {
        uart_write_bytes(uart_num, (const void *)&INIT_FAIL, 1);
        return err;
    }

    else {
      uart_write_bytes(uart_num, (const void *)&INIT_SUCCESS, 1);
    }

    return ESP_OK;
}

esp_err_t camera_capture(){

    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    
    if (!fb) { // check for NULL pointer
        uart_write_bytes(uart_num, (const void *)&CAPTURE_FAIL,1);
        return ESP_FAIL;
    }
    
    else { // If successfully get frame buffer, transmit it :)
        uart_write_bytes(uart_num, (const void *)&CAPTURE_SUCCESS,1);
    }
    
    //replace this with custom processing fcn
    //process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);
    
    //return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
    return ESP_OK;
    }

void capture_and_transmit(){

    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    
    if (!fb) { // check for NULL pointer
        uart_write_bytes(uart_num, (const void *)&CAPTURE_FAIL,1);
    }
    
    else { // If successfully get frame buffer, transmit it :)
    
        // Send length of buffer in bytes (size_t is 4 bytes)
        uart_write_bytes(uart_num, (const void *)&SYNC,1);
        uart_write_bytes(uart_num, (const void *)&HEAD,1);
        uart_write_bytes(uart_num, (const void *)&(fb->len), sizeof(fb->len));
    
        // Send width of buffer
        uart_write_bytes(uart_num, (const void *)&SYNC,1);
        uart_write_bytes(uart_num, (const void *)&HEAD,1);
        uart_write_bytes(uart_num, (const void *)&(fb->width), sizeof(fb->width));
        
        // Send height of buffer
        uart_write_bytes(uart_num, (const void *)&SYNC,1);
        uart_write_bytes(uart_num, (const void *)&HEAD,1);
        uart_write_bytes(uart_num, (const void *)&(fb->height), sizeof(fb->height));
    
        // Send camera pixel bytes
        
        //Serial.write((char *)(fb->buf), fb->len);
        uint8_t count = 0;
        uint8_t * tx_buf = (uint8_t *)malloc(fb->len);
        int num_chunks = 32;
    
        if (tx_buf != NULL){
    
        memcpy(tx_buf, &(fb->buf[0]), fb->len);
    
        for (int i = 0; i < fb->len; i = i + ((fb->len)/num_chunks)){ // >> 9 to divide by 512, UXGA, >> 5 to divide into chunks of 32
            count = count + 1;
            uart_write_bytes(uart_num, (const void *)&SYNC,1);
            uart_write_bytes(uart_num, (const void *)&HEAD,1);
            //Serial.write((uint8_t *)&(count), sizeof(count));
            //Serial.write((char *)&(tx_buf[i]), sizeof(&(tx_buf[i])));
            vTaskDelay(pdMS_TO_TICKS(1500));
            uart_write_bytes(uart_num, (const void *)&(tx_buf[i]),((fb->len)/num_chunks));
            vTaskDelay(pdMS_TO_TICKS(1500));
        }
    
        free(tx_buf);
        esp_camera_fb_return(fb); // return fb back to driver for reuse
        }
    }
    }