#ifndef SCREEN_h
#define SCREEN_h
#include </home/santiago/Arduino/silviaPID/graphics.h>
#include </home/santiago/Arduino/silviaPID/silviaPID.h>
#include </home/santiago/Arduino/silviaPID/temperature.h>

// LCD declarations
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Keeps track of the current screen
// Home screen          1
// Main Settings        2
// Temperature Settings 3
// Time Settings        4
// Brewing screen       5
unsigned short CurrentScreen = 0;

// Clear "Silvia PID" Title
void clearTitle(char* title, int titleLen) {
    tft.setTextSize(1);
    tft.setFont(&TITLEFONT);
    clearButton(0,0,10,3,0,0,title, TITLEFONTWIDTH, titleLen);
}

// Clear Temperature Settings Screen
void clearSettingsTemp() {
  clearTitle("Temperature", 11);

  tft.setTextSize(1);
  tft.setFont(&TITLEFONT);
  tft.setTextColor(BACKGROUND);
  clearButton(0.5, 3.5, 9, 2, 0, 0,"Brew Temp.", TITLEFONTWIDTH, 10);

  tft.setTextSize(2);
  tft.setFont(&REGULARFONT);

  tft.fillCircle(1.25*32, 6.5*32, 24, BACKGROUND);
  tft.fillCircle(8.75*32, 6.5*32, 24, BACKGROUND);

  tft.setCursor(6.5*32, 5.75*32 + 1.1 * 2 * REGULARFONTWIDTH);
  tft.println("C");

  tft.fillCircle(6.125*32 + 2, 6*32, 8, BACKGROUND);

  drawNumber(settings->brewTemp, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  
  tft.setTextSize(1);
  tft.setFont(&TITLEFONT);
  clearButton(0.5, 7.5, 9, 2, 0, 0, "Steam Temp.", TITLEFONTWIDTH, 11);

  tft.fillCircle(1.25*32, 10.5*32, 24, BACKGROUND);
  tft.fillCircle(8.75*32, 10.5*32, 24, BACKGROUND);

  tft.setTextSize(2);
  tft.setFont(&REGULARFONT);
  tft.setCursor(6.5*32, 9.75*32 + 1.1 * 2 *REGULARFONTWIDTH);
  tft.println("C");

  tft.fillCircle(6.125*32 + 2, 10*32, 8, BACKGROUND);

  drawNumber(settings->steamTemp, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  clearButton(5, 13, 4.5, 1.5, 0, 2, "Home", REGULARFONTWIDTH, 4);
  clearButton(0.5, 13, 4.5, 1.5, 0, 2, "Return", REGULARFONTWIDTH, 6);
}

// Clear Home Screen
void clearHomeScreen(unsigned int newScreen) {
  clearButton(0.5, 8, 9, 5.5, 16, 5, "", REGULARFONTWIDTH, 0);
  tft.fillRect(5*32 - 2, 8*32, 4, 5.5*32, BACKGROUND);
  tft.fillRect(0.5*32, 10.75*32 - 2, 9*32, 4, BACKGROUND);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  tft.setTextColor(BACKGROUND);
  clearButton(0.5, 8, 4.5, 2.75, 16, 0, "Brew", REGULARFONTWIDTH, 4); 
  clearButton(5, 8, 4.5, 2.75, 16, 0, "Preinf", REGULARFONTWIDTH, 6); 
  clearButton(0.5, 10.75, 4.5, 2.75, 16, 0, "Purge", REGULARFONTWIDTH, 5); 
  clearButton(5, 10.75, 4.5, 2.75, 16, 0, "Sett.", REGULARFONTWIDTH, 5); 

  if (newScreen != 5){
    tft.setTextSize(1);
    tft.setFont(&REGULARFONT);
    clearButton(0.5, 4, 0, 0, 0, 0, "Temperature: ", REGULARFONTWIDTH, 0);
    clearTitle("Silvia PID", 10);
    tft.setTextSize(2);
    tft.setFont(&TITLEFONT);
    drawNumber(temp->dispTemp, 3, 2, 5.5, 2 * TITLEFONTWIDTH, true);
    tft.setCursor(7.5*32, 5.5*32 + 1.1 * 2 * TITLEFONTWIDTH);
    tft.fillCircle(7.375*32, 5.5*32 + 5, 8, BACKGROUND);
    tft.print("C");
  }
}

// Clear Main Settings Screen
void clearSettingsHome() {
  clearTitle("Settings", 8);
  tft.setFont(&REGULARFONT);
  tft.setTextSize(1);
  clearButton(0.5, 4, 9, 2.5, 16, 4, "Temperature", REGULARFONTWIDTH, 11);
  clearButton(0.5, 7, 9, 2.5, 16, 4, "Brew Time", REGULARFONTWIDTH, 9);
  clearButton(0.5, 10, 9, 2.5, 16, 4, "Set PID", REGULARFONTWIDTH, 7);
  clearButton(5, 13, 4.5, 1.5, 0, 2, "Home", REGULARFONTWIDTH, 4);
}

// Clear Brew Timings Settings
void clearSettingsTime() {
  clearTitle("Brew Time",  9);
  tft.setTextColor(BACKGROUND);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  tft.setCursor(24, 4*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Regular Brew");
  
  tft.setCursor(24, 5.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Time: ");
  
  tft.fillCircle(4.5*32, 5.625*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 5.625*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 5.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->brewMillis/1000,  2, 5.5, 5.25, REGULARFONTWIDTH, true);

  tft.setCursor(24, 6.75*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Preinfusion");

  tft.setCursor(24, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Pre: ");
  
  tft.fillCircle(4.5*32, 8.375*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 8.375*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->preInfMillis / 1000, 2, 5.5, 8, REGULARFONTWIDTH, true);
  
  tft.setCursor(24, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Wait: ");
  
  tft.fillCircle(4.5*32, 9.5*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 9.5*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->waitMillis / 1000, 2, 5.5, 9.125, REGULARFONTWIDTH, true);

  tft.setCursor(24, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Brew: ");
  
  tft.fillCircle(4.5*32, 10.625*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 10.625*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->preInfBrewMillis/1000, 2, 5.5, 10.25, REGULARFONTWIDTH, true);

  clearButton(5, 13, 4.5, 1.5, 0, 2, "Home", REGULARFONTWIDTH, 4);
  clearButton(0.5, 13, 4.5, 1.5, 0, 2, "Return", REGULARFONTWIDTH, 6);
}

void clearBrewingScreen() {
  tft.setTextSize(1);
  tft.setTextColor(BACKGROUND);
  tft.setFont(&REGULARFONT);
  clearButton(0.5, 8, 0, 1.1 * REGULARFONTWIDTH/ 32.0, 0, 0, "Shot Timer:", REGULARFONTWIDTH, 0);

  tft.setFont(&TITLEFONT);
  tft.setTextSize(2);
  drawNumber(time->brewElapsedSec, 3, 0.5, 9.5, TITLEFONTWIDTH * 2, true);
  tft.setCursor(0.5 * 32 + TITLEFONTWIDTH * 2 * 3, 9.5*32 + 2 * TITLEFONTWIDTH * 1.1);
  tft.print("s");

  tft.fillCircle(5*32, 13.25*32, 1.5*32, BACKGROUND);
}

void clearScreen(int newScreen) {
  switch (CurrentScreen){
    case 1:
      clearHomeScreen(newScreen);
      break;
    case 2:
      clearSettingsHome();
      break;
    case 3:
      clearSettingsTemp();
      break;
    case 4:
      clearSettingsTime();
      break;
    case 5:
      clearBrewingScreen();
  }
}

void drawSettingsTemp() {
  clearScreen(3);
  CurrentScreen = 3;
  titleText("Temperature",  11);

  // brew temperature interface
  tft.setTextColor(TEXTTWO);
  tft.setTextSize(1);
  tft.setFont(&TITLEFONT);
  drawRectButton(0.5, 3.5, 9, 2, 0, "Brew Temp.", TITLEFONTWIDTH, 10);
  //tft.setCursor(32, 4*32);
  //tft.print("Brew Temp.");

  tft.setTextSize(2);
  tft.setFont(&REGULARFONT);

  drawMinusButton(1.25, 6.5, 24);
  drawPlusButton(8.75, 6.5, 24);

  tft.setCursor(6.5*32, 5.75*32 + 1.1 * 2 * REGULARFONTWIDTH);
  tft.println("C");

  tft.fillCircle(6.125*32 + 2, 6*32, 8, TEXTTWO);
  tft.fillCircle(6.125*32 + 2, 6*32, 5, BACKGROUND);

  drawNumber(settings->brewTemp, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  
  // steam temperature interface
  tft.setTextSize(1);
  tft.setFont(&TITLEFONT);
  drawRectButton(0.5, 7.5, 9, 2, 0, "Steam Temp.", TITLEFONTWIDTH, 11);

  drawMinusButton(1.25, 10.5, 24);
  drawPlusButton(8.75, 10.5, 24);

  tft.setTextSize(2);
  tft.setFont(&REGULARFONT);
  tft.setCursor(6.5*32, 9.75*32 + 1.1 * 2 *REGULARFONTWIDTH);
  tft.println("C");

  tft.fillCircle(6.125*32 + 2, 10*32, 8, TEXTTWO);
  tft.fillCircle(6.125*32 + 2, 10*32, 5, BACKGROUND);

  drawNumber(settings->steamTemp, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  drawRectButton(5, 13, 4.5, 1.5, 2, "Home", REGULARFONTWIDTH, 4);
  drawRectButton(0.5, 13, 4.5, 1.5, 2, "Return", REGULARFONTWIDTH, 6);
}



void drawSettingsTime() {
  clearScreen(4);
  CurrentScreen = 4;
  titleText("Brew Time",  9);
  tft.setTextColor(TEXTTWO);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  tft.setCursor(24, 4*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Regular Brew");
  
  tft.setCursor(24, 5.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Time: ");
  
  drawMinusButton(4.5, 5.625, 16);
  drawPlusButton(8, 5.625, 16);

  tft.setCursor(6.75*32, 5.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->brewMillis / 1000, 2, 5.5, 5.25, REGULARFONTWIDTH, true);

  tft.setCursor(24, 6.75*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Preinfusion");

  tft.setCursor(24, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Pre: ");
  
  drawMinusButton(4.5, 8.375, 16);
  drawPlusButton(8, 8.375, 16);

  tft.setCursor(6.75*32, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->preInfMillis / 1000, 2, 5.5, 8, REGULARFONTWIDTH, true);
  
  tft.setCursor(24, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Wait: ");
  
  drawMinusButton(4.5, 9.5, 16);
  drawPlusButton(8, 9.5, 16);

  tft.setCursor(6.75*32, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->waitMillis / 1000, 2, 5.5, 9.125, REGULARFONTWIDTH, true);

  tft.setCursor(24, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Brew: ");
  
  drawMinusButton(4.5, 10.625, 16);
  drawPlusButton(8, 10.625, 16);

  tft.setCursor(6.75*32, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(settings->preInfBrewMillis/1000,  2, 5.5, 10.25, REGULARFONTWIDTH, true);

  drawRectButton(5, 13, 4.5, 1.5, 2, "Home", REGULARFONTWIDTH, 4);
  drawRectButton(0.5, 13, 4.5, 1.5, 2, "Return", REGULARFONTWIDTH, 6);
}

void drawBrewingScreen() {
  clearScreen(5);
  CurrentScreen = 5;

  tft.setTextSize(1);
  tft.setTextColor(TEXTTWO);
  tft.setFont(&REGULARFONT);
  drawRectButton(0.5, 8, 0, 1.1 * REGULARFONTWIDTH/ 32.0, 0, "Shot Timer:", REGULARFONTWIDTH, 0);

  tft.setFont(&TITLEFONT);
  tft.setTextSize(2);
  tft.setTextColor(TEXTTWO);
  drawNumber(0, 3, 0.5, 9.5, TITLEFONTWIDTH * 2, true);
  tft.setCursor(0.5 * 32 + TITLEFONTWIDTH * 2 * 3, 9.5*32 + 2 * TITLEFONTWIDTH * 1.1);
  tft.print("s");

  tft.fillCircle(5*32, 13.25*32, 1.5*32, RED);
  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  tft.setTextColor(WHITE);
  drawRoundButton(5-1.5, 13.25-1.5, 1.5*2, 1.5*2, 0, 0, "STOP", REGULARFONTWIDTH, 4);
}



void drawHomeScreen() {
  clearScreen(1);
  CurrentScreen = 1;
  
  // title text
  
  titleText("Silvia PID",  10);

  // temperature display
  tft.setTextSize(2);
  tft.setTextColor(TEXTTWO);
  tft.setFont(&TITLEFONT);
  tft.setCursor(7.5*32, 5.5*32 + 1.1 * 2 * TITLEFONTWIDTH);
  tft.fillCircle(7.375*32, 5.5*32 + 5, 8, TEXTTWO);
  tft.fillCircle(7.375*32, 5.5*32 + 5, 5, BACKGROUND);
  tft.print("C");

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  tft.setTextColor(TEXTTWO);
  drawRoundButton(0.5, 4, 0, 0, 0, 0, "Temperature: ", REGULARFONTWIDTH, 0);

  // option buttons
  drawRoundButton(0.5, 8, 9, 5.5, 16, 5, "", REGULARFONTWIDTH, 0);
  tft.fillRect(5*32 - 2, 8*32, 4, 5.5*32, BORDER);
  tft.fillRect(0.5*32, 10.75*32 - 2, 9*32, 4, BORDER);

  drawRoundButton(0.5, 8, 4.5, 2.75, 16, 0, "Brew", REGULARFONTWIDTH, 4); 
  drawRoundButton(5, 8, 4.5, 2.75, 16, 0, "Preinf", REGULARFONTWIDTH, 6); 
  drawRoundButton(0.5, 10.75, 4.5, 2.75, 16, 0, "Purge", REGULARFONTWIDTH, 5); 
  drawRoundButton(5, 10.75, 4.5, 2.75, 16, 0, "Sett.", REGULARFONTWIDTH, 5); 

  // Draw temperature
  tft.setTextSize(2);
  tft.setFont(&TITLEFONT);
  refreshNumber(temp->approxTemp, temp->dispTemp, 3, 2, 5.5, 2 * TITLEFONTWIDTH, true);
}

void drawSettingsHome() {
  clearScreen(2);
  CurrentScreen = 2;

  titleText("Settings",  8);
  tft.setTextColor(TEXTTWO);
  tft.setFont(&REGULARFONT);
  tft.setTextSize(1);
  drawRoundButton(0.5, 4, 9, 2.5, 16, 4, "Temperature", REGULARFONTWIDTH, 11);
  drawRoundButton(0.5, 7, 9, 2.5, 16, 4, "Brew Time", REGULARFONTWIDTH, 9);
  drawRoundButton(0.5, 10, 9, 2.5, 16, 4, "Set PID", REGULARFONTWIDTH, 7);
  drawRectButton(5, 13, 4.5, 1.5, 2, "Home", REGULARFONTWIDTH, 4);
}
#endif
