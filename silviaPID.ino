#include <PID_v1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#define TITLEFONT FreeMonoBold24pt7b
#define TITLEFONTWIDTH 27 
#define REGULARFONT FreeMonoBold18pt7b
#define REGULARFONTWIDTH 21

// Pin declarations
#define TEMP_PIN A15
#define HEAT_PIN 53
#define PUMP_PIN 52
#define VALVE_PIN 51
#define STEAM_PIN 28
#define BREW_PIN 29

// Touch screen declarations
#define YP A2
#define XM A3
#define YM 8
#define XP 9

#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define MINPRESSURE 100
#define MAXPRESSURE 1000

// LCD declarations
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BOXSIZE 40
#define PENRADIUS 2

// Keeps track of the current screen
// Home screen          1
// Main Settings        2
// Temperature Settings 3
// Time Settings        4
// Brewing screen       5
unsigned short CurrentScreen = 0;

// Kuman LCD TFT library
MCUFRIEND_kbv tft; 

// Default colour values
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	PURPLE   0x07E0
#define CYAN    0x07FF
#define GREEN 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define BACKGROUND 0X1EBC
#define BORDER 0X4179
#define TEXTONE 0xFA80
#define TEXTTWO 0x0000

// PID algorithm variables
double Temp;
double HeatTime;
double TargetTemp;
double Kp = 0.15;
double Ki = 0;
double Kd = 0;
PID TempPID(&Temp, &HeatTime, &TargetTemp, Kp, Ki, Kd, P_ON_E, DIRECT);

// Temperature tracking
#define TEMPTRACKWINDOW 300
double SSxx;
double SSxy;
double MeanTemp;
double MeanTime;
double OldTemps[2][TEMPTRACKWINDOW];
double PrevTemp;

// Program Settings
int SteamTemp = 155;
int BrewTemp = 120;
int BrewTime = 28;
int PreInfTime = 1;
int WaitTime = 2;
int PreInfBrewTime = 28;

int WindowLength = 1000;
bool PreInfuse = false;
bool Purge = false;
bool Brewing = false;
bool RegularBrew = false;

// Time tracking
unsigned long StartMillis;
unsigned long PreviousMillis;
unsigned long BrewStartMillis;
unsigned int BrewElapsedSeconds;
unsigned int WindowFrames;


// FUNCTION DECLARATIONS
// *********************
void updLinReg(double newTemp, double newMilli) {
  SSxx = SSxx - (OldTemps[0][0] * OldTemps[0][0] - 20 * MeanTime * MeanTime);
  SSxy = SSxy - (OldTemps[1][0] * OldTemps[0][0] - 20 * MeanTemp * MeanTime);

  MeanTemp = MeanTemp + (newTemp - OldTemps[1][0]) / 20;
  MeanTime = MeanTime + (newMilli - OldTemps[0][0]) / 20;

  SSxx = SSxx + (newMilli * newMilli - 20 * MeanTime * MeanTime);
  SSxy = SSxy + (newTemp * newMilli - REGULARFONTWIDTH * MeanTime * MeanTemp);

  for (int i = 0; i < TEMPTRACKWINDOW - 1; i++) {
    OldTemps[0][i] = OldTemps[0][i+1];
    OldTemps[1][i] = OldTemps[1][i+1];
  }

  OldTemps[0][TEMPTRACKWINDOW - 1] = newMilli;
  OldTemps[1][TEMPTRACKWINDOW - 1] = newTemp;
}

