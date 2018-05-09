#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "SD.h"
boolean isStillPressing = false;
boolean setTempUser = true;
boolean setHours = true;
String paramFileNames[6] = {"pan 1", "pan 2", "pan 3", "pan 4", "pan 5", "pan 6"};
String seqFileNames[6] = {"seq 1", "seq 2", "seq 3", "seq 4", "seq 5", "seq 6"};
int noOfFiles = 0;
boolean filesAvailableForSelct = false;
boolean disableSelect = true;
boolean firstPress = true;
boolean numberOfFilesCounted = false;
boolean printErrorMsg = false;
int numberOfSteps = 0;
long stepTime [6] =  {0, 0, 0, 0, 0, 0};
float stepTemp [6] =  {40.0, 40.0, 40.0, 40.0, 40.0, 40.0};
String timeString = "";
int hours = 0;
int minutes = 0;
int tempUser = 0;
int presscounter = 0;
int tempUserDot = 0;
boolean disableUp = false;
boolean disableDown = false;
void initDisplay()
{
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);
  tempUser = 40;//((int)getValSens1());
  tempUserDot = 0;//((getValSens1() * 100) - (tempUser * 100));



}


//###################################################################################################################################################################################################################################
//###############################################################################################   Getter und Setter Methoden   ####################################################################################################
//###################################################################################################################################################################################################################################

long getStepTime(int index)
{
  return stepTime[index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

float getStepTemp(int index)
{
  return stepTemp[index];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

boolean getButtonRight()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_RIGHT);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

boolean getButtonLeft()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_LEFT);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

boolean getButtonUp()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_UP);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

boolean getButtonDown()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_DOWN);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

boolean getButtonSelect()
{
  buttons = lcd.readButtons();
  return (buttons & BUTTON_SELECT);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

boolean getButtonNoone()
{
  buttons = lcd.readButtons();
  return (buttons == 0);
}

//###################################################################################################################################################################################################################################
//###############################################################################################  Methoden um nächste Schritte zu bestimmen  #######################################################################################
//###################################################################################################################################################################################################################################


void requestFurtherStepsTime(long timeCurrentStep)
{
  if (numberOfSteps < 6)
  {

    stepTime[numberOfSteps] = timeCurrentStep;



  }

}

void requestFurtherStepsTemp(float tempCurrentStep)
{
  if (numberOfSteps < 6)
  {
    stepTemp[numberOfSteps] = tempCurrentStep;
    current_main_state = fastCookingModeTime;
    numberOfSteps = numberOfSteps + 1;
  }

  if (numberOfSteps == 6)
  {

    current_main_state = notStarted_Main;
    requestGlobalStart();
    startLcdTempPrinting = true;
  }

}

//###################################################################################################################################################################################################################################
//###############################################################################################   Methoden Display Print   ########################################################################################################
//###################################################################################################################################################################################################################################

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
  lcd.print("Time" + (String)(numberOfSteps + 1) + ". to hold: ");
  lcd.setCursor(0, 1);
  timeString = ((String)hours) + " h : " + ((String)minutes) + " min";
  lcd.print(timeString);
  lcd.setCursor(0, 0);
}
void disPrintTemp()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp" + (String)(numberOfSteps + 1) + ". to hold: ");
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

