#ifndef SILVIAPID_H
#define SILVIAPID_H
#define TEMPTRACKWINDOW 300

struct tempTrack {
    float ssxx;
    float ssxy;
    float meanTemp;
    float meanTime;
    double approxTemp;
    float instTempLog[2][TEMPTRACKWINDOW];
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

// Pin declarations
#define TEMP_PIN A15
#define HEAT_PIN 53
#define PUMP_PIN 51
#define VALVE_PIN 49
#define STEAM_PIN 47
#define BREW_PIN 45


#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930

#define MINPRESSURE 100
#define MAXPRESSURE 1000

#endif
