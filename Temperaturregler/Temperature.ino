#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <DS18B20.h>
#include <Arduino.h>



#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 10
float minTemp = 0.0;
float startWaterTemp = 0.0;
float tempC = 0.0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 = {0x28,  0xFF,  0xFE,  0xEA,  0x70,  0x17,  0x3,  0x91 }; // langer Sensor (inside)
DeviceAddress sensor2 = { 0x28,  0xE5,  0x19,  0xFC,  0x9,  0x0,  0x0,  0xBE };// kurzer Sensor

void initTemperature()
{
  if (!sensors.getAddress(sensor1, 0)) Serial.println("Sensor 1 nicht richtig verbunden");
  if (!sensors.getAddress(sensor2, 1)) Serial.println("Sensor 2 nicht richtig verbunden");

  // set the resolution to 9 bit per device
  sensors.setResolution(sensor1, TEMPERATURE_PRECISION);
  sensors.setResolution(sensor2, TEMPERATURE_PRECISION);

  Serial.begin(250000);
  sensors.begin();
  Serial.println("Wassertemperatur Umgebungstemperatur Zeit");
  minTemp = getValSens1();
  startWaterTemp = getValSens2();  
}


void printTemperature(String label, DeviceAddress deviceAddress) {


  tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Error getting temperature");
  } else {

    Serial.print(tempC);

  }

}

float getstartWaterTemp()
{
  return startWaterTemp;
}

void requestTemp()
{
  sensors.requestTemperatures();
}

float getStartTemp()
{
  return getValSens2();

}

void printSensorVals()
{
  Serial.print(" ");
  printTemperature("Wassertemperatur : ", sensor2);
  Serial.print(" ");
  printTemperature("Umgebungstemperatur : ", sensor1);
  Serial.print(" ");
}

float getValSens2()
{
  //sensors.requestTemperatures();
  return sensors.getTempC(sensor2);
}

float getValSens1()
{
  return sensors.getTempC(sensor1);
}

