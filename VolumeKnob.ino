// Based on the Oversized Media Control Volume Knob project from Prusa Research.
// https://blog.prusaprinters.org/3d-print-an-oversized-media-control-volume-knob-arduino-basics_30184/

#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define ENCODER_CLK A0 
#define ENCODER_DT A1
#define ENCODER_SW A2

volatile ClickEncoder encoder = ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW); 

void timerISR() {
  encoder.service();
}

void setup() {
  encoder.setAccelerationEnabled(true);
  
  Serial.begin(115200);
  Consumer.begin();
  Mouse.begin();
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerISR);
}

const bool MODE_VOLUME = false;
const bool MODE_SCROLL = true;

void loop() {  
  static bool mode = MODE_VOLUME;
  static bool isHeld = false;
  static int16_t realValue = 0;
  realValue += encoder.getValue();

  switch (encoder.getButton()) {
    case ClickEncoder::Clicked:
      Serial.println("clicked");
      if (mode == MODE_VOLUME) Consumer.write(MEDIA_PLAY_PAUSE);
    break;      
    case ClickEncoder::DoubleClicked:
      Serial.println("double clicked");
      if (mode == MODE_VOLUME) Consumer.write(MEDIA_NEXT); 
    break;
    case ClickEncoder::Held:
      Serial.println("held");
      isHeld = true;
    break;      
    case ClickEncoder::Open:
      if (isHeld) {
        Serial.println("long clicked");
        // Consumer.write(MEDIA_PREV);
        isHeld = false;
        mode = !mode;
      }
  
      int16_t fullStepValue = realValue / 2;
      static int16_t fullStepLast = 42;
 
      if (mode == MODE_VOLUME) {
        if (fullStepValue != fullStepLast) {
          if (fullStepLast < fullStepValue){
            Consumer.write(MEDIA_VOLUME_UP);
          } else {
            Consumer.write(MEDIA_VOLUME_DOWN); 
          }
        }
      } else if (mode == MODE_SCROLL) {
        if (fullStepValue != fullStepLast) {
          if (fullStepValue < fullStepLast) {
            Mouse.move(0, 0, 1);
          } else {
            Mouse.move(0, 0, -1);
          }
        }
      }
      
      fullStepLast = fullStepValue;
    break;
  }
}
