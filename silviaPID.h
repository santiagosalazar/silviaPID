#ifndef SILVIAPID_H
#define SILVIAPID_H

// Determines frequency of data sampling
#define TEMP_TRACK_POINTS 150
// Pin declarations
#define TEMP_PIN A15
#define HEAT_PIN 53
#define PUMP_PIN 51
#define VALVE_PIN 49
#define STEAM_PIN 39
#define BREW_PIN 37

struct tempTrack {
    double ssxx;
    double ssxy;
    double meanVolt;
    double meanTime;
    double approxTemp;
    uint16_t analogVoltLog[2][TEMP_TRACK_POINTS];
    unsigned int dispTemp;
    unsigned long prevTempRead;
    unsigned long tempWindow;
};
struct timeTrack {
    unsigned long windowStartMillis;
    unsigned long frameStartMillis;
    unsigned long brewStartMillis;
    unsigned int brewElapsedSec;
};
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


#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930

#define MINPRESSURE 100
#define MAXPRESSURE 1000

#endif
