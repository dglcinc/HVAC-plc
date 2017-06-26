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

void initDisplay()
{
  // Init
  PLDuino::enableLCD();
  tft.begin();
  tft.setRotation(2);

  clrDisplay();

  // Initializing touch screen.
  logDisplay("Initializing touch...")
  touch.init(1);
}

void clrDisplay()
{
  tft.fillScreen(ILI9341_BLACK);
}

void logDisplay(String msg)
{
    // print message to display
  tft.println(msg);
}

void updateHvacDisplay()
{
  
}

