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




void writeTemppToArray()
{
  if (sevenSecCounter == 7)
      {        
        sevenSecCounter = 0;
        writeTemperature(getValSens2());
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
       


        if(getError() < 0.5)
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

void setStartVoltage(float startVoltageEnter)
{
  if(voltageHasBeenSet == 0)
  {
  setVoltage(startVoltageEnter);
  voltageHasBeenSet = 1;
  setVarStartVoltage(startVoltageEnter);
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
    setStartVoltage(182.857);    
    calculateFinalValue();
    setVoltage(getStartVoltage());
    //requestTemp();
    secCounter();
    writeTemppToArray();
    break;
    case gotParameter:
    secCounter();
    //turnOffHeating();
    printParameter();
    setCurrentState(start_PI);
    current_main_state = PI_on_Main; 
    Serial.println("PI Regler ist jetzt aktiv");   
    break;
    case PI_on_Main:
    secCounter();
     PIisOn = true;
     controlVoltage();
  }




}