// compute temperature and write to Temp
void updateTemp() {
  double b0;
  double b1;
  double tempmV = analogRead(TEMP_PIN) * 5.0 / 1023.0;
  double newTemp = (tempmV * 0.98) * 100;
  updLinReg(newTemp, millis());

  b1 = SSxy / SSxx;
  b0 = MeanTemp - b1 * MeanTime;
  Temp = b1 * millis() + b0;
}
void drawCorner(float xCoord, float yCoord, int radius, float thickness, int orientation) {
  int xCentre1;
  int yCentre1;
  int xCentre2;
  int yCentre2;
  switch (orientation) {
    case 1:
      tft.fillCircle(xCoord * 32 + radius, yCoord * 32 + radius, 
                     radius, BORDER);
      tft.fillCircle(xCoord * 32 + radius, 
                     yCoord * 32 + radius, radius - thickness, BACKGROUND);
      tft.fillRect(xCoord * 32 + radius, yCoord * 32 + thickness,
                   radius + 1, 2 * radius - thickness + 1, BACKGROUND);
      tft.fillRect(xCoord * 32 + thickness, yCoord * 32 + radius,
                       radius - thickness + 1, radius + 1, BACKGROUND);
        
      break;
    case 2:
      tft.fillCircle(xCoord * 32 - radius, yCoord * 32 + radius, 
                     radius, BORDER);
      tft.fillCircle(xCoord * 32 - radius, yCoord * 32 + radius, 
                     radius - thickness, BACKGROUND);
      tft.fillRect(xCoord * 32 - 2 * radius, yCoord * 32 + thickness,
                   radius, 2 * radius - thickness + 1, BACKGROUND);
      tft.fillRect(xCoord * 32 - radius, yCoord * 32 + radius,
                   radius- thickness + 1, radius + 1, BACKGROUND);
      break;
    case 3:
      tft.fillCircle(xCoord * 32 - radius, yCoord * 32 - radius, 
                     radius, BORDER);
      tft.fillCircle(xCoord * 32 - radius, yCoord * 32 - radius, 
              radius - thickness, BACKGROUND);
      tft.fillRect(xCoord * 32 - 2 * radius, yCoord * 32 - 2 * radius,
                   radius, 2 * radius - thickness + 1, BACKGROUND);
      tft.fillRect(xCoord * 32 - radius, yCoord * 32 - 2 * radius,
                   radius- thickness + 1, radius, BACKGROUND);
      break;
    case 4:
      tft.fillCircle(xCoord * 32 + radius, yCoord * 32 - radius, 
                     radius, BORDER);
      tft.fillCircle(xCoord * 32 + radius, yCoord * 32 - radius, 
              radius - thickness, BACKGROUND);
      tft.fillRect(xCoord * 32 +  radius, yCoord * 32 - 2 * radius,
                   radius + 1, 2 * radius - thickness + 1, BACKGROUND);
      tft.fillRect(xCoord * 32 + thickness, yCoord * 32 - 2 * radius,
                   radius - thickness, radius, BACKGROUND);
      break;
  }
}

void clearButton(float xCoord, float yCoord, float width, float height, 
                     int radius, int thickness, char buttonText[], int charWidth,
                     int textLength){
  if (thickness != 0) {
    tft.fillRect(32 * xCoord + thickness, 32 * yCoord, 32 * width - 2 * thickness, thickness, BACKGROUND);
    tft.fillRect(32 * xCoord, 32 * yCoord, thickness, 32 * height, BACKGROUND);
    tft.fillRect(32 * xCoord + thickness, 32 * (yCoord + height) + 1 - thickness, 32 * width - 2 * thickness, thickness, BACKGROUND);
    tft.fillRect(32 * (xCoord + width) + 1 - thickness, 32 * yCoord, thickness, 32 * height, BACKGROUND);

    tft.fillCircle(xCoord* 32 + radius, yCoord*32 + radius, radius, BACKGROUND);
    tft.fillCircle((xCoord + width) * 32 - radius, yCoord*32 + radius, radius, BACKGROUND);
    tft.fillCircle(xCoord* 32 + radius, (yCoord + height)*32 - radius, radius, BACKGROUND);
    tft.fillCircle((xCoord + width)* 32 - radius, (yCoord + height)*32 - radius, radius, BACKGROUND);
  }
  tft.setCursor(32 * xCoord + (32 * width - charWidth*textLength)/2, 
                32 * yCoord + (32 * height + 1.1 * charWidth)/2);
  tft.setTextColor(BACKGROUND);
  tft.print(buttonText);
}
// x, y coordinates, width, height are given as coordinates for a 10x15 grid for screen,
// thickness is given in pixels
void drawRoundButton(float xCoord, float yCoord, float width, float height, 
                     int radius, int thickness, char buttonText[], int charWidth,
                     int textLength) {
  if (thickness != 0) {
      // draw outer straight lines
      tft.fillRect(32 * xCoord + radius, 32 * yCoord, 32 * width - 2 * radius, thickness, BORDER);
      tft.fillRect(32 * xCoord, 32 * yCoord + radius, thickness, 32 * height - 2 * radius, BORDER);
      tft.fillRect(32 * xCoord + radius, 32 * (yCoord + height) + 1- thickness, 32 * width - 2 * radius, thickness, BORDER);
      tft.fillRect(32 * (xCoord + width) + 1 - thickness, 32 * yCoord + radius, thickness, 32 * height - 2 * radius, BORDER);
      // draw rounded corners, breaks if thickness is 2 thicc
      drawCorner(xCoord, yCoord, radius, thickness, 1);
      drawCorner(xCoord + width, yCoord, radius, thickness, 2);
      drawCorner(xCoord + width, yCoord + height, radius, thickness, 3);
      drawCorner(xCoord, yCoord + height, radius, thickness, 4);
  }
  tft.setCursor(32 * xCoord + (32 * width - charWidth*textLength)/2, 
                32 * yCoord + (32 * height + 1.1 * charWidth)/2);
  tft.print(buttonText);
}

