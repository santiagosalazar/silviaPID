#ifndef SILVIAPID_H
#define SILVIAPID_H

// Determines frequency of data sampling
#define TEMP_TRACK_POINTS 150
// Pin declarations
#define TEMP_PIN A15
#define HEAT_PIN 53
#define PUMP_PIN 51
#define VALVE_PIN 49
#define STEAM_PIN 43
#define BREW_PIN 41

// Keeps track of brew times, screen frame times, and heating window times
struct timeTrack {
    unsigned long windowStartMillis;
    unsigned long frameStartMillis;
    unsigned long brewStartMillis;
    unsigned int brewElapsedSec;
};

// Keeps track of time and temperature settings for brewing and steaming
struct sproSettings {
    unsigned int brewTemp;
    unsigned int steamTemp;
    unsigned int brewMillis;
    unsigned int preInfMillis;
    unsigned int waitMillis;
    unsigned int preInfBrewMillis;
    unsigned int purgeMillis;
    unsigned long windowLength;
    unsigned long frameLength;
};

timeTrack *time;
sproSettings *settings;

// Initialize time tracking
void initTimeTrack(timeTrack *iniTime) {
  iniTime->windowStartMillis = millis();
  iniTime->frameStartMillis = millis();
  iniTime->brewStartMillis = 0;
  iniTime->brewElapsedSec = 0;
  time = iniTime;
}

// initialize time/temp settings with default values
void initSettings(sproSettings *iniSett){
  // default settings
  iniSett->brewTemp = 112;
  iniSett->steamTemp = 155;
  iniSett->brewMillis = 28000;
  iniSett->preInfMillis = 1000;
  iniSett->waitMillis = 2000;
  iniSett->preInfBrewMillis = 28000;
  iniSett->purgeMillis = 2000;
  iniSett->windowLength = 1000;
  iniSett->frameLength = 1000;
  settings = iniSett;
}

// Touch screen configuration
#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930


#endif
