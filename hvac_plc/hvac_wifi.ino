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

HardwareSerial &wifi = Serial2;

// an equivalent to (sstream << setw(width) << setfill(fill) << value).str()
String fmt (int value, int width, char fill='0')
{
  String r = String(value);
  while(r.length() < width)
    r = fill + r;
  return r;
}

void processCommand (const String &cmd, HardwareSerial &wifi)
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //    Command: 2-char long, <op: Rr/Oo/Ss><index>
  //    op:
  //      Rr: relay, R=on, r=off
  //      Oo: output, R=on, r=off
  //      Ss: get overall device state (both)
  //    Index:
  //      Rr - 1-6
  //      Oo - 1-8
  //
  ///////////////////////////////////////////////////////////////////////////////

  char op = cmd[0];
  int index = cmd[1] - '1';
  if (index < 0) return;
  switch(op)
  {
    case 'r':
    case 'R':
      // switch relay
      if (index < 6) {
        Serial.println("relay" + String(index) + (op=='R'? "ON" : "off"));
        digitalWrite(RELAY1+index, (op=='R'? HIGH : LOW));
      }
      break;
      
    case 'o':
    case 'O':
      // switch DOUT
      if (index < 8) {
        Serial.println("dout" + String(index) + (op=='O'? "ON" : "off"));
        digitalWrite(38+index, (op=='O'? HIGH : LOW));
      }
      break;

    case 's':
    case 'S':
      // current state was requested
 //     Serial.print("Sending current state...");
      for(int i=0; i<6; ++i)
        wifi.write(digitalRead(RELAY1+i)==HIGH? "1" : "0");
      for(int i=0; i<8; ++i)
        wifi.write(digitalRead(30+i)==HIGH? "1" : "0");
      for(int i=0; i<8; ++i)
        wifi.write(digitalRead(38+i)==HIGH? "1" : "0");
      for(int i=0; i<8; ++i)
        wifi.write(fmt(analogRead(A0+i),4).c_str());
      wifi.write((
          fmt(hour(),2) +
          fmt(minute(), 2)
        ).c_str());
      wifi.write((
          fmt(year(), 4) +
          fmt(month(), 2) +
          fmt(day(), 2)
        ).c_str());
      wifi.write("e");
      wifi.flush();
 //     Serial.println(" done.");
      break;
  }
}


void initWifi()
{
  // Setup wifi
  logDisplay("Initializing ESP8266... ");

  // Initializing ESP module
  PLDuino::enableESP();

  logDisplay("done.");
  
  // Reset ESP
  digitalWrite(PLDuino::ESP_RST, LOW);
  delay(500);
  digitalWrite(PLDuino::ESP_RST, HIGH);
  delay(3000);
  
  // Print its boot messages
  logDisplay("Booting ESP...");
  
  String result = "";
  char ch;
  
  while(wifi.available())
  {
    ch = wifi.read();
    result += ch;
  }
  logDisplay(result);
  
  logDisplay("done");

/*
  String result;
  wifi.write("wifi.sta.getip()");
  getWifiResponse(result);
  logDisplay( "IP addr: " + result );
  wifi.write("wifi.sta.getmac()");
  getWifiResponse(result);
  logDisplay( "MAC addr: " + result );
  */
  logDisplay("Ready to execute commands.");
}

void getWifiResponse(String result)
{
  char ch;
  result = "";
  int a, b;
  for (a = 0, b = wifi.available(); a < b; a++ )
  {
    ch = wifi.read();
    result += ch;
  }
  logDisplay("getWifiResponse: " + result);
}

void checkWifiCmd()
{
  String recv = "";
  while (wifi.available())
    {
      char ch = wifi.read();
      if (ch == 'C') recv = String();
      else recv += ch;
      
      while(recv.length() >= 2)
      {
        // Extract 2-char command
        String cmd = recv.substring(0, 2);
        // Remove this command from the buffer
        recv = recv.substring(2);
        
        Serial.print(cmd);

        // Execute the command
        processCommand(cmd, wifi);
      }
  }

}

