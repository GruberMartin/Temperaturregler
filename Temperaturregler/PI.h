#ifndef PI_h
#define PI_h
void initPID();
float controlVoltage();
typedef enum {
  notStarted_PI,
  start_PI,
  savePI_Parameter,
  running_PI,
  stop_PI,
  idle_PI
} PIstate;
void setCurrentState(PIstate stateToSet);
float getT();
float getKpr();
void setKpr(float tmpKpr);
float getTn();
void setTn(float tmpTn);
float getTm();
void setTm(float tmpTm);
int getN();
void setN(int tmpN);
float getCurrentVoltage();
boolean hardStop();
float getError();
float setSetPoint(float setPoint);
float getSetPoint();
void setStartVoltageIPart(float IpartOffset);
void setT(float sampletime);
boolean hasRechedFinalValue();
void setParameterProgrammatically(float K, float Tr, float T1, float Ta, int orderSet);
boolean checkParameters(float K, float Tr, float T1, float Ta, int orderSet);
void printPIParams();
float getVoltageIpart();


#endif
