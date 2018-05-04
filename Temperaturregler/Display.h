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
#endif
