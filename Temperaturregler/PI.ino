#include "Dynamic_Parameter_determination.h"
#include <Arduino.h>
#include "Temperature.h"
#include "PI.h"
#include "SD.h"



float oldVoltage = 0.0;
float newVoltage = 0.0;
float oldError = 0.0;
float newError = 0.0;
float Kpr = 0.0;
float Tn = 0.0;
float Tm = 0.0;
float Sollwert = 70;
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

boolean reachedFinalTemperature = false;
float minIpart = 0.0;
boolean minIpartHasBeenSet = false;
boolean shutdownPI = false;

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

float getKpr()
{
  return Kpr;
}

void setKpr(float tmpKpr)
{
  Kpr = tmpKpr;
}

float getTn()
{
  return Tn;
}

void setTn(float tmpTn)
{
  Tn = tmpTn;
}

float getTm()
{
  return Tm;
}

void setTm(float tmpTm)
{
  Tm = tmpTm;
}

int getN()
{
  return n;
}

void setN(int tmpN)
{
  n = tmpN;
}

void setParameterProgrammatically(float K, float Tr, float T1, float Ta, int orderSet)
{
  Kpr = K;
  Tn = Tr;
  T1 = Tm;
  T = Ta;
  n = orderSet;
}

boolean checkParameters(float K, float Tr, float T1, float Ta, int orderSet)
{
  /*Serial.print("Kpr = ");
    Serial.println(Kpr);
    Serial.print("Tn = ");
    Serial.println(Tn);
    Serial.print("Tm = ");
    Serial.println(Tm);
    Serial.print("T = ");
    Serial.println(T);
    Serial.print("n = ");
    Serial.println(n);
    Serial.print("StartVoltage: ");
    Serial.println(voltageIold);
    Serial.print("SetPoint: ");
    Serial.println(Sollwert);*/
  if (K == Kpr && Tr == Tn && T1 == Tm && Ta == T && orderSet == n )
  {
    return true;
  }
  else
  {
    return false;
  }

}

void setT(float sampletime)
{
  T = sampletime;
}

void setCurrentState(PIstate stateToSet)
{
  currentState = stateToSet;
}

/*void getCurrentState(PIstate stateToSet)
  {
  currentState = stateToSet;
  }*/

void printPIParams()
{
  Serial.print("Kpr = ");
  Serial.println(Kpr);
  Serial.print("Tn = ");
  Serial.println(Tn);
  Serial.print("Tm = ");
  Serial.println(Tm);
  Serial.print("T = ");
  Serial.println(T);
  Serial.print("n = ");
  Serial.println(getN());
  Serial.print("Start Voltage= ");
  Serial.println(getStartVoltageIPart());
  Serial.print("Kp = ");
  Serial.println(getKps());

}

boolean hasRechedFinalValue()
{
  return reachedFinalTemperature;
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

void setStartVoltageIPart(float IpartOffset)
{
  voltageIold = IpartOffset;
}

float getStartVoltageIPart()
{
  return voltageIold;
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
      for (int i = 0; i < 4; i++)
      {
        if (muTemp > abs(muC[i] - mu))
        {
          muTemp = abs(muC[i] - mu);
          order = i;
        }
      }
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

      // 2.91;//
      Tm = (1.0 / 3.0) * (alpha10 * t10r + alpha50 * t50r + alpha90 * t90r); //624.01;
      T = 0.1 * n * Tm ; // analog zu buch seite 290 46.8;
      Kpr = (KprKps / getKps());
      Tn = TnTm * Tm; //967.21;
      currentState = savePI_Parameter;
      break;

    case savePI_Parameter:

      savePIParameters();
      imediateCalcVoltage();

      break;

    case running_PI:
      if (calcVoltage())
      {

        requestTemp();
        newError = Sollwert - getValSens2();
        voltageP = Kpr * newError;
        voltageI = voltageIold + (Kpr / Tn) * (T / 2) * newError + (Kpr / Tn) * (T / 2) * oldError;

        newVoltage = voltageP + voltageI;

        setDonewCalc();
      }



      if (newVoltage > 1511.43)
      {
        newVoltage = 1511.43;
        voltageIold = 1511.43;

      }
      else if (newVoltage <= 0.0)
      {
        newVoltage = 0.0;


      }

      if (requestRegulatorChange() == true)
      {
        if (newError >=  0.25)
        {
          newVoltage = 1511.43;
        }
        else if (newError <= 0.0)
        {
          newVoltage = 0;
        }
      }



      oldError = newError;
      setVoltage(newVoltage);
      break;
  }
}

