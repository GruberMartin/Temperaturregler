#ifndef PI_h
#define PI_h
void initPID();
float controlVoltage();
typedef enum {
  notStarted_PI,
  start_PI,
  running_PI,
  stop_PI,
  idle_PI
} PIstate;
void setCurrentState(PIstate stateToSet);
float getT();
float getCurrentVoltage();

#endif
