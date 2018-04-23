/*********************

  Example code for the Adafruit RGB Character LCD Shield and Library

  This code displays text on the shield, and also reads the buttons on the keypad.
  When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "Display.h"


// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color




menu_states currentMenuState = selectCookingType;

void initDisplay() {
  // Debugging output
  Serial.begin(250000);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  //lcd.print("Drücken Sie RESET um zum vorigen Menu zurückzukehren.");
  lcd.setBacklight(WHITE);
  lcd.print("Press Select to ");
  lcd.setCursor(0,1);
  lcd.print("Start");
  while (waitForUserImput == false)
  {
    
     buttons = lcd.readButtons();
    if (buttons) {
      lcd.clear();
      lcd.setCursor(0, 0);
      if (buttons & BUTTON_SELECT) {
        waitForUserImput = true;
      }
    }
  }
}

void printMinutes()
{
  
        
        if(minutes < 10)
        {
          minutesString = "0" + (String)minutes;
        }
        else
        {
          minutesString = (String)minutes + "";
        }
        if(hours < 10)
        {
          hoursString = "0" + (String)hours;
        }
        else
        {
          hoursString = (String)hours + "";
        }
      
            
      
     
      lcd.setCursor(0, 1);
      lcd.print(hoursString + " h : " + minutesString + " min");
      
}

void printHours()
{
  
        if(hours < 10)
        {
          hoursString = "0" + (String)hours;
        }
        else
        {
          hoursString = (String)hours + "";
        }
        if(minutes < 10)
        {
          minutesString = "0" + (String)minutes;
        }
        else
        {
          minutesString = (String)minutes + "";
        }
      
          
      
      
      lcd.setCursor(0, 1);
      lcd.print(hoursString + " h : " + minutesString + " min");
      
      changeRequested = false;
      
}


void printPointTemp()
{
  
        
        if(pointTemp < 10)
        {
          pointTempString = "0" + (String)pointTemp;
        }
        else
        {
          pointTempString = (String)pointTemp + "";
        }
        if(temp < 10)
        {
          tempString = "0" + (String)temp;
        }
        else
        {
          tempString = (String)temp + "";
        }
      
            
      
     
      lcd.setCursor(0, 1);
      lcd.print(tempString + "." + pointTempString + " deg.");
      
}



void StartMenu() {

  buttons = lcd.readButtons();
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  /*if (currentMillis - previousMillis >= interval && changeRequested == true) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    lcd.clear();
    // if the LED is off turn it on and vice-versa:
    if (hoursSelected == false) {
      hoursSelected = true;
    } else {
      hoursSelected = false;
    }

    if(minutesSelected == false)
    {
      minutesSelected = true;
    }
    else
    {
      minutesSelected = false;
    }

    
  }*/

  
  switch (currentMenuState)
  {
    case selectCookingType:
      lcd.setCursor(0, 0);
      waitForUserImput = false;
      if(textSet == false)
      {
      lcd.print("<- Only 1 Temp.");
      lcd.setCursor(0,1);
      lcd.print("-> Spec. Config.");
      lcd.setCursor(0,0);
      }
      if (buttons) {
        lcd.clear();
        lcd.setCursor(0, 0);
        if (buttons & BUTTON_LEFT) {
          currentMenuState = selectFastStart;
          changeRequested = true;
        }
        if (buttons & BUTTON_RIGHT) {
          currentMenuState = selectBrewActions;
          changeRequested = true;
        }
      }
      break;

    case selectFastStart:
    
     lcd.setCursor(0, 0);
      waitForUserImput = false;
      
      lcd.print("Time to hold:");
      if(changingHours == true)
      {
      printHours();
      
      }
      else
      {
        printMinutes();
      }
      if (buttons) {
        lcd.clear();
        
        if (buttons & BUTTON_SELECT) {
          changeRequested = false;
          currentMenuState = selectFastStartTemp;
          lcd.clear();
        }
        if (buttons & BUTTON_RIGHT) {
          changingHours = false;
          changeRequested = true;
        }
        if (buttons & BUTTON_LEFT) {
          changingHours = true;
          
        }
        if (buttons & BUTTON_UP) {
          if(changingHours == true)
          {
            hours = hours + 1;
                      }
          else
          {
            minutes = minutes + 1;
                     }
        }
        if (buttons & BUTTON_DOWN) {
          if(changingHours == true)
          {
            hours = hours - 1;
            
          }
          else
          {
            minutes = minutes - 1;
            
          }
        }
      }
      break;

      case selectFastStartTemp:
      
      lcd.setCursor(0,0);
      lcd.print("Temperature: ");
      //lcd.setCursor(0,1);
        printPointTemp();
        //lcd.setCursor(0, 0);
       if (buttons) {
        lcd.clear();

        if(changingTemp == true)
      {
      printPointTemp();
      
      }
      else
      {
        printPointTemp();
      }
        
        if (buttons & BUTTON_SELECT) {
          changeRequested = false;
          currentMenuState = selectionDone;
          lcd.clear();
        }
        if (buttons & BUTTON_RIGHT) {
          changingTemp = false;
          
        }
        if (buttons & BUTTON_LEFT) {
          changingTemp = true;
          
        }
        if (buttons & BUTTON_UP) {
          if(changingTemp == true)
          {
            temp = temp + 1;
                      }
          else
          {
            pointTemp = pointTemp + 25;
                     }
        }
        if (buttons & BUTTON_DOWN) {
          if(changingTemp == true)
          {
            temp = temp - 1;
            
          }
          else
          {
            pointTemp = pointTemp - 25;
            
          }
        }
      }
      break;

      case selectionDone:
      
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting program");
        lcd.setCursor(0, 1);
        lcd.print("............");
        Serial.print("Einzustellende Zeit: ");
        secsToSet = minutes * 60 + hours * 60 * 60;
        Serial.println(secsToSet);
        Serial.print("Einzustellende Temperatur: ");
        tempUserSet = temp + ((float)pointTemp) / 100;
        Serial.println(tempUserSet);
        while(1){}
        break;
      
  }

}





