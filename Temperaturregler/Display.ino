#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "SD.h"
boolean isStillPressing = false;
boolean setTempUser = true;
boolean setHours = true;
String paramFileNames[6];
String seqFileNames[6];
int noOfFiles = 0;
boolean filesAvailableForSelct = false;
boolean disableSelect = true;
boolean firstPress = true;
boolean numberOfFilesCounted = false;
boolean finsihMessageHasBeenDisplayed = false;
boolean printErrorMsg = false;
int numberOfSteps = 0;
long stepTime [6] =  {0, 0, 0, 0, 0, 0};
float stepTemp [6] =  {40.0, 40.0, 40.0, 40.0, 40.0, 40.0};
boolean agitator[6];
String timeString = "";
int hours = 0;
int minutes = 0;
int tempUser = 0;
int presscounter = 0;
int tempUserDot = 0;
boolean disableUp = false;
boolean disableDown = false;
boolean gotOrderPIparams = false;
boolean blinky = false;
int testNumber = 0;
int blinkCounter =  0;
String hourString;
String minutesString;
boolean updateDisplay = false;
int newLog10h = 0; // To know when to update display Text Time
int oldLog10h = 0;
int newLog10m = 0;
int oldLog10m = 0;
String extraSting = "";
int minPos = 6;
boolean hoursInvisible = false;
boolean minutesInvisible = false;
boolean longPress = false;
boolean tempInvisible = false;
boolean dotTempInvisible = false;
String dotTempString;
String tempString;
int newLog10dt = 0;
int oldLog10dt = 0;
boolean changeLanguage = false;
int langChangeCounter = 0;
int agitatorCounter = 0;



//###################################################################################################################################################################################################################################
//###############################################################################################   Eigene Sonderzeichen   ####################################################################################################
//###################################################################################################################################################################################################################################

byte arrowDown[8] = {
  4, 4, 4, 4, 21, 14, 4
};

byte arrowUp[8] = {
  4, 14, 21, 4, 4, 4, 4
};

byte arrowRight[8] = {
  0, 4, 2, 31, 2, 4, 0
};

byte arrowLeft[8] = {
  0, 4, 8, 31, 8, 4, 0
};

byte degree[8] =
{
  7, 5, 7, 0, 0, 0, 0
};

byte clearOneSegment[8] = {
  0, 0, 0, 0, 0, 0, 0
};


// String set
String pressSelect;
String changeLangHint;
String newPanStr;
String knownPanStr;
String newSeqStr;
String knownSeqStr;
String tempNowStr;
String timeWord;
String tempWord;
String chooseAPanStr;
String chooseAseqStr;
String regActivatedStr;
String noMoreFilesStr;
String finishStr;
String pressStr;
String actTempStr;
String noFilesStr;
String tooManyFilesStr;
String removeFilesStr;
String toHoldStr;
String agitatorStr;
String agitatorAnsStr;
//Change Strings


