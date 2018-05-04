#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

boolean isStillPressing = false;
boolean setTempUser = true;
boolean setHours = true;

void initDisplay()
{
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);
  tempUser = ((int)getValSens1());
  tempUserDot = ((getValSens1() * 100) - (tempUser * 100));
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



void waitForStartSignal()
{
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
}

void waitForCookingMode()
{
   buttons = lcd.readButtons();
      if (displayedStartMessgae == false)
      {
        disPrint("<- No Param.", "-> Given Param.");
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
          //Serial.println("Button Right pressed");
          current_main_state = fastCookingModeTime;
          startWithGivenParametersRequest = true;
          isStillPressing = true;
          displayedStartMessgae = false;
        }
        else if (buttons == 0)
        {

          isStillPressing = false;
        }
}
}

void getCookingTime()
{
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
}

void getCookingTemp()
{
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
}

