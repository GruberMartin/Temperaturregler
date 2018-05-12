#include "Dynamic_Parameter_determination.h"
#include <Arduino.h>
#include "Temperature.h"
#include "PI.h"
#include <SPI.h>
#include <SD.h>
#include "Display.h"


File myFile;
String filename;
boolean writingPiParamSuccessfully = false;
boolean readingPiParamSuccessfully = false;
boolean writingSeqParamSuccessfully = false;
boolean readingSeqParamSuccessfully = false;
boolean writingAgitatorParamSuccessfully = false;
boolean readingAgitatorParamSuccessfully = false;
boolean filesAvailable = false;
String fileContent;
String tmp;
int stringCounter = 1;
int numberOfFiles = 0;
int seqNowCount = 0;
int agiNowCount = 0;


const int sdChip = 53;


int getNumberOfFiles()
{
  return numberOfFiles;
}

void savePIParameters()
{
  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Schreiben auf SD-Karte fehlgeschlagen");
    currentState = running_PI;
  }

  for (int i = 0; (i < 6) && (writingPiParamSuccessfully == false); i++)
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
        writingPiParamSuccessfully = true;
        currentState = running_PI;
      }
    }

  }
  if (writingPiParamSuccessfully == false)
  {
    setCurrentState(running_PI);
  }

}


void saveSeqParameters()
{


  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Schreiben auf SD-Karte fehlgeschlagen");
    current_main_state = fastCookingModeTime;
  }

  for (int i = 0; (i < 6) && (writingSeqParamSuccessfully == false); i++)
  {
    filename = "seq" + String(String(i) + ".txt");
    if (!SD.exists(filename))
    {
      myFile = SD.open(filename, FILE_WRITE);
      if (myFile) {
        for (int z = 0; z < 6; z++)
        {
          myFile.print(getStepTime(z));
          myFile.print(", ");
          myFile.print(getStepTemp(z));
          myFile.print(", ");
        }
        myFile.close();
        writingSeqParamSuccessfully = true;

        //currentState = running_PI;
        current_main_state = startWithGivenParameters;
      }
    }

  }
  if (writingSeqParamSuccessfully == false)
  {

    current_main_state = fastCookingModeTime;
    Serial.println("Wrtiting to seq file failed");
  }

}


void saveAgitatorParameters()
{


  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Schreiben auf SD-Karte fehlgeschlagen");
    //current_main_state = fastCookingModeTime;
  }

  for (int i = 0; (i < 6) && (writingAgitatorParamSuccessfully == false); i++)
  {
    filename = "ag" + String(String(i) + ".txt");
    if (!SD.exists(filename))
    {
      myFile = SD.open(filename, FILE_WRITE);
      if (myFile) {
        for (int z = 0; z < 6; z++)
        {

          myFile.print((String)getAgitatorAns(z));
          myFile.print(", ");

        }
        myFile.close();
        writingAgitatorParamSuccessfully = true;

        //current_main_state = startWithGivenParameters;
      }
    }

  }
  if (writingAgitatorParamSuccessfully == false)
  {


    Serial.println("Wrtiting to agiator file failed");
  }

}

int countNumberOfPiFiles()
{

  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Keine SD-Karte erkannt, countNumberOfPiFiles");
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
  //  Serial.print(numberOfFiles);
  //  Serial.println(" Files verfuegbar");
  return numberOfFiles;

}

int countNumberOfSeqFiles()
{

  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Keine SD-Karte erkannt, countNumberOfSeqFiles");
    filesAvailable = false;
    numberOfFiles = 0;
  }
  numberOfFiles = 0;
  for (int i = 0; (i < 40); i++)
  {
    filename = "seq" + String(String(i) + ".txt");
    if (SD.exists(filename))
    {
      numberOfFiles = numberOfFiles + 1;

    }
  }
  //  Serial.print(numberOfFiles);
  //  Serial.println(" Files verfuegbar");
  return numberOfFiles;

}



void readFile(int fileNumber)
{

  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
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
              //setStartVoltageIPart(fileContent.toFloat());
              break;
            default:
              break;


          }
          fileContent = "";
          stringCounter += 1;

        }
      }
      // close the file:
      readingPiParamSuccessfully = true;
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("Lesen von SD-Karte fehlgeschlagen");
    }
  }

}

void readSeqFile(int fileNumber)
{
  stringCounter = 1;
  fileContent = "";
  tmp = "";
  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Lesen von SD-Karte fehlgeschlagen");

  }


  filename = "seq" + String(String(fileNumber) + ".txt");
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
              setStempTime(seqNowCount, fileContent.toInt());
              break;
            case 2:
              setStepTemp(seqNowCount, fileContent.toFloat());
              seqNowCount += 1;
              break;
            case 3:
              setStempTime(seqNowCount, fileContent.toInt());
              break;
            case 4:
              setStepTemp(seqNowCount, fileContent.toFloat());
              seqNowCount += 1;
              break;
            case 5:
              setStempTime(seqNowCount, fileContent.toInt());
              break;
            case 6:
              setStepTemp(seqNowCount, fileContent.toFloat());
              seqNowCount += 1;
              break;
            case 7:
              setStempTime(seqNowCount, fileContent.toInt());
              break;
            case 8:
              setStepTemp(seqNowCount, fileContent.toFloat());
              seqNowCount += 1;
              break;
            case 9:
              setStempTime(seqNowCount, fileContent.toInt());
              break;
            case 10:
              setStepTemp(seqNowCount, fileContent.toFloat());
              seqNowCount += 1;
              break;
            case 11:
              setStempTime(seqNowCount, fileContent.toInt());
              break;
            case 12:
              setStepTemp(seqNowCount, fileContent.toFloat());
              seqNowCount += 1;
              break;
            default:
              break;


          }
          fileContent = "";
          stringCounter += 1;

        }
      }
      // close the file:
      readingSeqParamSuccessfully = true;
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("Lesen von SD-Karte fehlgeschlagen");
    }
  }

}


void readAgitatorFile(int fileNumber)
{
  stringCounter = 1;
  fileContent = "";
  tmp = "";
  pinMode(SS, OUTPUT);
  if (!SD.begin(sdChip)) {
    //Serial.println("Lesen von SD-Karte fehlgeschlagen");

  }


  filename = "ag" + String(String(fileNumber) + ".txt");
  if (SD.exists(filename))
  {
    myFile = SD.open(filename);
    if (myFile) {

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
              setAgitatorAns(agiNowCount, fileContent.toInt());
              agiNowCount += 1;
              break;
            case 2:
              setAgitatorAns(agiNowCount, fileContent.toInt());
              agiNowCount += 1;
              break;
            case 3:
              setAgitatorAns(agiNowCount, fileContent.toInt());
              agiNowCount += 1;
              break;
            case 4:
              setAgitatorAns(agiNowCount, fileContent.toInt());
              agiNowCount += 1;
              break;
            case 5:
              setAgitatorAns(agiNowCount, fileContent.toInt());
              agiNowCount += 1;
              break;
            case 6:
              setAgitatorAns(agiNowCount, fileContent.toInt());
              agiNowCount += 1;
              break;

            default:
              break;


          }
          fileContent = "";
          stringCounter += 1;

        }
      }
      // close the file:
      readingAgitatorParamSuccessfully = true;
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("Lesen von Ruehrwerkparametern fehlgeschlagen");
    }
  }

}


