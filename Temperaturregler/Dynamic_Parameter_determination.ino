#include <Arduino.h>
//#include <Serial.h>
#include "Temperature.h"
#include "PI.h"

float tempVal [901];
int arrayIndex = 0;
float tmpMax = 0;
float Kp = 0;
float Vt10 = 0.0;
float  Vt50 = 0.0;
float  Vt90 = 0.0;
float  t10tmp = 10.0;
float  t50tmp = 10.0;
float  t90tmp = 10.0;
int  it10 = 0;
int  it50 = 0;
int  it90 = 0;
float Temp0 = 0.0;
float T1 = 0.0;
int hasPrinted = 0;
float finalValue = 0.0;
int finalValueHasBeenCalculated = 0;
float lastDigits = 0.0;
float voltageToSet = 0.0;


void initParameterDetermination()
{
  Serial.begin(250000);

}


float calculateStartVoltage()
{
  voltageToSet = ((getSetPoint() - getstartWaterTemp()) / 0.6646) * 0.8;
  return voltageToSet;
}

float calculateStartVoltageForIpart()
{
  voltageToSet = ((getSetPoint() - getstartWaterTemp()) / getKps());
  return voltageToSet;
}

void calculateFinalValue()
{
  if (finalValueHasBeenCalculated == 0)
  {
    finalValue = 0.6646 * getStartVoltage() +  getstartWaterTemp();
    if ((finalValue - (int) finalValue) < 0.25)
    {
      finalValue = ((int) finalValue);

    }
    if ((finalValue - (int) finalValue) >= 0.25)
    {
      finalValue = ((int) finalValue) + 0.25;

    }
    else if ((finalValue - (int) finalValue) >= 0.5)
    {
      finalValue = ((int) finalValue) + 0.5;
    }
    else if ((finalValue - (int) finalValue) >= 0.75)
    {
      finalValue = ((int) finalValue) + 0.75;
    }

    finalValueHasBeenCalculated = 1;

  }
}

float getFinalValue()
{
  return finalValue;
}

boolean finish()
{
  requestTemp();
  if (getValSens2() >= finalValue - 0.5)
  {
    return true;
  }
  else
  {
    return false;
  }

}


void writeTemperature(float val)
{

  if (arrayIndex <= 900)
  {
    tempVal[arrayIndex] = val;
    arrayIndex = arrayIndex + 1;

  }

}

void printParameter()
{
  if (hasPrinted == 0)
  {
    for (int i = 0; i <= arrayIndex ; i++)
    {
      if (tempVal[i] > tmpMax)
      {
        tmpMax = tempVal[i];

      }
    }

    Kp = (tmpMax - getValSens1()) / (getStartVoltage());
    Vt10 = 0.1 * (tmpMax - tempVal[0]);
    Vt50 = 0.5 * (tmpMax - tempVal[0]);
    Vt90 = 0.9 * (tmpMax - tempVal[0]);
    it10 = 0;
    it50 = 0;
    it90 = 0;
    for (int i = 1; i < arrayIndex; i++)
    {


      if (abs(tempVal[i] - tempVal[0] - Vt10) < t10tmp)
      {

        t10tmp = abs(tempVal[i] - tempVal[0] - Vt10);
        //Serial.println(t10tmp);
        it10 = i;
      }
    }

    for (int i = 1; i < arrayIndex; i++)
    {
      if (abs(tempVal[i] - tempVal[0] - Vt50) < t50tmp)
      {
        t50tmp = abs(tempVal[i] - tempVal[0] - Vt50);
        //Serial.println(t50tmp);
        it50 = i;
      }
    }

    for (int i = 1; i < arrayIndex; i++)
    {
      if (abs(tempVal[i] - tempVal[0] - Vt90) < t90tmp)
      {
        t90tmp = abs(tempVal[i] - tempVal[0] - Vt90);
        //Serial.println(t90tmp);
        it90 = i;
      }
    }

    T1 = (1.0 / 3.0) * ((it10 * 7 / 0.531812) + (it50 * 7 / 1.678347) + (it90 * 7 / 3.889720));
    for (int i = 0; i < arrayIndex; i++)
    {
      tempVal[i] = 0;


    }

    hasPrinted = 1;
  }
}

float getT10()
{
  return it10 * 10;
}


float getT50()
{
  return it50 * 10;
}

float getT90()
{
  return it90 * 10;
}


float getKps()
{
  return Kp;
}

void setKps(float tmpKp)
{
  Kp = tmpKp;
}

