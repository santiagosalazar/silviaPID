#include <Time.h>
#include <DS1302RTC.h>
#include </home/santiago/Arduino/silviaPID/silviaPID.h>
#include <PID_v1.h>
#include </home/santiago/Arduino/silviaPID/screen.h>

// Touchscreen declaration
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// RTC Clock declaration RTC(CE, IO, CLK)
#define RTC_CE 43
#define RTC_IO 41
#define RTC_CLK 39
DS1302RTC RTC(RTC_CE,RTC_IO,RTC_CLK);

// PID algorithm variables
tempTrack Temp;
double HeatTime;
double TargetTemp;
double Ku = 0.3;
double Tu = 0;//127;
double Kp = 0.6 * Ku;
double Ki = Tu / 1.2;
//double Kd = Tu / 16;
/*double Kp = 0.3;
double Ki = 0;*/
double Kd = 0;
PID TempPID(&Temp.approxTemp, &HeatTime, &TargetTemp, Kp, Ki, Kd, P_ON_E, DIRECT);

// Program Settings
sproSettings Sett;

// Brew settings
bool PreInfuse = false;
bool Purge = false;
bool Brewing = false;
bool RegularBrew = false;

// Time tracking
timeTrack Time;
unsigned int WindowFrames;


// FUNCTION DECLARATIONS
// *********************
void updLinReg(double newTemp, double newMilli) {
  Temp.ssxx = Temp.ssxx - (Temp.instTempLog[0][0] * Temp.instTempLog[0][0] - 
                     20 * Temp.meanTime * Temp.meanTime);
  Temp.ssxy = Temp.ssxy - (Temp.instTempLog[1][0] * Temp.instTempLog[0][0] - 
                     20 * Temp.meanTemp * Temp.meanTime);

  Temp.meanTemp = Temp.meanTemp + (newTemp - Temp.instTempLog[1][0]) / 20;
  Temp.meanTime = Temp.meanTime + (newMilli - Temp.instTempLog[0][0]) / 20;

  Temp.ssxx = Temp.ssxx + newMilli * newMilli - 
                     20 * Temp.meanTime * Temp.meanTime;
  Temp.ssxy = Temp.ssxy + newTemp * newMilli - 
                     20 * Temp.meanTime * Temp.meanTemp;

  for (int i = 0; i < TEMP_TRACK_POINTS - 1; i++) {
    Temp.instTempLog[0][i] = Temp.instTempLog[0][i+1];
    Temp.instTempLog[1][i] = Temp.instTempLog[1][i+1];
  }

  Temp.instTempLog[0][TEMP_TRACK_POINTS - 1] = newMilli;
  Temp.instTempLog[1][TEMP_TRACK_POINTS - 1] = newTemp;
}

// compute temperature and write to Temp
void updateTemp() {
  double b0;
  double b1;
  double tempmV = analogRead(TEMP_PIN) * 5.0 / 1023.0;
  double newTemp = (tempmV * 0.98) * 100;
  updLinReg(newTemp, millis());

  b1 = Temp.ssxy / Temp.ssxx;
  b0 = Temp.meanTemp - b1 * Temp.meanTime;
  Temp.approxTemp = b1 * millis() + b0;
}
// print data to serial
void dispInfo() {
  Serial.print(millis());
  Serial.print(',' );
  Serial.print(Temp.approxTemp);
  //Serial.print(',' );
  //Serial.print(CurrentScreen);
  Serial.println();
}

// monitor machine switch states
void switchInput() {
  // Steam switch response
  if (digitalRead(STEAM_PIN) == 1) {
    TargetTemp = Sett.steamTemp;
  } else {
    TargetTemp = Sett.brewTemp;
  }
  // Brew switch response
  if (digitalRead(BREW_PIN) == 1 && Time.brewStartMillis == 0) {
    startBrew();
  } else if (digitalRead(BREW_PIN) == 0 && Time.brewStartMillis != 0 && Brewing == false) {
    endBrew();
  }
}
// controls heater state
void heaterCheck() {
  unsigned long elapsedMillis = millis() - Time.windowStartMillis;
  if (elapsedMillis >= Sett.windowLength) {
    digitalWrite(HEAT_PIN, 1);
    Time.windowStartMillis = Time.windowStartMillis + Sett.windowLength;
    WindowFrames = 0;
  }
  if (elapsedMillis >= Sett.windowLength * HeatTime) {
    digitalWrite(HEAT_PIN, 0);
  }
}

