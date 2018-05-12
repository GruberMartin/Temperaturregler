#ifndef Display_h
#define Display_h
void waitForStartSignal();
void waitForCookingMode();
void getCookingTime();
void getCookingTemp();
boolean getButtonRight();
boolean getButtonLeft();
boolean getButtonUp();
boolean getButtonDown();
boolean getButtonSelect();
boolean getButtonNoone();
void disPrint(String firstLine, String secondLine);
void disPrintTime();
void disPrintTemp();
void disPrintActualTemp(float actualTemp);
void disPrintRegualtorActivated(float actualTemp);
void initDisplay();
void requestFurtherStepsTime(long timeCurrentStep);
long getStepTime(int index);
float getStepTemp(int index);
void chooseParameters(int whichFile);
void setStempTime(int index, int value);
void setStepTemp(int index, float temp);
void disPrintSeqFile();
boolean getAgitatorAns(int index);
void setAgitatorAns(int index, boolean value);
#endif
