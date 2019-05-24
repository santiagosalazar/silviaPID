#ifndef TOUCH_H
#define TOUCH_H

// Touch screen parameters
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930
#define MIN_PRESSURE 200
#define MAX_PRESSURE 1000


#include <TouchScreen.h>
#include "silviaPID.h"
#include "brewControl.h"
#include "screen.h"


// FUNCTION DECLARATIONS
void touchInput(TSPoint p);
void handleTouch(TSPoint p);
void handleHomeTouch(TSPoint);
void handleSettingsTouch(TSPoint);
void handleTemperatureTouch(TSPoint);
void handleTimeTouch(TSPoint);
void handleBrewingTouch(TSPoint);
bool touchInButton(TSPoint, float, float, float, float);
bool touchInCircle(TSPoint p, float, float, int);

// Touchscreen declaration
TouchScreen Ts = TouchScreen(XP, YP, XM, YM, 300);

// Trigger touch action if pressure exceeds threshold
void touchInput(TSPoint p) {
  if (p.z > MIN_PRESSURE && p.z < MAX_PRESSURE) {
      handleTouch(p);
  }
}

// Delegate touch input according to current screen
void handleTouch(TSPoint p) {
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
}

// Manage touch on home screen buttons
void handleHomeTouch(TSPoint p) {
  if (touchInButton(p, 0.5, 10.75, 4.5, 2.75)) {
      currState->isBrewing = true;
      currState->modePurge = true;
      currState->modePreInfuse = false;
      currState->modeRegBrew = false;
      startBrew();
  } else if (touchInButton(p, 0.5, 8, 4.5, 2.75)) {
      currState->isBrewing = true;
      currState->modePurge = false;
      currState->modePreInfuse = false;
      currState->modeRegBrew = true;
      startBrew();
  } else if (touchInButton(p, 5, 8, 4.5, 2.75)) {
      currState->isBrewing = true;
      currState->modePurge = false;
      currState->modePreInfuse = true;
      currState->modeRegBrew = false;
      startBrew();
  } else if (touchInButton(p, 5, 10.75, 4.5, 2.75)) {
      drawSettingsHome();
  }
}
// Manage touch on settings screen buttons
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

// Manage touch on temperature settings screen
void handleTemperatureTouch(TSPoint p) {
  tft.setFont(&REGULARFONT);
  tft.setTextSize(2);
  if (touchInCircle(p, 1.25, 6.5, 32)) {
    settings->brewTemp--;
    refreshNumber(settings->brewTemp, settings->brewTemp + 1, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8.75, 6.5, 32)) {
    settings->brewTemp++;
    refreshNumber(settings->brewTemp, settings->brewTemp - 1, 3, 2, 5.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 1.25, 10.5, 32)) {
    settings->steamTemp--;
    refreshNumber(settings->steamTemp, settings->steamTemp + 1, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8.75, 10.5, 32)) {
    settings->steamTemp++;
    refreshNumber(settings->steamTemp, settings->steamTemp - 1, 3, 2, 9.75, 2 * REGULARFONTWIDTH, true);
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
    drawHomeScreen();
  } else if (touchInButton(p, 0.5, 13, 4.5, 1.5)) {
    drawSettingsHome();
  }
}

// Manage touch on brew timing settings screen
void handleTimeTouch(TSPoint p) {
  tft.setFont(&REGULARFONT);
  tft.setTextSize(1);
  tft.setTextColor(TEXTTWO);
  if (touchInCircle(p, 4.5, 5.625, 16)) {
      settings->brewMillis= settings->brewMillis - 1000;
      refreshNumber(settings->brewMillis / 1000, settings->brewMillis / 1000 + 1, 2, 5.5, 5.25, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 5.625, 16)) {
      settings->brewMillis = settings->brewMillis + 1000;
      refreshNumber(settings->brewMillis / 1000, settings->brewMillis / 1000 - 1, 2, 5.5, 5.25, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 8.375, 16)) {
      settings->preInfMillis = settings->preInfMillis - 1000;
      refreshNumber(settings->preInfMillis / 1000, settings->preInfMillis / 1000 + 1, 2, 5.5, 8, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 8.375, 16)) {
      settings->preInfMillis = settings->preInfMillis + 1000;
      refreshNumber(settings->preInfMillis / 1000, settings->preInfMillis / 1000 - 1, 2, 5.5, 8, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 9.125, 16)) {
      settings->waitMillis = settings->waitMillis - 1000;
      refreshNumber(settings->waitMillis / 1000, settings->waitMillis / 1000 + 1, 2, 5.5, 9.125, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 9.125, 16)) {
      settings->waitMillis = settings->waitMillis + 1000;
      refreshNumber(settings->waitMillis / 1000, settings->waitMillis / 1000 - 1, 2, 5.5, 9.125, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 4.5, 10.625, 16)) {
      settings->preInfBrewMillis = settings->preInfBrewMillis - 1000;
      refreshNumber(settings->preInfBrewMillis / 1000, settings->preInfBrewMillis / 1000 + 1, 2, 5.5, 10.25, 
          REGULARFONTWIDTH, true);
  } else if (touchInCircle(p, 8, 10.625, 16)) {
      settings->preInfBrewMillis = settings->preInfBrewMillis + 1000;
      refreshNumber(settings->preInfBrewMillis / 1000, settings->preInfBrewMillis / 1000 - 1, 2, 5.5, 10.25, 
          REGULARFONTWIDTH, true);
  } else if (touchInButton(p, 5, 13, 4.5, 1.5)) {
      drawHomeScreen();
  } else if (touchInButton(p, 0.5, 13, 4.5, 1.5)) {
      drawSettingsHome();
  }
}

// Manage touch on brewing screen
void handleBrewingTouch(TSPoint p) {
  if (touchInCircle(p, 5, 13.25, 1.5*32)) {
    Serial.println("Ending brew");
    endBrew();
  }
}

// Check to see if a touch is located on a given button
bool touchInButton(TSPoint p, float xCoord, float yCoord, float width, float height) {
  float touchX = 10 -(p.y / 32.0 - 3.5) * (10.0 / 25.1);
  float touchY = (p.x / 32.0 - 2.3) * (15.0 / 26.65);
  /*Serial.print("Touch xcoord: ");
  Serial.println(touchX);
  Serial.print("Touch ycoord: ");
  Serial.println(touchY);*/
  return (touchX >= xCoord && touchX <= xCoord + width &&
        touchY >= yCoord && touchY <= yCoord + height); 
}

// Check to see if touch is located on circle
bool touchInCircle(TSPoint p, float xCoord, float yCoord, int radius) {
  return touchInButton(p, xCoord - radius / 32.0, yCoord - radius / 32.0,
                       2 * radius / 32.0, 2 * radius / 32.0);
}
#endif
