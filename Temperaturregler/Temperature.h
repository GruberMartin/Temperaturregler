#ifndef Temperature_h
#define Temperature_h
void initTemperature();
void printTemperature(String label, DeviceAddress deviceAddress);
void requestTemp();
void printSensorVals();
float getValSens2();
float getStartTemp();
#endif
