#include <Arduino.h>
//#include "esp_camera.h"

#define LED 4
#define PAYLOAD_LEN 1
#define NO_ERR 0

// variables
char CMD;
char SYNC[1];
char HEAD[1];
char buf[1];
int READ_STATUS;
int wee_woo;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  wee_woo = 0;
  while (wee_woo){ // quick test of programability to go into led program depending on weewoo
    digitalWrite(LED, LOW);
    delay(1000);
    digitalWrite(LED, HIGH);
    delay(1000);
    Serial.write(0x77); // TX_SYNC
    delay(5000);
    Serial.write(0xBA); // HEADer from MCU => HOST
    delay(5000);
    Serial.write(0xAB);
    delay(5000);
    Serial.write(0xCB);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial.available() >= 2){
    
    Serial.read(SYNC, 1);
    Serial.read(HEAD, 1);

  // if HEAD && SYNC received
  if (SYNC[0] == 0xCB){ // did not enter...
    
    SYNC[0] = 0x00; // reset SYNC

    if (HEAD[0] == 0xAB){ // should be AB, testing the receiving of header
    
    HEAD[0] = 0x00; // reset HEAD
    Serial.write(0xBA);

    // while (Serial.available() < PAYLOAD_LEN){ // more general
      
    //   for (int i = 0; i < PAYLOAD_LEN; i++){
    //     Serial.read(buf[i], PAYLOAD_LEN);
    //   }
    // }
    READ_STATUS = 1;
    while (Serial.available() < PAYLOAD_LEN){   
        // do nothing while waiting for payload
    }
    Serial.read(buf, PAYLOAD_LEN);
    READ_STATUS = 0;
    CMD = buf[0];

    if (READ_STATUS == NO_ERR){
      
      switch (CMD){

        case 0x76: // stream serial data 1

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
        

        case 0x69: // stream serial data 2

          Serial.write(0x77); // TX_SYNC
          Serial.write(0xBA); // HEADer from MCU => HOST

          // payload of 20 bytes
          for (int i = 0; i < 5; i++){
            Serial.write(0x12);
            Serial.write(0x34);
            Serial.write(0x56);
            Serial.write(0x78);
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
      // Serial.write(0x77); // TX_SYNC
      // Serial.write(0xBA); // HEADer from MCU => HOST
      // Serial.write(0xCC); // Error Data..
      // Serial.write(0xAA); 
      // Serial.write(0xBB); 
      // Serial.write(0xFF); 
      
    }
  }

