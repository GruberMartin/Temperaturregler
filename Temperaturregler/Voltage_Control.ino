#include <Arduino.h>
#include "Temperature.h"
#include "PI.h"
#include "Dynamic_Parameter_determination.h"
#define heatPin  10
#define agitataorPin 9

int maxCount = 20000;
unsigned long previousTimeNew = 0;
float onTime = 0.0;
unsigned int counter = 0;
int onTimeHasBeenSet = 0;

void initVoltageControll() {
  pinMode(heatPin, OUTPUT);
  digitalWrite(heatPin, LOW);
  pinMode(agitataorPin, OUTPUT);
  digitalWrite(agitataorPin, LOW);

}

boolean getHeatPinState()
{
  if (digitalRead(heatPin) == LOW)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void setOnTime(float voltage)
{
  //if(onTimeHasBeenSet == 0)
  //{
  onTime = ((sq(voltage) / 35.0) * maxCount) / 1511.43;
  //onTimeHasBeenSet = 1;

  //}
}

void turnOffHeating()
{
  digitalWrite(heatPin, LOW);
}

void turnOffAgitator()
{
  digitalWrite(agitataorPin, LOW);
}

void setVoltage(float voltage) {


  if (getAgitatorState() == true)
  {
    digitalWrite(agitataorPin, HIGH);
  }
  else
  {
    digitalWrite(agitataorPin, LOW);
  }


  // genearte PWM 1% = 20ms  100% = 20'000ms = 230V
  setOnTime(voltage);
  //Serial.print(getFinalValue());
  //secCounter();

  
  if (onTime < 20)
  {
    onTime = 0;
  }

  if (millis() >= (previousTimeNew))
  {
    previousTimeNew = previousTimeNew + 1;  // use 100000 for uS
    counter = counter + 1;


  }
  if (onTime >= counter && onTime != 0 && false == hardStop())
  {
    digitalWrite(heatPin, HIGH);
  }
  else if (counter < maxCount)
  {
    digitalWrite(heatPin, LOW);
  }

  else
  {
    counter = 0;
  }


}
