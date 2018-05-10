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
int currentSequence = 0;
float currentSetPoint = 0.0;
unsigned long changeTime = 0;
boolean requestSequenceChange = true;
boolean calculateRealchangeTime = false;
boolean nextSequenceHasBenSet = false;
boolean sequencesStarted = false;
long timeForNextStep = 0;


unsigned long endTime = 0;
float tempTemp = 0.0;
boolean changeTimeHasBeenSet = false;
boolean startLcdTempPrinting = false;
boolean startWithGivenParametersRequest = false;
boolean changeRegulator = false;
boolean startWithGivenSeqRequest = false;



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
  selectParamFilesForPI,
  notStarted_Main,
  PI_on_Main,
  getParameter,
  gotParameter,
  startWithGivenParameters,
  selectParamFilesForSeq,
  globalShutDown
} main_states;

main_states current_main_state = getUserInput;



void setup()
{
  Serial.begin(250000);
  initDisplay();
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

void handleNextSequnece()
{
  if (sequencesStarted == false)
  {
    Serial.println("Schritt 0 wurde gestartet");
    handleSequences();
    sequencesStarted = true;
  }

  if (getError() == 0.0 && nextSequenceHasBenSet == false)
  {
    handleSequences();
    nextSequenceHasBenSet = true;
    timeForNextStep = changeTime + getSeconds();
    Serial.println("Schritt " + (String)currentSequence + " bis: " + (String)timeForNextStep + " mit Temp: " + (String)currentSetPoint);
    currentSequence += 1;
  }
  if (nextSequenceHasBenSet == true && getSeconds() >= timeForNextStep)
  {
    Serial.println("Start with step nr. " + (String)currentSequence);
    handleSequences();
    //Serial.println("Schritt " + (String)currentSequence + " mit Dauer: " + (String)changeTime + " und Temp: " + (String)currentSetPoint);
    nextSequenceHasBenSet = false;
    imediateCalcVoltage();
  }

}

void handleSequences()
{

  switch (currentSequence)
  {
    case 0:
      changeTime = getStepTime(currentSequence);
      currentSetPoint = getStepTemp(currentSequence);
      setSetPoint(currentSetPoint);


      break;
    case 1:

      changeTime = getStepTime(currentSequence);
      currentSetPoint = getStepTemp(currentSequence);
      setSetPoint(currentSetPoint);
      break;
    case 2:

      changeTime = getStepTime(currentSequence);
      currentSetPoint = getStepTemp(currentSequence);
      setSetPoint(currentSetPoint);
      break;
    case 3:

      changeTime = getStepTime(currentSequence);
      currentSetPoint = getStepTemp(currentSequence);
      setSetPoint(currentSetPoint);
      break;
    case 4:

      changeTime = getStepTime(currentSequence);
      currentSetPoint = getStepTemp(currentSequence);
      setSetPoint(currentSetPoint);
      break;
    case 5:

      changeTime = getStepTime(currentSequence);
      currentSetPoint = getStepTemp(currentSequence);
      setSetPoint(currentSetPoint);
      break;
  }
  if (changeTime == 0)
  {
    current_main_state = globalShutDown;
  }


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
  startLcdTempPrinting = true;
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

    if (PIisOn == true && startWithGivenParametersRequest == true)
    {

      handleNextSequnece();

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
    Serial.print(seconds, DEC);
    Serial.print(" ");
    Serial.print(getPpart());
    Serial.print(" ");
    Serial.print(getVoltageIpart());
     Serial.print(" ");
    Serial.println(getSetPoint());
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
    case selectParamFilesForPI:
      chooseParameters(0);
      break;
    case fastCookingModeTime:
      getCookingTime();
      break;
    case fastCookingModeTemp :
      getCookingTemp();
      break;
    case notStarted_Main:
      secCounter();
      if (startWithGivenParametersRequest == false)
      {
        current_main_state = getParameter;

      }
      else
      {
        if (startWithGivenSeqRequest == true)
        {
          current_main_state = startWithGivenParameters;
          //        disPrintSeqFile();
          //        printPIParams();
          //        while(1)
          //        {
          //
          //        }


        }
        else
        {
          current_main_state = fastCookingModeTime;
        }
      }
      break;
    case getParameter:
      setSetPoint(70.0);
      setStartVoltage();
      calculateFinalValue();
      secCounter();
      writeTemppToArray();
      antiDeadLock();
      break;
    case gotParameter:
      secCounter();
      printParameter();
      setCurrentState(start_PI);
      setStartVoltageIPart(calculateStartVoltageForIpart());
      current_main_state = globalShutDown;
      break;
    case startWithGivenParameters:
      secCounter();
      setSetPoint(getStepTemp(currentSequence));
      //setStartVoltageIPart(calculateStartVoltageForIpart());
      //      }
      setCurrentState(running_PI);
      imediateCalcVoltage();
      current_main_state = PI_on_Main;
      break;
    case selectParamFilesForSeq:
      chooseParameters(1);
      break;
    case PI_on_Main:
      secCounter();
      PIisOn = true;
      controlVoltage();
      break;
    case globalShutDown:
      turnOffHeating();
      disPrintFinishMessgae();
      break;
  }




}
