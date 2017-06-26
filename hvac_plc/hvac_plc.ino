#include <pcmConfig.h>
#include <pcmRF.h>
//#include <TMRpcm.h>

#include <TMRpcm_PLDuino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <PLDuino.h>
#include <PLDTouch.h>
#include <PLDuinoGUI.h>
#include <using_namespace_PLDuinoGUI.h>
#include <DS3232RTC.h>
#include <Time.h>
#include <TimeLib.h>
#include <Wire.h>
#include "utils.h"

#define VERSION "1.2.2.0"

Adafruit_ILI9341 tft = Adafruit_ILI9341(PLDuino::LCD_CS, PLDuino::LCD_DC);
PLDTouch touch(PLDuino::TOUCH_CS, PLDuino::TOUCH_IRQ);
TMRpcm tmrpcm;
Sd2Card card; bool card_initialized;

void blinkLED()
{
  for(int i=0; i<3; ++i)
  {
    digitalWrite(PLDuino::LED_PIN, LOW); delay(200);
    digitalWrite(PLDuino::LED_PIN, HIGH); delay(200);
  }
}

void initDisplay();
void initWifi();
void logDisplay();
void clearDisplay();
void checkWifiCommand();
void refreshHvacDisplay();

void setup()
{  
  // Set pin modes and initialize stuff
  // NB: This line is necessary in all sketches which use PLDuino library stuff.
  PLDuino::init();
  
  // Signal that the PLDuino setup has started.
  blinkLED();
  
  // Power-on LCD and set it up
  initDisplay();

  // Setup serials. Serial2 is connected to ESP-02 Wi-Fi module.
  Serial.begin(115200);
  Serial2.begin(115200);
  
  // Print version info.
  clearDisplay();
  logDisplay("PLDuino firmware v" VERSION ", built " __DATE__)
  logDisplay("")

  // We need to initialize SD card at startup!
  logDisplay("Initializing SD card...")
  card_initialized = card.init(SPI_HALF_SPEED, PLDuino::SD_CS);
  if (!SD.begin(PLDuino::SD_CS))
    logDisplay("ERROR: Can't initialize SD card!")

  // Initializing real-time clock.
  logDisplay("Initializing RTC...")
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet)
    logDisplay("ERROR: Unable to sync with the RTC")
  
  // Setup speaker pin to play sounds.
  logDisplay("Initializing speaker...");
  tmrpcm.speakerPin = 9;

  // Initialize Wifi
  initWifi();
  
  // Initialization is complete. 
  logDisplay("")
  logDisplay("Initialization complete.")
  logDisplay("")
  logDisplay("-- Touch to keep the log on screen --")
  delay(1500);
  while(touch.dataAvailable()) touch.read();
}

void loop()
{
  // Check if there are any wifi requests (needed?)
  checkWifiCommand();
  
  // update display
  refreshHvacDisplay();
}

