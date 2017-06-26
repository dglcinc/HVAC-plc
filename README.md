NOTE: the code does not currently compile. I'll remove this note when I upload the first running version.

# HVAC-plc
Digital Loggers PLDuino code for custom HVAC monitoring

# Overview
This code is hacked and reassembled from PLDuino repository (<https://github.com/digitalloggers/PLDuino>) published by Digital Loggers for their Arduino-based Programmable Logic Controller (PLC), to meet the purposes of my home's custom HVAC system. I am using the PLDuino to monitor it (and perhaps in future versions control it.) I give no representations or apologies for quality, functionality, or reusability. :-)

# Newb Gotchas
Since I'm a relative newb to the Arduino universe, along with the ESP8266 Wifi shield and Adafruit ILI9341 touchscreen used by this PLC, it's taken me a ton of time working in my spare time to figure out how to tweak this thing. I'm making a few notes here so other newbs can suffer somewhat less. These notes may be more valuable than the code. I report, you decide.

## How to talk to the ESP82866 and program in Lua
Yeah, so I guess the smart people already know this, but there are some tricks.
* Like any Arduino, you compile and upload sketches to the PLDuino. Most of the PLDuino features can be accessed from your
sketch code, by following the various examples in the PLDuino repository.
* EXCEPT, the ESP82866! The ESP82866 is running a Lua-based firmware install called NodeMCU. This firmware allows you to upload and run Lua files, html, etc., which tell the ESP82866 to be a web client, server, access point, etc. In order to change the Lua code, you need to follow a special process to upload it to the NodeMCU firmware.
* You cannot upload files to the ESP82866 directly, and for the most part you cannot control it from your sketch (the demo sketch shows you how the Lua files already installed on it provide a hard-coded access point and web page to report status of the PLDuino. What they don't tell you is how to change it.
### Flashing the NodeMCU firmware
First, you should make sure you are using the current NodeMCU firmware for the PLDuino. I cannot figure out an easy way to tell (there is probably way to do it using ESPlorer or Lua code, which I will explain shortly.)
* To update the firmware, you need to compile and install the esp_flash_helper sketch, and then press the "Flash" button that comes up. Now you're ready to flash.
* Then, you need to run the .bat file in the ESP8266/firmware directory of the repo. Since all this is set up for a Windows machine, I hope you have one or are clever enough to know how to translate this to a UNIX script. I decided to use a Win7 virtual machine on my mac, so I could just use Windows. Note per the readme you need Python and the Python serial library installed. Regardless you need to review/edit the batfile to make sure the com port and Arduino install directory are correct.
* If you got this far, running the bat file in a shell window on Windows, the script will spit out some encouraging looking status messages and progress bars, then tell you not to touch anything for 20 minutes. Marshmallow Test!
### Uploading Lua code
OK, so now your firmware is up to date. The firmware flash wiped out the original Lua files, so you need to re-upload them. To do this, you need to install ESPlorer (<https://esp8266.ru/esplorer/>). It's a Java jar. I run it on OSX, so you just double-click on the jar to run it after downloading. If your Java is not installed properly this won't work, and good luck.
* First, you need to compile and run the nodemcu_upload_helper sketch. Didn't think of that, did ya?
* Next, make sure your your PLDuino is on and connected to your USB port on your computer. If it is not, ESPlorer will not see the port, and you need to restart ESPlorer to pick it up if you forgot.
* Once ESPlorer is installed and launched (it takes a couple of minutes to initialize), on the right top side of the panel you need to set your port, then click "open". You don't need to set anything else. After a minute or two it will say it's connected, give a warning about "can't autodetect firmware", and then you can type Lua commands in the command window at the bottom, or click the FSInfo button on the right, or click some of the Node MCU commands in the "Commands" tab on the left. The "Chip ID" button prints out something that resembles a version identifier, but I haven't seen info anywhere that says what the codes are.
* To put the PLDuino back to "factory Lua", select the Scripts tab on the left, click the "Upload..." button at the bottom of the panel, and then select all the Lua files and the HTML file in the ESP8266 directory.
* NOTE: You did update your firmware didn't you? If you didn't you are a naughty bunny. In this case, it is possible nay likely that you have older firmware that only communicates at 9600 baud, and your attempts to open the port will fail or print gibberish. You can make it work by changing the upload helper sketch code to use 9600 baud and then set ESPlorer to open the port at 9600 baud, but really just do the right thing.
### Writing your own Lua code
* Cue the Staples WAV file: "That was easy!" Yeah, you try to figure that out without my notes.
* So now, you know what you need to know to modify and upload Lua code to the PLDuino. I have not tried much of this yet, but there are some interesting warnings on the NodeMCU site and couple other places. It is not particularly intuitive to code the ESP8266. It is a highly event oriented style with some very subtle gotchas and limitations. Be careful or will find yourself following the instructions on the Digital Loggers site on how to de-brick your ESP8266. You have been warned.
### Customizing the firmware
If you really want to get all Indiana Jones on this thing, you can flash different firmwares on the ESP8266, or even write your own. I don't have enough hair on my chest for that. Godspeed to you and follow the links on the Digital Loggers website to get to the Temple of Doom.

## The Touchscreen Display
If you have spent much time looking at the example code you know there are not a lot of obvious libraries to build the kind of displays you typically see on control equipment, like Honeywell's touchscreen IAQ thermostats. It's all pretty hacky if you ask me. For example, there is no windowing library with widgets. At least nothing obvious, though the PLDuino libraries seem to have some basics like grid layout and buttons. The sample code has macros to write log messages to the screen and writes directly to the device to clear the screen, etc. I would love to have a windowing system that allows me to do standard setup and manipulation, lay out the screen with widgets, automatically associate touches with the appropriate widget and callback, and do optimized repaint, so it looks like nice high-end device display. Ah well, the next adventure. Maybe I'll realize there is such a thing and I just haven't figured it out yet. Or someone will fork this repo or post an issue and set me straight. It could be ... you!

Till then, my MVP is going to do some basics, hardcoded for my own purposes, and I may get around to doing more. Or maybe someone else will.
