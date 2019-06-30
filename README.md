# Solder Paste Dispenser

This is a controller board for [zapta solder paste dispensers](https://www.thingiverse.com/thing:1119914). The solder paste dispenser supports a foot pedal switch, an OLED display, and a menu system. The board has been designed with easy soldering in mind.

## Foot switch

Dispensing can be initiated either by the *push* button on the dispenser, or by pushing a foot pedal switch. 

I recommend using a foot switch because it frees up a hand. Using a foot switch is also more stable; when pushing the button on the dispenser itself the syringe always moves a little bit. 

Suitable foot pedals are [Adafruit product ID 423](https://www.adafruit.com/product/423) or [Sparkfun COM-11192](https://www.sparkfun.com/products/11192). These foot pedals are simple on/off switches,  normally open. Speed controller foot pedals with a potentiometer will **not** work.

## Display

The display shows the motor speed. 

The OLED display is a generic OLED module 0.91" diagonal, 128x32 pixel, I2C, SSD1306 as found on ebay, aliexpress and [others](https://www.google.com/search?q=OLED+module+0.91%22+128x32+I2C+SSD1306+price). Before buying, check the module has a 4-pin connection - GND, VCC, SDA, SCK - and uses a SSD1306 controller. The display *ought* not to touch any of the components below, but I put a bit of Kaptan tape on the back of the display, just to make sure.

The board also has a LED which blinks faster when the motor runs faster.

## Menu

There is a settings menu on the serial port. Access the console using a vt100 ansi terminal emulation, e.g. [PuTTY](https://putty.org/) on Windows, or minicom on Linux. On Android, use an USB OTG adapter and Serial USB Terminal. The settings menu allows you to configure the solder paste dispenser. The menu looks like this:

	[f] Forward speed 125 steps/sec
	[s] Forward steps 0 steps
	[b] Backward speed 500 steps/sec
	[d] Pullback delay 2000 millisec
	[p] Pullback steps 0 steps
	[n] Profile number 0
	[r] Restore profile
	[w] Save profile
	[h] Help

Forward speed can be set using the *faster*/*slower* buttons on the dispenser itself or using the console menu. 

If *forward steps* is zero, the syringe plunger is pushed down as long as you press the *push* button or the foot switch pedal. This is useful for making beads, and the normal setting when dispensing solder paste. If *forward steps* is non-zero, pushing the *forward* button or the foot switch moves the plunger down exactly the number of steps in the settings. This is useful for making precise, repeatable dots, e.g. as a glue or lubricating oil dispenser.

Sometimes paste keeps oozing out after you stop dispensing. If *pullback steps* is non-zero, then after a *pullback delay* period of inactivity the plunger will move back *pullback steps*, to stop paste oozing out. How successful this is and how many steps the plunger needs to pull back to stop the oozing depends upon paste viscosity. *Pullback* works better with low viscosity liquids than for high viscosity liquids.

## Saved Profiles

Once you have found a setting which works, you can save it for later. Up to five profiles can be saved. These profiles are remembered even when the power is turned off. After power-up the last saved settings are used.

## Power supply

Use a 5.0V 1A phone charger as power supply. The TB6612 stepper motor driver IC supports 5V 1.2A continuous, but can be damaged by higher currents. Using a power supply with a maximum current less than 1.2A protects the driver IC in case of short circuit or stall.

The voltage range is 4.5V-5.5V. More than 5.5V is too much for the atmega328P microcontroller; less than 4.5V and the TB6612 stepper driver stops working.

## Connecting everything

To access the built-in console, a USB to serial adapter needs to be connected between solder paste dispenser and PC.
 
## Practice makes perfect

For practice fill a syringe with a small amount (approx. 1 ml) of toothpaste. Print a pcb at scale 1:1 in black and white on paper, [like this](https://github.com/koendv/paste_dispenser4zapta/raw/master/toothpaste-practice.pdf), and practice depositing toothpaste until you are comfortable with the solder paste dispenser.

Before filling the syringe, run a nylon wire inside the syringe body. A small piece of fishing line is suitable. This nylon wire creates a small leak in the seal between plunger and syringe body, allowing the air to escape while you push the plunger down. Once the plunger is down, simply pull the nylon wire out and you can begin dispensing.

## Zapta

The controller board is 40mm x 30mm, and fits on [zapta](https://www.thingiverse.com/thing:1119914) solder paste dispensers with a [28BYJ-48](https://www.adafruit.com/product/858) stepper. However, the controller is not specific to *zapta* solder paste dispensers; if a syringe pump uses a 5V, 5- or 6-wire unipolar or 4-wire bipolar stepper motor, there's a reasonable chance this will work. Just check the stepper works at 5V and current stays below 1.2 amps.

## Firmware

The solder paste dispenser is an arduino system and can be programmed using the arduino IDE. 

There is no capacitor between DTR and reset. This avoids spurious resets when connecting to the serial port. If you want to develop software for this board you have three options: 

* Upload firmware using the ISP connector 
* Use the serial port to upload. Just before uploading briefly connect RST and ground - they are next to each other on the ISP connector. 
* When developing connect a 100n capacitor between serial adapter DTR and ISP connector RST pin. This allows the IDE to reset the microcontroller automatically.

## Compiling

The first compile is the hardest. Open the Arduino IDE. Compile the *paste dispenser* sketch, with these settings:

* Board: Arduino Pro or Pro Mini, 
* Processor: Atmega328P (5V, 16MHz).

Generate an Intel Hex file with _Sketch -> Export Compiled Binary_. This creates two .hex files in the project directory, `arduino.ino.eightanaloginputs.hex` and `arduino.ino.with_bootloader.eightanaloginputs.hex`. 
Now connect a 5V Arduino, e.g. an Arduino Uno to your system. Compile and upload the _File -> Examples -> ArduinoISP_ sketch to the Uno. Connect the Arduino Uno to the solder paste dispenser board as follows:

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
where `/dev/cu.usbmodem1d11` is the usb port of your Arduino Uno (On linux probably `/dev/ttyACM0`). Flashing should take approx. 20 seconds, and ends with "`xxx bytes of flash verified`" and "`Fuses OK`". This finishes writing bootloader and sketch to the solder paste dispenser. You can now disconnect the Arduino Uno. 


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

## Making your own

The github contains arduino source and kicad pcb design files. You'll find the [schematic](https://github.com/koendv/paste_dispenser4zapta/raw/master/kicad/paste_dispenser/paste_dispenser_schematic.pdf), the [board layout](https://github.com/koendv/paste_dispenser4zapta/raw/master/kicad/paste_dispenser/paste_dispenser_board.pdf), the [bill of materials](https://raw.githubusercontent.com/koendv/paste_dispenser4zapta/master/kicad/paste_dispenser/paste_dispenser.csv). Three pcbs cost $9.35 at [oshpark](http://www.oshpark.com/shared_projects/V5txbi41). 

If you prefer not to solder, you can also build the controller on a breadboard, like in this [fritzing sketch](https://github.com/koendv/paste_dispenser4zapta/raw/master/fritzing/paste_dispenser_fritzing.pdf).

## Note

The kicad pcb hardware design is under Creative Commons - Share Alike license. In the arduino sketch software all files marked "Koen De Vleeschauwer 2019 CC0" are in the public domain.

A last note: A solder paste dispenser is a small motor which pushes the plunger of a syringe. You can use it for solder paste, but it can also be used as a syringe pump, to create small dots of glue in a precise and repeatable way, or to put small drops of lubricating oil on a mechanism. Have fun!
  