void changeLanguageRequest()
{


  if (changeLanguage == false)
  {

    for (int panCnt = 0; panCnt < 6; panCnt++)
    {
      paramFileNames[panCnt] = "Pfanne " + (String)(panCnt + 1);
    }
    for (int panCnt = 0; panCnt < 6; panCnt++)
    {
      seqFileNames[panCnt] = "Abfolge " + (String)(panCnt + 1);
    }
    pressSelect = "Druecke Select";
    changeLangHint = "";//"Lange fuer EN";
    newPanStr = "<- Neue Pfanne" ;
    knownPanStr = "-> Bekannte Pfa.";
    newSeqStr = "<- Neu Abfolge";
    knownSeqStr = "-> Bekannte Abf.";
    timeWord = "Zeit";
    tempWord = "Temperatur";
    chooseAPanStr = "Waehle eine Pfa.";
    chooseAseqStr = "Waehle eine Abf.";
    regActivatedStr = "Regler aktiviert";
    noMoreFilesStr = "Keine weiteren";
    finishStr = "Fertig";
    pressStr = "Druecke";
    actTempStr = "Aktuelle Temper.";
    noFilesStr = "Keine Files";
    tooManyFilesStr = "Zu viele Files";
    removeFilesStr = "Entferne einige";
    toHoldStr = "";
    agitatorStr = "Ruehrwerk an ?";
    agitatorAnsStr = "<- JA,  -> NEIN";
  }
  else
  {
    for (int panCnt = 0; panCnt < 6; panCnt++)
    {
      paramFileNames[panCnt] = "pan " + (String)(panCnt + 1);
    }
    for (int panCnt = 0; panCnt < 6; panCnt++)
    {
      seqFileNames[panCnt] = "sequence " + (String)(panCnt + 1);
    }
    pressSelect = "Press Select";
    changeLangHint = "Long for DE";
    newPanStr = "<- New pan";
    knownPanStr = "-> Konwn pan";
    newSeqStr = "<- New sequence";
    knownSeqStr = "-> Known sequen.";
    timeWord = "Time";
    tempWord = "Temp";
    chooseAPanStr = "Choose a pan";
    chooseAseqStr = "Choose a seque.";
    regActivatedStr = "Regulator activ.";
    noMoreFilesStr = "No more files";
    finishStr = "Finish";
    pressStr = "Press";
    actTempStr = "Temp. now";
    noFilesStr = "No Files";
    tooManyFilesStr = "To many Files";
    removeFilesStr = "Remove some";
    toHoldStr = "";
    agitatorStr = "Agitator on ?";
    agitatorAnsStr = "<- YES,  -> NO";
  }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void initDisplay()
{
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);
  tempUser = 40;//((int)getValSens1());
  tempUserDot = 0;//((getValSens1() * 100) - (tempUser * 100));
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);
  lcd.createChar(2, arrowLeft);
  lcd.createChar(3, arrowRight);
  lcd.createChar(4, degree);
  lcd.createChar(5, clearOneSegment);


}


//###################################################################################################################################################################################################################################
//###############################################################################################   Getter und Setter Methoden   ####################################################################################################
//###################################################################################################################################################################################################################################


boolean getAgitatorAns(int index)
{
  return agitator[index];
}

void setAgitatorAns(int index, boolean value)
{
  agitator[index] = value;
}

long getStepTime(int index)
{
  return stepTime[index];
}

void setStempTime(int index, long value)
{
  stepTime[index] = value;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

float getStepTemp(int index)
{
  return stepTemp[index];
}

void setStepTemp(int index, float temp)
{
  stepTemp[index] = temp;
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

    saveSeqParameters();
    saveAgitatorParameters();
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

void disPrintWithSpecialChar(String firstLine, String secondLine , int posSpecialChar , int specialChar)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  if (posSpecialChar == 1)
  {
    lcd.print(" ");
    lcd.write(specialChar);
  }
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  if (posSpecialChar == 2)
  {
    lcd.print(" ");
    lcd.write(specialChar);
  }
  lcd.setCursor(0, 0);
}

void disBlinkHours()
{
  hourString = (String) hours;
  lcd.setCursor(0, 1);


  if (blinkCounter < 20)
  {
    hourString = (String) hours;
    blinkCounter += 1;
    hoursInvisible = false;

  }
  else if (blinkCounter < 40)
  {
    if (longPress == false)
    {
      hourString = "  ";
    }
    hoursInvisible = true;
    blinkCounter += 1;

  }
  else
  {
    blinkCounter = 0;
  }
  lcd.print(hourString);

}

void disBlinkMinutes()
{

  minutesString = (String) minutes;
  lcd.setCursor(minPos, 1);
  if (blinkCounter < 20)
  {
    minutesString = (String) minutes ;
    blinkCounter += 1;
    minutesInvisible = false;

  }
  else if (blinkCounter < 40)
  {
    if (longPress == false)
    {
      minutesString = "  ";
    }
    minutesInvisible = true;
    blinkCounter += 1;

  }
  else
  {
    blinkCounter = 0;
  }
  lcd.print(minutesString);

}

void disBlinkTemp()
{
  tempString = (String) tempUser;
  lcd.setCursor(0, 1);


  if (blinkCounter < 20)
  {
    tempString = (String) tempUser;
    blinkCounter += 1;
    tempInvisible = false;

  }
  else if (blinkCounter < 40)
  {
    if (longPress == false)
    {
      tempString = "  ";
    }
    tempInvisible = true;
    blinkCounter += 1;

  }
  else
  {
    blinkCounter = 0;
  }
  lcd.print(tempString);
}


void disBlinkDotTemp()
{
  dotTempString = (String) tempUserDot;
  lcd.setCursor(3, 1);


  if (blinkCounter < 20)
  {
    dotTempString = (String) tempUserDot;
    blinkCounter += 1;
    dotTempInvisible = false;

  }
  else if (blinkCounter < 40)
  {
    if (longPress == false)
    {
      dotTempString = "  ";
    }
    dotTempInvisible = true;
    blinkCounter += 1;

  }
  else
  {
    blinkCounter = 0;
  }
  lcd.print(dotTempString);

}



void disPrintTime()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(timeWord + " " + (String)(numberOfSteps + 1) + toHoldStr);
  lcd.setCursor(0, 1);
  timeString = ((String)hours) + " h : "  + ((String)minutes) + " min";
  lcd.print(timeString);
  lcd.setCursor(0, 0);
}
void disPrintTemp()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(tempWord + " " + (String)(numberOfSteps + 1) + toHoldStr);
  lcd.setCursor(0, 1);
  timeString = ((String)tempUser) + "." + ((String)tempUserDot + " ");
  lcd.print(timeString);
  lcd.write(4);
  lcd.print("C");
  lcd.setCursor(0, 0);
}



