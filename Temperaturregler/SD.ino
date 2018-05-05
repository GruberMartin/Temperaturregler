#include "Dynamic_Parameter_determination.h"
#include <Arduino.h>
#include "Temperature.h"
#include "PI.h"
#include <SPI.h>
#include <SD.h>


File myFile;
String filename;
boolean writingSuccessfully = false;
boolean readingSuccessfully = false;
boolean filesAvailable = false;
String fileContent;
String tmp;
int stringCounter = 1;
int numberOfFiles = 0;
//const int chipSelect = 53; // Braucht es das ? ----------------------------------------------------------------------------




void savePIParameters()
{
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Schreiben auf SD-Karte fehlgeschlagen");
    currentState = running_PI;
  }

  for (int i = 0; (i < 40) && (writingSuccessfully == false); i++)
  {
    filename = String(String(i) + ".txt");
    if (!SD.exists(filename))
    {
      myFile = SD.open(filename, FILE_WRITE);
      if (myFile) {
        myFile.print(getKpr());
        myFile.print(",");
        myFile.print(getTn());
        myFile.print(",");
        myFile.print(getTm());
        myFile.print(",");
        myFile.print(getT());
        myFile.print(",");
        myFile.print(getN());
        myFile.print(",");
        /*  myFile.print(getSeconds());
          myFile.print(",");*/
        myFile.print(getKps());
        myFile.print(",");
        myFile.print(getStartVoltage());
        myFile.print(",");
        /*myFile.print(",");
          myFile.print(getSetPoint());*/
        // close the file:
        myFile.close();
        writingSuccessfully = true;
        currentState = running_PI;
      }
    }

  }
  if (writingSuccessfully == false)
  {
    setCurrentState(running_PI);
  }

}

int countNumberOfFiles()
{
  
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Keine SD-Karte erkannt, countNumberOfFiles");
    filesAvailable = false;
  }

  for (int i = 0; (i < 40); i++)
  {
    filename = String(String(i) + ".txt");
    if (SD.exists(filename))
    {
      numberOfFiles = numberOfFiles + 1;
      
    }
  }
  Serial.print(numberOfFiles);
  Serial.println(" Files verfuegbar");
  return numberOfFiles;

}



void readFile(int fileNumber)
{
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    //Serial.println("Lesen von SD-Karte fehlgeschlagen");

  }


  filename = String(String(fileNumber) + ".txt");
  if (SD.exists(filename))
  {
    myFile = SD.open(filename);
    if (myFile) {
      //Serial.println(filename + " :");

      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        //Serial.write();
        tmp = (char)myFile.read();
        if (tmp != ",")
        {
          fileContent = fileContent + tmp;

        }
        else
        {


          switch (stringCounter)
          {
            case 1:
              setKpr(fileContent.toFloat());
              break;
            case 2:
              setTn(fileContent.toFloat());
              break;
            case 3:
              setTm(fileContent.toFloat());
              break;
            case 4:
              setT(fileContent.toFloat());
              break;
            case 5:
              setN(fileContent.toInt());
              break;
            case 6:
              setKps(fileContent.toFloat());
              break;
            case 7:
              setStartVoltageIPart(fileContent.toFloat());
              break;
            default:
              break;


          }
          fileContent = "";
          stringCounter += 1;

        }
      }
      // close the file:
      readingSuccessfully = true;
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("Lesen von SD-Karte fehlgeschlagen");
    }
  }

}

