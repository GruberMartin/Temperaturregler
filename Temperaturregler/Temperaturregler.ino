#include <DallasTemperature.h>
#include <TimedAction.h>


//For more information visit: http://aeq-web.com/?ref=arduinoide


#include "Dynamic_Parameter_determination.h"
#include "PI.h"
#include "Voltage_Control.h"
#include "Temperature.h"
#define schalter 8
#define myPin 7

unsigned long previousTime = 0;
int seconds ;
int sevenSecCounter = 0;
int fiveSecCounter = 0;
int started = 0;
int voltageHasBeenSet = 0;
float startVoltage = 0.0;
boolean PIisOn = false;
boolean gotT = false;
int sampleTime = 0;
int sampleCounter = 0;
float voltageSetNow = 0.0;
boolean gotVoltage = false;
boolean doNewCalc = false;
float tempMaxTemp = 0.0;
float maxTemp = 0.0;
boolean antiDeadLockActivated = false;
int deadLockCounter = 0;
boolean deadLockHasReachedCriticalValue = false;

typedef enum {
  notStarted_Main,
  started_Main,
  PI_on_Main,
  getParameter,
  gotParameter
  
} main_states;

main_states current_main_state = notStarted_Main;

void setup()
{
  pinMode(schalter, INPUT);
  pinMode(myPin, OUTPUT);  
  digitalWrite(myPin, HIGH);
  initParameterDetermination();
  initPID();
  initTemperature();
  initVoltageControll();
}

void setMaxTemp()
{
  maxTemp = 0.8 * getFinalValue();
  
}



void writeTemppToArray()
{
  if (sevenSecCounter == 7)
      {        
        sevenSecCounter = 0;
        writeTemperature(getValSens2());
      }
}

void antiDeadLock()
{
  if(antiDeadLockActivated == false)
  {
  setMaxTemp();
  antiDeadLockActivated = true;
  }
    
}

void secCounter()
{
  if (millis() >= (previousTime))
    {
      previousTime = previousTime + 1000;  // use 100000 for uS
      seconds = seconds + 1;

      //doVoltageControll.check();
      
      //Serial.print("TEMP: ");
      //  printTemperature("Inside : ", sensor1);
      requestTemp();

      
      if(antiDeadLockActivated == true && PIisOn == false)
      {
          
        
          if( maxTemp < getValSens2())
          {
            deadLockHasReachedCriticalValue = true;
            deadLockCounter = 0;
            maxTemp =  getValSens2();
          }
          else
          {
            if(deadLockHasReachedCriticalValue == true)
            {
            deadLockCounter = deadLockCounter + 1;
            }
          }
      }

      
      if(deadLockCounter >= 360 && PIisOn == false)
      {
        antiDeadLockActivated = false;
        setMainState(gotParameter);
      }
      
      if(PIisOn == true)
      {
        
        if(gotT == false)
        {
          sampleTime = (int)getT();
          sampleCounter = 0;
          gotT = true;
          /*Serial.print("smapleTime = ");
          Serial.println(sampleTime);*/
        }

        if(sampleCounter == sampleTime)
        {
        gotVoltage = false;
        //TimedAction doVoltageControll = TimedAction(sampleTime,myCotroller);
        doNewCalc = true;
        sampleCounter = 0;
        }
       


        if(getError() <= 0.0)
      {
         doNewCalc = true;
      }
        
        
        
        //controlVoltage();        
      }

      
      printSensorVals();
      Serial.println(seconds, DEC);
      sevenSecCounter = sevenSecCounter + 1;
      fiveSecCounter = fiveSecCounter + 1;
      sampleCounter = sampleCounter + 1;

  
    }
}

boolean calcVoltage()
{
  return doNewCalc;
}

void setDonewCalc()
{
  doNewCalc = false;
}

int getSeconds()
{
  return seconds;
}

/*void myCotroller()
{
  if(gotVoltage == false)
  {
  voltageSetNow = getCurrentVoltage();
  gotVoltage = true;
  }
  setVoltage(voltageSetNow);
}*/

void setMainState(main_states newState)
{
  current_main_state = newState;
}

void setStartVoltage()
{
  if(voltageHasBeenSet == 0)
  {
  setVoltage(calculateStartVoltage());
  voltageHasBeenSet = 1;
  setVarStartVoltage(calculateStartVoltage());
  }
  
}

float getStartVoltage()
{
  return startVoltage;
}

void setVarStartVoltage(float startVoltageEnter)
{
  startVoltage = startVoltageEnter;
}

main_states getMainState()
{
  return current_main_state;
}

void loop()
{

  
    
    current_main_state = getMainState();
    
  switch(current_main_state)
  {
    case notStarted_Main: 
    //requestTemp();
    secCounter();
    current_main_state = getParameter;
    break;
    case getParameter:
    setStartVoltage();    
    calculateFinalValue();    
    setVoltage(getStartVoltage());
    //requestTemp();
    secCounter();
    writeTemppToArray();
    antiDeadLock();
    break;
    case gotParameter:
    secCounter();
    //turnOffHeating();
    printParameter();
    setCurrentState(start_PI);
    setStartVoltageIPart(getStartVoltage());
    current_main_state = PI_on_Main; 
    Serial.println("PI Regler ist jetzt aktiv");   
    break;
    case PI_on_Main:
    //Serial.println("PI läuft");
    secCounter();
     PIisOn = true;
     controlVoltage();
  }




}
