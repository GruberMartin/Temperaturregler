#include <Arduino.h>
#include "Temperature.h"
#include "PI.h"
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
  //if(onTimeHasBeenSet == 0)
  //{
  onTime = (voltage * 20000)/1511.43;
  //onTimeHasBeenSet = 1;
  
  //}
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

  
if(onTime < 20)
{
  onTime = 0;
}
  
if (millis() >= (previousTimeNew))
    {
      previousTimeNew = previousTimeNew + 1;  // use 100000 for uS    
      counter = counter + 1;
      /*if(hardStop() == true)
      {
        //Serial.println("Stopppppppppppppppppppp !!!!!!");
        onTime = 0;
      }*/
    
    
    }
   if(onTime >= counter && onTime != 0 && false == hardStop())
   {
    digitalWrite(heatPin,HIGH);
    //Serial.println(counter);
   }
   else if(counter < maxCount)
   {
   digitalWrite(heatPin,LOW);
   //Serial.println("Hiezung aus");
   }
   /*else if(hardStop())
   {
    
   digitalWrite(heatPin,LOW);
   }*/
   else
   {
    counter = 0;
   }
  
   
}
