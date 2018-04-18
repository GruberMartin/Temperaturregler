#include "Dynamic_Parameter_determination.h"
#include <Arduino.h>
#include "Temperature.h"
#include "PI.h"
#include <SPI.h>
#include <SD.h>

File myFile;


float oldVoltage = 0.0;
float newVoltage = 0.0;
float oldError = 0.0;
float newError = 0.0;
float Kpr = 0.0;
float Tn = 0.0;
float Tm = 0.0;
float Sollwert = 80;
float alpha10, alpha50, alpha90, TnTm, KprKps;
float t10r;
float t50r;
float t90r;
float mu;
int order = 10;
float muTemp = 5.0;
float muC [] = {0.137, 0.174, 0.207, 0.261};
float T = 0.0;
int n = 0;
float voltageP = 0.0;
float voltageI = 0.0;
float voltageIold = 0.0; // gerade geändert
boolean newCalc = false;
boolean fastStopRequested = false;
//const int chipSelect = 10;
String filename;
boolean writingSuccessfully = false;
/*typedef enum {
  notStarted_PI,
  start_PI,
  running_PI,
  stop_PI,
  idle_PI
  } PIstate;*/

PIstate currentState = notStarted_PI;

void initPID() {
  //oldVoltage = getVoltage();
  newVoltage = oldVoltage;

}

boolean hardStop()
{
  return fastStopRequested;
}

float getT()
{

  return T;

}

void setCurrentState(PIstate stateToSet)
{
  currentState = stateToSet;
}

void getCurrentState(PIstate stateToSet)
{
  currentState = stateToSet;
}

float getCurrentVoltage()
{
  return newVoltage;
}

PIstate getCurrentState()
{
  return currentState;
}

float getError()
{
  return (Sollwert - getValSens2());
}

float getSetPoint()
{
  return Sollwert;
}

float setSetPoint(float setPoint)
{
  Sollwert = setPoint;
}



float controlVoltage()
{
  currentState = getCurrentState();
  switch (currentState)
  {
    case notStarted_PI:

      break;
    case start_PI:


      t10r = getT10(); //  308;
      t50r = getT50(); // 924;
      t90r = getT90();// 1820;
      mu = t10r / t90r * 1.0;
      /*Serial.print("mu = ");
        Serial.println(mu);*/

      for (int i = 0; i < 4; i++)
      {
        if (muTemp > abs(muC[i] - mu))
        {
          muTemp = abs(muC[i] - mu);
          order = i;
        }
      }
      /* Serial.print("order = ");
        Serial.println(order);*/

      if (order == 0)
      {
        alpha10 = 1.880;
        alpha50 = 0.596;
        alpha90 = 0.257;
        TnTm = 1.55;
        KprKps = 1.352;
        n = 2;
      }
      else if (order == 1)
      {
        alpha10 = 1.245;
        alpha50 = 0.460;
        alpha90 = 0.216;
        TnTm = 1.77;
        KprKps = 1.024;
        n = 2.5;
      }
      else if (order == 2)
      {
        alpha10 = 0.907;
        alpha50 = 0.374;
        alpha90 = 0.188;
        TnTm = 1.96;
        KprKps = 0.794;
        n = 3;
      }
      else
      {
        alpha10 = 0.573;
        alpha50 = 0.272;
        alpha90 = 0.150;
        TnTm = 2.3;
        KprKps = 0.598;
        n = 4;
      }

      /*Serial.println(t10r);
        Serial.println(t50r);
        Serial.println(t90r);*/
      //Serial.print("Achtung n manuel gesetzt");
      Kpr = (KprKps / getKps())*0.9; // 2.91;//
      Tm = (1.0/3.0)*(alpha10*t10r + alpha50*t50r + alpha90 * t90r); //624.01;
      T = 0.1 * Tm; // analog zu buch seite 290 46.8;
      //T = 1.0;
      //Serial.print("1.0/3.0");
      /*Serial.print("alpha10 = ");
        Serial.println(alpha10);
        Serial.print("alpha50 = ");
        Serial.println(alpha50);
        Serial.print("alpha90 = ");
        Serial.println(alpha90);*/

      Tn = TnTm*Tm;//967.21;
      //setCurrentState(running_PI);
      /*Serial.println("PI läuft mit folgenden Parametern");
        Serial.print("Kpr = ");
        Serial.println(Kpr);
        Serial.print("Tn = ");
        Serial.println(Tn);
        Serial.print("Tm = ");
        Serial.println(Tm);
        Serial.print("n = ");
        Serial.println(n);
        Serial.print("T = ");
        Serial.println(T);*/
     // Serial.println("Achtuuuuuuuuuuuung Parameter statisch gesetzt !!");
      currentState = savePI_Parameter;
      break;

    case savePI_Parameter:
      pinMode(SS, OUTPUT);
      if (!SD.begin(chipSelect)) {
        Serial.println("Schreiben auf SD-Karte fehlgeschlagen");
        currentState = running_PI;
      }

      for (int i = 0; i < 30 && writingSuccessfully == false; i++)
      {
        filename = String(String(i) + ".txt");
        if (!SD.exists(filename))
        {
          myFile = SD.open(filename, FILE_WRITE);
          if (myFile) {
            myFile.print("Kpr = ");
            myFile.println(Kpr);
            myFile.print("Tn = ");
            myFile.println(Tn);
            myFile.print("Tm = ");
            myFile.println(Tm);
            myFile.print("T = ");
            myFile.println(T);
            myFile.print("n = ");
            myFile.println(n);
            // close the file:
            myFile.close();
            writingSuccessfully = true;

          }
        }

      }


      currentState = running_PI;

      break;

    case running_PI:
      if (calcVoltage())
      {
        //Serial.print("newVoltage = ");
        //Serial.println(newVoltage);
        requestTemp();
        newError = Sollwert - getValSens2();
        voltageP = Kpr * newError;
        voltageI = voltageIold + (Kpr / Tn) * (T / 2) * newError + (Kpr / Tn) * (T / 2) * oldError;

        newVoltage = voltageP + voltageI;
        /*Serial.println("");
          Serial.print("newVoltage = ");
          Serial.println(newVoltage);
          Serial.print("error = ");
          Serial.println(newError);*/
        setDonewCalc();
      }

      if (newVoltage > 1511.43)
      {
        newVoltage = 1511.43;
        voltageIold = 1511.43;

      }
      else if (newVoltage <= 0)
      {
        newVoltage = 0;
        //voltageIold = 0; // damit es nicht noch einen zyklus länger dauert
        //turnOffHeating();
        //voltageIold = 0;

      }
      else if (newError <= 0.0)
      {
        newVoltage = 0;
        fastStopRequested = true;
        //voltageIold = 0;
      }
      else
      {

        voltageIold = newVoltage;
        fastStopRequested = false;
      }
      // oldVoltage = newVoltage;
      
      oldError = newError;
      setVoltage(newVoltage);
      /*Serial.println("");
        Serial.print("newVoltage = ");
        Serial.println(newVoltage);
        Serial.print("error = ");
        Serial.println(newError);*/
      /*Serial.println("PI ok");
        Serial.print("voltage: ");
        Serial.println(newVoltage);*/
      break;
  }
}

