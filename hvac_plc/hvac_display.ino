#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <PLDuino.h>
#include <DS3232RTC.h>
#include <Time.h>
#include <Wire.h>
#include "utils.h"
using namespace PLDuino;

#define FSIZE 4

bool di_status[8];
bool di_initted = false;

void initDisplay()
{
  // Init
  PLDuino::enableLCD();
  tft.begin();
  tft.setRotation(0);

  clrDisplay();

  // Initializing touch screen.
  logDisplay("Initializing touch...");
  touch.init(1);
}

void clrDisplay()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0,0);
}

void logDisplay(String msg)
{
  logDisplaySize(msg,1);
}
void logDisplaySize(String msg, int size)
{
  // print message to display
  tft.setTextSize(size);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(msg);
  Serial.println(msg);
}

void refreshHvacDisplay()
{
  bool do_refresh = false;

  int i, res;
  for (i = 0; i < 8; i++)
  {
    if (di_initted == false)
    {
      // use the pullup resistors
      pinMode(30+1, INPUT_PULLUP);
      digitalWrite(30+i, HIGH);
      di_status[i] = false;
      do_refresh = true;
    }
    else
    {
      res = (digitalRead(30+i) == LOW);
      if (di_status[i] != res)
      {
        di_status[i] = res;
        do_refresh = true;
      }
    }
  }
  di_initted = true;
  
  if (do_refresh == true)
  {
    clrDisplay();
    // positions of equipment
    // DIN0 = ZV
    // DIN1 = DHW
    // DIN2 = BLR
    // DIN3 = RCHL
    // DIN4 = LCHL
    // DIN5 = Y2
    String result;
    logDisplaySize( "", FSIZE);
    result = di_status[0]? "ON" : "off";
    logDisplaySize( "  ZV= " + result, FSIZE);
    result = di_status[1]? "ON" : "off";
    logDisplaySize( " DHW= " + result, FSIZE);
    result = di_status[2]? "ON" : "off";
    logDisplaySize( " BLR= " + result, FSIZE);
    result = di_status[3]? "ON" : "off";
    logDisplaySize( "RCHL= " + result, FSIZE);
    result = di_status[4]? "ON" : "off";
    logDisplaySize( "LCHL= " + result, FSIZE);
    result = di_status[5]? "ON" : "off";
    logDisplaySize( "  Y2= " + result, FSIZE);
    result = di_status[6]? "ON" : "off";
    logDisplaySize( "YOFF= " + result, FSIZE);
    logDisplaySize( "", FSIZE);
    logDisplaySize( "", FSIZE);
  }
}

