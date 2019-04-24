#include <Time.h>
#include <DS1302RTC.h>
#include </home/santiago/Arduino/silviaPID/silviaPID.h>
#include <PID_v1.h>
#include </home/santiago/Arduino/silviaPID/screen.h>
#include </home/santiago/Arduino/silviaPID/temperature.h>
#include </home/santiago/Arduino/silviaPID/touch.h>
#include </home/santiago/Arduino/silviaPID/brewControl.h>


// RTC Clock declaration RTC(CE, IO, CLK)
#define RTC_CE 43
#define RTC_IO 41
#define RTC_CLK 39
DS1302RTC RTC(RTC_CE,RTC_IO,RTC_CLK);

// Touchscreen declaration
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Temp tracking
tempTrack Temp;

// Time tracking
timeTrack Time;
unsigned int WindowFrames;

// Brew tracking
brewState CurrState;

// Program Settings
sproSettings Sett;


// PID algorithm variables
double HeatTime;
double TargetTemp;
double Ku = 0.3;
double Tu = 0;//127;
/*double Kp = 0.6 * Ku;
double Ki = Tu / 1.2;
double Kd = Tu / 16;*/
double Kp = 0.1;
double Ki = 0;
double Kd = 0;
PID TempPID(&Temp.approxTemp, &HeatTime, &TargetTemp, Kp, Ki, Kd, P_ON_E, DIRECT);

// FUNCTION DECLARATIONS
// *********************
// print data to serial
void dispInfo() {
  //Serial.print("Time of info query: ");
  /*Serial.println(millis());
  Serial.print("Approx temp: ");
  Serial.println(Temp.approxTemp);
  Serial.print("Mean voltage: ");
  Serial.println(Temp.meanVoltRead);
  Serial.print("Temperature pin reading: ");
  Serial.println(analogRead(TEMP_PIN));
  Serial.print("heat time: ");
  Serial.println(HeatTime);
  Serial.println("Relay states: ");
  Serial.println(digitalRead(HEAT_PIN));
  Serial.println(digitalRead(PUMP_PIN));
  Serial.println(digitalRead(VALVE_PIN));

  Serial.print("CurrentScreen: ");
  Serial.println(CurrentScreen);
  Serial.println();*/
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
  } else if (digitalRead(BREW_PIN) == 0 && Time.brewStartMillis != 0 && CurrState.isBrewing == false) {
    endBrew();
  }
}
// controls heater state
void heaterCheck() {
  unsigned long elapsedMillis = millis() - Time.windowStartMillis;
  // starts heater if the machine is brewing
  // disabled for now since affects temperature stability with long shot times.
  bool isLoseHeat = false;// CurrState.isBrewing||(digitalRead(BREW_PIN)==1);

  if ((HeatTime!= 0 && elapsedMillis >= Sett.windowLength)|| isLoseHeat){
    digitalWrite(HEAT_PIN, 1);
    Time.windowStartMillis = Time.windowStartMillis + Sett.windowLength;
    WindowFrames = 0;
  }
  else if (elapsedMillis >= Sett.windowLength * HeatTime) {
    digitalWrite(HEAT_PIN, 0);
  }
}

// Check to see if screen needs to be updated and does so if necessary
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

// controls timing for brew
void brewCheck() {
  unsigned long elapsedMillis = millis() - Time.brewStartMillis;
  if (CurrState.modePurge) { 
      if (elapsedMillis >= 2000){
          endBrew();
      }
  } 
  // pre infusion timing
  else if (CurrState.modePreInfuse) {
    if (elapsedMillis >= Sett.preInfMillis + Sett.waitMillis + Sett.brewMillis) {
      endBrew();
    } else if (elapsedMillis >= Sett.preInfMillis + Sett.waitMillis) {
      brewOn();
    } else if (elapsedMillis >= Sett.preInfMillis) {
      brewOff();
    }
  } 
  // Regular brew timing 
  else if (CurrState.modeRegBrew){
    if (elapsedMillis >= Sett.brewMillis) {
      endBrew();
    }
  }
}
void renewShotTimerCheck() {
  unsigned int elapsedSeconds = (millis() - Time.brewStartMillis) / 1000;
  if (Time.brewStartMillis != 0 && elapsedSeconds > Time.brewElapsedSec && !CurrState.modePurge){
    Time.brewElapsedSec++;
    tft.setFont(&TITLEFONT);
    tft.setTextSize(2);
    refreshNumber(Time.brewElapsedSec, Time.brewElapsedSec-1,
                     3, 0.5, 9.5, TITLEFONTWIDTH * 2, true);
  }
}

// Checks to see if enough time has passed since last temp read
void tempCheck() {
  unsigned long elapsedMillis = millis() - Temp.prevTempRead;
  if (elapsedMillis > (float) Temp.tempWindow / TEMP_TRACK_POINTS) {
      updateTemp(&Temp);
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

// MAIN PROGRAM CALLS
// ******************
void setup() {
  Serial.begin(9600);
  initSettings(&Sett);
  initTempTrack(&Temp);
  initTimeTrack(&Time);
  initBrewState(&CurrState);

  // initialize screen
  tft.reset();
  uint16_t identifier = 0x9486;
  tft.begin(identifier);
  tft.setTextWrap(false);
  tft.setRotation(4);
  tft.fillScreen(BACKGROUND);
  tft.setFont(&REGULARFONT);
  tft.fillRect(0.5*32, 3*32, 9*32, 4, BORDER);

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
  pinMode(13, OUTPUT);

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

  touchInput(p);

  WindowFrames++;
  switchInput();
  TempPID.Compute();
  timeChecks();
}