void renewFrameCheck() {
  unsigned long elapsedMillis = millis() - Time.frameStartMillis;
  if (elapsedMillis >= Sett.frameLength) {
    dispInfo();
    switch (CurrentScreen) {
        case 5:
        case 1:
          if ((int) Temp.approxTemp != Temp.dispTemp && Temp.approxTemp >= 0) {
              tft.setTextSize(2);
              tft.setFont(&TITLEFONT);
              refreshNumber(Temp.approxTemp, Temp.dispTemp, 3, 2, 5.5, 2 * TITLEFONTWIDTH, true);
              if (Temp.approxTemp >= 0) {
                  Temp.dispTemp = (unsigned int) Temp.approxTemp;
              }
          } else if(Temp.approxTemp < 0){
              Temp.dispTemp = 0;
          }
          break;
    }
    Time.frameStartMillis = Time.frameStartMillis + Sett.frameLength;
    WindowFrames = 0;
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
  if (!Purge){
      drawBrewingScreen(Temp, Time, Sett);
      Time.brewStartMillis = millis();
  }
  brewOn();
}
// stop brew and reset brew timer
void endBrew() {
  brewOff();
  delay(500);
  if  (!Purge) {
      drawHomeScreen(Temp, Time, Sett);
  }
  PreInfuse = false;
  Brewing = false;
  Purge = false;
  RegularBrew = false;
  Time.brewStartMillis = 0;
  Time.brewElapsedSec = 0;
}
// controls timing for brew
void brewCheck() {
  unsigned long elapsedMillis = millis() - Time.brewStartMillis;
  if (Purge) { 
      if (elapsedMillis >= 2000){
          endBrew();
      }
  } 
  // pre infusion timing
  else if (PreInfuse) {
    if (elapsedMillis >= Sett.preInfMillis + Sett.waitMillis + Sett.brewMillis) {
      endBrew();
    } else if (elapsedMillis >= Sett.preInfMillis + Sett.waitMillis) {
      brewOn();
    } else if (elapsedMillis >= Sett.preInfMillis) {
      brewOff();
    }
  } 
  // Regular brew timing 
  else if (RegularBrew){ // Regular brew timing
    if (elapsedMillis >= Sett.brewMillis) {
      endBrew();
    }
  }
}
void renewShotTimerCheck() {
  unsigned int elapsedSeconds = (millis() - Time.brewStartMillis) / 1000;
  if (Time.brewStartMillis != 0 && elapsedSeconds > Time.brewElapsedSec){
    Time.brewElapsedSec++;
    tft.setFont(&TITLEFONT);
    tft.setTextSize(2);
    refreshNumber(Time.brewElapsedSec, Time.brewElapsedSec-1,
                     3, 0.5, 9.5, TITLEFONTWIDTH * 2, true);
  }
}

void tempCheck() {
  unsigned long elapsedMillis = millis() - Temp.prevTempRead;
  if (elapsedMillis > Temp.tempWindow / TEMP_TRACK_POINTS) {
      updateTemp();
      Temp.prevTempRead = millis();
  }
}

// check for time dependent function calls
void timeChecks() {
  heaterCheck();
  renewFrameCheck();
  tempCheck();
  renewShotTimerCheck();
  brewCheck();
}



// Touch handling (lol)
bool touchInButton(TSPoint p, float xCoord, float yCoord, float width, float height) {
  float touchX = (p.y / 32.0 - 3.5) * (10.0 / 25.1);
  float touchY = 15 -(p.x / 32.0 - 2.3) * (15.0 / 26.65);
  return (touchX >= xCoord && touchX <= xCoord + width &&
        touchY >= yCoord && touchY <= yCoord + height); 
}
bool touchInCircle(TSPoint p, float xCoord, float yCoord, int radius) {
  return touchInButton(p, xCoord - radius / 32.0, yCoord - radius / 32.0,
                       2 * radius / 32.0, 2 * radius / 32.0);
}
void handleHomeTouch(TSPoint p) {
  if (touchInButton(p, 0.5, 10.75, 4.5, 2.75)) {
      Serial.println("Purge");
      Brewing = false;
      Purge = true;
      PreInfuse = false;
      RegularBrew = true;
      startBrew();
  } else if (touchInButton(p, 0.5, 8, 4.5, 2.75)) {
      Serial.println("Brew");
      Brewing = true;
      Purge = false;
      PreInfuse = true;
      RegularBrew = false;
      startBrew();
  } else if (touchInButton(p, 5, 8, 4.5, 2.75)) {
      Serial.println("Preinf");
      Brewing = true;
      Purge = false;
      PreInfuse = true;
      RegularBrew = false;
      startBrew();
  } else if (touchInButton(p, 5, 10.75, 4.5, 2.75)) {
      Serial.println("Settings touched");
      drawSettingsHome(Temp, Time, Sett);
  }
}
void handleSettingsTouch(TSPoint p) {
  if (touchInButton(p, 0.5, 4, 9, 2.5)) {
      drawSettingsTemp(Temp, Time, Sett);
  } else if (touchInButton(p, 0.5, 7, 9, 2.5)) {
      drawSettingsTime(Temp, Time, Sett);
  } else if (touchInButton(p, 0.5, 10, 9, 2.5)) {
      Serial.println("PID Settings button press");
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
      drawHomeScreen(Temp, Time, Sett);
  }
}
void handleTemperatureTouch(TSPoint p) {
  tft.setFont(&REGULARFONT);
  tft.setTextSize(2);
  if (touchInCircle(p, 1.25, 6.5, 32)) {
    Sett.brewTemp--;
    refreshNumber(Sett.brewTemp, Sett.brewTemp + 1, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8.75, 6.5, 32)) {
    Sett.brewTemp++;
    refreshNumber(Sett.brewTemp, Sett.brewTemp - 1, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 1.25, 10.5, 32)) {
    Sett.steamTemp--;
    refreshNumber(Sett.steamTemp, Sett.steamTemp + 1, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8.75, 10.5, 32)) {
    Sett.steamTemp++;
    refreshNumber(Sett.steamTemp, Sett.steamTemp - 1, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
    drawHomeScreen(Temp, Time, Sett);
  } else if (touchInButton(p, 0.5, 13, 4.5, 1.5)) {
    drawSettingsHome(Temp, Time, Sett);
  }
}
void handleTimeTouch(TSPoint p) {
  tft.setFont(&REGULARFONT);
  tft.setTextSize(1);
  tft.setTextColor(TEXTTWO);
  if (touchInCircle(p, 4.5, 5.625, 16)) {
      Sett.brewMillis= Sett.brewMillis - 1000;
      refreshNumber(Sett.brewMillis / 1000, Sett.brewMillis / 1000 + 1, 2, 5.5, 5.25, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 5.625, 16)) {
      Sett.brewMillis = Sett.brewMillis - 1000;
      refreshNumber(Sett.brewMillis / 1000, Sett.brewMillis / 1000 - 1, 2, 5.5, 5.25, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 8.375, 16)) {
      Sett.preInfMillis = Sett.preInfMillis - 1000;
      refreshNumber(Sett.preInfMillis / 1000, Sett.preInfMillis / 1000 + 1, 2, 5.5, 8, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 8.375, 16)) {
      Sett.preInfMillis = Sett.preInfMillis + 1000;
      refreshNumber(Sett.preInfMillis / 1000, Sett.preInfMillis / 1000 - 1, 2, 5.5, 8, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 9.125, 16)) {
      Sett.waitMillis = Sett.waitMillis - 1000;
      refreshNumber(Sett.waitMillis / 1000, Sett.waitMillis / 1000 + 1, 2, 5.5, 9.125, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 9.125, 16)) {
      Sett.waitMillis = Sett.waitMillis + 1000;
      refreshNumber(Sett.waitMillis / 1000, Sett.waitMillis / 1000 - 1, 2, 5.5, 9.125, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 10.625, 16)) {
      Sett.preInfBrewMillis = Sett.preInfBrewMillis - 1000;
      refreshNumber(Sett.preInfBrewMillis / 1000, Sett.preInfBrewMillis / 1000 + 1, 2, 5.5, 10.25, REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 10.625, 16)) {
      Sett.preInfBrewMillis = Sett.preInfBrewMillis - 1000;
      refreshNumber(Sett.preInfBrewMillis / 1000, Sett.preInfBrewMillis / 1000 - 1, 2, 5.5, 10.25, REGULARFONTWIDTH, true);
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
      drawHomeScreen(Temp, Time, Sett);
  } else if (touchInButton(p, 0.5, 13, 4.5, 1.5)) {
      drawSettingsHome(Temp, Time, Sett);
  }
}
void handleBrewingTouch(TSPoint p) {
  if (touchInCircle(p, 5, 13.25, 1.5*32)) {
    Serial.println("Ending brew");
    endBrew();
  }
}
void touchInput(TSPoint p) {
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

  // default settings
  Sett.brewTemp = 120;
  Sett.steamTemp = 155;
  Sett.brewMillis = 28000;
  Sett.preInfMillis = 1000;
  Sett.waitMillis = 2000;
  Sett.preInfBrewMillis = 28000;
  Sett.purgeMillis = 2000;
  Sett.windowLength = 1000;
  Sett.frameLength = 1000;
  Temp.tempWindow = 1000;

  // initialize screen
  tft.reset();
  uint16_t identifier = 0x9486;
  tft.begin(identifier);
  tft.setTextWrap(false);
  tft.setRotation(2);
  tft.fillScreen(BACKGROUND);
  tft.setFont(&REGULARFONT);
  tft.fillRect(0.5*32, 3*32, 9*32, 4, BORDER);

  drawHomeScreen(Temp, Time, Sett);

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
  Time.windowStartMillis = millis();
  Time.frameStartMillis = millis();
  Time.brewStartMillis = 0;
  Time.brewElapsedSec = 0;
  Temp.prevTempRead = millis();


  // start clock
  if (RTC.haltRTC()) {
     Serial.println("The DS1302 is stopped.  Please run the SetTime");
     Serial.println("example to initialize the time and begin running.");
  }
  if (!RTC.writeEN()) {
      Serial.println("The DS1302 is write protected. This normal.");
  }

// PID settings
  TempPID.SetOutputLimits(0, 1);
  TempPID.SetSampleTime(Sett.windowLength);
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
  TempPID.Compute();
  timeChecks();
}