void disPrintActualTemp(float actualTemp)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(actTempStr);
  lcd.setCursor(0, 1);
  timeString = ((String)actualTemp) + " -> " + (String)getSetPoint();
  lcd.print(timeString);
  lcd.write(4);
  lcd.print("C");
  lcd.setCursor(0, 0);
}



void disPrintRegualtorActivated(float actualTemp)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(regActivatedStr);
  lcd.setCursor(0, 1);
  timeString = ((String)actualTemp) + " -> " + (String)getSetPoint();
  lcd.print(timeString);
  lcd.write(4);
  lcd.print("C");

  lcd.setCursor(0, 0);
}

void disPrintFinishMessgae()
{
  if (finsihMessageHasBeenDisplayed == false)
  {
    disPrint(finishStr, "");
    finsihMessageHasBeenDisplayed = true;
  }
}

// mus geändert werden, ist keine Methode die auf das Display schreibt
void disPrintSeqFile()
{
  for (int y = 0; y < 6; y++)
  {
    Serial.println(timeWord + (String)(y) + ": " + (String)stepTime[y] + ", " + tempWord + (String)(y) + ": " + (String)stepTemp[y]);
  }
}

//###################################################################################################################################################################################################################################
//###############################################################################################   Methoden Menu   #################################################################################################################
//###################################################################################################################################################################################################################################