void drawRectButton(float xCoord, float yCoord, float width, 
                     float height, int thickness, char buttonText[], int charWidth,
                  int textLength) {
  if (thickness != 0) {
      // draw outer straight lines
      tft.fillRect(32 * xCoord + thickness, 32 * yCoord, 32 * width - 2 * thickness, thickness, BORDER);
      tft.fillRect(32 * xCoord, 32 * yCoord, thickness, 32 * height, BORDER);
      tft.fillRect(32 * xCoord + thickness, 32 * (yCoord + height) + 1 - thickness, 32 * width - 2 * thickness, thickness, BORDER);
      tft.fillRect(32 * (xCoord + width) + 1 - thickness, 32 * yCoord, thickness, 32 * height, BORDER);
  }
  tft.setCursor(32 * xCoord + (32 * width - charWidth*textLength)/2, 
                32 * yCoord + (32 * height + 1.1 * charWidth)/2);
  tft.print(buttonText);
}

void drawPlusButton(float xCoord, float yCoord, int radius) {
  tft.fillCircle(32 * xCoord, 32 * yCoord, radius, BORDER);
  tft.fillRect(32 * xCoord - 0.825 * radius, 32 * yCoord  - 0.125 * radius,
               1.75 * radius, 0.25 * radius , BACKGROUND);  
  tft.fillRect(32 * xCoord - 0.125 * radius, 32 * yCoord  - 0.825 * radius, 
               0.25 * radius, 1.75 * radius , BACKGROUND);  
}

void drawMinusButton(float xCoord, float yCoord, int radius){ 
  tft.fillCircle(32 * xCoord, 32 * yCoord, radius, BORDER);
  tft.fillRect(32 * xCoord - 0.825 * radius, 32 * yCoord  - 0.125 * radius, 
               1.75 * radius, 0.25 * radius , BACKGROUND);  
}

void drawNumber(float num, int maxLength, float xCoord, 
                   float yCoord, int charWidth, bool isInt){
  int position = 0;
  if (num <= -100) { 
    position = maxLength - 4;
  } else if (num <= -10 || num >= 100) {
    position = maxLength - 3;
  } else if (num <= -1 || num >= 10) {
    position = maxLength - 2;
  } else {
    position = maxLength - 1;
  }
  if (!isInt) {
    position = position - 2;
  }
  if (position < 0) {
      position = maxLength - 1;
      num = 0;
  }
  
  tft.setCursor(xCoord * 32 + position * charWidth, yCoord * 32 + charWidth * 1.1);
  if (isInt) {
    tft.print((int)num);
  } else {
    tft.print(num);
  }
}
void refreshNumber(float newNum, float oldNum, int maxLength, float xCoord, 
                   float yCoord, int charWidth, bool isInt){
  tft.setTextColor(BACKGROUND);
  drawNumber(oldNum, maxLength, xCoord, yCoord, charWidth, isInt);
  tft.setTextColor(TEXTTWO);
  drawNumber(newNum, maxLength, xCoord, yCoord, charWidth, isInt);
}

