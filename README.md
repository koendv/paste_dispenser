# Solder Paste Dispenser

![](https://github.com/koendv/paste_dispenser/raw/master/kicad/paste_dispenser/paste_dispenser_front.png)

This is a controller board for [solder paste dispensers](https://www.thingiverse.com/thing:1119914). The controller supports unipolar and bipolar stepper motors, a foot pedal switch, an OLED display, a LED that blinks faster when the motor runs faster, and a menu system. The board has been designed with easy soldering in mind.

## Foot switch

Dispensing can be initiated either by pressing the *push* button on the dispenser, or by pushing down a foot pedal switch. 

A foot pedal switch has the advantage that it frees up a hand. Using a foot switch is also more stable; pushing a button on the solder dispenser always moves the syringe tip a little bit.

Suitable foot pedals are [Adafruit product ID 423](https://www.adafruit.com/product/423) and [Sparkfun COM-11192](https://www.sparkfun.com/products/11192), and their equivalents at ebay and aliexpress.

These foot pedal switches are simple on/off switches, normally open. Speed controller foot pedals with a potentiometer will **not** work.

## Display

The display shows the motor speed. 

The OLED display is a generic OLED module 0.91" diagonal, 128x32 pixel, I2C, SSD1306, as found on ebay, aliexpress and [others](https://www.google.com/search?q=OLED+module+0.91%22+128x32+I2C+SSD1306+price). Before buying, check the module has a 4-pin connection - GND, VCC, SDA, SCK - and uses a SSD1306 controller. The display *ought* not to touch any of the components below, but you can put a bit of Kaptan tape on the back of the display, just to make sure.

The OLED display is optional. If you choose not to install the OLED display you can still check stepper speed on the serial console.

## Menu

There is a settings menu on the serial port. The serial port runs at 115200, 8N1. Access the menu using a vt100 ansi terminal emulation, e.g. [PuTTY](https://putty.org/) on Windows, or *minicom* on Linux. On Android, use an USB OTG adapter and *Serial USB Terminal*. The settings menu allows you to configure forward and backward speed, pullback, and microstepping, and to store and recall these settings from non-volatile memory. The menu looks like this:

	[f] Forward speed 125 steps/sec
	[s] Forward steps 0 steps
	[b] Backward speed 500 steps/sec
	[d] Pullback delay 2000 millisec
	[p] Pullback steps 0 steps
	[m] Microstepping 2 
	[n] Profile number 0
	[r] Restore profile
	[w] Save profile
	[h] Help

### Speed
Forward speed can be set using the *faster*/*slower* buttons on the dispenser itself or using the console menu. 

If *forward steps* is zero, the syringe plunger is pushed down as long as you press the *push* button or the foot switch pedal. This is useful for making beads, and the normal setting when dispensing solder paste. If *forward steps* is non-zero, pushing the *forward* button or the foot switch moves the plunger down exactly the number of steps in the settings. This is useful for making precise, repeatable dots, e.g. as a glue or lubricating oil dispenser.

Backward speed can only be set using the console menu.

### Pullback
Sometimes paste keeps oozing out after you stop dispensing. If *pullback steps* is non-zero, then after a *pullback delay* period of inactivity the plunger will move back *pullback steps*, to stop paste oozing out. How successful this is and how many steps the plunger needs to pull back to stop the oozing depends upon paste viscosity. *Pullback* works better with low viscosity liquids than with high viscosity paste.

### Microstepping
The controller supports up to 1/64 microstepping. This is open loop voltage control microstepping.
Choose speed and microstepping that goes well with your stepper motor. This may take a bit of trial and error. Default is half-stepping.

### Saved Profiles

Once you have found a setting that works, you can save it for later. Up to five profiles can be saved. These profiles are remembered even when the power is turned off. After power-up the last saved settings are used.

## Power supply

If you want to play it safe, use a 5.0V 1A phone charger as power supply. The TB6612 stepper motor driver IC supports 5V 1.2A continuous, but can be damaged by higher currents. Using a power supply with a maximum current less than 1.2A protects the driver IC in case of short circuit or stall.

The voltage range is 4.5V to 5.5V. More than 5.5V is too much for the atmega328p microcontroller; less than 4.5V and the TB6612 stepper driver stops working.

The board has a MOSFET for reverse polarity protection.

## Connecting everything

The controller board has four headers:

* J1 is the ISP connector for downloading the initial firmware to the atmega microcontroller.
* J2 connects to an USB to serial adapter (pins +5V, RX, TX, GND) and the footswitch (pins FTSW, GND). The footswitch is optional, but very handy.
* J3 is the connector for the stepper motor. Both unipolar and bipolar steppers can be connected. Connect one coil to pins A1 and A2, and one coil to pins B1 and B2. Connect the center tap of unipolar steppers to pin GND.
* J4 is the connector for the optional OLED display. Solder a 4-position female Dupont housing here.
 
## Practice makes perfect

For practice fill a syringe with a small amount (approx. 1 ml) of toothpaste. Print a pcb at scale 1:1 in black and white on paper and practice depositing toothpaste until you are comfortable with the solder paste dispenser.

Before filling the syringe, run a nylon wire inside the syringe body. A small piece of fishing line is suitable. This nylon wire creates a small leak in the seal between plunger and syringe body, allowing the air to escape while you push the plunger down. Once the plunger is down, simply pull the nylon wire out and you can begin dispensing.

## Zapta

The controller board is 40mm x 30mm, and fits on [zapta](https://www.thingiverse.com/thing:1119914) solder paste dispensers with a [28BYJ-48](https://www.adafruit.com/product/858) stepper. However, the controller is not restricted to any specific model of solder paste dispenser; if a syringe pump uses a 5- or 6-wire unipolar or a 4-wire bipolar stepper motor, there's a reasonable chance this will work. Just check the stepper works at 5V and current stays below 1.2 amps.

## Firmware

The solder paste dispenser is an arduino system and can be programmed using the arduino IDE. 

There is no capacitor between DTR and reset. This avoids spurious resets when connecting to the serial port. But this also means that if you want to develop firmware for this board there is no automatic reset before uploading. Three options: 

* Upload firmware using the ISP connector. This has to be done at least once, to upload the bootloader.
* Use the serial port to upload. This is what I usually do. Before uploading connect RST and ground with a Dupont jumper wire. RST and ground are next to each other on the ISP connector. In the Arduino IDE, click Sketch -> Upload, count to three and remove the jumper wire. 
* When developing connect a 100n capacitor between serial adapter DTR and ISP connector RST pin. This allows the IDE to reset the microcontroller automatically just before uploading using the serial port.

## Compiling

The first compile is the hardest.

Open the Arduino IDE. Compile the *paste dispenser* sketch, with these settings:

* Board: Arduino Pro or Pro Mini, 
* Processor: Atmega328P (5V, 16MHz).

Generate an Intel Hex file with _Sketch -> Export Compiled Binary_. This creates two .hex files in the project directory, `arduino.ino.eightanaloginputs.hex` and `arduino.ino.with_bootloader.eightanaloginputs.hex`. The hex file we need is the one with the bootloader.
The ISP programmer is an arduino with the same logic levels as the target system. As the solder paste dispenser uses 5V logic, we need an Arduino with 5V logic levels as ISP programmer. Connect a 5V Arduino, e.g. an Arduino Uno, to your system. Compile and upload the _File -> Examples -> ArduinoISP_ sketch to the Uno. Connect the Arduino Uno to the solder paste dispenser board as follows:

Signal|Uno    |ISP pin
------|-------|--------
RST	|Uno pin 10|5
MOSI|Uno pin 11|4
MISO|Uno pin 12|1
SCK|Uno pin 13|3
VCC|Uno 5V|2
GND|Uno GND|6

Find `avrdude` in your Arduino IDE directories, change to the appropriate directory and do the following incantation:
`./bin/avrdude -C ./etc/avrdude.conf  -p m328p -P /dev/cu.usbmodem1d11 -c avrisp -b 19200 -U flash:w:/Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex`
where `/dev/cu.usbmodem1d11` is the usb port of your Arduino Uno (On linux probably `/dev/ttyACM0`). Flashing should take about 20 seconds, and ends with "`xxx bytes of flash verified`" and "`Fuses OK`".


    Mac-Pro:avr koen$ ./bin/avrdude -C ./etc/avrdude.conf  -p m328p -P /dev/cu.usbmodem1d11 -c avrisp -b 19200 -U flash:w:/Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex
    
    avrdude: AVR device initialized and ready to accept instructions
    
    Reading | ################################################## | 100% 0.02s
    
    avrdude: Device signature = 0x1e950f (probably m328p)
    avrdude: NOTE: "flash" memory has been specified, an erase cycle will be performed
             To disable this feature, specify the -D option.
    avrdude: erasing chip
    avrdude: reading input file "/Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex"
    avrdude: input file /Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex auto detected as Intel Hex
    avrdude: writing flash (32670 bytes):
    
    Writing | ################################################## | 100% 13.06s
    
    avrdude: 32670 bytes of flash written
    avrdude: verifying flash memory against /Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex:
    avrdude: load data flash data from input file /Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex:
    avrdude: input file /Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex auto detected as Intel Hex
    avrdude: input file /Users/koen/Documents/Arduino/paste_injector/arduino/arduino.ino.with_bootloader.eightanaloginputs.hex contains 32670 bytes
    avrdude: reading on-chip flash data:
    
    Reading | ################################################## | 100% 7.42s
    
    avrdude: verifying ...
    avrdude: 32670 bytes of flash verified
    
    avrdude: safemode: Fuses OK (E:FD, H:DA, L:FF)
    
    avrdude done.  Thank you.

This finishes writing bootloader and sketch to the solder paste dispenser. Disconnect the Arduino Uno. The atmega328p now contains a bootloader. Sketches can now be uploaded using the solder paste dispenser serial port.

## Making your own

The github contains arduino source and kicad pcb design files. You'll find the [schematic](https://github.com/koendv/paste_dispenser4zapta/raw/master/kicad/paste_dispenser/paste_dispenser_schematic.pdf), the [board layout](https://github.com/koendv/paste_dispenser4zapta/raw/master/kicad/paste_dispenser/paste_dispenser_board.pdf), and the [bill of materials](https://github.com/koendv/paste_dispenser4zapta/blob/master/kicad/paste_dispenser/paste_dispenser.csv). This is a link to the pcb as an orderable [shared project at oshpark](http://www.oshpark.com/shared_projects/V5txbi41), and these are the components as a [shared project at Mouser](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=61ff8d7d43). Here are [zipped gerbers](https://github.com/koendv/paste_dispenser/raw/master/kicad/paste_dispenser/gerbers.zip) for pcb manufacturing at [jlcpcb](https://jlcpcb.com/). 

The openscad/ directory contains the .stl files for 3d-printing the mechanical part of the solder paste dispenser. 

If you prefer not to solder you can also build the controller on a breadboard, as in this [fritzing sketch](https://github.com/koendv/paste_dispenser4zapta/raw/master/fritzing/paste_dispenser_fritzing.pdf) of a 5V Arduino Pro Mini and a TB6612 breakout module.

## Care

* There are a few situations which may result in breakage.
If the piston pushes against the bottom of an empty syringe, something has to give.
Likewise, if you try to squeeze paste out of a stoppered syringe. 
In general, avoid "unstoppable force meets inmovable object" situations. Something will break - the motor may tear itself off its mounts, for instance.

* When not in use, loosen the syringe holder screws so the syringe isn't under constant pressure.

## Notes

* The kicad pcb hardware design is under Creative Commons - Attribution Share Alike license.

* A solder paste dispenser is a small motor which pushes the plunger of a syringe. You can use it for solder paste, but it can also be used as a syringe pump, to create small dots of glue in a precise and repeatable way, or to put small drops of lubricating oil on a mechanism.

  
