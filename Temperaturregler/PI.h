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
float getCurrentVoltage();
boolean hardStop();
float getError();
float setSetPoint(float setPoint);
float getSetPoint();

#endif
