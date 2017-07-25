# HVAC-plc
Digital Loggers PLDuino code for custom HVAC monitoring

# Overview
This code is hacked and reassembled from PLDuino repository (<https://github.com/digitalloggers/PLDuino>) published by Digital Loggers for their Arduino-based Programmable Logic Controller (PLC), to meet the purposes of my home's custom HVAC system. I am using the PLDuino to monitor it (and perhaps in future versions control it.) I give no representations or apologies for quality, functionality, or reusability. :-)

[[Note that this is archived, because I finally decided, in spite of the fact that the Digital Loggers PLC is a pretty cool device (with features like optical couplers on the digital inputs and open collectors on the digital outputs) the Arduino and ESP2866 is just too low-level for my needs:
* The ESP2866 especially is a pain to work with - you can only communicate with it via Serial from your Arduino code
* The NodeMCU coding itself is very fussy event-driven code that is not very intuitive to program and debug
* Any time you change your NodeMCU code you have to use a Serial link to upload your Lua files, which is a pain because you also pretty much need to use the upload helper sketch on the Duino, so you're constantly toggling sketches and uploading NodeMCU Lua
* I don't really need the low-level speed Arduino and Arduino code offers; I'd rather get the rich toolset and easy experience on the Raspberry Pi; the main place this probably has a small impact is reading the thermometers (Dallas digital chips) which are slow anyway, but would probably be somewhat faster on the Duino set at a low precision

So I reluctantly abandoned this project and the PLC device and got a Pi3 with a breakout board. I was able to reimplement this entire project in ONE DAY because I didn't have to mess around with getting the wifi working (it "just works"), and the programming model is pretty similar, except I use Python, which frankly I prefer; it's way more powerful than the Arduino or Lua libraries. See that other repository (HVAC-pi) for detailed notes on implementation and gotchas. I plan on copying some of the PLC features like optical couplers and open collectors, the schematics and parts are fully documented on their website.]]

# Newb Gotchas
Since I'm a relative newb to the Arduino universe, along with the ESP8266 Wifi chip and Adafruit ILI9341 touchscreen used by this PLC, it's taken me a ton of time to figure out how to tweak this thing. I'm making a few notes here so other newbs can suffer somewhat less. These notes may be more valuable than the code. I report, you decide.

## How to talk to the ESP82866 and program in Lua
Yeah, so I guess the smart people already know this, but there are some tricks.

Like any Arduino, you compile and upload sketches to the PLDuino. Most of the PLDuino features can be accessed from your
sketch code, by following the various examples in the PLDuino repository.

EXCEPT, the ESP8266! The ESP8266 is running a firmware install called NodeMCU. This firmware allows you to upload and run Lua code files, which call the underlying ESP8266 firmware and configure it to run as a web client, server, access point, etc. In order to change the Lua code loaded into the NodeMCU firmware, you need to follow a special process to upload it to the NodeMCU firmware.

You cannot upload files to the ESP8266 directly, but you do have some ability to call out to it from your sketch using a serial port, for example the demo sketch that shows you how invoke the Lua program already installed on it to provide a hard-coded access point and web page to report status of the PLDuino (the Wifi portion of demo.ino). What they don't tell you is how to change it. If you're like me you have been scratching your head - where does the demo SSID and IP address get set? Where is the web page that gets returned if I hit that IP address, and how is it getting the PLDuino status?

### Flashing the NodeMCU firmware
First, you should make sure you are using the current NodeMCU firmware for the PLDuino. The software used to communicate with the ESP2866 via Serial lets you run interactive Lua commands on the NodeMCU firmware to print the version. I just flashed first (because I didn't figure out how to use ESPlorer right away.) If you want to check what you have first, see the section on "uploading Lua code" to install ESPlorer and check the version, then come back here.

NOTE: It is possible nay likely that you have older firmware that only communicates at 9600 baud, and your attempts to open the port will fail or print gibberish. You can make it work by changing the upload helper sketch code to use 9600 baud and then set ESPlorer to open the port at 9600 baud.

To update the firmware, you need to compile and install the esp_flash_helper sketch, and then press the "Flash" button that comes up. Now you're ready to flash.

Then, you need to run the .bat file in the ESP8266/firmware directory of the repo. Since all this is set up for a Windows machine, I hope you have one or are clever enough to know how to translate this to a UNIX script. I decided to use a Win7 virtual machine on my mac, so I could just use Windows. Note per the readme you need Python and the Python serial library installed. Regardless you need to review/edit the batfile to make sure the com port and Arduino install directory are correct.

If you got this far, running the bat file in a shell window on Windows, the script will spit out some encouraging looking status messages and progress bars, then tell you not to touch anything for 20 minutes. Marshmallow Test!
### Uploading Lua code
OK, so now your firmware is up to date. The firmware flash wiped out the original Lua files, so you need to re-upload them. To do this, you need to install ESPlorer (<https://esp8266.ru/esplorer/>). It's a Java jar. I run it on OSX, so you just double-click on the jar to run it after downloading. If your Java is not installed properly this won't work, and good luck.

First, you need to compile and run the nodemcu_upload_helper sketch. Didn't think of that, did ya?

Next, make sure your your PLDuino is on and connected to your USB port on your computer. If it is not, ESPlorer will not see the port, and you need to restart ESPlorer to pick it up if you forgot.

Once ESPlorer is installed and launched (it takes a couple of minutes to initialize), on the right top side of the panel you need to set your port, then click "open". You don't need to set anything else. After a minute or two it will say it's connected, give a warning about "can't autodetect firmware", and then you can type Lua commands in the command window at the bottom, or click the FSInfo button on the right, or click some of the Node MCU commands in the "Commands" tab on the left. The "Chip ID" button prints out something that resembles a version identifier ("1 5 4 ..." on mine, for the GitHub PLDuino repo dated 4/21/17). These codes correspond to a standard version identifier generated by the NodeMCU function. See (<https://nodemcu.readthedocs.io/en/master/en/modules/node/#nodeinfo>) for description of the output.

To put the PLDuino back to "factory Lua", select the Scripts tab on the left, click the "Upload..." button at the bottom of the panel, and then select all the Lua files and the HTML file in the ESP8266 directory of Digital Loggers' GitHub repository.

### Writing your own Lua code
Cue the Staples WAV file: "That was easy!" So now, you know what you need to know to modify and upload Lua code to the PLDuino. There are some interesting warnings on the NodeMCU site and couple other places. It is not particularly intuitive to code the ESP8266. It is a highly event oriented style with some very subtle gotchas and limitations. Be careful or will find yourself following the instructions on the Digital Loggers site on how to de-brick your ESP8266. You have been warned. My Lua files use a common technique to allow you to recover, by putting a delay call in init.lua, after which the real code in test.lua gets called. This gives you time to run a manual "=file.remove('init.lua')" on the ESPlorer command line, so you can get out of an accidental reboot loop without re-flashing the chip.
### Customizing the firmware
If you really want to get all Indiana Jones on this thing, you can flash different firmwares on the ESP8266, or even write your own. I don't have enough hair on my chest for that. Godspeed to you and follow the links on the Digital Loggers website to get to the Temple of Doom. There are some interesting-looking additions, such as an MQTT client, that I have not played with (yet.)

## The Touchscreen Display
If you have spent much time looking at the example code you know there are not a lot of obvious libraries to build the kind of displays you typically see on control equipment, like Honeywell's touchscreen IAQ thermostats. It's all pretty hacky if you ask me. For example, there is no windowing library with widgets. At least nothing obvious, though the PLDuino libraries seem to have some basics like grid layout and buttons. The sample code has macros to write log messages to the screen and writes directly to the device to clear the screen, etc. I would love to have a windowing system that allows me to do standard setup and manipulation, lay out the screen with widgets, automatically associate touches with the appropriate widget and callback, and do optimized repaint, so it looks like nice high-end device display. Ah well, the next adventure. Maybe I'll realize there is such a thing and I just haven't figured it out yet. Or someone will fork this repo or post an issue and set me straight. It could be ... you!

Till then, my MVP is going to do some basics, hardcoded for my own purposes, and I may get around to doing more. Or maybe someone else will.

## The digital inputs and outputs
People who have some basic electronics knowledge (which does not include me) will understand how to use the enhancements on this device, namely the optical couplers for the digital inputs and the open collector digital outputs. You can figure these out by tracking down Arduino sketches for using optical couplers, and you can also kinda figure it out using the example breadboard layouts that DL puts on their website (*without* the code) but honestly it would be nice if there were better instructions on how to use these features.

### Optical inputs
These are completely isolated from the Arduino digital inputs. To use the digital inputs you need to hook up the common terminal on the inputs to the ground or neutral for your voltage source (in my case the neutral for my HVAC transformer) and then direct the positive / voltage source into the desired input pins to complete the circuit and light the optical coupler. I also set the Arduino pins to INPUT_PULLUP, which turns on the Arduino internal pullup resistors and reverses the logic for the pin (HIGH = off, LOW = on). Not sure if this is necessary but DL implies this is necessary in their "quick start" guide.

### Open Collector outputs
Can't say that I fully understand these yet (there are some nice online tutorials on open collectors), but the key thing to understand is that when you connect a circuit to a PLC output pin, the output is "pulled to ground" at the pin. So you have to put your supply voltage on the "clamp" terminal (e.g. the input voltage for your PLC) and then use the same voltage and the output pin to drive your output - when the output pin is set to high, it pulls the voltage to ground (completes the circuit). There is an example on DL's website using a contactor whose solenoid is driven this way. Again, for neophytes like me it would be nice to have some fully realized projects including the code.