// minimize head to desk bashing
void dispInfo() {
  Serial.print(millis());
  Serial.print(',' );
  Serial.println(Temp);
  /*Serial.print("Purge: ");
  Serial.println(Purge);
  Serial.print("PreInfuse: ");
  Serial.println(PreInfuse);
  Serial.print("Brewing: ");
  Serial.println(Brewing);


  Serial.print("Frames per Window: ");
  Serial.println(WindowFrames);
  
  Serial.print("Brew Switch: ");
  Serial.println(digitalRead(BREW_PIN));
  Serial.print("Steam Switch: ");
  Serial.println(digitalRead(STEAM_PIN));
  Serial.print("Target Temp: ");
  Serial.println(TargetTemp);
  Serial.println();*/
}
// monitor machine switch states
void switchInput() {
  // Steam switch response
  if (digitalRead(STEAM_PIN) == 1) {
    TargetTemp = SteamTemp;
  } else {
    TargetTemp = BrewTemp;
  }
  // Brew switch response
  if (digitalRead(BREW_PIN) == 1 && BrewStartMillis == 0) {
    startBrew();
  } else if (digitalRead(BREW_PIN) == 0 && BrewStartMillis != 0 && Brewing == false) {
    endBrew();
  }
}
// check time and turn on heater / reset window
void windowRenewCheck() {
  unsigned long elapsedMillis = millis() - PreviousMillis;
  if (elapsedMillis >= WindowLength) {
    dispInfo();
    switch (CurrentScreen) {
        case 1:
        case 5:
          if ((int) Temp != (int) PrevTemp && elapsedMillis < 1.2 * WindowLength) {
              tft.setTextSize(2);
              tft.setFont(&TITLEFONT);
              refreshNumber(Temp, PrevTemp, 3, 2, 5.5, 2 * TITLEFONTWIDTH, true);
              PrevTemp = Temp;
          }
          break;
    }
    digitalWrite(HEAT_PIN, 1);
    PreviousMillis = PreviousMillis + WindowLength;
    WindowFrames = 0;
  }
}


// check to see if PID computed heater time has elapsed
// and open heater relay if so
void heaterOffCheck() {
  unsigned long elapsedMillis = millis() - PreviousMillis;
  if (elapsedMillis >= WindowLength * HeatTime) {
    digitalWrite(HEAT_PIN, 0);
  }
}
// close valve/pump relay
void brewOn() {
  digitalWrite(PUMP_PIN, 1);
  digitalWrite(VALVE_PIN, 1);
}
// open valve/pump relay
void brewOff() {
  digitalWrite(PUMP_PIN, 0);
  digitalWrite(VALVE_PIN, 0);
}
// start brew and initialize brew timer
void startBrew() {
  drawBrewingScreen();
  BrewStartMillis = millis();
  brewOn();
}
// stop brew and reset brew timer
void endBrew() {
  brewOff();
  PreInfuse = false;
  Brewing = false;
  Purge = false;
  RegularBrew = false;
  delay(500);
  drawHomeScreen();
  BrewStartMillis = 0;
  BrewElapsedSeconds = 0;
}
// controls timing for brew
void brewCheck() {
  unsigned long elapsedTime = (millis() - BrewStartMillis) / 1000;
  if (Purge) { 
      if (elapsedTime >= 2){
          endBrew();
      }
  } 
  // pre infusion timing
  else if (PreInfuse) {
    if (elapsedTime >= PreInfTime + WaitTime + BrewTime) {
      endBrew();
    } else if (elapsedTime >= PreInfTime + WaitTime) {
      brewOn();
    } else if (elapsedTime >= PreInfTime) {
      brewOff();
    }
  } 
  // Regular brew timing 
  else if (RegularBrew){ // Regular brew timing
    if (elapsedTime >= BrewTime) {
      endBrew();
    }
  }
}
void shotTimerRenewCheck() {
  unsigned int elapsedTime = (millis() - BrewStartMillis) / 1000;
  if (BrewStartMillis != 0 && elapsedTime > BrewElapsedSeconds){
    BrewElapsedSeconds++;
    tft.setFont(&TITLEFONT);
    tft.setTextSize(2);
    refreshNumber(BrewElapsedSeconds, BrewElapsedSeconds-1,
                     3, 0.5, 9.5, TITLEFONTWIDTH * 2, true);
  }
}

