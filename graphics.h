#ifndef GRAPHICS_h
#define GRAPHICS_h
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

// Touch screen declarations
#define YP A2
#define XM A3
#define YM 8
#define XP 9

#define TITLEFONT FreeMonoBold24pt7b
#define TITLEFONTWIDTH 27 
#define REGULARFONT FreeMonoBold18pt7b
#define REGULARFONTWIDTH 21

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

#define BOXSIZE 40
#define PENRADIUS 2


// Kuman LCD TFT library
MCUFRIEND_kbv tft; 

// x, y coordinates, width, height are given as coordinates for a 10x15 grid for screen,
// thickness is given in pixels
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


void titleText(char* title, int titleLen) {
  tft.setTextSize(1);
  tft.setTextColor(TEXTONE);
  tft.setFont(&TITLEFONT);
  drawRoundButton(0,0,10,3,0,0,title, TITLEFONTWIDTH, titleLen);
  tft.setFont(&REGULARFONT);
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

// Clears oldNum and replaces with newNum, correcting for number of digits
void refreshNumber(float newNum, float oldNum, int maxLength, float xCoord, 
                   float yCoord, int charWidth, bool isInt){
  tft.setTextColor(BACKGROUND);
  drawNumber(oldNum, maxLength, xCoord, yCoord, charWidth, isInt);
  tft.setTextColor(TEXTTWO);
  drawNumber(newNum, maxLength, xCoord, yCoord, charWidth, isInt);
}
#endif