//###################################################################################################################################################################################################################################
//###############################################################################################   Methoden Menu   #################################################################################################################
//###################################################################################################################################################################################################################################


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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void waitForCookingMode()
{
  buttons = lcd.readButtons();
  if (displayedStartMessgae == false)
  {
    disPrint("<- New pan", "-> Known pan");
    displayedStartMessgae = true;
  }
  else
  {
    if (getButtonLeft() && isStillPressing == false)
    {
      //Serial.println("Button Left pressed");
      current_main_state = getParameter;
      isStillPressing = true;
      displayedStartMessgae = false;
    }
    else if (getButtonRight() && isStillPressing == false)
    {

      current_main_state = selectParamFiles;
      isStillPressing = true;
      displayedStartMessgae = false;
    }
    else if (buttons == 0)
    {

      isStillPressing = false;
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void chooseParameters()
{
  // Bestimmen wie viele files vorhanden sind
  if (numberOfFilesCounted == false)
  {
    countNumberOfFiles();
    numberOfFilesCounted = true;
  }
  buttons = lcd.readButtons();
  if (displayedStartMessgae == false)
  {
    if (getNumberOfFiles() > 0)
    {
      disPrint("Choose a pan:", "Press down");
      filesAvailableForSelct = true;
      displayedStartMessgae = true;
    }
    else
    {

      disPrint("No files", "Press Select");
      filesAvailableForSelct = false;
      displayedStartMessgae = true;
      disableSelect = false;
      disableUp = true;
      disableDown = true;

    }
  }
  else
  {
    // Aktionen für taste nach unten
    // Falls nur 1 File vorhanden ist
    if (getButtonDown() && isStillPressing == false && disableDown == false)
    {
      if (getNumberOfFiles() == 1)
      {
        if (presscounter < 1)
        {
          disableUp = false;
          printErrorMsg = false;
          presscounter += 1;
        }
        else
        {
          printErrorMsg = true;
          disableDown = true;
          disableUp = false;
        }

      }
      // Falls mehrere Files vorhanden sind
      else
      {
        if (noOfFiles < getNumberOfFiles() - 1)
        {

          if (firstPress == false && printErrorMsg == false)
          {
            noOfFiles = noOfFiles + 1;
          }

          firstPress = false;
          printErrorMsg = false;
        }
        else
        {
          printErrorMsg = true;
        }
      }
      // Entscheiden was angezeigt werden soll
      if (printErrorMsg == false)
      {

        disPrint("Choose a pan:", paramFileNames[noOfFiles]);
        disableSelect = false;

      }
      else if (printErrorMsg == true)
      {

        disPrint("No more files", "Press up");
        disableSelect = true;
      }
      isStillPressing = true;
    }
    // Aktionene für Button nach unten
    else if (getButtonUp() && isStillPressing == false && disableUp == false)
    {
      // Wenn nur 1 file vorhanden ist
      if (getNumberOfFiles() == 1)
      {
        if (presscounter > 0)
        {
          printErrorMsg = false;
          presscounter -= 1;
          disableDown = false;
        }
        else
        {
          disableDown = false;
          disableUp = true;
          printErrorMsg = true;
        }

      }
      // Wenn es mehr als 1 file gibt
      else
      {

        if (noOfFiles > 0)
        {
          if (printErrorMsg == false)
          {
            noOfFiles = noOfFiles - 1;
          }

          printErrorMsg = false;
        }
        else
        {
          printErrorMsg = true;
        }
      }
      // Entscheiden was angezeigt werden soll
      if (printErrorMsg == false)
      {
        disPrint("Choose a pan:", paramFileNames[noOfFiles]);
        disableSelect = false;
      }
      else if (printErrorMsg == true)
      {
        disPrint("No more files", "Press down");
        disableSelect = true;
      }
      isStillPressing = true;
    }
    // Aktionen für Button select
    else if (getButtonSelect() && isStillPressing == false && disableSelect == false)
    {
      // Wenn file vorhanden sind, mit ausgewählten Parametern starten => Abfolgen erfassen oder auswählen
      if (filesAvailableForSelct)
      {
        displayedStartMessgae = false;
        readFile(noOfFiles);
        startWithGivenParametersRequest = true;
        setMainState(fastCookingModeTime);

      }
      // Falls keine Files vorhanden waren => Modus getParameter
      else
      {
        displayedStartMessgae = false;
        setMainState(getParameter);
      }
      
      isStillPressing = true;

    }
    else if (buttons == 0)
    {

      isStillPressing = false;
    }
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




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
    requestFurtherStepsTime(hours * 60.0 * 60.0 + minutes * 60.0);
    hours = 0;
    minutes = 0;
    current_main_state = fastCookingModeTemp;
    isStillPressing = true;
    displayedStartMessgae = false;
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


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


    isStillPressing = true;
    displayedStartMessgae = false;
    requestFurtherStepsTemp(((float)tempUserDot / 100.0) + tempUser);
    tempUserDot = 0;
    tempUser = 40;


  }
}

