#include <DallasTemperature.h>
#include <TimedAction.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

#include "Display.h"

//For more information visit: http://aeq-web.com/?ref=arduinoide


#include "Dynamic_Parameter_determination.h"
#include "PI.h"
#include "Voltage_Control.h"
#include "Temperature.h"
#include "Display.h"
#include "SD.h"
#define schalter 8
#define myPin 7

unsigned long previousTime = 0;
unsigned long seconds ;
int tenSecCounter = 0;
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
boolean temperatureIsStable = false;
int stabCounter = 0;
boolean rechedFinalState = false;
boolean fastTempControll = false;
boolean globalStart = false;

String timeString = "";
int hours = 0;
int minutes = 0;
int tempUser = 0;
int tempUserDot = 0;

unsigned long endTime = 0;
float tempTemp = 0.0;
boolean endtimeHasBeenSet = false;
boolean startLcdTempPrinting = false;
boolean startWithGivenParametersRequest = false;
boolean changeRegulator = false;



// These #defines make it easy to set the backlight color
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#define BLUE 0x4
#define WHITE 0x7
uint8_t buttons;
boolean displayedStartMessgae = false;




typedef enum {
  getUserInput,
  getCookingMode,
  fastCookingModeTime,
  fastCookingModeTemp,
  notStarted_Main,
  started_Main,
  PI_on_Main,
  getParameter,
  gotParameter,
  startWithGivenParameters,
  globalShutDown

} main_states;

main_states current_main_state = getUserInput;



void setup()
{

  initDisplay();
  pinMode(schalter, INPUT);
  pinMode(myPin, OUTPUT);
  digitalWrite(myPin, HIGH);
  initParameterDetermination();
  initPID();
  initTemperature();
  initVoltageControll();
  requestTemp();

}

void setMaxTemp()
{
  maxTemp = 0.6 * getFinalValue();

}



boolean requestRegulatorChange()
{
  return changeRegulator;
}



void writeTemppToArray()
{
  if (tenSecCounter == 10)
  {
    tenSecCounter = 0;
    writeTemperature(getValSens2());
  }
}

void requestGlobalStart()
{
  globalStart = true;
  previousTime = millis();
}

void setEndTime(unsigned long endT)
{
  endTime = endT;
}

void antiDeadLock()
{
  if (antiDeadLockActivated == false)
  {
    setMaxTemp();
    antiDeadLockActivated = true;
  }

}

void secCounter()
{
  if (millis() >= (previousTime) && globalStart == true)
  {
    previousTime = previousTime + 1000;  // use 100000 for uS
    seconds = seconds + 1;
    requestTemp();
    if ((seconds >= endTime) && endtimeHasBeenSet == true)
    {
      current_main_state = globalShutDown;
    }
    if (temperatureIsStable == false && PIisOn == true)
    {
      if (getError() == 0.0)
      {
        stabCounter = stabCounter + 1;
      }
      if (stabCounter >= 1)
      {
        temperatureIsStable = true;
        rechedFinalState = true;
      }

    }

    if (temperatureIsStable == true && rechedFinalState == true || fastTempControll == true)
    {
      if (endtimeHasBeenSet == false)
      {
        setEndTime(endTime + getSeconds());
        endtimeHasBeenSet = true;
      }
      rechedFinalState = false;
      fastTempControll = true;
      stabCounter = 0;
      changeRegulator = true;
      imediateCalcVoltage();
    }

    if (antiDeadLockActivated == true && PIisOn == false)
    {


      if ( maxTemp < getValSens2())
      {
        deadLockHasReachedCriticalValue = true;
        deadLockCounter = 0;
        maxTemp =  getValSens2();
      }
      else
      {
        if (deadLockHasReachedCriticalValue == true)
        {
          deadLockCounter = deadLockCounter + 1;
        }
      }
    }


    if (deadLockCounter >= 300 && PIisOn == false)
    {
      antiDeadLockActivated = false;
      setMainState(gotParameter);
    }

    if (PIisOn == true)
    {

      if (gotT == false)
      {
        sampleTime = (int)getT();
        sampleCounter = 0;
        gotT = true;
      }

      if (sampleCounter == sampleTime)
      {
        gotVoltage = false;
        doNewCalc = true;
        sampleCounter = 0;
      }

    }

    if ((tempTemp != getValSens2()) && startLcdTempPrinting == true)
    {
      tempTemp = getValSens2();
      if (PIisOn == false)
      {
        disPrintActualTemp(tempTemp);
      }
      else
      {
        disPrintRegualtorActivated(tempTemp);
      }
    }
    printSensorVals();
    Serial.println(seconds, DEC);
    tenSecCounter = tenSecCounter + 1;
    sampleCounter = sampleCounter + 1;


  }
}

boolean calcVoltage()
{
  return doNewCalc;
}



void imediateCalcVoltage()
{
  doNewCalc = true;
}

void setDonewCalc()
{
  doNewCalc = false;
}

unsigned long getSeconds()
{
  return seconds;
}


void setMainState(main_states newState)
{
  current_main_state = newState;
}

void setStartVoltage()
{
  if (voltageHasBeenSet == 0)
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

  switch (current_main_state)
  {
    case getUserInput:
      waitForStartSignal();
      break;

    case getCookingMode:
      waitForCookingMode();


      break;
    case fastCookingModeTime:
      getCookingTime();
      break;
    case fastCookingModeTemp :
      getCookingTemp();
      break;
    case notStarted_Main:
      setSetPoint(((float)tempUserDot / 100) + tempUser);
      endTime = hours * 60 * 60 + minutes * 60;
      secCounter();
      if (startWithGivenParametersRequest == false)
      {
        current_main_state = getParameter;

      }
      else
      {
        current_main_state = startWithGivenParameters;
      }
      break;
    case getParameter:
      setStartVoltage();
      calculateFinalValue();
      setVoltage(getStartVoltage());
      secCounter();
      writeTemppToArray();
      antiDeadLock();
      break;
    case gotParameter:
      secCounter();
      printParameter();
      setCurrentState(start_PI);
      setStartVoltageIPart(calculateStartVoltageForIpart());
      //printPIParams();
      current_main_state = PI_on_Main;
      //Serial.println("PI Regler ist jetzt aktiv");
      break;
    case startWithGivenParameters:
      secCounter();
      setStartVoltage();
      setParameterProgrammatically(2.15 , 1660.25, 1071.13, 214.23, 2);
      setStartVoltageIPart(calculateStartVoltageForIpart());
      setCurrentState(running_PI);
      imediateCalcVoltage();
      current_main_state = PI_on_Main;
      //Serial.println("PI Regler ist jetzt aktiv");
      break;
    case PI_on_Main:
      secCounter();
      PIisOn = true;
      controlVoltage();
      break;
    case globalShutDown:
      turnOffHeating();
      break;
  }




}
