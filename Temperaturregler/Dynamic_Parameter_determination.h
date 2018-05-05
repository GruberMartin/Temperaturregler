#ifndef Dynamic_Parameter_determination_h
#define Dynamic_Parameter_determination_h
void initParameterDetermination();
void writeTemperature(float val);
void printParameter();
float getFinalValue();
boolean finish();
float getT10();
float getT50();
float getT90();
float getKps();
void setKps(float tmpKp);
float calculateStartVoltage();
float calculateStartVoltageForIpart();
#endif
