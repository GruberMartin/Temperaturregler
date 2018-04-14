#include <Arduino.h>
//#include <Serial.h>
#include "Temperature.h"

float tempVal [1601];
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



void initParameterDetermination()
{
  Serial.begin(250000);
  
}


void calculateFinalValue()
{
  if(finalValueHasBeenCalculated == 0)
  {
  finalValue = 0.2625 * getStartVoltage() +  getValSens1();
  if((finalValue - (int) finalValue)> 0.25)
  {
    finalValue = ((int) finalValue);
    
    }
    else if((finalValue - (int) finalValue)> 0.5)
    {
      finalValue = ((int) finalValue) + 0.5;
    }
    //Serial.println(finalValue);
  finalValueHasBeenCalculated = 1;
  
  //Serial.println(finalValue);
  }
}

float getFinalValue()
{
  return finalValue;
}

boolean finish()
{
  requestTemp();
  if(getValSens2() >= finalValue)
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
  
  if((finalValue-0.75) <= getValSens2())
  {
    setMainState(gotParameter);
  }
  else if (arrayIndex <= 1600)
        {
          tempVal[arrayIndex] = val;
          arrayIndex = arrayIndex + 1;
          
        }
        else
        {
          setMainState(gotParameter);
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
      /*Serial.print("Array Index: ");
      Serial.println(arrayIndex);
      Serial.print("Anfangstemperatur: ");
      Serial.println(tempVal[0]);
      Serial.print("Spannung: ");
      Serial.println(getStartVoltage());
      Serial.print("Maximale Temperatur: ");
      Serial.println(tmpMax, 2);*/
      Kp = (tmpMax - getValSens1()) / (getStartVoltage());
      /*Serial.print("Kp :");
      Serial.println(Kp, 4);*/

      Vt10 = 0.1 * (tmpMax - tempVal[0]);
      Vt50 = 0.5 * (tmpMax - tempVal[0]);
      Vt90 = 0.9 * (tmpMax - tempVal[0]);
      /*Serial.print("Vt10 ist: ");
      Serial.println(Vt10);
      Serial.print("Vt50 ist: ");
      Serial.println(Vt50);
      Serial.print("Vt90 ist: ");
      Serial.println(Vt90);*/

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
      /*Serial.print("t1 = (7 *) ");
      Serial.println(it10);*/

      for (int i = 1; i < arrayIndex; i++)
      {
        if (abs(tempVal[i] - tempVal[0] - Vt50) < t50tmp)
        {
          t50tmp = abs(tempVal[i] - tempVal[0] - Vt50);
          //Serial.println(t50tmp);
          it50 = i;
        }
      }

      /*Serial.print("t50 = (7 *) ");
      Serial.println(it50);*/


      for (int i = 1; i < arrayIndex; i++)
      {
        if (abs(tempVal[i] - tempVal[0] - Vt90) < t90tmp)
        {
          t90tmp = abs(tempVal[i] - tempVal[0] - Vt90);
          //Serial.println(t90tmp);
          it90 = i;
        }
      }
      /*Serial.print("t90 = (7 *) ");
      Serial.println(it90);*/
      T1 = (1.0 / 3.0) * ((it10 * 7 / 0.531812) + (it50 * 7 / 1.678347) + (it90 * 7 / 3.889720));
     /* Serial.print("T1: ");
      Serial.println(T1);*/
      for (int i = 0; i < arrayIndex; i++)
      {
        tempVal[i] = 0;


      }

      hasPrinted = 1;
    }
}

float getT10()
{
  return it10 * 7;
}


float getT50()
{
  return it50 * 7;
}

float getT90()
{
  return it90 * 7;
}


float getKps()
{
  return Kp;
}