// check for time dependent function calls
void timeChecks() {
  windowRenewCheck();
  shotTimerRenewCheck();
  heaterOffCheck();
  brewCheck();
}

void titleText(char* title, int titleLen) {
  tft.setTextSize(1);
  tft.setTextColor(TEXTONE);
  tft.setFont(&TITLEFONT);
  drawRoundButton(0,0,10,3,0,0,title, TITLEFONTWIDTH, titleLen);
  tft.setFont(&REGULARFONT);
}

void clearTitle(char* title, int titleLen) {
  tft.setTextSize(1);
  tft.setFont(&TITLEFONT);
  clearButton(0,0,10,3,0,0,title, TITLEFONTWIDTH, titleLen);
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

void clearSettingsHome() {
  clearTitle("Settings", 8);
  tft.setFont(&REGULARFONT);
  tft.setTextSize(1);
  clearButton(0.5, 4, 9, 2.5, 16, 4, "Temperature", REGULARFONTWIDTH, 11);
  clearButton(0.5, 7, 9, 2.5, 16, 4, "Brew Time", REGULARFONTWIDTH, 9);
  clearButton(0.5, 10, 9, 2.5, 16, 4, "Set PID", REGULARFONTWIDTH, 7);
  clearButton(5, 13, 4.5, 1.5, 0, 2, "Home", REGULARFONTWIDTH, 4);
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

  drawNumber(BrewTemp, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  
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

  drawNumber(SteamTemp, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  drawRectButton(5, 13, 4.5, 1.5, 2, "Home", REGULARFONTWIDTH, 4);
  drawRectButton(0.5, 13, 4.5, 1.5, 2, "Return", REGULARFONTWIDTH, 6);
}

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

  drawNumber(BrewTemp, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  
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

  drawNumber(SteamTemp, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);

  tft.setTextSize(1);
  tft.setFont(&REGULARFONT);
  clearButton(5, 13, 4.5, 1.5, 0, 2, "Home", REGULARFONTWIDTH, 4);
  clearButton(0.5, 13, 4.5, 1.5, 0, 2, "Return", REGULARFONTWIDTH, 6);
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
  drawNumber(BrewTime, 2, 5.5, 5.25, REGULARFONTWIDTH, true);

  tft.setCursor(24, 6.75*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Preinfusion");

  tft.setCursor(24, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Pre: ");
  
  drawMinusButton(4.5, 8.375, 16);
  drawPlusButton(8, 8.375, 16);

  tft.setCursor(6.75*32, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(PreInfTime, 2, 5.5, 8, REGULARFONTWIDTH, true);
  
  tft.setCursor(24, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Wait: ");
  
  drawMinusButton(4.5, 9.5, 16);
  drawPlusButton(8, 9.5, 16);

  tft.setCursor(6.75*32, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(WaitTime, 2, 5.5, 9.125, REGULARFONTWIDTH, true);

  tft.setCursor(24, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Brew: ");
  
  drawMinusButton(4.5, 10.625, 16);
  drawPlusButton(8, 10.625, 16);

  tft.setCursor(6.75*32, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(PreInfBrewTime,  2, 5.5, 10.25, REGULARFONTWIDTH, true);

  drawRectButton(5, 13, 4.5, 1.5, 2, "Home", REGULARFONTWIDTH, 4);
  drawRectButton(0.5, 13, 4.5, 1.5, 2, "Return", REGULARFONTWIDTH, 6);
}
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
  drawNumber(BrewTime,  2, 5.5, 5.25, REGULARFONTWIDTH, true);

  tft.setCursor(24, 6.75*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Preinfusion");

  tft.setCursor(24, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Pre: ");
  
  tft.fillCircle(4.5*32, 8.375*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 8.375*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 8*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(PreInfTime, 2, 5.5, 8, REGULARFONTWIDTH, true);
  
  tft.setCursor(24, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Wait: ");
  
  tft.fillCircle(4.5*32, 9.5*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 9.5*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 9.125*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(WaitTime, 2, 5.5, 9.125, REGULARFONTWIDTH, true);

  tft.setCursor(24, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("Brew: ");
  
  tft.fillCircle(4.5*32, 10.625*32, 16, BACKGROUND);
  tft.fillCircle(8*32, 10.625*32, 16, BACKGROUND);

  tft.setCursor(6.75*32, 10.25*32 + 1.1 * REGULARFONTWIDTH);
  tft.print("s");
  drawNumber(PreInfBrewTime, 2, 5.5, 10.25, REGULARFONTWIDTH, true);

  clearButton(5, 13, 4.5, 1.5, 0, 2, "Home", REGULARFONTWIDTH, 4);
  clearButton(0.5, 13, 4.5, 1.5, 0, 2, "Return", REGULARFONTWIDTH, 6);
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

void clearBrewingScreen() {
  tft.setTextSize(1);
  tft.setTextColor(BACKGROUND);
  tft.setFont(&REGULARFONT);
  clearButton(0.5, 8, 0, 1.1 * REGULARFONTWIDTH/ 32.0, 0, 0, "Shot Timer:", REGULARFONTWIDTH, 0);

  tft.setFont(&TITLEFONT);
  tft.setTextSize(2);
  drawNumber(BrewElapsedSeconds, 3, 0.5, 9.5, TITLEFONTWIDTH * 2, true);
  tft.setCursor(0.5 * 32 + TITLEFONTWIDTH * 2 * 3, 9.5*32 + 2 * TITLEFONTWIDTH * 1.1);
  tft.print("s");

  tft.fillCircle(5*32, 13.25*32, 1.5*32, BACKGROUND);
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
}

void clearHomeScreen(int newScreen) {
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
    drawNumber(PrevTemp, 3, 2, 5.5, 2 * TITLEFONTWIDTH, true);
    tft.setCursor(7.5*32, 5.5*32 + 1.1 * 2 * TITLEFONTWIDTH);
    tft.fillCircle(7.375*32, 5.5*32 + 5, 8, BACKGROUND);
    tft.print("C");
  }
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


// Touch handling (lol)
bool touchInButton(TSPoint p, float xCoord, float yCoord, float width, float height) {
  float touchX = 10 - (p.y / 32.0 - 3.5) * (10.0 / 25.1);
  float touchY = (p.x / 32.0 - 2.3) * (15.0 / 26.65);
  return (touchX >= xCoord && touchX <= xCoord + width &&
        touchY >= yCoord && touchY <= yCoord + height); 
}
bool touchInCircle(TSPoint p, float xCoord, float yCoord, int radius) {
  return touchInButton(p, xCoord - radius / 32.0, yCoord - radius / 32.0,
                       2 * radius / 32.0, 2 * radius / 32.0);
}
void handleHomeTouch(TSPoint p) {
  if (touchInButton(p, 0.5, 8, 4.5, 2.75)) {
      Brewing = true;
      Purge = false;
      PreInfuse = false;
      RegularBrew = true;
      startBrew();
  } else if (touchInButton(p, 5, 8, 4.5, 2.75)) {
      Brewing = true;
      Purge = false;
      PreInfuse = true;
      RegularBrew = false;
      startBrew();
  } else if (touchInButton(p, 0.5, 10.75, 4.5, 2.75)) {
      Brewing = true;
      Purge = true;
      PreInfuse = false;
      RegularBrew = false;
      startBrew();
  } else if (touchInButton(p, 5, 10.75, 4.5, 2.75)) {
      drawSettingsHome();
  }
}
void handleSettingsTouch(TSPoint p) {
  if (touchInButton(p, 0.5, 4, 9, 2.5)) {
      drawSettingsTemp();
  } else if (touchInButton(p, 0.5, 7, 9, 2.5)) {
      drawSettingsTime();
  } else if (touchInButton(p, 0.5, 10, 9, 2.5)) {
      Serial.println("PID Settings button press");
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
      drawHomeScreen();
  }
}
void handleTemperatureTouch(TSPoint p) {
  tft.setFont(&REGULARFONT);
  tft.setTextSize(2);
  if (touchInCircle(p, 1.25, 6.5, 32)) {
    BrewTemp--;
    refreshNumber(BrewTemp, BrewTemp + 1, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8.75, 6.5, 32)) {
    BrewTemp++;
    refreshNumber(BrewTemp, BrewTemp - 1, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 1.25, 10.5, 32)) {
    SteamTemp--;
    refreshNumber(SteamTemp, SteamTemp + 1, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8.75, 10.5, 32)) {
    SteamTemp++;
    refreshNumber(SteamTemp, SteamTemp - 1, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
    drawHomeScreen();
  } else if (touchInButton(p, 0.5, 13, 4.5, 1.5)) {
    drawSettingsHome();
  }
}
void handleTimeTouch(TSPoint p) {
  tft.setFont(&REGULARFONT);
  tft.setTextSize(1);
  tft.setTextColor(TEXTTWO);
  if (touchInCircle(p, 4.5, 5.625, 16)) {
      BrewTime--;
      refreshNumber(BrewTime, BrewTime + 1, 2, 5.5, 5.25, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 5.625, 16)) {
      BrewTime++;
      refreshNumber(BrewTime, BrewTime - 1, 2, 5.5, 5.25, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 8.375, 16)) {
      PreInfTime--;
      refreshNumber(PreInfTime, PreInfTime + 1, 2, 5.5, 8, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 8.375, 16)) {
      PreInfTime++;
      refreshNumber(PreInfTime, PreInfTime - 1, 2, 5.5, 8, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 9.125, 16)) {
      WaitTime--;
      refreshNumber(WaitTime, WaitTime + 1, 2, 5.5, 9.125, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 9.125, 16)) {
      WaitTime++;
      refreshNumber(WaitTime, WaitTime - 1, 2, 5.5, 9.125, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 10.625, 16)) {
      PreInfBrewTime--;
      refreshNumber(PreInfBrewTime, PreInfBrewTime + 1, 2, 5.5, 10.25, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 10.625, 16)) {
      PreInfBrewTime++;
      refreshNumber(PreInfBrewTime, PreInfBrewTime - 1, 2, 5.5, 10.25, REGULARFONTWIDTH, true);
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
      drawHomeScreen();
  } else if (touchInButton(p, 0.5, 13, 4.5, 1.5)) {
      drawSettingsHome();
  }
}
void handleBrewingTouch(TSPoint p) {
  if (touchInCircle(p, 5, 13.25, 1.5*32)) {
    Serial.println("Ending brew");
    endBrew();
  }
}
void touchInput(TSPoint p) {
  float touchX = 10 - (p.y / 32.0 - 3.5) * (10.0 / 25.1);
  float touchY = (p.x / 32.0 - 2.3) * (15.0 / 26.65);
  //tft.fillCircle(touchX * 32, touchY * 32, 5, GREEN);
  switch (CurrentScreen) {
    case 1:
      handleHomeTouch(p);
      break;
    case 2:
      handleSettingsTouch(p);
      break;
    case 3:
      handleTemperatureTouch(p);
      break;
    case 4:
      handleTimeTouch(p);
      break;
    case 5:
      handleBrewingTouch(p);
      break;
  }
  delay(120);
}


// MAIN PROGRAM CALLS
// ******************
void setup() {
  Serial.begin(9600);

  // initialize screen
  tft.reset();
  uint16_t identifier = 0x9486;
  tft.begin(identifier);
  tft.setTextWrap(false);
  tft.fillScreen(BACKGROUND);
  tft.setFont(&REGULARFONT);
  tft.fillRect(0.5*32, 3*32, 9*32, 4, BORDER);

  Temp = 0;
  PrevTemp = 0;
  drawHomeScreen();

  // Pin Initializations
  // thermocouple pin
  pinMode(TEMP_PIN, INPUT);
  // machine switch pins
  pinMode(STEAM_PIN, INPUT);
  pinMode(BREW_PIN, INPUT);
  // relay pins
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);

  // start timing
  PreviousMillis = millis();
  StartMillis = millis();
  BrewStartMillis = 0;
  BrewElapsedSeconds = 0;

  // PID settings
  TempPID.SetOutputLimits(0, 1);
  TempPID.SetSampleTime(WindowLength);
  TempPID.SetMode(AUTOMATIC);
}


void loop() {
  // read touch screen and reset pins
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      touchInput(p);
  }
  WindowFrames++;
  switchInput();
  updateTemp();
  TempPID.Compute();
  timeChecks();
}

