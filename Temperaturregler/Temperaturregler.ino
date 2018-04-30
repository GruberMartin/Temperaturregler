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
#define schalter 8
#define myPin 7

unsigned long previousTime = 0;
unsigned long actualTime = 0;
unsigned long seconds ;
int tenSecCounter = 0;
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
boolean temperatureIsStable = false;
int stabCounter = 0;
boolean rechedFinalState = false;
boolean fastTempControll = false;
boolean globalStart = false;
boolean isStillPressing = false;
String timeString = "";
int hours = 0;
int minutes = 0;
int tempUser = 0;
int tempUserDot = 0;
boolean setTempUser = true;
boolean setHours = true;
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

  lcd.begin(16, 2);

  //lcd.print("Drücken Sie RESET um zum vorigen Menu zurückzukehren.");
  lcd.setBacklight(WHITE);
  pinMode(schalter, INPUT);
  pinMode(myPin, OUTPUT);
  digitalWrite(myPin, HIGH);
  initParameterDetermination();
  initPID();
  initTemperature();
  initVoltageControll();
  requestTemp();
  tempUser = ((int)getValSens1());
  tempUserDot = ((getValSens1() * 100) - (tempUser * 100));
}

void setMaxTemp()
{
  maxTemp = 0.8 * getFinalValue();

}

boolean getButtonRight()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_RIGHT);
}
boolean getButtonLeft()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_LEFT);
}
boolean getButtonUp()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_UP);
}
boolean getButtonDown()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_DOWN);
}
boolean getButtonSelect()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_SELECT);
}
boolean getButtonNoone()
{
  buttons = lcd.readButtons();
  return (buttons == 0);
}
void disPrint(String firstLine, String secondLine)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  lcd.setCursor(0, 0);
}

boolean requestRegulatorChange()
{
  return changeRegulator;
}

void disPrintTime()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time to hold: ");
  lcd.setCursor(0, 1);
  timeString = ((String)hours) + " h : " + ((String)minutes) + " min";
  lcd.print(timeString);
  lcd.setCursor(0, 0);
}
void disPrintTemp()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. to hold: ");
  lcd.setCursor(0, 1);
  timeString = ((String)tempUser) + "." + ((String)tempUserDot) + " deg.";
  lcd.print(timeString);
  lcd.setCursor(0, 0);
}



void disPrintActualTemp(float actualTemp)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. now: ");
  lcd.setCursor(0, 1);
  timeString = ((String)actualTemp) + " deg.";
  lcd.print(timeString);
  lcd.setCursor(0, 0);
}

