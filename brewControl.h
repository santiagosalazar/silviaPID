#ifndef BREWCONTROL_h
#define BREWCONTROL_h

#include </home/santiago/Arduino/silviaPID/screen.h>

struct brewState {
    bool isBrewing;
    bool modeRegBrew;
    bool modePreInfuse;
    bool modePurge;
};
brewState *currState;

void brewOn();
void brewOff();
void startBrew();
void endBrew();

void initBrewState(brewState *iniState){
    iniState->isBrewing = false;
    iniState->modeRegBrew = false;
    iniState->modePreInfuse = false;
    iniState->modePurge = false;
    currState = iniState;
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
  if (!currState->modePurge){
      drawBrewingScreen();
  }
  time->brewStartMillis = millis();
  Serial.println("Commence brewing!!!");
  brewOn();
}
// stop brew and reset brew timer
void endBrew() {
  brewOff();
  delay(500);
  if  (!currState->modePurge) {
      drawHomeScreen();
  }
  currState->isBrewing = false;
  currState->modePreInfuse = false;
  currState->modePurge = false;
  currState->modeRegBrew = false;
  time->brewStartMillis = 0;
  time->brewElapsedSec = 0;
}

#endif
