#ifndef Display_h
#define Display_h
typedef enum {
  selectCookingType,
  selectFastStart,
  selectFastStartTemp,
  selectBrewActions,
  selectionDone

} menu_states;
void initDisplay();
void printMinutes();
void printHours();
void printPointTemp();
void StartMenu();
#endif
