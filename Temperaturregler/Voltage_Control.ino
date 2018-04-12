#include <Arduino.h>
#include "Temperature.h"
#include "Dynamic_Parameter_determination.h"
#define heatPin  10
unsigned long time;       //Startzeit
int maxCount = 20000;
unsigned long previousTimeNew = 0;
float onTime = 0.0;
int counter = 0;
int onTimeHasBeenSet = 0;

void initVoltageControll() {
  pinMode(heatPin,OUTPUT);
  digitalWrite(heatPin,LOW);
 
}

void setOnTime(float voltage)
{
  if(onTimeHasBeenSet == 0)
  {
  onTime = ((sq(voltage)/35.0) * 20000)/1511.0;
  onTimeHasBeenSet = 1;
  
  }
}

void turnOffHeating()
{
   digitalWrite(heatPin,LOW);
}

void setVoltage(float voltage) {
// genearte PWM 1% = 20ms  100% = 20'000ms = 230V
setOnTime(voltage);
//Serial.print(getFinalValue());
  //secCounter();
  
if (millis() >= (previousTimeNew))
    {
      previousTimeNew = previousTimeNew + 1;  // use 100000 for uS    
      counter = counter + 1;
     
    
    }
   if(onTime >= counter)
   {
    digitalWrite(heatPin,HIGH);
    //Serial.println(counter);
   }
   else if(counter < maxCount)
   {
   digitalWrite(heatPin,LOW);
   //Serial.println("Hiezung aus");
   }
   else
   {
    counter = 0;
   }
  
   
}
