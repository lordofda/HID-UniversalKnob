/*
  Copyright (c) 2014-2015 NicoHood
  See the readme for credit to other people.

  System example
  Press a button to put pc into sleep/shut it down or wake it up again.

  You may also use SingleSystem to use a single report.

  See HID Project documentation for more Consumer keys.
  https://github.com/NicoHood/HID/wiki/System-API
*/
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 10, NEO_GRB + NEO_KHZ800);

#include "HID-Project.h"
#include <Encoder.h>
Encoder myEnc(2, 3);

const int pinLed1 = 5;
const int pinLed2 = 6;
const int pinEnabler = 7;
const int pinEncoder = 8;
int control = 0;
const int await = 15; // wait time for animation on encoder

long oldPosition  = 0;

struct color {
  int r;
  int g;
  int b;
};

struct color Conv(byte WheelPos) { //struct for ColorWheel, because I want 3 values, not one mangled together
struct color color_inst;
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    color_inst.r=255 - WheelPos * 3;
    color_inst.g=0;
    color_inst.b=WheelPos * 3;
    return color_inst;
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    color_inst.r=0;
    color_inst.g=WheelPos * 3;
    color_inst.b=255 - WheelPos * 3;
    return color_inst;
  }
    WheelPos -= 170;
    color_inst.r=WheelPos * 3;
    color_inst.g=255 - WheelPos * 3;
    color_inst.b=0;
return color_inst;
}

struct color color_obj=Conv(0);

void setup() {
  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);
  pinMode(pinEnabler, INPUT);
  pinMode(pinEncoder, INPUT);
  
  // Sends a clean report to the host. This is important on any Arduino type.
  Mouse.begin();
  Consumer.begin();
  Keyboard.begin();
  Serial.begin(9600);
  strip.begin();
  strip.show();
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  colorWipe(0,50);
}

void loop() {
  int enable = digitalRead(pinEnabler);
  if (enable == 0){
    breathe(pinLed1);
    delay(300);
  }
  else if (enable == 1){
    if (digitalRead(pinEncoder) == 1){
    control++;
    int temp=map(control,0,4,0,255);
    color color_obj=Conv(temp);
    colorWipe(strip.Color(color_obj.r, color_obj.g, color_obj.b), 25);
    colorWipe(0,25);
    }
    int temp=map(control,0,4,0,255);
    color color_obj=Conv(temp);
    long newPosition = myEnc.read();
//    Serial.print(newPosition);
//    Serial.print(" ");
//    Serial.println(control);
    switch (control) {
    case 0:
      if (newPosition != oldPosition) {
        int temp = (oldPosition - newPosition)/2;
        Mouse.move(0, 0, temp);
      }
      break;
    case 1:
      if (newPosition > oldPosition){
        Keyboard.press(KEY_LEFT_CTRL);
        delay(50);
        Keyboard.press(KEY_PAGE_UP);
        delay(50);
        Keyboard.releaseAll();
      }
      else if (newPosition < oldPosition){
        Keyboard.press(KEY_LEFT_CTRL);
        delay(50);
        Keyboard.press(KEY_PAGE_UP);
        delay(50);
        Keyboard.releaseAll();
      }
      break;
    case 2:
      if (newPosition > oldPosition){
        Keyboard.write(']');
      }
      else if (newPosition < oldPosition){
        Keyboard.write('[');
      }
      break;
    case 3:
      if (newPosition > oldPosition){
        Consumer.write(MEDIA_VOLUME_UP);
      }
      else if (newPosition < oldPosition){
        Consumer.write(MEDIA_VOLUME_DOWN);
      }
      break;
    default:
      control=0;
    break;
      }
    if (newPosition != oldPosition){
      int led=limit(newPosition/5,7);
      blank();
      strip.setPixelColor(led, color_obj.r, color_obj.g, color_obj.b);
      strip.show();
      //Serial.println(led);
    }
    oldPosition = newPosition;
  }
  else{
    //Serial.println(error);
  }
  delay(100);
}
void blank() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
    strip.show();
  }
}
void breathe(int ledpin){
  for (int i=0;i<256;i++){
    analogWrite(ledpin,i);
    delay(10);
  }
  for (int i=255;i>0;i--){
    analogWrite(ledpin,i);
    delay(10);
  }
}
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void revColorWipe(uint32_t c, uint8_t wait) {
  for(int i=strip.numPixels(); i > 0 ; i--) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
int limit(int var, int limit){
  while (var>limit){
    var=var-limit-1;
  }
  while (var<0){
    var=var+limit+1;
  }
  return var;
}

