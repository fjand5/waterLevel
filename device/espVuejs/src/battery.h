#include <Arduino.h>
#include "store.h"
void updateBattery();

uint32_t batteryTimer = millis();
void loopBattery() {

  if(millis() - batteryTimer > 2000){
      batteryTimer = millis();
      updateBattery();
  }
}
float getBatteryVoltage(){
    float raw = analogRead(A0);
    float voltage ;
    voltage = raw*6.20/1000.00;
    return voltage;
}
void updateBattery(){
    setValue("batteryVoltage",String(getBatteryVoltage()));
}
