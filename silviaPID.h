#include <PID_v1.h>
#include <Time.h>
#include <DS1302RTC.h>
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
#define PUMP_PIN 51
#define VALVE_PIN 49
#define STEAM_PIN 47
#define BREW_PIN 45
#define RTC_CE 43
#define RTC_IO 41
#define RTC_CLK 39

// Touch screen declarations
#define YP A2
#define XM A3
#define YM 8
#define XP 9

#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930

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


