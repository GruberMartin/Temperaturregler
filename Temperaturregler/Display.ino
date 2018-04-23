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




uint8_t i = 0;


  // print the number of seconds since reset:





}
