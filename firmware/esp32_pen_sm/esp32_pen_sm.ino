#include <Arduino.h>
#include "esp_camera.h"

//#define LED 4
#define PAYLOAD_LEN 1
#define NO_ERR 0

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

// variable declarations
char CMD;
char SYNC[1];
char HEAD[1];
char buf[1];
uint8_t * buf2;
camera_fb_t * fb;
int wee_woo;
uint32_t size = 1;
esp_err_t ERR_STATS;

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

// User Defined Fcns

esp_err_t camera_init(){

    //power up the camera if PWDN pin is defined
    if(CAM_PIN_PWDN != -1){
        pinMode(CAM_PIN_PWDN, OUTPUT);
        digitalWrite(CAM_PIN_PWDN, LOW);
    }

    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        Serial.write(0xCC);
        return err;
    }

    else {
      Serial.write(0xAA);
    }

    return ESP_OK;
}


void capture_and_transmit(camera_fb_t * fb){

// flush buffer of stale data, capture image, transmit pixel bytes...
esp_camera_fb_return(fb); // flushing
delay(25); // does this help??
fb = esp_camera_fb_get();

if (!fb) { // check for NULL pointer
    ESP_LOGE(TAG, "Camera Capture Failed");
    Serial.write(0xDD);
}

else { // If successfully get frame buffer, transmit it :)

  // Send length of buffer in bytes (size_t is 4 bytes)
  Serial.write(0x77); // TX_SYNC
  Serial.write(0xBA); // HEADer from MCU => HOST
  Serial.write((char *)&(fb->len), sizeof(fb->len));

  // // Send width of buffer
  Serial.write(0x77); // TX_SYNC
  Serial.write(0xBA); // HEADer from MCU => HOST
  Serial.write((char *)&(fb->width), sizeof(fb->width));
  
  // // Send height of buffer
  Serial.write(0x77); // TX_SYNC
  Serial.write(0xBA); // HEADer from MCU => HOST
  Serial.write((char *)&(fb->height), sizeof(fb->height));

  // Send camera pixel bytes
  
  //Serial.write((char *)(fb->buf), fb->len);
  uint8_t count = 0;
  uint8_t * tx_buf = (uint8_t *)malloc(fb->len);
  int num_chunks = 32;

  if (tx_buf != NULL){

    memcpy(tx_buf, &(fb->buf[0]), fb->len);

    for (int i = 0; i < fb->len; i = i + ((fb->len)/num_chunks)){ // >> 9 to divide by 512, UXGA, >> 5 to divide into chunks of 32
      count = count + 1;
      Serial.write(0x77); // TX_SYNC
      Serial.write(0xBA); // HEADer from MCU => HOST
      //Serial.write((uint8_t *)&(count), sizeof(count));
      //Serial.write((char *)&(tx_buf[i]), sizeof(&(tx_buf[i])));
      delay(1500);
      Serial.write((char *)&(tx_buf[i]), ((fb->len)/num_chunks)); 
      delay(1500);
    }

    free(tx_buf);
    esp_camera_fb_return(fb); // return fb back to driver for reuse
  }

  // malloc failure message
  Serial.write(0xDE);
  Serial.write(0xAD);
  Serial.write(0xBE);
  Serial.write(0xEF);
}

//return the frame buffer back to the driver for reuse
//fb = NULL;
}

// Frame Buffer Struct Contents

// typedef struct {
//     uint8_t * buf;              /*!< Pointer to the pixel data */
//     size_t len;                 /*!< Length of the buffer in bytes */
//     size_t width;               /*!< Width of the buffer in pixels */
//     size_t height;              /*!< Height of the buffer in pixels */
//     pixformat_t format;         /*!< Format of the pixel data */
//     struct timeval timestamp;   /*!< Timestamp since boot of the first DMA buffer of the frame */
// } camera_fb_t;


void setup() {
  
  Serial.begin(115200);
  //pinMode(LED, OUTPUT);
  ERR_STATS = camera_init();
  //delay(1000);

  // Configure Brightness
  // sensor_t * s = esp_camera_sensor_get();
  // s->set_exposure_ctrl(s,0); // turns off auto exposure control
  // s->set_aec_value(s,400); // set lower exposure
  // s->set_brightness(s,-2);

  wee_woo = 0;
  while (wee_woo){ // enable or disable debug scratchpad with weewoo

    // custom config of camera exposure
    sensor_t * s = esp_camera_sensor_get();
    if (s != NULL){
      Serial.write(0x77); // TX_SYNC
      Serial.write(0xBA); // HEADer from MCU => HOST

      s->set_exposure_ctrl(s,0); // turns off auto exposure control
      s->set_aec_value(s,1200); // set lower exposure
      s->set_brightness(s,2);
    }

    
    //buf2 = (uint8_t *)&(size);
    //Serial.write(buf2, sizeof(size)); 
    //ERR_STATS = camera_init();
    delay(5000);
    // delay(5000);
  }
}

void loop() {
  
  if (Serial.available() >= 2){
    
    Serial.read(SYNC, 1);
    Serial.read(HEAD, 1);

  // if HEAD && SYNC received
  if (SYNC[0] == 0xCB){ // did not enter...
    
    SYNC[0] = 0x00; // reset SYNC

    if (HEAD[0] == 0xAB){ // should be AB, testing the receiving of header
    
      HEAD[0] = 0x00; // reset HEAD

      // while (Serial.available() < PAYLOAD_LEN){ // more general
        
      //   for (int i = 0; i < PAYLOAD_LEN; i++){
      //     Serial.read(buf[i], PAYLOAD_LEN);
      //   }
      // }
    
      while (Serial.available() < PAYLOAD_LEN){   
          // do nothing while waiting for payload
      }

      Serial.read(buf, PAYLOAD_LEN);
      CMD = buf[0];

      if (CMD != 0x00){
        
        switch (CMD){

          case 0x76: // stream bytes from camera capture

            // Capture image and send frame buffer
            fb = esp_camera_fb_get();
            capture_and_transmit(fb);
            break;
          

          case 0x69: // send a 20 byte payload of test data to test serial coms

            Serial.write(0x77); // TX_SYNC
            Serial.write(0xBA); // HEADer from MCU => HOST

            // payload of 20 bytes
            for (int i = 0; i < 5; i++){
              Serial.write(0xCB);
              Serial.write(0xAB);
              Serial.write(0xCB);
              Serial.write(0xBA);
            }

            break;

          default: 
              break;

        }
      }
    }
  }
  }

    else{
      // if no bytes received, do nothing, stay at top of loop
    }
  }