void waitForStartSignal()
{
  changeLanguageRequest();
  buttons = lcd.readButtons();
  if (!(buttons & BUTTON_SELECT))
  {
    if (displayedStartMessgae == false)
    {
      disPrint(pressSelect, changeLangHint);
      displayedStartMessgae = true;
    }
  }
  else {

    while (getButtonSelect() == true)
    {

      langChangeCounter += 1;

      delay(50);
      if (langChangeCounter >= 10 && changeLanguage == false)
      {
        changeLanguage = true;
        changeLanguageRequest();
        break;
      }
    }


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
    if (gotOrderPIparams == false)
    {
      disPrint(newPanStr, knownPanStr);
      displayedStartMessgae = true;
    }
    else
    {
      disPrint(newSeqStr, knownSeqStr);
      displayedStartMessgae = true;
      numberOfFilesCounted = false;
    }
  }
  else
  {
    if (gotOrderPIparams == false)
    {
      if (getButtonLeft() && isStillPressing == false)
      {

        if (countNumberOfPiFiles() >= 6)
        {
          disPrint(tooManyFilesStr, removeFilesStr);
          while (1)
          {

          }
        }
        //Serial.println("Button Left pressed");
        requestGlobalStart();
        current_main_state = getParameter;
        isStillPressing = true;
        displayedStartMessgae = false;
      }
      else if (getButtonRight() && isStillPressing == false)
      {

        current_main_state = selectParamFilesForPI;
        isStillPressing = true;
        displayedStartMessgae = false;
      }
      else if (buttons == 0)
      {

        isStillPressing = false;
      }
    }
    else
    {
      if (getButtonLeft() && isStillPressing == false)
      {

        if (countNumberOfSeqFiles() >= 6)
        {
          disPrint(tooManyFilesStr, removeFilesStr);
          while (1)
          {

          }
        }

        current_main_state = fastCookingModeTime;
        isStillPressing = true;
        displayedStartMessgae = false;
      }
      else if (getButtonRight() && isStillPressing == false)
      {
        requestGlobalStart();
        current_main_state = selectParamFilesForSeq;
        isStillPressing = true;
        displayedStartMessgae = false;
      }
      else if (buttons == 0)
      {

        isStillPressing = false;
      }
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void chooseParameters(int whichFile)
{
  // Bestimmen wie viele files vorhanden sind
  if (numberOfFilesCounted == false)
  {
    if (whichFile == 0)
    {
      noOfFiles = 0;
      countNumberOfPiFiles();
    }
    else
    {
      noOfFiles = 0;
      countNumberOfSeqFiles();
    }
    numberOfFilesCounted = true;
  }
  buttons = lcd.readButtons();
  if (displayedStartMessgae == false)
  {
    if (getNumberOfFiles() > 0)
    {
      if (whichFile == 0)
      {
        disPrintWithSpecialChar(chooseAPanStr, pressStr, 2, 1);
      }
      else
      {
        disPrintWithSpecialChar(chooseAseqStr, pressStr, 2, 1);
      }
      filesAvailableForSelct = true;
      displayedStartMessgae = true;
    }
    else
    {

      disPrint(noFilesStr, pressSelect);
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
        if (whichFile == 0)
        {
          disPrint(chooseAPanStr, paramFileNames[noOfFiles]);
        }
        else
        {
          disPrint(chooseAseqStr, seqFileNames[noOfFiles]);
        }
        disableSelect = false;

      }
      else if (printErrorMsg == true)
      {

        disPrintWithSpecialChar(noMoreFilesStr, pressStr, 2, 0);
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
        if (whichFile == 0)
        {
          disPrint(chooseAPanStr, paramFileNames[noOfFiles]);
        }
        else
        {
          disPrint(chooseAseqStr, seqFileNames[noOfFiles]);
        }
        disableSelect = false;
      }
      else if (printErrorMsg == true)
      {
        disPrintWithSpecialChar(noMoreFilesStr, pressStr, 2, 1);
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
        if (whichFile == 0)
        {
          displayedStartMessgae = false;
          readFile(noOfFiles);
          startWithGivenParametersRequest = true;
          gotOrderPIparams = true;
          setMainState(getCookingMode);
          presscounter = 0;
          filesAvailableForSelct = false;
          numberOfFilesCounted = false;
          noOfFiles = 0;
          disableSelect = true;
          firstPress = true;
          printErrorMsg = false;
          disableUp = false;
          disableDown = false;
        }
        else
        {
          displayedStartMessgae = false;
          readSeqFile(noOfFiles);
          readAgitatorFile(noOfFiles);
          startWithGivenSeqRequest = true;
          setMainState(notStarted_Main);
        }

      }
      // Falls keine Files vorhanden waren => Modus getParameter
      else
      {
        if (whichFile == 0)
        {
          displayedStartMessgae = false;
          setMainState(getParameter);
          presscounter = 0;
          filesAvailableForSelct = false;
          numberOfFilesCounted = false;
          noOfFiles = 0;
          disableSelect = true;
          firstPress = true;
          printErrorMsg = false;
          disableUp = false;
          disableDown = false;
          requestGlobalStart();
        }
        else
        {
          displayedStartMessgae = false;
          setMainState(fastCookingModeTime);
        }
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

void getInputForAgitator()
{
  if (displayedStartMessgae == false)
  {
    disPrint(agitatorStr, agitatorAnsStr);
    displayedStartMessgae = true;
  }
  if (getButtonRight() && isStillPressing == false) {
    agitator[agitatorCounter] = false;
    agitatorCounter += 1;

    current_main_state = fastCookingModeTemp;
    isStillPressing = true;
    displayedStartMessgae = false;
  }
  else if (getButtonLeft() && isStillPressing == false) {
    agitator[agitatorCounter] = true;
    agitatorCounter += 1;
    current_main_state = fastCookingModeTemp;
    isStillPressing = true;
    displayedStartMessgae = false;
  }
  else if (getButtonNoone())
  {
    isStillPressing = false;
  }

}

void getCookingTime()
{
  newLog10h = (int) log10(hours);
  newLog10m = (int) log10(minutes);


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
    longPress = false;
  }

  if (setHours == true && getButtonUp() && isStillPressing == false)
  {


    hours = hours + 1;
    //disPrintTime();
    longPress = true;
    delay(300);

  }
  else if (setHours == false && getButtonUp() && isStillPressing == false && minutes < 59)
  {
    minutes = minutes + 1;
    //disPrintTime();
    longPress = true;
    delay(150);
  }
  else if (setHours == true && getButtonDown() && isStillPressing == false && hours > 0)
  {
    hours = hours - 1;
    //disPrintTime();
    longPress = true;
    delay(300);
  }
  else if (setHours == false && getButtonDown() && isStillPressing == false && minutes > 0)
  {
    minutes = minutes - 1;
    // disPrintTime();
    longPress = true;
    delay(150);
  }
  else if (getButtonSelect() && isStillPressing == false)
  {
    requestFurtherStepsTime(hours * 60.0 * 60.0 + minutes * 60.0);

    if (hours != 0 || minutes != 0)
    {
      current_main_state = getAgitatorParam;
    }
    else
    {
      current_main_state = startWithGivenParameters;
      saveSeqParameters();
      saveAgitatorParameters();
      requestGlobalStart();
      //disPrintSeqFile();

    }
    hours = 0;
    minutes = 0;
    isStillPressing = true;
    displayedStartMessgae = false;

  }

  if (setHours == true)
  {
    if (minutesInvisible == true)
    {
      disPrintTime();
      minutesInvisible = false;
    }
    disBlinkHours();
  }
  else
  {
    if (hoursInvisible == true)
    {
      disPrintTime();
      hoursInvisible = false;
    }
    disBlinkMinutes();
  }

  if (newLog10h != oldLog10h)
  {
    if (newLog10h > oldLog10h)
    {
      minPos +=  newLog10h;
    }
    else
    {
      minPos -=  1;
    }
    oldLog10h = newLog10h;
    disPrintTime();

  }
  if (newLog10m != oldLog10m)
  {
    oldLog10m = newLog10m;
    disPrintTime();
  }

}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void getCookingTemp()
{
  newLog10dt = (int)log10(tempUserDot);
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
    longPress = false;
  }

  if (setTempUser == true && getButtonUp() && isStillPressing == false && tempUser < 99)
  {
    tempUser = tempUser + 1;
    longPress = true;
    delay(150);
  }
  else if (setTempUser == false && getButtonUp() && isStillPressing == false && tempUserDot < 75)
  {
    tempUserDot = tempUserDot + 25;
    longPress = true;
    delay(300);
  }
  else if (setTempUser == true && getButtonDown() && isStillPressing == false && tempUser > getstartWaterTemp())
  {
    tempUser = tempUser - 1;
    longPress = true;
    delay(150);
  }
  else if (setTempUser == false && getButtonDown() && isStillPressing == false && tempUserDot >= 25)
  {
    tempUserDot = tempUserDot - 25;
    longPress = true;
    delay(300);
  }
  else if (getButtonSelect() && isStillPressing == false)
  {


    isStillPressing = true;
    displayedStartMessgae = false;
    requestFurtherStepsTemp(((float)tempUserDot / 100.0) + tempUser);
    tempUserDot = 0;
    tempUser = 40;


  }


  if (setTempUser == true)
  {
    if (dotTempInvisible == true)
    {
      disPrintTemp();
      dotTempInvisible = false;
    }
    disBlinkTemp();
  }
  else if (setTempUser == false)
  {
    if (tempInvisible == true)
    {
      disPrintTemp();
      tempInvisible = false;
    }
    disBlinkDotTemp();
  }

  if (newLog10dt != oldLog10dt)
  {
    oldLog10dt = newLog10dt;
    disPrintTemp();
  }





}