void disPrintRegualtorActivated(float actualTemp)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reg. activated: ");
  lcd.setCursor(0, 1);
  timeString = ((String)actualTemp) + " deg.";
  lcd.print(timeString);
  lcd.setCursor(0, 0);
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
      if(endtimeHasBeenSet == false)
      {
      setEndTime(endTime + getSeconds());
      endtimeHasBeenSet = true;
      }
      rechedFinalState = false;
      fastTempControll = true;
      stabCounter = 0;
      changeRegulator = true;
      //Serial.println("Error");
      /*if (getError() > 0.5)
      {
        doNewCalc = true;
      }*/
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



      /*if (getError() <= 0.0)
      {
        doNewCalc = true;
      }*/ 
    }

   if((tempTemp != getValSens2()) && startLcdTempPrinting == true)
   {
    tempTemp = getValSens2();
    if(PIisOn == false)
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
    fiveSecCounter = fiveSecCounter + 1;
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
      buttons = lcd.readButtons();
      if (!(buttons & BUTTON_SELECT))
      {
        if (displayedStartMessgae == false)
        {
          disPrint("Press Select", "");
          displayedStartMessgae = true;
        }
      }
      else {
        current_main_state = getCookingMode;

        displayedStartMessgae = false;
      }
      break;

    case getCookingMode:
      buttons = lcd.readButtons();
      if (displayedStartMessgae == false)
      {
        disPrint("<- Only 1 Temp.", "-> Spec. Config.");
        displayedStartMessgae = true;
      }
      else
      {
        if (getButtonLeft() && isStillPressing == false)
        {
          //Serial.println("Button Left pressed");
          current_main_state = fastCookingModeTime;
          isStillPressing = true;
          displayedStartMessgae = false;
        }
        else if (getButtonRight() && isStillPressing == false)
        {
          Serial.println("Button Right pressed");
          isStillPressing = true;
          displayedStartMessgae = false;
        }
        else if (buttons == 0)
        {

          isStillPressing = false;
        }

      }
      break;
    case fastCookingModeTime:
      if (displayedStartMessgae == false)
      {
        displayedStartMessgae = true;
        disPrintTime();
      }
      if (getButtonLeft() && isStillPressing == false)
      {
        setHours = true;
        isStillPressing = true;
      }
      else if (getButtonRight() && isStillPressing == false)
      {
        setHours = false;
        isStillPressing = true;
      }
      else if (getButtonNoone())
      {
        isStillPressing = false;
      }

      if (setHours == true && getButtonUp() && isStillPressing == false)
      {
        hours = hours + 1;
        disPrintTime();
        //isStillPressing = true;

      }
      else if (setHours == false && getButtonUp() && isStillPressing == false && minutes < 60)
      {
        minutes = minutes + 1;
        disPrintTime();
        //isStillPressing = true;
      }
      else if (setHours == true && getButtonDown() && isStillPressing == false && hours > 0)
      {
        hours = hours - 1;
        disPrintTime();
        //isStillPressing = true;
      }
      else if (setHours == false && getButtonDown() && isStillPressing == false && minutes > 0)
      {
        minutes = minutes - 1;
        disPrintTime();
        //isStillPressing = true;
      }
      else if (getButtonSelect() && isStillPressing == false)
      {

        current_main_state = fastCookingModeTemp;
        isStillPressing = true;
        displayedStartMessgae = false;
      }
      break;
    case fastCookingModeTemp :
      // ----------------------------------------------------------------------------------------------------------------
      if (displayedStartMessgae == false)
      {
        displayedStartMessgae = true;
        disPrintTemp();
      }
      if (getButtonLeft() && isStillPressing == false)
      {
        setTempUser = true;
        isStillPressing = true;
      }
      else if (getButtonRight() && isStillPressing == false)
      {
        setTempUser = false;
        isStillPressing = true;
      }
      else if (getButtonNoone())
      {
        isStillPressing = false;
      }

      if (setTempUser == true && getButtonUp() && isStillPressing == false)
      {
        tempUser = tempUser + 1;
        disPrintTemp();
        //isStillPressing = true;
      }
      else if (setTempUser == false && getButtonUp() && isStillPressing == false && tempUserDot < 75)
      {
        tempUserDot = tempUserDot + 25;
        disPrintTemp();
        isStillPressing = true;
      }
      else if (setTempUser == true && getButtonDown() && isStillPressing == false)
      {
        tempUser = tempUser - 1;
        disPrintTemp();
        //isStillPressing = true;
      }
      else if (setTempUser == false && getButtonDown() && isStillPressing == false && tempUserDot >= 25)
      {
        tempUserDot = tempUserDot - 25;
        disPrintTemp();
        isStillPressing = true;
      }
      else if (getButtonSelect() && isStillPressing == false)
      {
        //requestGlobalStart();
        current_main_state = notStarted_Main;
        isStillPressing = true;

        requestGlobalStart();
        startLcdTempPrinting = true;

      }

      break;

    case notStarted_Main:
      setSetPoint(((float)tempUserDot / 100) + tempUser);
      endTime = hours * 60 * 60 + minutes * 60;
      secCounter();
      if(startWithGivenParametersRequest == false)
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
      setStartVoltageIPart(getStartVoltage());
      current_main_state = PI_on_Main;
      //Serial.println("PI Regler ist jetzt aktiv");
      break;
    case startWithGivenParameters:
    secCounter();
    setStartVoltage();
    setParameterProgrammatically(2.3*1.2 , 1594.45, 1028.68,60, 2);    
    setStartVoltageIPart(getStartVoltage());
    //if(true == checkParameters(2.3, 1594.45, 1028.68, 205.74, 2))
    //{
    setCurrentState(running_PI);
    imediateCalcVoltage();
    current_main_state = PI_on_Main;
    /*}
    else
    {
            printPIParams();
            while(1){}
    }*/
    //Serial.println("PI Regler ist jetzt aktiv");
    break;
    case PI_on_Main:
      secCounter();
      PIisOn = true;
      controlVoltage();
      break;
     case globalShutDown:
     turnOffHeating();
     //Serial.println("error!!!!!");
      break;
  }




}
